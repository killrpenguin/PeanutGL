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
#include "BufferResource.hpp"
#include "Component.hpp"
#include "DebugSystem.hpp"
#include "ResourceBase.hpp"
#include "Utilities.hpp"

#include <cassert>
#include <glm/glm.hpp>
#include <initializer_list>
#include <quill/LogMacros.h>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace PeanutGL {

    /**
     * @brief Component that handles the mesh data of an entity.
     */
    class MeshComponent final : public Component {
      public:
        using vertices_type = float;
        using indices_type  = unsigned int;

      private:
        std::vector< vertices_type > vertices{};
        std::vector< indices_type > indices{};

        int stride{};
        std::vector< VertexBufferElement > layout{};

        unsigned int VAO{};

        ResourceHandle< BufferResource< vertices_type > > vbo;
        ResourceHandle< BufferResource< indices_type > > ebo;

        bool initialized{ false };

      public:
        /**
         * @brief Constructor with an optional name.
         * @param componentName The name of the component.
         */
        explicit MeshComponent(
            ResourceHandle< BufferResource< vertices_type > > mesh_resource,
            const std::string& componentName = "MeshComponent" ) noexcept
            : Component( componentName ), vbo{ std::move( mesh_resource ) } {
        }

        explicit MeshComponent(
            ResourceHandle< BufferResource< vertices_type > > vbo_resource,
            ResourceHandle< BufferResource< indices_type > > ebo_resource,
            const std::string& componentName = "MeshComponent" ) noexcept
            : Component( componentName ), vbo{ std::move( vbo_resource ) }, ebo{ std::move( ebo_resource ) } {
        }
        MeshComponent( const MeshComponent& )            = delete;
        MeshComponent( MeshComponent&& )                 = delete;
        MeshComponent& operator=( const MeshComponent& ) = delete;
        MeshComponent& operator=( MeshComponent&& )      = delete;

        ~MeshComponent() noexcept override {
            glDeleteVertexArrays( 1, &VAO );
        }
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
        auto Render() const noexcept -> void;

        /**
         * @brief Define the vertex array attributes of the mesh.
         * @param count The count of the element.
         * @return A span of elements to pass to the SetLayout member function of a VAO resource.
         */
        template < typename T >
        auto SetAttributes( const std::initializer_list< int > attrs ) noexcept
            -> std::span< const VertexBufferElement >;

        template <>
        auto SetAttributes< float >( const std::initializer_list< int > attrs ) noexcept
            -> std::span< const VertexBufferElement > {
            if ( equal( attrs.size(), 0 ) ) {
                LOG_WARNING( QuillPtr(), "No attributes were added to {}.", Component::GetName() );
                return { layout };
            }

            for ( const auto count : attrs ) {
                layout.push_back( { .type = GL_FLOAT, .count = count, .normalized = GL_FALSE } );
                stride += count * VertexBufferElement::size_of_enum_type( GL_FLOAT );
            }
            initialized = true;
            return { layout };
        }

        template <>
        auto SetAttributes< unsigned int >( const std::initializer_list< int > attrs ) noexcept
            -> std::span< const VertexBufferElement > {
            if ( equal( attrs.size(), 0 ) ) {
                LOG_WARNING( QuillPtr(), "No attributes were added to {}.", Component::GetName() );
                return { layout };
            }

            for ( const auto count : attrs ) {
                layout.push_back( { .type = GL_UNSIGNED_INT, .count = count, .normalized = GL_FALSE } );
                stride += count * VertexBufferElement::size_of_enum_type( GL_UNSIGNED_INT );
            }

            initialized = true;
            return { layout };
        }

        template <>
        auto SetAttributes< unsigned char >( const std::initializer_list< int > attrs ) noexcept
            -> std::span< const VertexBufferElement > {
            if ( equal( attrs.size(), 0 ) ) {
                LOG_WARNING( QuillPtr(), "No attributes were added to {}.", Component::GetName() );
                return { layout };
            }

            for ( const auto count : attrs ) {
                layout.push_back( { .type = GL_UNSIGNED_BYTE, .count = count, .normalized = GL_TRUE } );
                stride += count * VertexBufferElement::size_of_enum_type( GL_UNSIGNED_BYTE );
            }
            initialized = true;
            return { layout };
        }

        /**
         * @brief Get a non-owning view of the Mesh verticies layout.
         * @return The vertices.
         */
        auto GetLayout() const noexcept -> std::span< const VertexBufferElement > {
            return { layout };
        }

        /**
         * @brief Set the Vertices of the mesh.
         * @param new_vertices The new indices.
         */
        auto SetVertices( const std::span< indices_type > new_vertices ) noexcept -> void {
            vertices.assign( new_vertices.begin(), new_vertices.end() );

            if ( vbo ) { vbo->write( { vertices } ); }
        }

        /**
         * @brief Set the vertices of the mesh.
         * @param new_vertices The new vertices as an initializer list.
         */
        auto SetVertices( const std::initializer_list< vertices_type > new_vertices ) noexcept -> void {
            vertices = new_vertices;

            if ( vbo ) { vbo->write( { vertices } ); }
        }

        /**
         * @brief Get a non-owning view of the vertices of the mesh.
         * @return The vertices.
         */
        auto GetVertices() const noexcept -> std::span< const vertices_type > {
            return { vertices };
        }

        /**
         * @brief Set the indices of the mesh.
         * @param new_indices The new indices as an initializer list.
         */
        auto SetIndices( const std::initializer_list< indices_type > new_indices ) noexcept -> void {
            indices = new_indices;

            if ( ebo ) { ebo->write( { indices } ); }
        }

        /**
         * @brief Set the indices of the mesh.
         * @param new_indices The new indices.
         */
        auto SetIndices( const std::span< indices_type > new_indices ) noexcept -> void {
            indices.assign( new_indices.begin(), new_indices.end() );

            if ( ebo ) { ebo->write( { indices } ); }
        }

        /**
         * @brief Get a non-owning view of the indices of the mesh.
         * @return The indices.
         */
        auto GetIndices() const noexcept -> std::span< const indices_type > {
            return { indices };
        }
    };
} // namespace PeanutGL
