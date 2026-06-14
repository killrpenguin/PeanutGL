/*
 * Copyright (c) 2026 David McFarland
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "CameraComponent.hpp"
#include "DebugSystem.hpp"
#include "Engine.hpp"
#include "Entity.hpp"
#include "ImGuiSystem.hpp"
#include "Platform.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Utilities.hpp"

#include "quill/LogMacros.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <ranges>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace PeanutGL {
    namespace chrono = std::chrono;

    auto constexpr Engine::IsMainThread() const -> bool {
        return equal( std::this_thread::get_id(), mainThreadId );
    }

    auto Engine::ProcessPendingEntityRemovals() noexcept -> void {
        std::vector< std::string > names{};
        {
            const std::scoped_lock< std::mutex > lock{ pendingEntityRemovalsMutex };

            if ( pendingEntityRemovalNames.empty() ) { return; }

            names.swap( pendingEntityRemovalNames );
        }

        // Only ever process on the main thread for safety.
        if ( !IsMainThread() ) {
            const std::scoped_lock< std::mutex > lock{ pendingEntityRemovalsMutex };

            pendingEntityRemovalNames.insert( pendingEntityRemovalNames.end(), names.begin(), names.end() );

            return;
        }

        for ( const auto& name : names ) {
            // TODO: Log failure once logging system is implemented.
            (void)RemoveEntity( name );
        }
    }

    auto Engine::Initialize() -> bool {
        mainThreadId = std::this_thread::get_id();
        constexpr Width ScreenWidth{ 800 };
        constexpr Height ScreenHeight{ 600 };

        platform = CreatePlatform();

        if ( !platform->Initialize( "PeanutGL", ScreenWidth, ScreenHeight ) ) {
            throw std::runtime_error( "Could not initialize platform." );
        }

        platform->SetWindowResizeCallback(
            [this]( const Width width, const Height height ) -> void { HandleResize( width, height ); } );

        platform->SetFramebufferResizeCallback(
            [this]( const Width width, const Height height ) -> void { HandleResize( width, height ); } );

        platform->SetMouseCallback( [this]( const XAxis x, const YAxis y, std::uint32_t buttons ) -> void {
            this->handleMouseInput( x, y, buttons );
        } );

        platform->SetKeyboardCallback(
            [this]( const std::uint32_t key, const bool pressed ) { this->handleKeyInput( key, pressed ); } );

        platform->SetCharCallback( [this]( std::uint32_t character ) {
            if ( imguiSystem ) { imguiSystem->HandleChar( character ); }
        } );

        renderer = std::make_unique< Renderer >( platform.get() );

        resourceManager = std::make_unique< ResourceManager >();

        if ( !renderer->Initialize() ) { throw std::runtime_error( "Could not initialize renderer." ); }

        try {
            imguiSystem = std::make_unique< ImGuiSystem >( platform.get(), ScreenWidth, ScreenHeight );
        } catch ( const std::exception& err ) {
            LOG_ERROR( QuillPtr(), "Subsystem initialization failed: {}", err.what() );
            return false;
        }

        assert( platform != nullptr && "Platform is unninialized." );
        assert( renderer != nullptr && "Renderer is unninialized." );
        assert( resourceManager != nullptr && "ResourceManager is unninialized." );
        assert( imguiSystem != nullptr && "ImGuiSystem is unninialized." );

        initialized = true;

        LOG_INFO( QuillPtr(), "Engine Initialized." );

        return initialized;
    }

    auto Engine::Run() -> void {
        if ( !initialized ) { throw std::runtime_error( "Engine is not initialized" ); }

        running = true;

        while ( running ) {
            if ( !platform->ProcessEvents() ) {
                running = false;
                break;
            }

            deltaTimeMs = CalculateDeltaTimeMs();

            UpdateFrameCounter();

            if ( fpsUpdateTimer >= 1.0 ) { UpdateWindow(); }

            Update( deltaTimeMs );

            Render();
        }
    }

    constexpr auto Engine::UpdateWindow() noexcept -> void {
        constexpr static double AvgMsMultiplier{ 1000.0 };

        std::uint64_t framesSinceLastUpdate = frameCount - lastFPSUpdateFrame;

        double avgMs{ 0.0 };

        if ( greater( framesSinceLastUpdate, 0 ) and greater( fpsUpdateTimer, 0.0 ) ) {
            currentFPS = static_cast< double >( framesSinceLastUpdate ) / fpsUpdateTimer;
            avgMs      = ( fpsUpdateTimer / static_cast< double >( framesSinceLastUpdate ) ) * AvgMsMultiplier;
        } else {
            currentFPS = ranges::max( currentFPS, 1.0 );
            avgMs      = static_cast< double >( deltaTimeMs.count() );
        }

        const std::string title = "PeanutGL - Frame: " + std::to_string( frameCount ) +
                                  " | FPS: " + std::to_string( static_cast< int >( currentFPS ) ) +
                                  " | ms: " + std::to_string( static_cast< int >( avgMs ) );

        platform->SetWindowTitle( title );

        fpsUpdateTimer     = 0.0;
        lastFPSUpdateFrame = frameCount;
    }

    constexpr auto Engine::UpdateFrameCounter() noexcept -> void {
        constexpr static double Tick{ 0.001 };
        frameCount++;
        fpsUpdateTimer += static_cast< double >( deltaTimeMs.count() ) * Tick;
    }

    auto Engine::Cleanup() noexcept -> void {
        if ( initialized ) {
            {
                const std::unique_lock< std::shared_mutex > lock{ entitiesMutex };
                entities.clear();
                entityMap.clear();
            }

            // Always Clean up subsystems in reverse order of creation.
            imguiSystem.reset();
            physicsSystem.reset();
            resourceManager.reset();
            renderer.reset();
            platform.reset();

            initialized = false;
        }
    }

    auto Engine::SetActiveCamera( CameraComponent* const cameraComponent ) noexcept -> void {
        if ( equal( cameraComponent, nullptr ) ) {
            LOG_WARNING( QuillPtr(), "Attempting to set active camera to nullptr." );
            return;
        }
        activeCamera = cameraComponent;
    }

    auto Engine::GetActiveCamera() const -> const CameraComponent* {
        return activeCamera;
    }

    auto Engine::CreateEntity( const std::string& name ) noexcept -> Entity* {
        const std::unique_lock< std::shared_mutex > lock{ entitiesMutex };

        /* Potential Memory Leak: Do not use emplace_back with unique_ptr here.
         * It is not exception safe. If reallocation occurs and fails, the raw pointer
         * passed to emplace_back would be leaked because the unique_ptr object is never fully
         *  constructed in the container's storage.
         */
        auto entity{ std::make_unique< Entity >( name ) };

        entities.push_back( std::move( entity ) );

        Entity* new_entity{ entities.back().get() };

        entityMap[name] = new_entity;

        return new_entity;
    }

    auto Engine::GetEntity( const std::string& name ) const noexcept -> Entity* {
        const std::shared_lock< std::shared_mutex > lock{ entitiesMutex };

        auto iter{ entityMap.find( name ) };

        if ( iter != entityMap.end() ) { return iter->second; }

        return nullptr;
    }

    auto Engine::RemoveEntity( Entity* entity ) noexcept -> bool {
        if ( equal( entity, nullptr ) ) { return false; }

        // If called from a background thread, defer removal to avoid deleting entities
        // while the render thread may be iterating a snapshot.
        if ( !IsMainThread() ) {
            const std::scoped_lock< std::mutex > lock{ pendingEntityRemovalsMutex };

            pendingEntityRemovalNames.emplace_back( entity->GetName() );

            return true;
        }

        const std::unique_lock< std::shared_mutex > lock{ entitiesMutex };

        const std::string name{ entity->GetName() };

        const auto iter = ranges::find_if(
            entities, [entity]( const std::unique_ptr< Entity >& ent ) { return equal( ent.get(), entity ); } );

        if ( not_equal( iter, entities.end() ) ) {
            entities.erase( iter );

            const auto remaining_iter = ranges::find_if(
                entities, [&name]( const std::unique_ptr< Entity >& ent ) { return equal( ent->GetName(), name ); } );

            if ( not_equal( remaining_iter, entities.end() ) ) {
                entityMap[name] = remaining_iter->get();
            } else {
                entityMap.erase( name );
            }

            return true;
        }

        return false;
    }

    auto Engine::RemoveEntity( const std::string& name ) noexcept -> bool {
        // If called from a background thread, defer removal to avoid deleting entities
        // while the render thread may be iterating a snapshot.
        if ( !IsMainThread() ) {
            const std::scoped_lock< std::mutex > lock{ pendingEntityRemovalsMutex };

            pendingEntityRemovalNames.push_back( name );

            return true;
        }

        const std::unique_lock< std::shared_mutex > lock{ entitiesMutex };

        const auto iter = entityMap.find( name );

        if ( iter == entityMap.end() ) { return false; }

        const Entity* entity = iter->second;

        if ( equal( entity, nullptr ) ) { return false; }

        auto vec_iter = ranges::find_if(
            entities, [entity]( const std::unique_ptr< Entity >& ent ) { return equal( ent.get(), entity ); } );

        if ( equal( vec_iter, entities.end() ) ) {
            entityMap.erase( name );
            return false;
        }

        entities.erase( vec_iter );

        const auto remaining_iter = ranges::find_if( entities, [&name]( const std::unique_ptr< Entity >& ent ) {
            return ent and equal( ent->GetName(), name );
        } );

        if ( not_equal( remaining_iter, entities.end() ) ) {
            entityMap[name] = remaining_iter->get();
        } else {
            entityMap.erase( name );
        }

        return true;
    }

    auto Engine::Update( TimeDelta deltaTime ) noexcept -> void {
        // Apply any entity removals requested by background threads.
        ProcessPendingEntityRemovals();

        // During background scene loading we avoid touching the live entity
        // list from the main thread. This lets the loading thread construct
        // entities/components safely while the main thread only drives the
        // UI/loading overlay.

        if ( renderer and renderer->IsLoading() ) {
            if ( imguiSystem ) { imguiSystem->NewFrame(); }
            return;
        }

        // if ( not_equal( activeCamera, nullptr ) ) {
        //     // NOLINTNEXTLINE(misc-include-cleaner)
        //     const glm::vec3 currentCameraPosition{ activeCamera->GetPosition() };
        //     physicsSystem->SetCameraPosition( currentCameraPosition );
        // }

        physicsSystem->Update( deltaTime );

        if ( imguiSystem ) { imguiSystem->NewFrame(); }

        if ( not_equal( activeCamera, nullptr ) ) { UpdateCameraControls( deltaTime ); }

        // Do not hold `entitiesMutex` while calling `Entity::Update()`.
        // Background threads may need the unique lock to add entities during loading,
        // and holding a shared lock for a long time can starve them.
        std::vector< Entity* > snapshot{};
        {
            const std::shared_lock< std::shared_mutex > lock{ entitiesMutex };
            snapshot.reserve( entities.size() );
            for ( auto& uptr : entities ) {
                snapshot.push_back( uptr.get() );
            }
        }

        for ( Entity* entity : snapshot ) {
            if ( ( equal( entity, nullptr ) ) or !entity->IsActive() ) { continue; }

            entity->Update( deltaTime );
        }
    }

    auto Engine::Render() noexcept -> void {
        if ( !renderer or !renderer->IsInitialized() ) { return; }

        ProcessPendingEntityRemovals();

        // Snapshot entity pointers under a short shared lock, then release the lock
        // before rendering. This prevents starving the background loader/physics threads
        // that need the unique lock to create entities/components.
        std::vector< Entity* > snapshot{};
        {
            const std::shared_lock< std::shared_mutex > lock{ entitiesMutex };
            snapshot.reserve( entities.size() );
            for ( auto& uptr : entities ) {
                snapshot.push_back( uptr.get() );
            }
        }

        renderer->Render( snapshot, activeCamera, imguiSystem.get(), resourceManager.get() );
    }

    auto Engine::CalculateDeltaTimeMs() noexcept -> chrono::milliseconds {
        const std::uint64_t current_time{ static_cast< uint64_t >(
            chrono::duration_cast< chrono::milliseconds >( chrono::steady_clock::now().time_since_epoch() ).count() ) };

        if ( equal( lastFrameTimeMs, 0 ) ) {
            lastFrameTimeMs = current_time;

            constexpr static int Guess{ 16 }; // ~16ms as a sane initial guess

            return chrono::milliseconds( Guess );
        }

        const std::uint64_t delta{ current_time - lastFrameTimeMs };

        lastFrameTimeMs = current_time;

        return chrono::milliseconds( static_cast< long long >( delta ) );
    }

    auto Engine::handleMouseInput( const XAxis mouseX, const YAxis mouseY, std::uint32_t buttons ) noexcept -> void {
        const bool imguiWantsMouse{ imguiSystem and imguiSystem->WantCaptureMouse() };

        if ( !imguiWantsMouse ) {
            auto xpos = static_cast< float >( mouseX() );
            auto ypos = static_cast< float >( mouseY() );

            if ( cameraControl.firstMouse ) {
                cameraControl.lastMouseX = static_cast< float >( mouseX() );
                cameraControl.lastMouseY = static_cast< float >( mouseY() );
                cameraControl.firstMouse = false;
            }

            float xoffset = xpos - cameraControl.lastMouseX;
            float yoffset = cameraControl.lastMouseY - ypos; // reversed since y-coordinates go from bottom to top

            cameraControl.lastMouseX = xpos;
            cameraControl.lastMouseY = ypos;

            cameraControl.yaw   += xoffset;
            cameraControl.pitch += yoffset;

            constexpr float PitchBound{ 89.0F };
            constexpr float NegPitchBound{ -89.0F };

            cameraControl.pitch = ranges::min( cameraControl.pitch, PitchBound );
            cameraControl.pitch = ranges::max( cameraControl.pitch, NegPitchBound );
        }

        if ( imguiSystem ) { imguiSystem->HandleMouse( mouseX, mouseY, buttons ); }

        HandleMouseHover( mouseX, mouseY );
    }

    auto Engine::HandleMouseHover( const XAxis mouseX, const YAxis mouseY ) noexcept -> void {
        currentMouseX = mouseX;
        currentMouseY = mouseY;
    }

    auto Engine::HandleResize( const Width width, const Height height ) const noexcept -> void {
        if ( height <= 0 or width <= 0 ) { return; }

        glViewport( 0, 0, width(), height() );

        if ( not_equal( activeCamera, nullptr ) ) {
            activeCamera->SetAspectRatio( static_cast< float >( width() ) / static_cast< float >( height() ) );
        }

        if ( renderer ) { renderer->SetFramebufferResized(); }

        if ( imguiSystem ) { imguiSystem->HandleResize( width, height ); }
    }

    auto Engine::handleKeyInput( const std::uint32_t key, const bool pressed ) noexcept -> void {
        if ( !pressed ) { return; }
        switch ( key ) {
            case GLFW_KEY_ESCAPE   : glfwSetWindowShouldClose( platform->GetWindow(), 1 ); break;

            case GLFW_KEY_W        :
            case GLFW_KEY_UP       : cameraControl.moveForward = pressed; break;

            case GLFW_KEY_S        :
            case GLFW_KEY_DOWN     : cameraControl.moveBackward = pressed; break;

            case GLFW_KEY_A        :
            case GLFW_KEY_LEFT     : cameraControl.moveLeft = pressed; break;

            case GLFW_KEY_D        :
            case GLFW_KEY_RIGHT    : cameraControl.moveRight = pressed; break;

            case GLFW_KEY_Q        :
            case GLFW_KEY_PAGE_UP  : cameraControl.moveUp = pressed; break;

            case GLFW_KEY_E        :
            case GLFW_KEY_PAGE_DOWN: cameraControl.moveDown = pressed; break;

            default                : break;
        }

        if ( imguiSystem ) { imguiSystem->HandleKeyboard( key, pressed ); }
    }

    auto Engine::UpdateCameraControls( const TimeDelta deltaTime ) noexcept -> void {
        if ( equal( activeCamera, nullptr ) ) { return; }

        auto* cameraTransform = activeCamera->GetOwner()->GetComponent< CameraComponent >();
        if ( equal( cameraTransform, nullptr ) ) { return; }

        constexpr float VelocityConst{ 0.001F };

        float velocity = cameraControl.cameraSpeed * static_cast< float >( deltaTime.count() ) * VelocityConst;

        // Apply movement based on input
        if ( cameraControl.moveForward ) {
            cameraTransform->UpdateMovement( CameraMovement::FORWARD, velocity );
            cameraControl.moveForward = false;
        }
        if ( cameraControl.moveBackward ) {
            cameraTransform->UpdateMovement( CameraMovement::BACKWARD, velocity );
            cameraControl.moveBackward = false;
        }
        if ( cameraControl.moveLeft ) {
            cameraTransform->UpdateMovement( CameraMovement::LEFT, velocity );
            cameraControl.moveLeft = false;
        }
        if ( cameraControl.moveRight ) {
            cameraTransform->UpdateMovement( CameraMovement::RIGHT, velocity );
            cameraControl.moveRight = false;
        }
        if ( cameraControl.moveUp ) {
            cameraTransform->UpdateMovement( CameraMovement::UP, velocity );
            cameraControl.moveUp = false;
        }
        if ( cameraControl.moveDown ) {
            cameraTransform->UpdateMovement( CameraMovement::DOWN, velocity );
            cameraControl.moveDown = false;
        }

        cameraTransform->SetPitch( cameraControl.pitch );

        cameraTransform->SetYaw( cameraControl.yaw );

        cameraTransform->UpdateCameraVectors();
    }

} // namespace PeanutGL
