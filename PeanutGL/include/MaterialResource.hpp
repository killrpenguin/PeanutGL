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
#include "GeneratedConstants.hpp"
#include "ResourceBase.hpp"
#include "Utilities.hpp"

#include <glad/gl.h>
#include <quill/LogMacros.h>
#include <stb/stb_image.h>

namespace PeanutGL {
    namespace fs = std::filesystem;
    /**
     * @brief Class for representing a texture resource.
     *
     * This class stores the GPU information for vertex attributes and index information of geometric data.
     */
    template < GLenum Target > class Material final : public Resource {
      private:
        unsigned int handle{};
        unsigned int texture_unit{};

        GLenum format{};
        GLenum internal_format{};

        fs::path file_name{};

        int width{ 0 };
        int height{ 0 };
        int nr_channels{ 0 };
        bool flip_image{ false };

        constexpr auto load_texture( const std::string& name ) noexcept -> stbi_uc* {
            stbi_set_flip_vertically_on_load( static_cast< int >( flip_image ) );

            auto* data{ stbi_load( name.c_str(), &width, &height, &nr_channels, 0 ) };

            assert( data != nullptr && "The texture info data field is a nullptr." );
            assert( width != 0 && "The texture info width field cannot be 0." );
            assert( height != 0 && "The texture info height field cannot be 0." );
            assert( nr_channels != 0 && "The texture info nr_channels field cannot be 0." );

            return data;
        }

        constexpr auto SetFormat() noexcept -> void {
            if ( equal( file_name.extension(), ".png" ) ) {
                format          = GL_RGBA;
                internal_format = GL_RGBA8;
            } else if ( equal( file_name.extension(), ".jpg" ) ) {
                format          = GL_RGB;
                internal_format = GL_RGB8;
            } else {
                LOG_ERROR( QuillPtr(), "Could not identify Texture resource extension type." );
            }
        }

      public:
        Material() noexcept = delete;

        // Delete the copy constructor and copy assignment operator. Resources are stored as unique pointers in
        // the resource manager.
        Material( const Material& )                = delete;
        Material( Material&& ) noexcept            = default;
        Material& operator=( const Material& )     = delete;
        Material& operator=( Material&& ) noexcept = default;

        /**
         * @brief Constructor for a Material resource.
         *
         * This constructor may catch an implementation-defined exceptions if fs::path throws.
         */
        explicit Material( const std::string_view file, const unsigned int unit, bool flip_image = false ) noexcept try
            : Resource( file.substr( 0, file.find( '.' ) ) ),
              texture_unit{ unit },
              file_name{ std::string( ASSETS_ROOT ) + "/" + file },
              flip_image{ flip_image } {
            SetFormat();
            Load();
            SetTextureUnit( texture_unit );
        } catch ( const std::exception& err ) {
            LOG_ERROR( QuillPtr(), "Error during initialization of Material constructor.\n\tnWhat: {}", err.what() );
        }

        ~Material() override {
            Unload();
        }

        auto Unload() noexcept -> void override {
            glDeleteTextures( 1, &handle );
        }

        auto Load() noexcept -> bool override {
            stbi_uc* const pixels{ load_texture( file_name ) };

            if ( equal( pixels, nullptr ) ) {
                LOG_WARNING( QuillPtr(), "Image library could not load {}.", file_name.string() );
                return false;
            }

            glCreateTextures( Target, 1, &handle );

            glTextureParameteri( handle, GL_TEXTURE_WRAP_S, GL_REPEAT );
            glTextureParameteri( handle, GL_TEXTURE_WRAP_T, GL_REPEAT );
            glTextureParameteri( handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTextureParameteri( handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

            glTextureStorage2D( handle, 1, internal_format, width, height );
            glTextureSubImage2D( handle, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, pixels );

            glGenerateTextureMipmap( handle );

            stbi_image_free( pixels );

            return true;
        }

        constexpr auto SetTextureUnit( const unsigned int unit ) const noexcept -> void {
            glBindTextureUnit( unit, handle );
        }

        constexpr auto TextureUnit() const noexcept -> int {
            return static_cast< int >( texture_unit );
        }
    };

    using Texture2D   = Material< GL_TEXTURE_2D >;
    using Texture3D   = Material< GL_TEXTURE_3D >;
    using CubeTexture = Material< GL_TEXTURE_CUBE_MAP >;

} // Namespace PeanutGL
