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
#include "ResourceManager.hpp"
#include "ShaderProgram.hpp"
#include "Utilities.hpp"

#include <chrono>
#include <memory>
#include <unordered_map>
#include <vector>

namespace PeanutGL {

    namespace chrono = std::chrono;

    template < typename T >
    concept DerivedComponentBase = std::is_base_of_v< Component, T >;

    template < typename... Ts >
    concept DerivedComponentsBase = ( DerivedComponentBase< Ts > && ... );

    class Entity {
      private:
        std::string name{};
        bool active{ true };

        std::vector< std::unique_ptr< Component > > components;
        std::unordered_map< std::size_t, Component* > component_map;

      public:
        // Deleted to eliminate the chances of object slicing.
        Entity( const Entity& )            = delete;
        Entity( Entity&& ) noexcept        = delete;
        Entity& operator=( const Entity& ) = delete;
        Entity& operator=( Entity&& )      = delete;

        /**
         * @brief Constructor with optional name.
         * @param componentName The name of the component.
         */
        constexpr explicit Entity( std::string component_name = "Component" ) noexcept
            : name( std::move( component_name ) ) {
        }

        /**
         * @brief Virtual destructor for proper cleanup.
         */
        virtual ~Entity() = default;

        /**
         * @brief Get the name of the entity.
         * @return The name of the entity.
         */
        constexpr auto GetName() noexcept -> std::string& {
            return name;
        }

        /**
         * @brief Check if the entity is active.
         * @return True if the entity is active, false otherwise.
         */
        constexpr auto IsActive() const noexcept -> bool {
            return active;
        }

        /**
         * @brief Set the active state of the entity.
         * @param isActive The new active state.
         */
        constexpr auto SetActive( const bool isActive ) noexcept -> void {
            active = isActive;
        }

        /**
         * @brief Initialize all components of the entity.
         */
        auto Initialize() -> void;

        /**
         * @brief Update all components of the entity.
         * @param deltaTime The time elapsed since the last frame.
         */
        auto Update( chrono::milliseconds deltaTime ) -> void;

        /**
         * @brief Render all components of the entity.
         */
        auto Render( ResourceManager* const resourceManager ) -> void;

        /**
         * @brief Add a component to the entity.
         * @tparam T The type of component to add.
         * @tparam Args The types of arguments to pass to the component constructor.
         * @param args The arguments to pass to the component constructor.
         * @return A pointer to the newly created component.
         */
        template < typename T, typename... Args > constexpr auto AddComponent( Args&&... args ) noexcept -> T* {
            static_assert( std::is_base_of_v< Component, T >, "T must derive from Component" );

            std::size_t typeID = Component::GetTypeID< T >();

            auto iter = component_map.find( typeID );
            if ( not_equal( iter, component_map.end() ) ) { return static_cast< T* >( iter->second ); }

            auto component{ std::make_unique< T >( std::forward< Args >( args )... ) };
            T* componentPtr{ component.get() };

            component_map[typeID] = componentPtr;

            components.push_back( std::move( component ) );

            return componentPtr;
        }

        /**
         * @brief Get a component of a specific type.
         * @tparam T The type of component to get that derives from Component.
         * @return A pointer to the component, or nullptr if not found.
         */
        template < typename T >
            requires DerivedComponentBase< T >
        auto GetComponent() const noexcept -> T* {
            std::size_t typeID = Component::GetTypeID< T >();
            auto iter{ component_map.find( typeID ) };

            if ( not_equal( iter, component_map.end() ) ) {
                return static_cast< T* >( iter->second );
            } else {
                return nullptr;
            }
        }

        /**
         * @brief Remove a component of a specific type.
         * @tparam T The type of component to remove that derives from Component.
         * @return True if the component was removed, false otherwise.
         */
        template < typename T >
            requires DerivedComponentBase< T >
        auto RemoveComponent() noexcept -> bool;

        /**
         * @brief This was for practice and testing.
         * @brief Check if the entity has a component of a specific type.
         * @tparam T The type of component to check for.
         * @return True if the entity has the component, false otherwise.
         */
        template < typename T >
            requires DerivedComponentBase< T >
        auto HasComponent() const noexcept -> bool;

        template < typename T >
            requires DerivedComponentBase< T >
        auto DownCastComponent( const Component* ptr ) noexcept -> const T*;
    };

} // namespace PeanutGL
