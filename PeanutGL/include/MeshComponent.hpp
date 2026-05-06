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
#include "MeshResource.hpp"
#include "ResourceBase.hpp"

#include <cassert>
#include <glm/glm.hpp>
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
        using indices_type  = int;

      private:
        std::vector< vertices_type > vertices{};
        std::vector< indices_type > indices{};

        unsigned int VAO{};
        ResourceHandle< Mesh > vbo;

        bool initialized{ false };

        std::string texturePath{};

      public:
        /**
         * @brief Constructor with an optional name.
         * @param componentName The name of the component.
         */
        explicit MeshComponent(
            ResourceHandle< Mesh > mesh_resource, const std::string& componentName = "MeshComponent" ) noexcept
            : Component( componentName ), vbo{ std::move( mesh_resource ) } {
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
         * @return True if initialization was successful, false otherwise.
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
         * @brief Set the vertices of the mesh.
         * @param new_vertices The new vertices as an initializer list.
         */
        auto SetVertices( const std::initializer_list< vertices_type > new_vertices ) noexcept -> void {
            vertices = new_vertices;
            vbo->tmp_write_data( { vertices } );
        }

        /**
         * @brief Get a non-owning view of the vertices of the mesh.
         * @return The vertices.
         */
        auto GetVertices() noexcept -> std::span< const vertices_type > {
            return { vertices };
        }

        /**
         * @brief Set the indices of the mesh.
         * @param new_indices The new indices as an initializer list.
         */
        auto SetIndices( const std::initializer_list< indices_type > new_indices ) noexcept -> void {
            indices = new_indices;
        }
        /**
         * @brief Set the indices of the mesh.
         * @param new_indices The new indices.
         */
        auto SetIndices( const std::span< indices_type > new_indices ) noexcept -> void {
            indices.assign( new_indices.begin(), new_indices.end() );
        }

        /**
         * @brief Get a non-owning view of the indices of the mesh.
         * @return The indices.
         */
        auto GetIndices() noexcept -> std::span< const indices_type > {
            return { indices };
        }

        /**
         * @brief Set the texture path for the mesh.
         * @param path The path to the texture file.
         */
        auto SetTexturePath( const std::string& path ) noexcept -> void {
            texturePath = path;
        }

        /**
         * @brief Get the texture path for the mesh.
         * @return The path to the texture file.
         */
        const std::string& GetTexturePath() const {
            return texturePath;
        }
    };
} // namespace PeanutGL
