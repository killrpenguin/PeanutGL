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

#include <glad/gl.h>
#include <stb/stb_image.h>

namespace PeanutGL {
    /**
     * @brief Class for representing a texture resource.
     *
     * This class stores the GPU information for vertex attributes and index information of geometric data.
     */
    template < GLenum Target > class Material final : public Resource {
      private:
        unsigned int handle{};

        std::string file_name{};

        int width{ 0 };
        int height{ 0 };
        int nr_channels{ 0 };

        constexpr auto load_texture( const std::string& name ) noexcept -> stbi_uc* {
            constexpr int YES{ 1 };

            stbi_set_flip_vertically_on_load( YES );
            auto* data{ stbi_load( name.c_str(), &width, &height, &nr_channels, 0 ) };

            assert( data != nullptr && "The texture info data field is a nullptr." );
            assert( width != 0 && "The texture info width field cannot be 0." );
            assert( height != 0 && "The texture info height field cannot be 0." );
            assert( nr_channels != 0 && "The texture info nr_channels field cannot be 0." );

            return data;
        }

      public:
        Material() noexcept = default;

        // Delete the copy constructor and copy assignment operator. Resources are stored as unique pointers in
        // the resource manager.
        Material( const Material& )                = delete;
        Material( Material&& ) noexcept            = default;
        Material& operator=( const Material& )     = delete;
        Material& operator=( Material&& ) noexcept = default;

        // NOLINTNEXTLINE
        explicit Material( const std::string& identifier, std::string file ) noexcept
            : Resource( identifier ), file_name{ std::move( file ) } {
            Load();
        }

        ~Material() override {
            Unload();
        }

        auto Unload() noexcept -> void override {
            glDeleteTextures( 1, &handle );
        }

        auto Load() noexcept -> bool override {
            glCreateTextures( Target, 1, &handle );

            if constexpr ( Target == GL_TEXTURE_2D ) {
                glTextureParameteri( handle, GL_TEXTURE_WRAP_S, GL_REPEAT );
                glTextureParameteri( handle, GL_TEXTURE_WRAP_T, GL_REPEAT );
                glTextureParameteri( handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                glTextureParameteri( handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

                stbi_uc* const pixels{ load_texture( file_name ) };

                glTextureStorage2D( handle, 1, GL_RGB8, width, height );
                glTextureSubImage2D( handle, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels );

                glGenerateTextureMipmap( handle );

                stbi_image_free( pixels );
            }
            return true;
        }
    };

    using Texture2D = Material< GL_TEXTURE_2D >;

} // Namespace PeanutGL
