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
#include "Shader.hpp"
#include "Utilities.hpp"

#include <glad/gl.h>
#include <string_view>

namespace PeanutGL {
    /**
     * @brief Class for representing a compiled OpenGL shader program.
     *
     * This class stores the OpenGL handle to a shader.
     */
    class ShaderProgram : public Resource {
      private:
        unsigned int program_handle{};

      public:
        ShaderProgram() noexcept = default;

        explicit ShaderProgram(
            std::string_view identifier, const Shader& vert_shader, const Shader& frag_shader ) noexcept
            : Resource( identifier ), program_handle{ link_shaders( vert_shader(), frag_shader() ) } {
            loaded = !compile_error( program_handle, "PROGRAM" );
        }

        ShaderProgram( const ShaderProgram& )                = delete;
        ShaderProgram( ShaderProgram&& ) noexcept            = default;
        ShaderProgram& operator=( const ShaderProgram& )     = delete;
        ShaderProgram& operator=( ShaderProgram&& ) noexcept = default;

        ~ShaderProgram() override {
            Unload();
            glDeleteProgram( program_handle );
        }

        auto Unload() noexcept -> void override {
            if ( loaded ) {
                glUseProgram( 0 );
                loaded = false;
            }
        }

        auto Load() noexcept -> bool override {
            glUseProgram( program_handle );

            loaded = true;
            return loaded;
        }
    };

} // namespace PeanutGL
