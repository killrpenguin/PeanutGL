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
#include "ResourceManager.hpp"
#include "Utilities.hpp"
#include "UtilityTypes.hpp"

#include <glad/gl.h>
#include <quill/LogMacros.h>

#include <cassert>
#include <numeric>

namespace PeanutGL {
    namespace detail {
        struct VAO {};
        struct InstancedVAO {};
        struct EmptyVAO {};
    } // namespace detail

    template < typename V > struct Stride {
        Stride() = default;

        template < typename T >
            requires std::same_as< T, int >
        Stride( std::initializer_list< const T > inner ) noexcept
            : inner{ std::accumulate( inner.begin(), inner.end(), 0 ) * static_cast< GLsizei >( sizeof( V ) ) } {};

        template < typename T >
            requires std::same_as< T, int >
        explicit Stride( T inner ) noexcept
            : inner{ inner * static_cast< GLsizei >( sizeof( V ) ) } {};

        template < typename Self > auto operator()( this Self&& self ) -> int& {
            return std::forward< Self >( self ).inner;
        };

      private:
        [[maybe_unused]] int inner{}; // Go home clang you're drunk!
    };

    template < typename T, typename ArrayType > class VertexArray final : public Resource {
        using vertex_type = T;

      private:
        unsigned int VAO{};
        unsigned int vbo_name{};
        unsigned int ebo_name{};
        Stride< vertex_type > stride{};

      public:
        VertexArray() = delete;

        explicit VertexArray( const std::string& identifier, const unsigned int vbo ) noexcept
            : Resource( identifier ), vbo_name{ vbo } {
            Load();
        };

        explicit VertexArray(
            const std::string& identifier, const unsigned int vbo, const Stride< vertex_type > stride ) noexcept
            : Resource( identifier ), vbo_name{ vbo }, stride{ stride } {
            Load();
        };

        explicit VertexArray(
            const std::string& identifier, const unsigned int vbo, const unsigned int ebo,
            const Stride< vertex_type > stride ) noexcept
            : Resource( identifier ), vbo_name{ vbo }, ebo_name{ ebo }, stride{ stride } {
            Load();
            assert( VAO != 0 );
        };

        VertexArray( const VertexArray& )                = delete;
        VertexArray( VertexArray&& ) noexcept            = default;
        VertexArray& operator=( const VertexArray& )     = delete;
        VertexArray& operator=( VertexArray&& ) noexcept = default;

        ~VertexArray() noexcept override;

        auto Unload() noexcept -> void override;

        auto Load() noexcept -> bool override;

        auto SetLayout( const std::span< const VertexBufferElement > elements ) noexcept -> void;
    };

    template < typename T > class VertexArray< T, detail::VAO > final : public Resource {
        using vertex_type = T;

      private:
        unsigned int VAO{};
        unsigned int vbo_name{};
        unsigned int ebo_name{};
        Stride< vertex_type > stride{};

      public:
        VertexArray() = delete;

        explicit VertexArray( const std::string& identifier, const unsigned int vbo ) noexcept
            : Resource( identifier ), vbo_name{ vbo } {
            Load();
        };

        explicit VertexArray(
            const std::string& identifier, const unsigned int vbo, const Stride< vertex_type > stride ) noexcept
            : Resource( identifier ), vbo_name{ vbo }, stride{ stride } {
            Load();
        };

        explicit VertexArray(
            const std::string& identifier, const unsigned int vbo, const unsigned int ebo,
            const Stride< vertex_type > stride ) noexcept
            : Resource( identifier ), vbo_name{ vbo }, ebo_name{ ebo }, stride{ stride } {
            Load();
            assert( VAO != 0 );
        };

        VertexArray( const VertexArray& )                = delete;
        VertexArray( VertexArray&& ) noexcept            = default;
        VertexArray& operator=( const VertexArray& )     = delete;
        VertexArray& operator=( VertexArray&& ) noexcept = default;

        ~VertexArray() noexcept override;

        auto Unload() noexcept -> void override;

        auto Load() noexcept -> bool override;

        auto SetLayout( const std::span< const VertexBufferElement > elements ) noexcept -> void;
    };
    template < typename T > auto VertexArray< T, detail::VAO >::Load() noexcept -> bool {
        glCreateVertexArrays( 1, &VAO );

        return true;
    }

    template < typename T > auto VertexArray< T, detail::VAO >::Unload() noexcept -> void {
        glDeleteVertexArrays( 1, &VAO );
    }

    template < typename T > VertexArray< T, detail::VAO >::~VertexArray() noexcept {
        Unload();
    }

    template < typename T >
    auto VertexArray< T, detail::VAO >::SetLayout( const std::span< const VertexBufferElement > elements ) noexcept
        -> void {
        if ( equal( elements.size(), 0 ) ) {
            LOG_ERROR( QuillPtr(), "Layout can not contain 0 elements." );
            return;
        }

        unsigned int offset{ 0 };
        for ( const auto [attr_index, element] : std::views::enumerate( std::views::as_const( elements ) ) ) {
            glEnableVertexArrayAttrib( VAO, attr_index );

            glVertexArrayAttribFormat(
                VAO, attr_index, VertexBufferElement::size_of_enum_type( element.type ), element.type,
                element.normalized, offset );

            glVertexArrayAttribBinding( VAO, attr_index, 0 );

            offset += element.count * VertexBufferElement::size_of_enum_type( element.type );
        }

        glVertexArrayVertexBuffer( VAO, 0, vbo_name, 0, stride() );

        glVertexArrayElementBuffer( VAO, ebo_name );

        glBindVertexArray( VAO );
    }

    template < typename T > class VertexArray< T, detail::EmptyVAO > final : public Resource {
        using vertex_type = T;

      private:
        unsigned int VAO{};
        unsigned int vbo_name{};
        unsigned int ebo_name{};
        Stride< vertex_type > stride{};

      public:
        VertexArray() = delete;

        explicit VertexArray( const std::string& identifier, const unsigned int vbo ) noexcept
            : Resource( identifier ), vbo_name{ vbo } {
            Load();
        };

        explicit VertexArray(
            const std::string& identifier, const unsigned int vbo, const Stride< vertex_type > stride ) noexcept
            : Resource( identifier ), vbo_name{ vbo }, stride{ stride } {
            Load();
        };

        explicit VertexArray(
            const std::string& identifier, const unsigned int vbo, const unsigned int ebo,
            const Stride< vertex_type > stride ) noexcept
            : Resource( identifier ), vbo_name{ vbo }, ebo_name{ ebo }, stride{ stride } {
            Load();
            assert( VAO != 0 );
        };

        VertexArray( const VertexArray& )                = delete;
        VertexArray( VertexArray&& ) noexcept            = default;
        VertexArray& operator=( const VertexArray& )     = delete;
        VertexArray& operator=( VertexArray&& ) noexcept = default;

        ~VertexArray() noexcept override;

        auto Unload() noexcept -> void override;

        auto Load() noexcept -> bool override;

        auto SetLayout( const std::span< const VertexBufferElement > elements ) noexcept -> void;
    };

    template < typename T > auto VertexArray< T, detail::EmptyVAO >::Load() noexcept -> bool {
        glCreateVertexArrays( 1, &VAO );
        glBindVertexArray( VAO );
        return true;
    }

    template < typename T > auto VertexArray< T, detail::EmptyVAO >::Unload() noexcept -> void {
        glDeleteVertexArrays( 1, &VAO );
    }

    template < typename T > VertexArray< T, detail::EmptyVAO >::~VertexArray() noexcept {
        Unload();
    }

    template < typename T > using BasicVAO = VertexArray< T, detail::VAO >;
    template < typename T > using EmptyVAO = VertexArray< T, detail::EmptyVAO >;

} // namespace PeanutGL
