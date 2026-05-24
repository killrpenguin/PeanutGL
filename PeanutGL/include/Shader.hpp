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

#include "GeneratedConstants.hpp"
#include "ResourceBase.hpp"
#include "Utilities.hpp"

#include "quill/LogMacros.h"

#include <glad/gl.h>
#include <string>

namespace PeanutGL {
    /**
     * @brief Class for representing a compiled OpenGL shader.
     *
     * This class stores the OpenGL handle to a shader.
     */
    template < GLenum T > class Shader : public Resource {
      private:
        GLenum type{};
        unsigned int shader_handle{};

      public:
        Shader() noexcept = default;

        explicit Shader( std::string_view identifier ) noexcept
            : Resource( identifier ), type{ T } {
        }

        // Delete the copy constructor and copy assignment operator. Resources are stored as unique pointers in
        // the resource manager.
        Shader( const Shader& )                = delete;
        Shader( Shader&& ) noexcept            = default;
        Shader& operator=( const Shader& )     = delete;
        Shader& operator=( Shader&& ) noexcept = default;

        ~Shader() override {
            Unload();
        }

        auto Unload() noexcept -> void override {
            if ( loaded ) {
                glDeleteShader( shader_handle );
                loaded = false;
            }
        }

        auto Load() noexcept -> bool override {
            std::string extension{};

            switch ( type ) {
                case GL_VERTEX_SHADER  : extension = ".vert"; break;
                case GL_FRAGMENT_SHADER: extension = ".frag"; break;
                default                : LOG_ERROR( QuillPtr(), "Unsupported shader type identified." ); return false;
            }

            const std::string shader_path{ std::string( ASSETS_ROOT ) + "/Shaders/" + GetId() + extension };

            const std::string shader_source{ read_file( shader_path ) };

            const unsigned int shader{ compile_shader( type, shader_source ) };

            if ( loaded = !compile_error( shader, resourceId ); loaded ) { shader_handle = shader; }

            return loaded;
        }

        constexpr auto Type() const noexcept -> GLenum {
            return type;
        }

        auto operator()() const -> unsigned int {
            return shader_handle;
        }
    };
    using VertShader = Shader< GL_VERTEX_SHADER >;
    using FragShader = Shader< GL_FRAGMENT_SHADER >;
} // namespace PeanutGL
