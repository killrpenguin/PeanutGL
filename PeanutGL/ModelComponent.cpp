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

#include "ModelComponent.hpp"
#include "Component.hpp"
#include "DebugSystem.hpp"

#include <cassert>
#include <quill/LogMacros.h>

namespace PeanutGL {

    ModelComponent::ModelComponent( const std::string& componentName )
        : Component( componentName ) {
        Initialize();
    }

    auto ModelComponent::Initialize() noexcept -> void {
        SetState();
    }

    auto ModelComponent::MatrixData() const noexcept -> glm::mat4 {
        return model;
    }

    auto ModelComponent::Update( const std::chrono::milliseconds /*deltaTime*/ ) -> void {
        auto ident_matrix{ glm::mat4( 1.0F ) };

        model = glm::translate( ident_matrix, position );

        // model = glm::rotate( ident_matrix, glm::radians( angle ), rotation_axis );
    }

    auto ModelComponent::Render() const noexcept -> void {
        constexpr GLsizei Count{ 36 };
        glDrawArrays( GL_TRIANGLES, 0, Count );
    }

    auto ModelsArray::Initialize() noexcept -> void {
        for ( auto& model : models ) {
            model.SetState();
        }
        SetState();
    }

    auto ModelsArray::Update( const std::chrono::milliseconds deltaTime ) -> void {
        for ( auto& model : models ) {
            model.Update( deltaTime );
        }
    }

    auto ModelsArray::Render() const noexcept -> void {
    }

    auto ModelsArray::Size() const noexcept -> size_type {
        return models.size();
    }

    auto ModelsArray::Hello() const noexcept -> void {
        LOG_INFO( QuillPtr(), "Hello from my test. My size is {}", Size() );
    }
} // namespace PeanutGL
