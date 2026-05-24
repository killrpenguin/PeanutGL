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
#include "renderdoc_app.h"

#include <dlfcn.h>

#include <quill/LogMacros.h>

namespace PeanutGL {

    /**
     * @brief A class for managing renderdoc integration.
     */
    class RenderDocSystem {
      public:
        RenderDocSystem() noexcept;

        ~RenderDocSystem() {
            if ( renderdoc_avaliable ) { dlclose( renderdoc_lib_handle ); }
        }

        RenderDocSystem( RenderDocSystem&& ) noexcept            = delete;
        RenderDocSystem& operator=( RenderDocSystem&& ) noexcept = delete;

        RenderDocSystem( const RenderDocSystem& )            = delete;
        RenderDocSystem& operator=( const RenderDocSystem& ) = delete;

        /**
         * @brief Initialize the singleton instance of the renderdoc system.
         */
        static void Initialize() noexcept {
            static RenderDocSystem instance;
        }

        /**
         * @brief Get the singleton instance of the renderdoc system.
         * @return Reference to the renderdoc system instance.
         */
        static RenderDocSystem& Get() noexcept {
            static RenderDocSystem instance;
            return instance;
        }

        /**
         * @brief Attempt to load the RenderDoc API from the current process. Safe to call multiple times
         */
        auto LoadRenderDocAPI() -> bool;

        /**
         * @brief Returns true if the RenderDoc API has been successfully loaded.
         */
        constexpr auto IsAvailable() const noexcept -> bool {
            return renderdoc_avaliable;
        }

        /**
         * @brief Triggers an immediate capture (equivalent to pressing the capture hotkey in the UI).
         */
        auto TriggerCapture() -> void;

        /**
         * @brief Starts a frame capture for the given device/window (can be nullptr to auto-detect on many backends).
         */
        auto StartFrameCapture( void* device = nullptr, void* window = nullptr ) noexcept -> void;

        /**
         * @brief Ends a previously started frame capture.
         */
        auto EndFrameCapture( void* device = nullptr, void* window = nullptr ) noexcept -> void;

        /**
         * @brief Ends a previously started frame capture. The capture will be immediately discarded.
         */
        auto DiscardFrameCapture( void* device, void* window ) noexcept -> void;

      private:
        auto configure_renderdoc_api() noexcept -> bool;

        void* renderdoc_lib_handle{ nullptr };

        RENDERDOC_API_1_4_1* rdoc_api{ nullptr };

        bool renderdoc_avaliable{ false };

        std::filesystem::path file_path_template_folder{ FILE_PATH_TEMPLATE_DIR };
    };
} // namespace PeanutGL
