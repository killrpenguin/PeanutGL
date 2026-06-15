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

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <quill/LogMacros.h>

namespace PeanutGL {

    /**
     * @brief View component.
     *
     * This class implements the component interface.
     */
    class ViewComponent final : public Component {
      private:
        float zAxis{};
        glm::mat4 view{ 1.0F };

      public:
        ViewComponent() = delete;
        /**
         * @brief Constructor with an optional name.
         * @param handle The handle to the shader program.
         * @param componentName The name of the component.
         */
        explicit ViewComponent( const std::string& componentName, const float zAxis = -3.0F );

        ViewComponent( const ViewComponent& )            = delete;
        ViewComponent( ViewComponent&& )                 = delete;
        ViewComponent& operator=( const ViewComponent& ) = delete;
        ViewComponent& operator=( ViewComponent&& )      = delete;

        ~ViewComponent() noexcept override = default;

        /**
         * @brief Initialize the component.
         */
        auto Initialize() noexcept -> void override;

        /**
         * @brief Update the component.
         * Called every frame.
         * @param deltaTime The time elapsed since the last frame.
         */
        auto Update( const std::chrono::milliseconds deltaTime ) -> void override;

        /**
         * @brief Render the component.
         */
        auto Render() const noexcept -> void override;

        /**
         * @brief Get the matrix data calculated during the last update call.
         */
        auto MatrixData() const noexcept -> glm::mat4;

        /**
         * @brief Set the matrix data for the next update call..
         */
        auto SetView( const glm::mat4 new_view ) noexcept -> void {
            view = new_view;
        }
    };

} // namespace PeanutGL
