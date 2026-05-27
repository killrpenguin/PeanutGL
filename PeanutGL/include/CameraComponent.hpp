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
#include "Utilities.hpp"

#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

        std::chrono::milliseconds delta_time{};
        std::chrono::milliseconds last_frame{};

        auto UpdateCameraVectors() noexcept -> void {
            const float frontX{ glm::cos( glm::radians( yaw() ) ) * glm::cos( glm::radians( pitch() ) ) };
            const float frontY{ glm::sin( glm::radians( pitch() ) ) };
            const float frontZ{ glm::sin( glm::radians( yaw() ) ) * glm::cos( glm::radians( pitch() ) ) };

            front = glm::normalize( glm::vec3( frontX, frontY, frontZ ) );

            right = glm::normalize( glm::cross( front, world_up ) );

            up = glm::normalize( glm::cross( right, front ) );
        }

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
        explicit CameraComponent( const std::string& componentName = "Camera" )
            : Component( componentName ) {
            Initialize();
            UpdateCameraVectors();
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
        auto Update( [[maybe_unused]] const std::chrono::milliseconds deltaTime ) -> void override {
            // const std::chrono::milliseconds current_frame{ GetTime() };
            // delta_time = current_frame - last_frame;
            // last_frame = current_frame;
        }

        /**
         * @brief Render the component.
         */
        auto Render() const noexcept -> void override;

        /**
         * @brief Set the aspect ratio for perspective projection.
         * @param ratio The aspect ratio (width / height).
         */
        auto SetAspectRatio( const float ratio ) noexcept -> void {
            aspect_ratio = ratio;
        }

        /**
         * @brief Get the view matrix calculated using Euler Angles.
         */
        auto GetViewMatrix() const noexcept -> glm::mat4 {
            return glm::lookAt( position, position + front, up );
        }

        /**
         * @brief Get the camera position.
         * @return The camera position.
         */
        auto GetPosition() const noexcept -> glm::vec3 {
            return position;
        }

        /**
         * @brief Get the camera front value.
         * @return The camera position.
         */
        auto GetFront() const noexcept -> glm::vec3 {
            return front;
        }

        /**
         * @brief Get the camera front value.
         * @return The camera position.
         */
        auto GetZoom() const noexcept -> float {
            return zoom();
        }
    };

} // namespace PeanutGL
