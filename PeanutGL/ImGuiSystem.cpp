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

#include "ImGuiSystem.hpp"
#include "Utilities.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <cstdint>

namespace PeanutGL {
    auto ImGuiSystem::Cleanup() noexcept -> void {
        if ( !initialized ) { return; }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        if ( context != nullptr ) {
            ImGui::DestroyContext( context );
            context = nullptr;
        }

        initialized = false;
    }

    auto ImGuiSystem::Render() noexcept -> void {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent( backup_current_context ); // Restore context (required for OpenGL)
    }

    auto ImGuiSystem::NewFrame() noexcept -> void {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport* viewport{ ImGui::GetMainViewport() };
        ImGui::DockSpaceOverViewport( 0, viewport, ImGuiDockNodeFlags_None ); // Sets Dockspace to transparent.

        ImGui::ShowDemoWindow();
    }

    auto ImGuiSystem::HandleMouse( const XAxis& xpos, const YAxis& ypos, const std::uint32_t buttons ) noexcept
        -> void {
        // NOTE FOR LATER: Using MultiViewPort settings changes the ImGui mouse information to a relative coordinate
        // system.

        if ( !initialized ) { return; }

        mouseButtons = buttons;

        ImGuiIO& imgui_io{ ImGui::GetIO() };

        // Update mouse position
        imgui_io.MousePos = ImVec2( static_cast< float >( xpos() ), static_cast< float >( ypos() ) );

        // Update mouse buttons
        constexpr static int LeftMouseButton{ 0 };
        constexpr static int MiddleMouseButton{ 1 };
        constexpr static int RightMouseButton{ 2 };

        // NOLINTBEGIN
        if ( ( buttons & 0x01 ) != 0 ) { imgui_io.AddMouseButtonEvent( LeftMouseButton, true ); }
        if ( ( buttons & 0x02 ) != 0 ) { imgui_io.AddMouseButtonEvent( RightMouseButton, true ); }
        if ( ( buttons & 0x04 ) != 0 ) { imgui_io.AddMouseButtonEvent( MiddleMouseButton, true ); }
        // NOLINTEND
    }
} // namespace PeanutGL
