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
#include "ResourceBase.hpp"
#include "Utilities.hpp"

#include <glad/gl.h>

#include <cassert>

namespace PeanutGL {

    struct Stride {
        Stride() = default;

        template < typename T >
            requires std::same_as< T, int >
        Stride( std::initializer_list< const T > inner ) noexcept
            : inner{ std::accumulate( inner.begin(), inner.end(), 0 ) *
                     static_cast< GLsizei >( sizeof( VertexBuffer::BufferType ) ) } {};

        template < typename T >
            requires std::same_as< T, int >
        explicit Stride( T inner ) noexcept
            : inner{ inner * static_cast< GLsizei >( sizeof( VertexBuffer::BufferType ) ) } {};

        template < typename Self > auto operator()( this Self&& self ) -> int& {
            return std::forward< Self >( self ).inner;
        };

      private:
        [[maybe_unused]] int inner{}; // Go home clang you're drunk!
    };

    class VertexArray final : public Resource {
      private:
        unsigned int VAO{};
        unsigned int vbo_name{};
        unsigned int ebo_name{};
        Stride stride{};

      public:
        VertexArray() = delete;

        explicit VertexArray( const std::string& identifier, const unsigned int vbo, const Stride stride ) noexcept
            : Resource( identifier ), vbo_name{ vbo }, stride{ stride } {
            Load();
        };

        explicit VertexArray(
            const std::string& identifier, const unsigned int vbo, const unsigned int ebo,
            const Stride stride ) noexcept
            : Resource( identifier ), vbo_name{ vbo }, ebo_name{ ebo }, stride{ stride } {
          Load();
          assert(VAO != 0);
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

} // namespace PeanutGL
