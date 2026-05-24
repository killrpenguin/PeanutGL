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

#include "DebugSystem.hpp"
#include "ResourceBase.hpp"
#include "Utilities.hpp"
#include <cstddef>

#include <glad/gl.h>
#include <numeric>
#include <quill/LogMacros.h>

namespace PeanutGL {
    /**
     * @brief Class for representing a persistent coherent OpenGL buffer object.
     *
     * This class owns the buffer memory of an OpenGL buffer object.
     */
    template < typename T > class BufferResource final : public Resource {
      public:
        using BufferType    = T;
        using BufferTypePtr = T*;

      private:
        constexpr static std::size_t BUFFERSIZE{ 1024 * sizeof( BufferType ) };

        BufferTypePtr begin_ptr{ nullptr };
        BufferTypePtr end_ptr{};

        unsigned int handle{ 0 };
        std::size_t offset{ 0 };

        constexpr auto static CreateBuffer( unsigned int& handle ) noexcept -> BufferTypePtr;

      public:
        BufferResource() noexcept = default;

        // Delete the copy constructor and copy assignment operator. Resources are stored as unique pointers in
        // the resource manager.
        BufferResource( const BufferResource& )                = delete;
        BufferResource( BufferResource&& ) noexcept            = default;
        BufferResource& operator=( const BufferResource& )     = delete;
        BufferResource& operator=( BufferResource&& ) noexcept = default;

        explicit BufferResource( const std::string& identifier ) noexcept
            : Resource( identifier ) {
            Load();
        }

        ~BufferResource() override {
            Unload();
        }

        auto Unload() noexcept -> void override {
            glUnmapNamedBuffer( handle );
            glDeleteBuffers( 1, &handle );
            loaded = false;
        }

        auto Load() noexcept -> bool override {
            begin_ptr = CreateBuffer( handle );

            if ( loaded = not_equal( begin_ptr, nullptr ); loaded ) { end_ptr = begin_ptr + BUFFERSIZE; }

            assert( not_equal( end_ptr, nullptr ) && "end_ptr is null in BufferResource object." );

            return loaded;
        }

        auto Name() const noexcept -> unsigned int {
            return handle;
        }

        auto write( const std::span< const BufferType > data ) noexcept -> void {
            if ( BufferTypePtr current_pos = begin_ptr + offset; not_equal( current_pos, end_ptr ) ) {
                std::memcpy( current_pos, data.data(), data.size_bytes() );
                offset += data.size_bytes();
            } else {
                LOG_CRITICAL( QuillPtr(), "OpenGL {} Buffer is out of memory.", Resource::GetId() );
            }
        }
    };
    template < typename T >
    constexpr auto BufferResource< T >::CreateBuffer( unsigned int& handle ) noexcept -> BufferTypePtr {
        constexpr int access_flags{ GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT };

        constexpr int storage_flags{ GL_DYNAMIC_STORAGE_BIT | access_flags };

        glCreateBuffers( 1, &handle );

        glNamedBufferStorage( handle, BUFFERSIZE, nullptr, storage_flags );

        return static_cast< BufferTypePtr >( glMapNamedBufferRange( handle, 0, BUFFERSIZE, access_flags ) );
    }

    using ElementBuffer = BufferResource< unsigned int >;
    using VertexBuffer  = BufferResource< float >;

} // namespace PeanutGL
