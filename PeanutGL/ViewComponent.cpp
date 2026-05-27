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

#include "ViewComponent.hpp"

namespace PeanutGL {
    ViewComponent::ViewComponent(
        const ResourceHandle< ShaderProgram >& handle, const float zAxis, const std::string& componentName )
        : Component( componentName ), shader_program{ handle }, zAxis{ zAxis } {
        Initialize();
    }

    auto ViewComponent::Initialize() noexcept -> void {
        SetState();
    }

    auto ViewComponent::Update( [[maybe_unused]] const std::chrono::milliseconds deltaTime ) -> void {
    }

    auto ViewComponent::Render() const noexcept -> void {
        auto view = glm::mat4( 1.0F );

        view = glm::translate( view, glm::vec3( 0.0F, 0.0F, zAxis ) );

        shader_program->SetUniform( GetName(), view );
    }

} // namespace PeanutGL
