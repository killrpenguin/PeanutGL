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

#include "CameraComponent.hpp"
#include <glm/ext/matrix_transform.hpp>

namespace PeanutGL {

    auto CameraComponent::Initialize() noexcept -> void {
        SetState();
    }

    auto CameraComponent::Update( [[maybe_unused]] const std::chrono::milliseconds deltaTime ) -> void {
    }

    auto CameraComponent::Render() const noexcept -> void {
        const glm::mat4 view_matrix = GetViewMatrix();

        constexpr float zAxis{ -3.0F };
        auto view = glm::translate( view_matrix, glm::vec3( 0.0F, 0.0F, zAxis ) );

        shader_program->SetUniform( GetName(), view );
    }
    auto CameraComponent::UpdateMovement( const CameraMovement direction, const float velocity ) noexcept -> void {
        switch ( direction ) {
            case CameraMovement::FORWARD : position += front * velocity; break;
            case CameraMovement::BACKWARD: position -= front * velocity; break;
            case CameraMovement::LEFT    : position -= right * velocity; break;
            case CameraMovement::RIGHT   : position += right * velocity; break;
            case CameraMovement::UP      : position += up * velocity; break;
            case CameraMovement::DOWN    : position -= up * velocity; break;
            default                      : break;
        }
    }

} // namespace PeanutGL
