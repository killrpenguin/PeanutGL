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

#include "Renderer.hpp"
#include "CameraComponent.hpp"
#include "Entity.hpp"

#include "ImGuiSystem.hpp"
#include "Utilities.hpp"

#include "glad/gl.h"

#include <cassert>
#include <span>

namespace PeanutGL {

    auto Renderer::Initialize() -> bool {
        initialized = true;

        return initialized;
    }

    auto Renderer::Render(
        std::span< Entity* > entities, CameraComponent* camera, ImGuiSystem* imguiSystem ) const noexcept -> void {
        if ( not_equal( camera, nullptr ) ) { /*placeholder*/
        }

        constexpr float Alpha{ 1.0F };

        glClearColor( 0.0F, 0.0F, 0.0F, Alpha );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        for ( const auto& entity : entities ) {
            entity->Render();
        }

        // glDrawElements( GL_TRIANGLES, Count, GL_UNSIGNED_INT, nullptr );

        if ( not_equal( imguiSystem, nullptr ) ) { imguiSystem->Render(); }

        glfwSwapBuffers( platform->GetWindow() );
    }

} // namespace PeanutGL
