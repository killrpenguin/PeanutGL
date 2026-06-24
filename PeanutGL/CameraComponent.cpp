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
#include "Entity.hpp"
#include "ProjectionComponent.hpp"
#include "Utilities.hpp"
#include "ViewComponent.hpp"

#include <glm/ext/matrix_transform.hpp>

namespace PeanutGL {

    auto CameraComponent::Initialize() noexcept -> void {
        SetState();
    }

    auto CameraComponent::Update( const std::chrono::milliseconds /* deltaTime */ ) -> void {
        const float frontX{ glm::cos( glm::radians( yaw() ) ) * glm::cos( glm::radians( pitch() ) ) };
        const float frontY{ glm::sin( glm::radians( pitch() ) ) };
        const float frontZ{ glm::sin( glm::radians( yaw() ) ) * glm::cos( glm::radians( pitch() ) ) };

        front = glm::normalize( glm::vec3( frontX, frontY, frontZ ) );

        right = glm::normalize( glm::cross( front, world_up ) );

        up = glm::normalize( glm::cross( right, front ) );

        Entity* const parent{ GetOwner() };

        if ( not_equal( parent, nullptr ) ) {
            if ( auto* const view = parent->GetComponent< ViewComponent >(); not_equal( view, nullptr ) ) {
                view->SetView( GetViewMatrix() );
            }

            if ( auto* const projection = parent->GetComponent< ProjectionComponent >();
                 not_equal( projection, nullptr ) ) {
                projection->SetDegrees( GetFieldofView() );
            }
        }
    }

    auto CameraComponent::Render() const noexcept -> void {
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
