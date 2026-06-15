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
#include "DebugSystem.hpp"
#include "Entity.hpp"

#include "ImGuiSystem.hpp"
#include "ModelComponent.hpp"
#include "ProjectionComponent.hpp"
#include "ResourceManager.hpp"
#include "ShaderProgram.hpp"
#include "Utilities.hpp"

#include "ViewComponent.hpp"
#include "glad/gl.h"

#include <cassert>
#include <quill/LogMacros.h>
#include <span>

namespace PeanutGL {

    auto Renderer::Initialize() -> bool {
        initialized = true;

        return initialized;
    }

    auto Renderer::Render(
        std::span< Entity* > entities, CameraComponent* camera, ImGuiSystem* imguiSystem,
        ResourceManager* resourceManager ) const noexcept -> void {
        if ( equal( camera, nullptr ) ) {
            LOG_CRITICAL( QuillPtr(), "The camera pointer was null." );
            return;
        }

        if ( equal( resourceManager, nullptr ) ) {
            LOG_CRITICAL( QuillPtr(), "The resource manager pointer was null." );
            return;
        }

        constexpr float Alpha{ 1.0F };
        glClearColor( 0.0F, 0.0F, 0.0F, Alpha );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        for ( const auto& entity : entities ) {
            auto* shader_program{ resourceManager->GetResource< ShaderProgram >( entity->GetName() ) };

            if ( equal( shader_program, nullptr ) ) {
                LOG_ERROR( QuillPtr(), "The resource manager returned a nullptr to a shader program." );
                return;
            }

            if ( auto* const model = entity->GetComponent< ModelComponent >(); not_equal( model, nullptr ) ) {
                if ( const bool active = model->IsActive(); active ) {
                    shader_program->SetUniform( model->GetName(), model->MatrixData() );
                }
            }

            if ( auto* const view = entity->GetComponent< ViewComponent >(); not_equal( view, nullptr ) ) {
                if ( const bool active = view->IsActive(); active ) {
                    shader_program->SetUniform( view->GetName(), view->MatrixData() );
                }
            }

            if ( auto* const projection = entity->GetComponent< ProjectionComponent >();
                 not_equal( projection, nullptr ) ) {
                if ( const bool active = projection->IsActive(); active ) {
                    shader_program->SetUniform( projection->GetName(), projection->MatrixData() );
                }
            }

            entity->Render();
        }

        if ( not_equal( imguiSystem, nullptr ) ) { imguiSystem->Render(); }

        glfwSwapBuffers( platform->GetWindow() );
    }

} // namespace PeanutGL
