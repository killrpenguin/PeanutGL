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

#include <concepts>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <quill/LogMacros.h>
#include <type_traits>

namespace PeanutGL {

    /**
     * @brief A component for transforming local space into world space.
     *
     */
    class ModelComponent final : public Component {
      private:
        float angle{ 0.0F };

        glm::vec3 position{ 0.0F, 0.0F, 0.0F };
        glm::vec3 rotation_axis{ 0.0F, 0.0F, 0.0F };
        glm::mat4 model{ 1.0F };

      public:
        ModelComponent() = delete;
        /**
         * @brief Constructor with a name.
         * @param componentName The name of the component.
         */
        explicit ModelComponent( const std::string& componentName );

        ModelComponent( const ModelComponent& )            = delete;
        ModelComponent( ModelComponent&& )                 = default;
        ModelComponent& operator=( const ModelComponent& ) = delete;
        ModelComponent& operator=( ModelComponent&& )      = default;

        ~ModelComponent() noexcept override = default;

        friend class ModelsArray;

        /**
         * @brief Initialize the component.
         */
        auto Initialize() noexcept -> void override;

        /**
         * @brief Update the component.
         * Called every frame.
         * @param deltaTime The time elapsed since the last frame.
         */
        auto Update( const std::chrono::milliseconds deltaTime ) -> void override;

        /**
         * @brief Render the component.
         */
        auto Render() const noexcept -> void override;

        /**
         * @brief Get the Model matrix data calculated during the last update call.
         */
        auto MatrixData() const noexcept -> glm::mat4 override;

        /**
         * @brief Set the angle.
         */
        constexpr auto SetAngle( const float new_angle ) noexcept -> void {
            angle = new_angle;
        }

        /**
         * @brief Set the rotation axis.
         */
        constexpr auto SetRotationAxis( const glm::vec3 rotation ) noexcept -> void {
            rotation_axis = rotation;
        }
        /**
         * @brief Set the Position used to calculate the matrix.
         */
        constexpr auto SetPosition( const glm::vec3 pos ) noexcept -> void {
            position = pos;
        }
    };

    class ModelsArray final : public Component {
      private:
        using Models    = std::vector< ModelComponent >;
        using size_type = Models::size_type;

        Models models;

      public:
        ModelsArray() = delete;

        /**
         * @brief Constructor with a name.
         * @param componentName The name of the component.
         */
        explicit ModelsArray( const std::string& componentName )
            : Component( componentName ) {
            Initialize();

            // NOLINTNEXTLINE
            models.reserve( 10 );
        }

        ~ModelsArray() noexcept override = default;

        ModelsArray( const ModelsArray& other )                = delete;
        ModelsArray( ModelsArray&& other ) noexcept            = default;
        ModelsArray& operator=( const ModelsArray& other )     = delete;
        ModelsArray& operator=( ModelsArray&& other ) noexcept = default;

        auto Initialize() noexcept -> void override;

        auto Update( const std::chrono::milliseconds deltaTime ) -> void override;

        auto Render() const noexcept -> void override;

        auto MatrixData() const noexcept -> glm::mat4 override {
            return {};
        }

        auto Size() const noexcept -> size_type;

        /**
         * @brief Add a new ModelComponent to the array.
         * @param args[0] ComponenetName as a const std::string&.
         * @param args[1] ComponentPosition as a glm::vec3.
           @param args[2] RotationAxis as a glm::vec3.
           @param args[3] Angle as a float.
         */
        // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
        template < typename... Args > auto AddModel( Args&&... args ) noexcept -> void {
            constexpr std::size_t TemplateArgsMax{ 4 };

            static_assert( sizeof...( Args ) > 0, "A component name is required for this function." );
            static_assert( sizeof...( Args ) <= TemplateArgsMax, "To many arguments passed to the AddModel function." );

            constexpr std::size_t NameIdx{ 0 };
            using CleanNameT = std::decay_t< Args...[NameIdx] >;

            static_assert(
                std::same_as< CleanNameT, const char* >,
                "The first parameter in AddModel must be the name of the ModelComponent." );

            ModelComponent& new_model{ models.emplace_back( std::forward< Args...[NameIdx] >( args...[NameIdx] ) ) };

            if constexpr ( sizeof...( Args ) == 2 ) {
                constexpr std::size_t PositionIdx{ 1 };
                using CleanPositionT = std::decay_t< Args...[PositionIdx] >;

                static_assert(
                    std::same_as< CleanPositionT, glm::vec3 >,
                    "The second parameter must be an optional glm::vec3 position for the ModelComponent." );

                new_model.SetPosition( std::forward< Args...[PositionIdx] >( args...[PositionIdx] ) );
            }
            if constexpr ( sizeof...( Args ) == 3 ) {
                constexpr std::size_t AxisIdx{ 2 };
                using CleanRotationAxisT = std::decay_t< Args...[AxisIdx] >;

                static_assert(
                    std::same_as< CleanRotationAxisT, glm::vec3 >,
                    "The third parameter must be an optional glm::vec3 for the rotation axis of the ModelComponent." );

                new_model.SetRotationAxis( std::forward< Args...[AxisIdx] >( args...[AxisIdx] ) );
            }
            if constexpr ( sizeof...( Args ) == 4 ) {
                constexpr std::size_t AngleIdx{ 3 };
                using CleanAngleT = std::decay_t< Args...[AngleIdx] >;

                static_assert(
                    std::same_as< CleanAngleT, float >,
                    "The fourth parameter must be an optional float for the angle of the ModelComponent." );

                new_model.SetAngle( std::forward< Args...[AngleIdx] >( args...[AngleIdx] ) );
            }
        }

        template < typename Self > auto begin( this Self&& self ) {
            return std::forward< Self >( self ).models.begin();
        }

        template < typename Self > auto end( this Self&& self ) {
            return std::forward< Self >( self ).models.end();
        }
    };

} // namespace PeanutGL
