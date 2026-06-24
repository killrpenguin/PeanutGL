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

#include "Component.hpp"
#include "ShaderProgram.hpp"

#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <quill/LogMacros.h>

namespace PeanutGL {
    enum class CameraMovement : std::uint8_t {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    /**
     * @brief A flying camera component.
     *
     * This class implements the component interface.
     */
    class CameraComponent final : public Component {
        constexpr static float DefaultAspectRatio{};

      private:
        glm::vec3 position{ 0.0F, 0.0F, 0.0F };
        glm::vec3 front{ 0.0F, 0.0F, -1.0F };
        glm::vec3 up{ 0.0F, 1.0F, 0.0F };
        glm::vec3 right{};
        glm::vec3 world_up{ 0.0F, 1.0F, 0.0F };

        // euler Angles
        Yaw yaw{};
        Pitch pitch{};

        // camera options
        MovementSpeed movement_speed{};
        MouseSensitivity mouse_sensitivity{};
        Zoom zoom{};

        // Perspective projection parameters
        FieldOfView field_of_view{};
        CameraAspectRatio aspect_ratio{};

        constexpr auto static GetTime() noexcept -> std::chrono::milliseconds {
            const std::chrono::duration< double > seconds{ glfwGetTime() };
            return std::chrono::duration_cast< std::chrono::milliseconds >( seconds );
        }

      public:
        CameraComponent() noexcept = default;

        /**
         * @brief Constructor with an optional name.
         * @param componentName The name of the component.
         */
        explicit CameraComponent( const std::string& componentName )
            : Component{ componentName } {
            Initialize();
        };

        CameraComponent( const CameraComponent& )            = delete;
        CameraComponent( CameraComponent&& )                 = delete;
        CameraComponent& operator=( const CameraComponent& ) = delete;
        CameraComponent& operator=( CameraComponent&& )      = delete;

        ~CameraComponent() noexcept override = default;

        /**
         * @brief Initialize the component.
         */
        auto Initialize() noexcept -> void override;

        /**
         * @brief Update the component.
         * Called every frame.
         * @param deltaTime The time elapsed since the last frame.
         */
        auto Update( [[maybe_unused]] const std::chrono::milliseconds deltaTime ) -> void override;

        /**
         * @brief Render the component.
         */
        auto Render() const noexcept -> void override;

        auto MatrixData() const noexcept -> glm::mat4 override {
            return {};
        }

        /**
         * @brief Set the aspect ratio for perspective projection.
         * @param ratio The aspect ratio (width / height).
         */
        auto SetAspectRatio( const float ratio ) noexcept -> void {
            aspect_ratio = ratio;
        }

        /**
         * @brief Set the aspect ratio for perspective projection.
         * @param ratio The aspect ratio (width / height).
         */
        auto SetYaw( const float val ) noexcept -> void {
            yaw = val;
        }

        auto SetPitch( const float val ) noexcept -> void {
            pitch = val;
        }

        /**
         * @brief Get the view matrix calculated using Euler Angles.
         */
        auto GetViewMatrix() const noexcept -> glm::mat4 {
            return glm::lookAt( position, position + front, up );
        }

        /**
         * @brief Get the field of view from the camera.
         */
        auto GetFieldofView() const noexcept -> float {
            return static_cast< float >( field_of_view );
        }

        /**
         * @brief Apply direction based movememnt to camera data.
         * @param direction The direction to move as a CameraMovement enum).
         * @param velocity The velocity of the movement.
         */
        auto UpdateMovement( const CameraMovement direction, const float velocity ) noexcept -> void;
    };

} // namespace PeanutGL
