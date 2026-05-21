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

#pragma once

#include "CameraComponent.hpp"
#include "Entity.hpp"
#include "ImGuiSystem.hpp"
#include "PhysicsSystem.hpp"
#include "Platform.hpp"
#include "Renderer.hpp"
#include "ResourceBase.hpp"
#include "ResourceManager.hpp"
#include "Utilities.hpp"

#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>

namespace PeanutGL {
    namespace chrono = std::chrono;

    class Engine {
      public:
        using TimeDelta = chrono::milliseconds;

        /**
         * @brief Default constructor.
         */
        Engine() noexcept = default;

        /**
         * @brief The Copy/Move assignment operators and constructors are deleted for shared_mutex.
         */
        Engine( const Engine& )                = delete;
        Engine( Engine&& ) noexcept            = delete;
        Engine& operator=( const Engine& )     = delete;
        Engine& operator=( Engine&& ) noexcept = delete;

        /**
         * @brief Destructor for proper cleanup.
         */
        ~Engine() {
            Cleanup();
        };

        /**
         * @brief Initialize the engine.
         * @param appName The name of the application.
         * @param width The width of the window.
         * @param height The height of the window.
         * @param enableValidationLayers Whether to enable Vulkan validation layers.
         * @return True if initialization was successful, false otherwise.
         */
        auto Initialize() -> bool;

        auto Run() -> void;

        auto Cleanup() noexcept -> void;

        /**
         * @brief Unload a resource.
         * @tparam T The type of resource.
         * @param identifier The resource ID.
         * @return True if the resource was unloaded, false otherwise.
         */
        template < typename T >
            requires IsResourceBase< T >
        auto UnloadResource( const std::string& identifier ) noexcept -> bool {
            return resourceManager->UnloadResource< T >( identifier );
        }

        /**
         * @brief Check if a resource exists.
         * @tparam T The type of resource.
         * @param identifier The resource ID.
         * @return True if the resource exists, false otherwise.
         */
        template < typename T >
            requires IsResourceBase< T >
        auto HasResource( const std::string& identifier ) noexcept -> bool {
            return resourceManager->HasResource< T >( identifier );
        }

        /**
         * @brief Get a resource.
         * @tparam T The type of resource.
         * @param identifier The resource ID.
         * @return A pointer to the resource, or nullptr if not found.
         */
        template < typename T >
            requires IsResourceBase< T >
        auto GetResource( const std::string& identifier ) -> T* {
            return resourceManager->GetResource< T >( identifier );
        }

        /**
         * @brief Load a resource.
         * @tparam T The type of resource.
         * @tparam Args The types of arguments to pass to the resource constructor.
         * @param identifier The resource ID.
         * @param args The arguments to pass to the resource constructor.
         * @return A handle to the resource.
         */
        template < typename T, typename... Args >
            requires IsResourceBase< T >
        auto LoadResource( const std::string& identifier, Args&&... args ) -> ResourceHandle< T > {
            return resourceManager->LoadResource< T >( identifier, std::forward< Args >( args )... );
        }

        /**
         * @brief Create a new entity.
         * @param name The name of the entity.
         * @return A pointer to the newly created entity.
         */
        auto CreateEntity( const std::string& name ) noexcept -> Entity*;

        /**
         * @brief Get an entity.
         * @param name The name of the entity to get.
         * @return Pointer to the requested entity if it exists, nullpointer otherwise.
         */
        auto GetEntity( const std::string& name ) const noexcept -> Entity*;

        /**
         * @brief Remove an entity.
         * @param entity The entity to remove.
         * @return True if the entity was removed, false otherwise.
         */
        auto RemoveEntity( Entity* entity ) noexcept -> bool;

        /**
         * @brief Remove an entity by name.
         * @param name The name of the entity to remove.
         * @return True if the entity was removed, false otherwise.
         */
        auto RemoveEntity( const std::string& name ) noexcept -> bool;

        /**
         * @brief Get the active camera.
         * @return A raw pointer to the active camera component, or nullptr if none is set.
         */
        template < typename Self > constexpr auto GetActiveCamera( this Self&& self ) noexcept -> CameraComponent* {
            return std::forward< Self >( self ).activeCamera;
        }

        /**
         * @brief Get the resource manager.
         * @return A raw pointer to the resource manager.
         */
        template < typename Self > constexpr auto GetResourceManager( this Self&& self ) noexcept -> ResourceManager* {
            return std::forward< Self >( self ).resourceManager.get();
        }

        /**
         * @brief Get the platform.
         * @return A raw pointer to the platform.
         */
        template < typename Self > constexpr auto GetPlatform( this Self&& self ) noexcept -> Platform* {
            return std::forward< Self >( self ).platform.get();
        }

        /**
         * @brief Get the renderer.
         * @return A raw pointer to the renderer.
         */
        template < typename Self > constexpr auto GetRenderer( this Self&& self ) noexcept -> Renderer* {
            return std::forward< Self >( self ).renderer.get();
        }

        /**
         * @brief Get the ImGui system.
         * @return A raw pointer to the ImGui system.
         */
        template < typename Self > constexpr auto GetImGuiSystem( this Self&& self ) noexcept -> ImGuiSystem* {
            return std::forward< Self >( self ).imguiSystem.get();
        }

      private:
        // Subsystems
        std::unique_ptr< Platform > platform;
        std::unique_ptr< PhysicsSystem > physicsSystem;
        std::unique_ptr< Renderer > renderer;
        std::unique_ptr< ResourceManager > resourceManager;
        std::unique_ptr< ImGuiSystem > imguiSystem;

        // Entities
        // NOTE: Entities can be created from a background loading thread.
        // Protect the containers to avoid iterator invalidation/data races while the render thread iterates them.
        mutable std::shared_mutex entitiesMutex;
        std::vector< std::unique_ptr< Entity > > entities;
        std::unordered_map< std::string, Entity* > entityMap;

        // Main thread identity (used to defer destructive operations from background threads)
        std::thread::id mainThreadId;

        // Background threads may request entity removal while the render thread is iterating snapshots.
        // To keep `Entity*` snapshots safe, defer removals to the main thread at a safe point.
        std::mutex pendingEntityRemovalsMutex;
        std::vector< std::string > pendingEntityRemovalNames;
        auto ProcessPendingEntityRemovals() noexcept -> void;

        auto constexpr IsMainThread() const -> bool;

        CameraComponent* activeCamera{ nullptr };

        bool initialized{ false };
        bool running{ false };

        // Delta time calculation
        // deltaTimeMs: time since last frame in milliseconds (for clarity)
        chrono::milliseconds deltaTimeMs{ 0 };
        std::uint64_t lastFrameTimeMs{ 0 };

        // Frame counter and FPS calculation
        std::uint64_t frameCount{ 0 };
        double fpsUpdateTimer{ 0.0 };
        double currentFPS{ 0.0 };

        std::uint64_t lastFPSUpdateFrame{ 0 };

        // Mouse position tracking
        XAxis currentMouseX{ 0.0 };
        YAxis currentMouseY{ 0.0 };

        /**
         * @brief Update window title with FPS and frame time.
         */
        constexpr auto UpdateWindow() noexcept -> void;

        /**
         * @brief Update frame counter and FPS.
         */
        constexpr auto UpdateFrameCounter() noexcept -> void;

        /**
         * @brief Update the engine state.
         * @param deltaTime The time elapsed since the last update.
         */
        // Accepts a time delta in milliseconds for clarity
        auto Update( TimeDelta deltaTime ) noexcept -> void;

        /**
         * @brief Render the scene.
         */
        auto Render() noexcept -> void;

        /**
         * @brief Calculate the time delta between frames.
         * @return The delta time in milliseconds (steady_clock based).
         */
        auto CalculateDeltaTimeMs() noexcept -> std::chrono::milliseconds;

        /**
         * @brief Handle window resize events.
         * @param width The new width of the window.
         * @param height The new height of the window.
         */
        auto HandleResize( const Width width, const Height height ) const noexcept -> void;

        /**
         * @brief Handle mouse hover to track current mouse position.
         * @param mouseX The x-coordinate of the mouse position.
         * @param mouseY The y-coordinate of the mouse position.
         */
        auto HandleMouseHover( const XAxis mouseX, const YAxis mouseY ) noexcept -> void;

        /**
         * @brief Handles mouse input for interaction and camera control.
         *
         * This method processes mouse input for various functionalities, including interacting with the scene,
         * camera rotation, and delegating handling to ImGui or hover systems.
         *
         * @param mouseX The x-coordinate of the mouse position.
         * @param mouseY The y-coordinate of the mouse position.
         * @param buttons A bitmask representing the state of mouse buttons.
         *                Bit 0 corresponds to the left button, and Bit 1 corresponds to the right button.
         */
        auto handleMouseInput( const XAxis mouseX, const YAxis mouseY, const std::uint32_t buttons ) noexcept -> void;

        /**
         * @brief Handles keyboard input events for controlling the camera and other subsystems.
         *
         * This method processes key press and release events to update the camera's movement state.
         * It also forwards the input to other subsystems like the ImGui interface if applicable.
         *
         * @param key The key code of the keyboard input.
         * @param pressed Indicates whether the key is pressed (true) or released (false).
         */
        auto handleKeyInput( const std::uint32_t key, const bool pressed ) noexcept -> void;

        /**
         * @brief Update camera controls based on input state.
         * @param deltaTime The time elapsed since the last update.
         */
        auto UpdateCameraControls( const TimeDelta deltaTime ) noexcept -> void;
    };

    void EngineSetup( Engine* const engine );
} // namespace PeanutGL
