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

        template < typename T > constexpr auto Set( const std::string& name, const T& value ) const noexcept -> void;

        template < typename T >
        constexpr auto Set( const std::string& name, const T xpos, const T ypos ) const noexcept -> void;

        template < typename T >
        constexpr auto Set( const std::string& name, const T xpos, const T ypos, const T zpos ) const noexcept -> void;

        template < typename T >
        constexpr auto Set(
            const std::string& name, const T xpos, const T ypos, const T zpos, const T wpos ) const noexcept -> void;
    };

    template <> constexpr auto ShaderProgram::Set( const std::string& name, const bool& value ) const noexcept -> void {
        glUniform1i( glGetUniformLocation( program_handle, name.c_str() ), static_cast< int >( value ) );
    }

    template <> constexpr auto ShaderProgram::Set( const std::string& name, const int& value ) const noexcept -> void {
        glUniform1i( glGetUniformLocation( program_handle, name.c_str() ), value );
    }

    template <>
    constexpr auto ShaderProgram::Set( const std::string& name, const float& value ) const noexcept -> void {
        glUniform1f( glGetUniformLocation( program_handle, name.c_str() ), value );
    }

    template <>
    constexpr auto ShaderProgram::Set< glm::vec< 2, float > >(
        const std::string& name, const glm::vec< 2, float >& value ) const noexcept -> void {
        glUniform2fv( glGetUniformLocation( program_handle, name.c_str() ), 1, &value[0] );
    }

    template <>
    constexpr auto ShaderProgram::Set( const std::string& name, const float xpos, const float y ) const noexcept
        -> void {
        glUniform2f( glGetUniformLocation( program_handle, name.c_str() ), xpos, y );
    }

    template <>
    constexpr auto ShaderProgram::Set< glm::vec< 3, float > >(
        const std::string& name, const glm::vec< 3, float >& value ) const noexcept -> void {
        glUniform3fv( glGetUniformLocation( program_handle, name.c_str() ), 1, &value[0] );
    }

    template <>
    constexpr auto ShaderProgram::Set(
        const std::string& name, const float xpos, const float ypos, const float zpos ) const noexcept -> void {
        glUniform3f( glGetUniformLocation( program_handle, name.c_str() ), xpos, ypos, zpos );
    }

    template <>
    constexpr auto ShaderProgram::Set< glm::vec< 4, float > >(
        const std::string& name, const glm::vec< 4, float >& value ) const noexcept -> void {
        glUniform4fv( glGetUniformLocation( program_handle, name.c_str() ), 1, &value[0] );
    }

    template <>
    constexpr auto ShaderProgram::Set(
        const std::string& name, const float xpos, const float ypos, const float zpos, const float wpos ) const noexcept
        -> void {
        glUniform4f( glGetUniformLocation( program_handle, name.c_str() ), xpos, ypos, zpos, wpos );
    }

    template <>
    constexpr auto ShaderProgram::Set< glm::mat< 2, 2, float > >(
        const std::string& name, const glm::mat< 2, 2, float >& value ) const noexcept -> void {
        glUniformMatrix2fv( glGetUniformLocation( program_handle, name.c_str() ), 1, GL_FALSE, &value[0][0] );
    }

    template <>
    constexpr auto ShaderProgram::Set< glm::mat< 3, 3, float > >(
        const std::string& name, const glm::mat< 3, 3, float >& value ) const noexcept -> void {
        glUniformMatrix3fv( glGetUniformLocation( program_handle, name.c_str() ), 1, GL_FALSE, &value[0][0] );
    }

    template <>
    constexpr auto ShaderProgram::Set< glm::mat< 4, 4, float > >(
        const std::string& name, const glm::mat< 4, 4, float >& value ) const noexcept -> void {
        glUniformMatrix4fv( glGetUniformLocation( program_handle, name.c_str() ), 1, GL_FALSE, &value[0][0] );
    }

} // namespace PeanutGL
