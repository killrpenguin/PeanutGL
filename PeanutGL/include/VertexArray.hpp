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

#include <glad/gl.h>

#include <cassert>

namespace PeanutGL {

    class VertexArray final : public Resource {
      private:
        unsigned int VAO{};
        unsigned int vbo_name{};
        unsigned int ebo_name{};

      public:
        VertexArray() = delete;

        explicit VertexArray( const std::string& identifier, const unsigned int vbo ) noexcept
            : Resource( identifier ), vbo_name{ vbo } {
            Load();
        };

        explicit VertexArray( const std::string& identifier, const unsigned int vbo, const unsigned int ebo ) noexcept
            : Resource( identifier ), vbo_name{ vbo }, ebo_name{ ebo } {
            Load();
        };

        VertexArray( const VertexArray& )                = delete;
        VertexArray( VertexArray&& ) noexcept            = default;
        VertexArray& operator=( const VertexArray& )     = delete;
        VertexArray& operator=( VertexArray&& ) noexcept = default;

        ~VertexArray() noexcept override;

        auto Unload() noexcept -> void override;

        auto Load() noexcept -> bool override;
        auto SetLayout( std::span< const VertexBufferElement > elements ) const noexcept -> void;
    };

} // namespace PeanutGL
