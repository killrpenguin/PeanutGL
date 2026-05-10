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

#include "MeshComponent.hpp"
#include "Entity.hpp"
#include "TransformComponent.hpp"

#include <glad/gl.h>

namespace PeanutGL {
    auto MeshComponent::Initialize() noexcept -> void {
        glCreateVertexArrays( 1, &VAO );

        glVertexArrayVertexBuffer( VAO, 0, vbo->Name(), 0, 6 * sizeof( float ) );

        glVertexArrayElementBuffer( VAO, ebo->Name() );

        // VAO STUFF TO MOVE LATER.
        glEnableVertexArrayAttrib( VAO, 0 );

        glVertexArrayAttribFormat( VAO, 0, 3, GL_FLOAT, GL_FALSE, 0 );

        glVertexArrayAttribBinding( VAO, 0, 0 );

        glEnableVertexArrayAttrib( VAO, 1 );

        glVertexArrayAttribFormat( VAO, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ) );

        glVertexArrayAttribBinding( VAO, 1, 0 );

        glBindVertexArray( VAO );

        initialized = true;
    }

    auto MeshComponent::Update( [[maybe_unused]] std::chrono::milliseconds deltaTime ) -> void {
        Entity* owner{ GetOwner() };

        TransformComponent* transform{ owner->GetComponent< TransformComponent >() };

        if ( transform != nullptr ) {}
    }

    auto MeshComponent::Render() const noexcept -> void {
        glBindVertexArray( VAO );
    }

} // namespace PeanutGL
