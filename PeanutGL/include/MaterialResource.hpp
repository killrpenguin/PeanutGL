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
// #include "Utilities.hpp"

#include <glad/gl.h>
#include <stb/stb_image.h>

namespace PeanutGL {
    /**
     * @brief Class for representing .
     *
     * This class stores the GPU information for vertex attributes and index information of geometric data.
     */
    class Material : public Resource {
      private:
      public:
        Material() noexcept = default;

        // Delete the copy constructor and copy assignment operator. Resources are stored as unique pointers in
        // the resource manager.
        Material( const Material& )                = delete;
        Material( Material&& ) noexcept            = default;
        Material& operator=( const Material& )     = delete;
        Material& operator=( Material&& ) noexcept = default;

        explicit Material( const std::string& identifier ) noexcept
            : Resource( identifier ) {
            Load();
        }

        ~Material() override {
            Unload();
        }

        auto Unload() noexcept -> void override {
        }

        auto Load() noexcept -> bool override {
        }
    };
} // Namespace PeanutGL
