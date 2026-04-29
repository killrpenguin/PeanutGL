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

#include "ResourceBase.hpp"
#include "Utilities.hpp"
#include <cstddef>

#include <glad/gl.h>

namespace PeanutGL {
    /**
     * @brief Class for representing geometric data on the GPU.
     *
     * This class stores the GPU information for vertex attributes and index information of geometric data.
     */
    class Mesh : public Resource {
        using VertexType = float;
        using IndexType  = int;

      private:
        VertexType* vertexBuffer{ nullptr };
        unsigned int vbo{ 0 };

        std::size_t vertexBufferOffset{ 0 };
        std::size_t vertexCount{ 0 };

        IndexType* indexBuffer{ nullptr };
        unsigned int ebo{ 0 };

        [[maybe_unused]] std::size_t indexBufferOffset{ 0 };
        std::size_t indexCount{ 0 };

        template < typename T > constexpr auto static CreateBuffer( unsigned int& handle ) noexcept -> T*;

      public:
        Mesh() noexcept = default;

        // Delete the copy constructor and copy assignment operator. Resources are stored as unique pointers in
        // the resource manager.
        Mesh( const Mesh& )                = delete;
        Mesh( Mesh&& ) noexcept            = default;
        Mesh& operator=( const Mesh& )     = delete;
        Mesh& operator=( Mesh&& ) noexcept = default;

        explicit Mesh( const std::string& identifier ) noexcept
            : Resource( identifier ) {
            Load();
        }

        ~Mesh() override {
            Unload();
        }

        auto Unload() noexcept -> void override {
            // TODO: unique_ptr ownership causing segfault.
            glUnmapNamedBuffer( vbo );
            glDeleteBuffers( 1, &vbo );
            glUnmapNamedBuffer( ebo );
            glDeleteBuffers( 1, &ebo );
            loaded = false;
        }

        auto Load() noexcept -> bool override {
            vertexBuffer = CreateBuffer< VertexType >( vbo );

            indexBuffer = CreateBuffer< IndexType >( ebo );

            loaded = not_equal( vertexBuffer, nullptr ) and not_equal( indexBuffer, nullptr );

            return loaded;
        }

        auto GetVertexBuffer() noexcept -> VertexType* {
            return vertexBuffer;
        }

        auto GetIndexBuffer() noexcept -> IndexType* {
            return indexBuffer;
        }

        auto GetVertexCount() const noexcept -> std::size_t {
            return vertexCount;
        }

        auto GetIndexCount() const noexcept -> std::size_t {
            return indexCount;
        }

        auto VertexBufferName() const noexcept -> unsigned int {
            return vbo;
        }

        auto tmp_write_data( const std::span< VertexType > data ) noexcept -> void {
            std::memcpy( vertexBuffer, data.data(), data.size_bytes() );

            vertexBufferOffset += data.size_bytes();
        }
    };

    template < typename T > constexpr auto Mesh::CreateBuffer( unsigned int& handle ) noexcept -> T* {
        constexpr static std::size_t BUFFERSIZE{ 1024 };

        constexpr static int FLAGS{ GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT };

        glCreateBuffers( 1, &handle );

        glNamedBufferStorage( handle, BUFFERSIZE, nullptr, FLAGS );

        return static_cast< T* >( glMapNamedBufferRange( handle, 0, BUFFERSIZE, FLAGS ) );
    }

} // namespace PeanutGL
