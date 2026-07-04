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

#include "Entity.hpp"
#include "Component.hpp"
#include "DebugSystem.hpp"
#include "ModelComponent.hpp"
#include "ShaderProgram.hpp"
#include "UniformBufferResource.hpp"
#include "Utilities.hpp"

#include <quill/LogMacros.h>

#include <chrono>
#include <cstddef>
#include <functional>
#include <ranges>
#include <vector>

namespace PeanutGL {
    namespace chrono = std::chrono;

    auto Entity::Initialize() -> void {
        for ( auto& component : components ) {
            component->Initialize();
            component->SetOwner( this );
        }
    }

    auto Entity::Update( chrono::milliseconds deltaTime ) -> void {
        if ( !active ) { return; }

        for ( auto& component : components ) {
            if ( component->IsActive() ) { component->Update( deltaTime ); }
        }
    }

    auto Entity::Render( ResourceManager* const resourceManager ) -> void {
        constexpr auto Transform = std::views::transform;

        if ( !active ) { return; }

        const ShaderProgram* const shader_program{ resourceManager->GetResource< ShaderProgram >( GetName() ) };

        if ( equal( shader_program, nullptr ) ) {
            LOG_ERROR( QuillPtr(), "The resource manager returned a nullptr to a shader program." );
            return;
        }

        shader_program->Use();

        const auto component_ptrs =
            components | Transform( []( std::unique_ptr< Component >& cmp ) { return cmp.get(); } );

        std::vector< const ModelComponent* > models_array_sub_range;

        if ( auto* const model_component = GetComponent< ModelsArray >(); not_equal( model_component, nullptr ) ) {
            const auto model_ptrs =
                *model_component | Transform( []( const ModelComponent& model ) { return &model; } );

            models_array_sub_range = std::ranges::to< std::vector< const ModelComponent* > >( model_ptrs );
        }

        const auto all_components = std::views::concat( component_ptrs, models_array_sub_range );

        for ( const auto* component : all_components ) {
            if ( !component->IsActive() ) { continue; }

            if ( component->NeedsUniform() ) {
                shader_program->SetUniform( component->GetName(), component->MatrixData() );
            }
            component->Render();
        }
    }

    template < typename T >
        requires DerivedComponentBase< T >
    auto Entity::RemoveComponent() noexcept -> bool {
        const std::size_t type_id{ Component::GetTypeID< T >() };

        auto iter{ component_map.find( type_id ) };

        if ( not_equal( iter, component_map.end() ) ) {
            Component* component_ptr{ iter->second };
            component_map.erase( iter );

            for ( auto compIt = components.begin(); compIt != components.end(); ++compIt ) {
                if ( equal( compIt->get(), component_ptr ) ) {
                    components.erase( compIt );
                    return true;
                }
            }
        }
        return false;
    }

    template < typename T >
        requires DerivedComponentBase< T >
    auto Entity::HasComponent() const noexcept -> bool {
        const std::size_t type_id{ Component::GetTypeID< T >() };

        auto iter{ component_map.find( type_id ) };
        return not_equal( iter, component_map.end() );
    }

    template < typename T >
        requires DerivedComponentBase< T >
    auto Entity::DownCastComponent( const Component* ptr ) noexcept -> const T* {
        const std::size_t type_id{ Component::GetTypeID< T >() };

        if ( auto iter = component_map.find( type_id ); not_equal( iter, component_map.end() ) ) {
            const T* derived_type = dynamic_cast< const T* >( ptr );
            if ( derived_type == iter->second ) { return derived_type; }
        }
        return nullptr;
    }

} // namespace PeanutGL
