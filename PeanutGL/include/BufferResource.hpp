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

#include "ResourceManager.hpp"
#include "Utilities.hpp"

#include <cstddef>

#include <glad/gl.h>
#include <quill/LogMacros.h>

namespace PeanutGL {


  
    namespace detail {
        struct PersistentVBO {};
        struct ShaderBufferObject {};
    }; // namespace detail

    template < typename T, typename BufferType > class Buffer final : public Resource {
      public:
        using vertex_type = T;
        using pointer     = vertex_type*;

      private:
        constexpr static std::size_t BUFFERSIZE{ 1024 * sizeof( vertex_type ) };

        unsigned int handle{ 0 };
        unsigned int binding_index{ 0 };
        std::size_t offset{ 0 };

        constexpr auto static CreateBuffer( unsigned int& handle ) noexcept -> pointer;

      public:
        Buffer() = default;

        ~Buffer() noexcept override;

        Buffer( const Buffer& )                = delete;
        Buffer( Buffer&& ) noexcept            = default;
        Buffer& operator=( const Buffer& )     = delete;
        Buffer& operator=( Buffer&& ) noexcept = default;

        auto Load() noexcept -> bool override {
            static_assert( false, "Method `Load` is not implemented for custom BufferTypes." );
        }

        auto Unload() noexcept -> void override {
            static_assert( false, "Method `Unload` is not implemented for custom BufferTypes." );
        }

        constexpr auto Name() const noexcept -> unsigned int {
            return handle;
        }

        auto Write( const std::span< const vertex_type > data ) noexcept -> void;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SSBO
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template < typename T >
    concept FourThirtyLayoutReq = requires( T data ) {
        // NOLINTNEXTLINE
        equal( sizeof( data ) % 16, 0 );
    };

    template < FourThirtyLayoutReq T > class Buffer< T, detail::ShaderBufferObject > final : public Resource {
      public:
        using vertex_type = T;
        using pointer     = vertex_type*;

      private:
        constexpr static std::size_t BUFFERSIZE{ 1024 * sizeof( vertex_type ) };

        unsigned int handle{ 0 };
        unsigned int binding_index{ 0 };
        std::size_t offset{ 0 };

        pointer begin_ptr{ nullptr };
        pointer end_ptr{ nullptr };

        constexpr auto static CreateBuffer( unsigned int& handle ) noexcept -> pointer;

      public:
        Buffer() = default;

        explicit Buffer( const std::string& identifier, const unsigned int binding = 0 ) noexcept
            : Resource( identifier ), binding_index{ binding } {
            Load();
        }

        ~Buffer() noexcept override;

        Buffer( const Buffer& )                = delete;
        Buffer( Buffer&& ) noexcept            = default;
        Buffer& operator=( const Buffer& )     = delete;
        Buffer& operator=( Buffer&& ) noexcept = default;

        auto Load() noexcept -> bool override;

        auto Unload() noexcept -> void override;

        constexpr auto Name() const noexcept -> unsigned int {
            return handle;
        }

        auto Write( const std::span< const vertex_type > data ) noexcept -> void;
    };

    template < FourThirtyLayoutReq VertexType > Buffer< VertexType, detail::ShaderBufferObject >::~Buffer() noexcept {
        Unload();
        begin_ptr = nullptr;
        end_ptr   = nullptr;
    }

    template < FourThirtyLayoutReq VertexType >
    auto Buffer< VertexType, detail::ShaderBufferObject >::Load() noexcept -> bool {
        begin_ptr = CreateBuffer( handle );

        if ( loaded = not_equal( begin_ptr, nullptr ); loaded ) {
            end_ptr = std::ranges::next( begin_ptr, BUFFERSIZE );
        }

        assert( not_equal( end_ptr, nullptr ) && "end_ptr is null in ShaderBufferResource object." );

        return loaded;
    }

    template < FourThirtyLayoutReq VertexType >
    auto Buffer< VertexType, detail::ShaderBufferObject >::Unload() noexcept -> void {
        glUnmapNamedBuffer( handle );
        glDeleteBuffers( 1, &handle );
        glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 0, 0 );
        loaded = false;
    }

    template < FourThirtyLayoutReq VertexType >
    auto Buffer< VertexType, detail::ShaderBufferObject >::Write( const std::span< const VertexType > data ) noexcept
        -> void {
        if ( pointer current_pos = begin_ptr + offset; not_equal( current_pos, end_ptr ) ) {
            std::memcpy( current_pos, data.data(), data.size_bytes() );

            offset += data.size_bytes();

            glBindBufferBase( GL_SHADER_STORAGE_BUFFER, binding_index, handle );
        } else {
            LOG_CRITICAL( QuillPtr(), "OpenGL {} Buffer is out of memory.", Resource::GetId() );
        }
    }

    template < FourThirtyLayoutReq VertexType >
    constexpr auto Buffer< VertexType, detail::ShaderBufferObject >::CreateBuffer( unsigned int& handle ) noexcept
        -> pointer {
        constexpr int access_flags{ GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT };

        constexpr int storage_flags{ GL_DYNAMIC_STORAGE_BIT | access_flags };

        glCreateBuffers( 1, &handle );

        glNamedBufferStorage( handle, BUFFERSIZE, nullptr, storage_flags );

        return static_cast< pointer >( glMapNamedBufferRange( handle, 0, BUFFERSIZE, access_flags ) );
    }

    template < typename T > using SSBOResource = Buffer< T, detail::ShaderBufferObject >;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // PersistentVBO
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template < typename T > class Buffer< T, detail::PersistentVBO > final : public Resource {
      public:
        using vertex_type = T;
        using pointer     = vertex_type*;

      private:
        constexpr static std::size_t BUFFERSIZE{ 1024 * sizeof( vertex_type ) };

        unsigned int handle{ 0 };
        unsigned int binding_index{ 0 };
        std::size_t offset{ 0 };

        pointer begin_ptr{ nullptr };
        pointer end_ptr{ nullptr };

        constexpr auto static CreateBuffer( unsigned int& handle ) noexcept -> pointer;

      public:
        Buffer() = default;

        explicit Buffer( const std::string& identifier ) noexcept
            : Resource( identifier ) {
            Load();
        }

        ~Buffer() noexcept override;

        Buffer( const Buffer& )                = delete;
        Buffer( Buffer&& ) noexcept            = default;
        Buffer& operator=( const Buffer& )     = delete;
        Buffer& operator=( Buffer&& ) noexcept = default;

        auto Load() noexcept -> bool override;

        auto Unload() noexcept -> void override;

        constexpr auto Name() const noexcept -> unsigned int {
            return handle;
        }

        auto Write( const std::span< const vertex_type > data ) noexcept -> void;
    };

    template < typename T > Buffer< T, detail::PersistentVBO >::~Buffer() noexcept {
        Unload();
        begin_ptr = nullptr;
        end_ptr   = nullptr;
    }

    template < typename T > auto Buffer< T, detail::PersistentVBO >::Load() noexcept -> bool {
        begin_ptr = CreateBuffer( handle );

        if ( loaded = not_equal( begin_ptr, nullptr ); loaded ) { end_ptr = begin_ptr + BUFFERSIZE; }

        assert( not_equal( end_ptr, nullptr ) && "end_ptr is null in BufferResource object." );

        return loaded;
    }

    template < typename T > auto Buffer< T, detail::PersistentVBO >::Unload() noexcept -> void {
        glUnmapNamedBuffer( handle );
        glDeleteBuffers( 1, &handle );
        loaded = false;
    }

    template < typename T >
    auto Buffer< T, detail::PersistentVBO >::Write( const std::span< const vertex_type > data ) noexcept -> void {
        if ( pointer current_pos = begin_ptr + offset; not_equal( current_pos, end_ptr ) ) {
            std::memcpy( current_pos, data.data(), data.size_bytes() );
            offset += data.size_bytes();
        } else {
            LOG_CRITICAL( QuillPtr(), "OpenGL {} Buffer is out of memory.", Resource::GetId() );
        }
    }

    template < typename T >
    constexpr auto Buffer< T, detail::PersistentVBO >::CreateBuffer( unsigned int& handle ) noexcept -> pointer {
        constexpr int access_flags{ GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT };

        constexpr int storage_flags{ GL_DYNAMIC_STORAGE_BIT | access_flags };

        glCreateBuffers( 1, &handle );

        glNamedBufferStorage( handle, BUFFERSIZE, nullptr, storage_flags );

        return static_cast< pointer >( glMapNamedBufferRange( handle, 0, BUFFERSIZE, access_flags ) );
    }

    template < typename T > using PersistentVBO = Buffer< T, detail::PersistentVBO >;
} // namespace PeanutGL
