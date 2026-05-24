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
#include "Utilities.hpp"

#include <chrono>
#include <cstddef>
#include <functional>

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

    auto Entity::Render() -> void {
        if ( !active ) { return; }

        for ( auto& component : components ) {
            if ( component->IsActive() ) { component->Render(); }
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

} // namespace PeanutGL
