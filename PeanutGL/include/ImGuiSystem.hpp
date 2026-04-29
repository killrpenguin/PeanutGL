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

#include "Platform.hpp"
#include "Utilities.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <cstdint>
#include <stdexcept>

namespace PeanutGL {

    class Renderer;

    class ImGuiSystem {
      private:
        ImGuiContext* context{ nullptr };

        Width width{ 0 };
        Height height{ 0 };

        XAxis mouseX{ 0.0 };
        YAxis mouseY{ 0.0 };

        std::uint32_t mouseButtons{ 0 };

        bool initialized{ false };
        bool cameraTrackingEnabled{ false };
        bool frameAlreadyRendered{ false };

        /**
         * @brief Initialize the ImGui system (called by constructor).
         * @param renderer Pointer to the renderer.
         * @param width The width of the window.
         * @param height The height of the window.
         * @return True if initialization was successful, false otherwise.
         */
        constexpr auto Initialize(
            // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
            Platform* platform, const int init_width, const int init_height ) noexcept -> bool {
            if ( initialized ) { return true; }

            this->width  = Width( init_width );
            this->height = Height( init_height );

            const float main_scale{ ImGui_ImplGlfw_GetContentScaleForMonitor( glfwGetPrimaryMonitor() ) };

            context = ImGui::CreateContext();

            if ( equal( context, nullptr ) ) {
                std::cerr << "Failed to create ImGui context" << '\n';
                return false;
            }

            ImGuiIO& imgui_io = ImGui::GetIO();

            imgui_io.ConfigFlags = safe_or_assign< ImGuiConfigFlags_NavEnableKeyboard >( imgui_io.ConfigFlags );
            imgui_io.ConfigFlags = safe_or_assign< ImGuiConfigFlags_NavEnableGamepad >( imgui_io.ConfigFlags );

            // imgui_io.ConfigFlags = safe_or_assign< ImGuiConfigFlags_DockingEnable >( imgui_io.ConfigFlags );
            // imgui_io.ConfigFlags = safe_or_assign< ImGuiConfigFlags_ViewportsEnable >( imgui_io.ConfigFlags );

            imgui_io.ConfigDpiScaleFonts     = true;
            imgui_io.ConfigDpiScaleViewports = true;

            ImGuiStyle& style = ImGui::GetStyle();
            style.ScaleAllSizes( main_scale );
            style.FontScaleDpi              = main_scale;
            style.Colors[ImGuiCol_WindowBg] = ImVec4( 0.0F, 0.0F, 0.0F, 0.0F );

            // NOLINTNEXTLINE(hicpp-signed-bitwise)
            if ( not_equal( ( imgui_io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable ), 0 ) ) {
                style.WindowRounding              = 0.0F;
                style.Colors[ImGuiCol_WindowBg].w = 1.0F;
            }

            ImGui_ImplGlfw_InitForOpenGL( platform->GetWindow(), true );
            ImGui_ImplOpenGL3_Init( platform->GetGLSLVersion() );

            return true;
        }

      public:
        constexpr ImGuiSystem() noexcept = default;

        // NOLINTNEXTLINE
        constexpr ImGuiSystem( Platform* platform, const int width, const int height ) {
            if ( !Initialize( platform, width, height ) ) {
                throw std::runtime_error( "ImGuiSystem: initialization failed" );
            }
        }

        /**
         * @brief Clean up ImGui resources.
         */
        auto Cleanup() noexcept -> void;

        /**
         * @brief Start a new ImGui frame.
         */
        auto static NewFrame() noexcept -> void;

        /**
         * @brief Render the ImGui frame.
         */
        auto static Render() noexcept -> void;

        /**
         * @brief Handle mouse input.
         * @param xpos The x-coordinate of the mouse.
         * @param ypos The y-coordinate of the mouse.
         * @param buttons The state of the mouse buttons.
         */
        auto HandleMouse( const XAxis& xpos, const YAxis& ypos, const std::uint32_t buttons ) noexcept -> void;

        /**
         * @brief Handle character input.
         * @param c The character.
         */
        constexpr auto HandleChar( const std::uint32_t character ) const noexcept -> void {
            if ( !initialized ) { return; }

            ImGuiIO& imgui_io{ ImGui::GetIO() };
            imgui_io.AddInputCharacter( character );
        }

        /**
         * @brief Handle keyboard input.
         * @param key The key code.
         * @param pressed Whether the key was pressed or released.
         */
        constexpr auto HandleKeyboard( const std::uint32_t key, const bool pressed ) const noexcept -> void {
            if ( !initialized ) { return; }

            ImGuiIO& imgui_io = ImGui::GetIO();

            imgui_io.AddKeyEvent( static_cast< ImGuiKey >( key ), pressed );
        }

        /**
         * @brief Handle window resize.
         * @param width The new width of the window.
         * @param height The new height of the window.
         */
        constexpr auto HandleResize( const Width new_width, const Height new_height ) noexcept -> void {
            if ( !initialized ) { return; }

            this->width  = Width( width );
            this->height = Height( height );

            ImGuiIO& input_output = ImGui::GetIO();
            input_output.DisplaySize =
                ImVec2( static_cast< float >( new_width() ), static_cast< float >( new_height() ) );
        }

        /**
         * @brief Check if ImGui wants to capture keyboard input.
         * @return True if ImGui wants to capture keyboard input, false otherwise.
         */
        auto WantCaptureKeyboard() const noexcept -> bool {
            if ( !initialized ) { return false; }

            return ImGui::GetIO().WantCaptureKeyboard;
        }

        /**
         * @brief Check if ImGui wants to capture mouse input.
         * @return True if ImGui wants to capture mouse input, false otherwise.
         */
        auto WantCaptureMouse() const noexcept -> bool {
            if ( !initialized ) { return false; }

            return ImGui::GetIO().WantCaptureMouse;
        }

        /**
         * @brief Get the current camera tracking state.
         * @return True if camera tracking is enabled, false otherwise.
         */
        constexpr auto IsCameraTrackingEnabled() const noexcept -> bool {
            return cameraTrackingEnabled;
        }

        /**
         * @brief Check if ImGui has already been rendered for the current frame.
         * @return True if Render() was already called in NewFrame(), false otherwise.
         */
        constexpr auto IsFrameRendered() const noexcept -> bool {
            return frameAlreadyRendered;
        }
    };
} // namespace PeanutGL
