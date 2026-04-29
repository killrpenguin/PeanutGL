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

#include "RenderDocSystem.hpp"
#include "DebugSystem.hpp"
#include "Utilities.hpp"

#include "quill/LogMacros.h"

#include "renderdoc_app.h"

#include <dlfcn.h>
#include <filesystem>
#include <string>

namespace PeanutGL {
    RenderDocSystem::RenderDocSystem() noexcept {
        LoadRenderDocAPI();
    }

    auto RenderDocSystem::configure_renderdoc_api() noexcept -> bool {
        namespace fs = std::filesystem;

        assert( not_equal( rdoc_api, nullptr ) && "The rdoc_api pointer has become invalid." );

        assert(
            fs::is_directory( file_path_template_folder ) &&
            "The generated renderdoc file path template folder is invalid." );

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
        rdoc_api->SetCaptureFilePathTemplate( file_path_template_folder.c_str() );

        // Disable renderdoc default frame count and keybinding overlay for a cleaner interface.
        rdoc_api->MaskOverlayBits( eRENDERDOC_Overlay_None, eRENDERDOC_Overlay_None );

        // Control renderdoc through PeanutGL. Not the default keybinding setup.
        rdoc_api->SetCaptureKeys( nullptr, 0 );

        const int capture_callstack{ rdoc_api->SetCaptureOptionU32( eRENDERDOC_Option_CaptureCallstacks, 1 ) };

        const int only_actions_callstack{ rdoc_api->SetCaptureOptionU32(
            eRENDERDOC_Option_CaptureCallstacksOnlyActions, 1 ) };

        return equal( capture_callstack, 1 ) and equal( only_actions_callstack, 1 );
    }

    auto RenderDocSystem::LoadRenderDocAPI() -> bool {
        if ( renderdoc_avaliable ) { return true; }

        // NOLINTNEXTLINE(concurrency-mt-unsafe)
        dlerror();
        renderdoc_lib_handle = dlopen( "librenderdoc.so", RTLD_NOW | RTLD_NOLOAD );

        if ( equal( renderdoc_lib_handle, nullptr ) ) {
            // NOLINTNEXTLINE(concurrency-mt-unsafe)
            const char* error{ dlerror() };

            if ( not_equal( error, nullptr ) ) {
                LOG_INFO( QuillPtr(), "Failed to get renderdoc library handle: {}", error );
            }

            return false;
        }

        auto RENDERDOC_GetAPI =
            reinterpret_cast< pRENDERDOC_GetAPI >( dlsym( renderdoc_lib_handle, "RENDERDOC_GetAPI" ) );

        const int result{ RENDERDOC_GetAPI( eRENDERDOC_API_Version_1_4_1, reinterpret_cast< void** >( &rdoc_api ) ) };

        if ( renderdoc_avaliable = equal( result, 1 ) and not_equal( rdoc_api, nullptr ); !renderdoc_avaliable ) {
            LOG_WARNING( QuillPtr(), "Failed to acquire RenderDoc API." );
            return false;
        }

        if ( renderdoc_avaliable = configure_renderdoc_api(); !renderdoc_avaliable ) {
            LOG_WARNING( QuillPtr(), "Failed to acquire RenderDoc API." );
        }

        return renderdoc_avaliable;
    }

    auto RenderDocSystem::TriggerCapture() -> void {
        if ( !renderdoc_avaliable and !LoadRenderDocAPI() ) { return; }

        assert( not_equal( rdoc_api, nullptr ) && "The rdoc_api pointer has become invalid." );

        rdoc_api->TriggerCapture();

        /*
          TODO: When I add the inotify monitor to the resource manager I should log this message
          after a new capture has been written to the renderdoc capture folder.
         */

        LOG_INFO( QuillPtr(), "Capture triggered.\n Capture count: {}", rdoc_api->GetNumCaptures() );
    }

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    auto RenderDocSystem::StartFrameCapture( void* device, void* window ) noexcept -> void {
        if ( !renderdoc_avaliable and !LoadRenderDocAPI() ) { return; }

        assert( not_equal( rdoc_api, nullptr ) && "The rdoc_api pointer has become invalid." );

        rdoc_api->StartFrameCapture( device, window );

        LOG_INFO( QuillPtr(), "Framecapture started." );
    }

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    auto RenderDocSystem::EndFrameCapture( void* device, void* window ) noexcept -> void {
        if ( !renderdoc_avaliable and !LoadRenderDocAPI() ) { return; }

        assert( not_equal( rdoc_api, nullptr ) && "The rdoc_api pointer has become invalid." );

        const std::uint32_t success{ rdoc_api->EndFrameCapture( device, window ) };

        if ( not_equal( success, 1 ) ) { LOG_WARNING( QuillPtr(), "Frame capture failed" ); }

        LOG_INFO( QuillPtr(), "Framecapture ended." );
    }

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    auto RenderDocSystem::DiscardFrameCapture( void* device, void* window ) noexcept -> void {
        if ( !renderdoc_avaliable and !LoadRenderDocAPI() ) { return; }

        assert( not_equal( rdoc_api, nullptr ) && "The rdoc_api pointer has become invalid." );

        const std::uint32_t success{ rdoc_api->DiscardFrameCapture( device, window ) };

        if ( not_equal( success, 1 ) ) { LOG_WARNING( QuillPtr(), "Frame capture failed" ); }

        LOG_INFO( QuillPtr(), "Framecapture discarded." );
    }

} // namespace PeanutGL
