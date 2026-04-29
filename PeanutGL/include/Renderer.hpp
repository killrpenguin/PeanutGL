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
#include "ImGuiSystem.hpp"
#include "Platform.hpp"

#include <atomic>
#include <cstdint>
#include <glm/glm.hpp>
#include <span>

namespace PeanutGL {

    class Renderer {
      public:
        Renderer() noexcept = default;

        /**
         * @brief Constructor with a Resource Manager.
         * @param resource_mananger_ptr The platform to use for rendering.
         */
        explicit Renderer( Platform* platform_ptr )
            : platform{ platform_ptr } {};

        /**
         * @brief Copy and Move constructors and operators deleted.
         */
        Renderer( const Renderer& )            = delete;
        Renderer( Renderer&& )                 = delete;
        Renderer& operator=( const Renderer& ) = delete;
        Renderer& operator=( Renderer&& )      = delete;

        /**
         * @brief Destructor for proper cleanup.
         */
        ~Renderer() = default;

        /**
         * @brief Initialize the renderer.
         * @param resourceManager A pointer to the resource manager.
         * @return True if initialization was successful, false otherwise.
         */
        auto Initialize() -> bool;

        /**
         * @brief Clean up renderer resources.
         */
        auto Cleanup() -> void;

        /**
         * @brief Wait for the device to be idle.
         */
        auto WaitIdle() -> void;

        /**
         * @brief Check if the renderer is initialized.
         * @return True if the renderer is initialized, false otherwise.
         */
        constexpr auto IsInitialized() const noexcept -> bool {
            return initialized;
        }

        /**
         * @brief Set the framebuffer resized flag.
         * This should be called when the window is resized.
         */
        auto SetFramebufferResized() -> void {
            framebufferResized.store( true, std::memory_order_relaxed );
        }

        constexpr auto IsLoading() const -> bool {
            // Keep the blocking overlay visible until the engine has finished
            // post-load blockers (AS build, descriptor cold-init, etc.).
            return (
                loadingFlag.load( std::memory_order_relaxed ) or
                criticalJobsOutstanding.load( std::memory_order_relaxed ) > 0U or
                !initialLoadComplete.load( std::memory_order_relaxed ) );
        }

        /**
         * @brief Render the scene.
         * @param entities The entities to render.
         * @param camera The camera to use for rendering.
         * @param imguiSystem The ImGui system for UI rendering (optional).
         */
        auto Render( std::span< Entity* > entities, CameraComponent* camera, ImGuiSystem* imguiSystem ) const noexcept
            -> void;

      private:
        Platform* platform{ nullptr };

        bool initialized{ false };

        std::atomic< bool > loadingFlag{ false };
        std::atomic< std::uint32_t > criticalJobsOutstanding{ 0 };
        std::atomic< bool > initialLoadComplete{ true };

        // Framebuffer resized flag (atomic to handle platform callback vs. render thread)
        std::atomic< bool > framebufferResized{ false };

        //        Shader current_shader{};
    };
} // namespace PeanutGL
