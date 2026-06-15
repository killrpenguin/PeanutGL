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
#include "MaterialResource.hpp"
#include "ResourceManager.hpp"
#include "Shader.hpp"
#include "Utilities.hpp"

#include <glad/gl.h>

#include <glm/gtc/type_ptr.hpp>
#include <quill/LogMacros.h>
#include <string_view>
#include <unordered_map>

namespace PeanutGL {
    struct uniform_info {
        GLint location;
        GLsizei count;
        GLenum type;
    };
    /**
     * @brief Class for representing a compiled OpenGL shader program.
     *
     * This class stores the OpenGL handle to a shader.
     */
    class ShaderProgram : public Resource {
        using UniformMap     = std::unordered_map< std::string, uniform_info >;
        using iterator       = UniformMap::iterator;
        using const_iterator = UniformMap::const_iterator;

      private:
        unsigned int program_handle{};
        UniformMap uniforms{};

      public:
        ShaderProgram() noexcept = default;

        explicit ShaderProgram(
            std::string_view identifier, const VertShader& vert_shader, const FragShader& frag_shader ) noexcept
            : Resource( identifier ), program_handle{ link_shaders( vert_shader(), frag_shader() ) } {
            loaded = !compile_error( program_handle, detail::Link );
        }

        ShaderProgram( const ShaderProgram& )                = delete;
        ShaderProgram( ShaderProgram&& ) noexcept            = default;
        ShaderProgram& operator=( const ShaderProgram& )     = delete;
        ShaderProgram& operator=( ShaderProgram&& ) noexcept = default;

        ~ShaderProgram() override {
            Unload();
            glDeleteProgram( program_handle );
        }

        auto begin() -> iterator {
            return uniforms.begin();
        }
        auto end() -> iterator {
            return uniforms.end();
        }
        auto cbegin() -> const_iterator {
            return uniforms.cbegin();
        }
        auto cend() -> const_iterator {
            return uniforms.cend();
        }

        auto Unload() noexcept -> void override {
            if ( loaded ) {
                glUseProgram( 0 );
                loaded = false;
            }
        }

        auto Load() noexcept -> bool override {
            glUseProgram( program_handle );

            populate_uniforms();

            loaded = true;

            return loaded;
        }

        auto constexpr populate_uniforms() noexcept -> void;

        auto HasUniform( const std::string& name ) const noexcept -> bool {
            const auto uniform = uniforms.find( name );
            return not_equal( uniform, uniforms.end() );
        }

        auto Handle() const noexcept -> unsigned int {
            return program_handle;
        }

        template < typename T >
        constexpr auto SetUniform( const std::string& name, const T& value ) const noexcept -> void;

        template < typename T >
        constexpr auto SetUniform( const std::string& name, const T xpos, const T ypos ) const noexcept -> void;

        template < typename T >
        constexpr auto SetUniform( const std::string& name, const T xpos, const T ypos, const T zpos ) const noexcept
            -> void;

        template < typename T >
        constexpr auto SetUniform(
            const std::string& name, const T xpos, const T ypos, const T zpos, const T wpos ) const noexcept -> void;

        template < typename T > constexpr auto SetUniform( T& resource_handle ) const noexcept -> void;
    };

    auto constexpr ShaderProgram::populate_uniforms() noexcept -> void {
        int uniform_count{ 0 };

        glGetProgramiv( program_handle, GL_ACTIVE_UNIFORMS, &uniform_count );

        if ( equal( uniform_count, 0 ) ) {
            LOG_INFO( QuillPtr(), "No uniforms identified in the current shader program." );
            return;
        }

        GLint max_name_len{ 0 };

        glGetProgramiv( program_handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len );

        GLsizei length{ 0 };
        GLsizei uni_count{ 0 };
        GLenum type{ GL_NONE };

        // NOLINTNEXTLINE
        auto uniform_name{ std::make_unique< char[] >( max_name_len ) };

        for ( const int index : std::views::iota( 0, uniform_count ) ) {
            glGetActiveUniform( program_handle, index, max_name_len, &length, &uni_count, &type, uniform_name.get() );

            if ( uniform_name ) {
                const uniform_info uniform_data{ .location = glGetUniformLocation( program_handle, uniform_name.get() ),
                                                 .count    = uni_count,
                                                 .type     = type };

                uniforms.emplace( std::string( uniform_name.get() ), uniform_data );
            }
        }
    }

    template <>
    constexpr auto ShaderProgram::SetUniform( const std::string& name, const bool& value ) const noexcept -> void {
        glProgramUniform1i(
            program_handle, glGetUniformLocation( program_handle, name.c_str() ), static_cast< int >( value ) );
    }

    template <>
    constexpr auto ShaderProgram::SetUniform( const std::string& name, const int& value ) const noexcept -> void {
        if ( const auto uniform = uniforms.find( name ); not_equal( uniform, uniforms.end() ) ) {
            glProgramUniform1i( program_handle, uniform->second.location, value );
        }
    }

    template <>
    constexpr auto ShaderProgram::SetUniform( const std::string& name, const float& value ) const noexcept -> void {
        if ( const auto uniform = uniforms.find( name ); not_equal( uniform, uniforms.end() ) ) {
            glProgramUniform1f( program_handle, uniform->second.location, value );
        }
    }

    template <>
    constexpr auto ShaderProgram::SetUniform< glm::vec< 2, float > >(
        const std::string& name, const glm::vec< 2, float >& value ) const noexcept -> void {
        if ( const auto uni = uniforms.find( name ); not_equal( uni, uniforms.end() ) ) {
            glProgramUniform2fv( program_handle, uni->second.location, uni->second.count, &value[0] );
        }
    }

    template <>
    constexpr auto ShaderProgram::SetUniform( const std::string& name, const float xpos, const float y ) const noexcept
        -> void {
        if ( const auto uni = uniforms.find( name ); not_equal( uni, uniforms.end() ) ) {
            glProgramUniform2f( program_handle, uni->second.location, xpos, y );
        }
    }

    template <>
    constexpr auto ShaderProgram::SetUniform< glm::vec< 3, float > >(
        const std::string& name, const glm::vec< 3, float >& value ) const noexcept -> void {
        if ( const auto uni = uniforms.find( name ); not_equal( uni, uniforms.end() ) ) {
            glProgramUniform3fv( program_handle, uni->second.location, uni->second.count, &value[0] );
        }
    }

    template <>
    constexpr auto ShaderProgram::SetUniform(
        const std::string& name, const float xpos, const float ypos, const float zpos ) const noexcept -> void {
        if ( const auto uni = uniforms.find( name ); not_equal( uni, uniforms.end() ) ) {
            glProgramUniform3f( program_handle, uni->second.location, xpos, ypos, zpos );
        } else {
            LOG_ERROR( QuillPtr(), "{} is not a registered uniform.", name );
        }
    }

    template <>
    constexpr auto ShaderProgram::SetUniform< glm::vec< 4, float > >(
        const std::string& name, const glm::vec< 4, float >& value ) const noexcept -> void {
        if ( const auto uni = uniforms.find( name ); not_equal( uni, uniforms.end() ) ) {
            glProgramUniform4fv( program_handle, uni->second.location, uni->second.count, &value[0] );
        }
    }

    template <>
    constexpr auto ShaderProgram::SetUniform(
        const std::string& name, const float xpos, const float ypos, const float zpos, const float wpos ) const noexcept
        -> void {
        if ( const auto uni = uniforms.find( name ); not_equal( uni, uniforms.end() ) ) {
            glProgramUniform4f( program_handle, uni->second.location, xpos, ypos, zpos, wpos );
        }
    }

    template <>
    constexpr auto ShaderProgram::SetUniform< glm::mat< 2, 2, float > >(
        const std::string& name, const glm::mat< 2, 2, float >& value ) const noexcept -> void {
        if ( const auto uni = uniforms.find( name ); not_equal( uni, uniforms.end() ) ) {
            glProgramUniformMatrix2fv(
                program_handle, uni->second.location, uni->second.count, GL_FALSE, glm::value_ptr( value ) );
        }
    }

    template <>
    constexpr auto ShaderProgram::SetUniform< glm::mat< 3, 3, float > >(
        const std::string& name, const glm::mat< 3, 3, float >& value ) const noexcept -> void {
        if ( const auto uni = uniforms.find( name ); not_equal( uni, uniforms.end() ) ) {
            glProgramUniformMatrix3fv(
                program_handle, uni->second.location, uni->second.count, GL_FALSE, glm::value_ptr( value ) );
        }
    }

    template <>
    constexpr auto ShaderProgram::SetUniform< glm::mat< 4, 4, float > >(
        const std::string& name, const glm::mat< 4, 4, float >& value ) const noexcept -> void {
        if ( const auto uni = uniforms.find( name ); not_equal( uni, uniforms.end() ) ) {
            glProgramUniformMatrix4fv(
                program_handle, uni->second.location, uni->second.count, GL_FALSE, glm::value_ptr( value ) );
        }
    }

    template <>
    constexpr auto ShaderProgram::SetUniform( const ResourceHandle< Texture2D >& resource_handle ) const noexcept
        -> void {
        SetUniform( resource_handle->GetId(), resource_handle->TextureUnit() );
    }

    template <>
    constexpr auto ShaderProgram::SetUniform( const ResourceHandle< Texture3D >& resource_handle ) const noexcept
        -> void {
        SetUniform( resource_handle->GetId(), resource_handle->TextureUnit() );
    }

    template <>
    constexpr auto ShaderProgram::SetUniform( const ResourceHandle< CubeTexture >& resource_handle ) const noexcept
        -> void {
        SetUniform( resource_handle->GetId(), resource_handle->TextureUnit() );
    }

} // namespace PeanutGL
