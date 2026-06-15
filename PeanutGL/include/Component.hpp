/*
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

#include "ShaderProgram.hpp"
#include "Utilities.hpp"

#include <cstddef>
#include <string>
namespace PeanutGL {
    // Forward declaration
    class Entity;

    class ComponentTypeIDSystem {
      private:
        static std::size_t next_type_id;

      public:
        template < typename T > static std::size_t GetTypeID() {
            static std::size_t type_id = next_type_id++;
            return type_id;
        }
    };

    /**
     * @brief Base class for all components in the engine.
     *
     * Components are the building blocks of the entity-component system.
     * Each component encapsulates a specific behavior or property.
     */
    class Component {
      private:
        Entity* owner{ nullptr };
        std::string name;
        State state{ State::Uninitialized };

      public:
        // Deleted to eliminate the chances of object slicing.
        Component( const Component& )            = delete;
        Component( Component&& )                 = delete;
        Component& operator=( const Component& ) = delete;
        Component& operator=( Component&& )      = delete;

        /**
         * @brief Constructor with optional name.
         * @param componentName The name of the component.
         */
        explicit Component( std::string component_name = "Component" )
            : name( std::move( component_name ) ) {
        }

        /**
         * @brief Virtual destructor for proper cleanup.
         */
        virtual ~Component() = default;

        /**
         * @brief Initialize the component.
         * Called when the component is added to an entity.
         */
        virtual auto Initialize() noexcept -> void = 0;
        // template < typename Self > auto Initialize( this Self&& self ) noexcept -> void {
        //     std::forward< Self >( self ).Initialize();
        // }

        /**
         * @brief Update the component.
         * Called every frame.
         * @param deltaTime The time elapsed since the last frame.
         */
        virtual auto Update( const std::chrono::milliseconds /*deltaTime*/ ) -> void = 0;

        /**
         * @brief Render the component.
         * Called during the rendering phase.
         */
        virtual auto Render() const noexcept -> void = 0;

        /**
         * @brief Set the owner entity of this component.
         * @param entity The entity that owns this component.
         */
        constexpr auto SetOwner( Entity* entity ) noexcept -> void {
            owner = entity;
        }

        /**
         * @brief Get the owner entity of this component.
         * @return The entity that owns this component.
         */
        constexpr auto GetOwner() const noexcept -> Entity* {
            return owner;
        }

        /**
         * @brief Get the name of the component.
         * @return The name of the component.
         */
        constexpr auto GetName() const noexcept -> const std::string& {
            return name;
        }

        /**
         * @brief Check if the component is active.
         * @return True if the component is active, false otherwise.
         */
        constexpr auto IsActive() const noexcept -> bool {
            return state == State::Active;
        }

        /**
         * @brief Activate the component..
         */
        constexpr auto SetState( State new_state = State::Active ) noexcept -> void {
            switch ( new_state ) {
                case State::Active:
                case State::InActive: {
                    state = State::Active;
                    break;
                }
                case State::Initialized:
                case State::Uninitialized:
                case State::Destroyed    : return;
            };
        }

        /**
         * @brief Get the id of a component.
         * @return
         */
        template < typename T > constexpr auto static GetTypeID() noexcept -> std::size_t {
            return ComponentTypeIDSystem::GetTypeID< T >();
        }
    };
} // namespace PeanutGL
