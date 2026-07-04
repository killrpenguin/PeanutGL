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
#include "DebugSystem.hpp"
#include "ResourceManager.hpp"
#include "Utilities.hpp"

#include <algorithm>
#include <functional>
#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <quill/LogMacros.h>

namespace PeanutGL {

    namespace detail {
        inline constexpr std::size_t RequiredLayoutValue = 15;
    } // namespace detail

    template < typename T >
    concept RequiredLayout = requires { requires( sizeof( T ) & detail::RequiredLayoutValue ) == 0; };

    struct CameraData {
        glm::mat4 view;
        glm::mat4 projection;
    };

    template < RequiredLayout T > class UniformBuffer final : public Resource {
      public:
        using data_type = T;
        using pointer   = data_type*;

      private:
        struct Field {
            Field() = delete;
            explicit Field( const std::type_index type_id, const std::size_t offset )
                : id{ type_id }, offset{ offset } {
            }
            std::type_index id;
            std::size_t offset;
        };

        using DataFields = std::vector< Field >;
        constexpr static std::size_t BUFFERSIZE{ sizeof( data_type ) };

        unsigned int handle{ 0 };
        unsigned int binding_index{ 0 };

        pointer begin_ptr{ nullptr };
        pointer end_ptr{ nullptr };

        DataFields data_fields;

        constexpr auto static CreateBuffer( unsigned int& handle ) noexcept -> pointer;

      public:
        UniformBuffer() = default;

        ~UniformBuffer() noexcept override;

        explicit UniformBuffer( const std::string& identifier, const unsigned int index = 0 ) noexcept
            : Resource( identifier ), binding_index{ index } {
        }

        UniformBuffer( const UniformBuffer& other )            = delete;
        UniformBuffer& operator=( const UniformBuffer& other ) = delete;

        UniformBuffer( UniformBuffer&& other ) noexcept            = default;
        UniformBuffer& operator=( UniformBuffer&& other ) noexcept = default;

        auto Load() noexcept -> bool override;

        auto Unload() noexcept -> void override;

        constexpr auto Name() const noexcept -> unsigned int {
            return handle;
        }

        constexpr auto SetBindIndex( const unsigned int value ) noexcept -> void {
            binding_index = value;
        }

        auto RegisterComponent( const Component* const component, const std::size_t offset ) noexcept -> void;

        auto Write( const Component* const component ) noexcept -> void;
    };

    template < RequiredLayout T > UniformBuffer< T >::~UniformBuffer() noexcept {
        Unload();
        begin_ptr = nullptr;
        end_ptr   = nullptr;
    }

    template < RequiredLayout T >
    constexpr auto UniformBuffer< T >::CreateBuffer( unsigned int& handle ) noexcept -> pointer {
        constexpr int storage_flags{ GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT |
                                     GL_CLIENT_STORAGE_BIT };

        constexpr int map_flags{ GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT };

        glCreateBuffers( 1, &handle );

        glNamedBufferStorage( handle, BUFFERSIZE, nullptr, storage_flags );

        return static_cast< pointer >( glMapNamedBufferRange( handle, 0, BUFFERSIZE, map_flags ) );
    }

    template < RequiredLayout T > auto UniformBuffer< T >::Load() noexcept -> bool {
        begin_ptr = CreateBuffer( handle );

        if ( loaded = not_equal( begin_ptr, nullptr ); loaded ) {
            end_ptr = begin_ptr + BUFFERSIZE;

            glBindBufferBase( GL_UNIFORM_BUFFER, binding_index, handle );
        }

        assert( not_equal( end_ptr, nullptr ) && "end_ptr is null in UniformBuffer object." );

        return loaded;
    }

    template < RequiredLayout T > auto UniformBuffer< T >::Unload() noexcept -> void {
        glUnmapNamedBuffer( handle );

        glDeleteBuffers( 1, &handle );

        glBindBufferBase( GL_UNIFORM_BUFFER, 0, 0 );

        loaded = false;
    }

    template < RequiredLayout T >
    auto UniformBuffer< T >::RegisterComponent( const Component* const component, const std::size_t offset ) noexcept
        -> void {
        if ( not_equal( component, nullptr ) ) {
            const auto type_id = std::type_index( typeid( *component ) );

            if ( !ranges::contains( data_fields, type_id, &Field::id ) ) {
                data_fields.emplace_back( type_id, offset );

                assert( data_fields.size() != 0 && "Failed to register a component during setup." );
            }
            return;
        }
        LOG_WARNING( QuillPtr(), "Attempting to pass a nullptr as a component in UniformBuffer.write()." );
    }

    template < RequiredLayout T > auto UniformBuffer< T >::Write( const Component* const component ) noexcept -> void {
        if ( not_equal( component, nullptr ) ) {
            const auto type_id = std::type_index( typeid( *component ) );

            if ( auto val = ranges::find( data_fields, type_id, &Field::id ); not_equal( val, data_fields.end() ) ) {
                auto offset_ptr = reinterpret_cast< pointer >( reinterpret_cast< char* >( begin_ptr ) + val->offset );

                std::memcpy( offset_ptr, glm::value_ptr( component->MatrixData() ), sizeof( glm::mat4 ) );

                // glNamedBufferSubData(
                //     handle, val->offset, sizeof( glm::mat4 ), glm::value_ptr( component->MatrixData() ) );
            }
            return;
        }

        LOG_WARNING( QuillPtr(), "Attempting to pass a nullptr as a component in UniformBuffer.write()." );
    }

    using CameraUniformBuffer = UniformBuffer< CameraData >;
} // namespace PeanutGL
