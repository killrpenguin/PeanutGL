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

#include "Utilities.hpp"

// clang-format off
#include "glad/gl.h"
#include <GLFW/glfw3.h>
// clang-format on

#include <functional>
#include <memory>
#include <string>
#include <utility>

namespace PeanutGL {

    /**
     * @brief Interface for platform-specific functionality.
     *
     * This class implements the platform abstraction as described in the Engine_Architecture chapter:
     * @see en/Building_a_Simple_Engine/Engine_Architecture/02_architectural_patterns.adoc
     */
    class PlatformBase {
      public:
        /**
         * @brief Default constructor.
         */
        constexpr PlatformBase() noexcept = default;

        PlatformBase( const PlatformBase& )                = default;
        PlatformBase( PlatformBase&& ) noexcept            = default;
        PlatformBase& operator=( const PlatformBase& )     = default;
        PlatformBase& operator=( PlatformBase&& ) noexcept = default;
        /**
         * @brief Virtual destructor for proper cleanup.
         */
        virtual ~PlatformBase() = default;

        /**
         * @brief Initialize the platform.
         * @param appName The name of the application.
         * @param width The width of the window.
         * @param height The height of the window.
         * @return True if initialization was successful, false otherwise.
         */
        virtual auto Initialize(
            const std::string& appName, const Width requested_width, const Height requested_height ) -> bool = 0;

        /**
         * @brief Clean up platform resources.
         */
        virtual auto Cleanup() -> void = 0;

        /**
         * @brief Process platform events.
         * @return True if the application should continue running, false if it should exit.
         */
        virtual auto ProcessEvents() -> bool = 0;

        /**
         * @brief Check if the window has been resized.
         * @return True if the window has been resized, false otherwise.
         */
        [[nodiscard]] virtual constexpr auto HasWindowResized() noexcept -> bool = 0;

        /**
         * @brief Get the current window width.
         * @return The window width.
         */
        [[nodiscard]] virtual constexpr auto GetWindowWidth() const noexcept -> Width = 0;

        /**
         * @brief Get the current window height.
         * @return The window height.
         */
        [[nodiscard]] virtual constexpr auto GetWindowHeight() const noexcept -> Height = 0;

        /**
         * @brief Get the current window size.
         * @param width Pointer to store the window width.
         * @param height Pointer to store the window height.
         */
        virtual constexpr auto GetWindowSize( Width* width, Height* height ) const noexcept -> void {
            *width  = GetWindowWidth();
            *height = GetWindowHeight();
        }

        /**
         * @brief Set a callback for framebuffer resize events.
         * @param callback The callback function to be called when the framebuffer is resized.
         */
        virtual constexpr auto SetFramebufferResizeCallback( std::function< void( Width, Height ) > callback ) noexcept
            -> void = 0;

        /**
         * @brief Set a callback for window resize events.
         * @param callback The callback function to be called when the window is resized.
         */
        virtual constexpr auto SetWindowResizeCallback( std::function< void( Width, Height ) > callback ) noexcept
            -> void = 0;

        /**
         * @brief Set a callback for mouse input events.
         * @param callback The callback function to be called when mouse input is received.
         */
        virtual constexpr auto SetMouseCallback(
            std::function< void( XAxis, YAxis, std::uint32_t ) > callback ) noexcept -> void = 0;

        /**
         * @brief Set a callback for keyboard input events.
         * @param callback The callback function to be called when keyboard input is received.
         */
        virtual constexpr auto SetKeyboardCallback( std::function< void( std::uint32_t, bool ) > callback ) noexcept
            -> void = 0;

        /**
         * @brief Set a callback for character input events.
         * @param callback The callback function to be called when character input is received.
         */
        virtual constexpr auto SetCharCallback( std::function< void( std::uint32_t ) > callback ) noexcept -> void = 0;

        /**
         * @brief Set the window title.
         * @param title The new window title.
         */
        virtual constexpr auto SetWindowTitle( const std::string& title ) noexcept -> void = 0;
    };

    class Platform final : public PlatformBase {
      private:
        GLFWwindow* window{ nullptr };

        constexpr static std::string GLSL_Version{ "#version 460" };

        Width width{ 0 };
        Height height{ 0 };

        bool window_resized{ false };

        std::function< void( Width, Height ) > framebuffer_resize_callback;
        std::function< void( Width, Height ) > window_resize_callback;
        std::function< void( XAxis, YAxis, std::uint32_t ) > mouse_callback;
        std::function< void( std::uint32_t, bool ) > keyboard_callback;
        std::function< void( std::uint32_t ) > char_callback;

        /**
         * @brief Static callback for OpenGL error message callback.
         * @param source The source of the debug message (e.g., GL_DEBUG_SOURCE_API, GL_DEBUG_SOURCE_SHADER_COMPILER).
         * @param type The type of the debug message.
         * @param id A unique ID number for the specific message.
         * @param severity The severity of the message (e.g., GL_DEBUG_SEVERITY_HIGH, GL_DEBUG_SEVERITY_NOTIFICATION).
         * @param length The length of the message string.
         * @param message A pointer to a C-style string containing the debug message description.
         * @param userParam The user-supplied pointer that was originally passed to glDebugMessageCallback.
         */
        static void GLAPIENTRY GLDebugMessageCallback(
            GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
            const void* userParam );

        /**
         * @brief Static callback for GLFW internal error callback.
         * @param error_code A GLFW error code.
         * @param description A UTF-8 encoded string describing the error.
         */
        static void GLFWErrorCallback( int error_code, const char* description );

        /**
         * @brief Static callback for GLFW framebuffer resize events.
         * @param window The GLFW window.
         * @param width The new width.
         * @param height The new height.
         */
        static void FramebufferResizeCallback( GLFWwindow* window, int width, int height );

        /**
         * @brief Static callback for GLFW window resize events.
         * @param window The GLFW window.
         * @param width The new width.
         * @param height The new height.
         */
        static void WindowResizeCallback( GLFWwindow* window, int width, int height );

        /**
         * @brief Static callback for GLFW mouse position events.
         * @param window The GLFW window.
         * @param xpos The x-coordinate of the cursor.
         * @param ypos The y-coordinate of the cursor.
         */
        static void MousePositionCallback( GLFWwindow* window, double xpos, double ypos );

        /**
         * @brief Static callback for GLFW mouse button events.
         * @param window The GLFW window.
         * @param button The mouse button that was pressed or released.
         * @param action The action (GLFW_PRESS or GLFW_RELEASE).
         * @param mods The modifier keys that were held down.
         */
        static void MouseButtonCallback( GLFWwindow* window, int button, int action, int mods );

        /**
         * @brief Static callback for GLFW keyboard events.
         * @param window The GLFW window.
         * @param key The key that was pressed or released.
         * @param scancode The system-specific scancode of the key.
         * @param action The action (GLFW_PRESS, GLFW_RELEASE, or GLFW_REPEAT).
         * @param mods The modifier keys that were held down.
         */
        static void KeyCallback( GLFWwindow* window, int key, int scancode, int action, int mods );

        /**
         * @brief Static callback for GLFW character events.
         * @param window The GLFW window.
         * @param codepoint The Unicode code point of the character.
         */
        static void CharCallback( GLFWwindow* window, unsigned int codepoint );

      public:
        /**
         * @brief Default constructor.
         */
        constexpr Platform() noexcept = default;

        /**
         * @brief Parent destructor override.
         */
        ~Platform() override {
            Cleanup();
        }

        // Our application should never need to copy a platform.
        // There is only ever one but I do not want an explicit singleton.
        Platform( const Platform& )                = delete;
        Platform( Platform&& ) noexcept            = default;
        Platform& operator=( const Platform& )     = delete;
        Platform& operator=( Platform&& ) noexcept = default;

        /**
         * @brief Initialize the platform.
         * @param appName The name of the application.
         * @param width The width of the window.
         * @param height The height of the window.
         * @return True if initialization was successful, false otherwise.
         */
        auto Initialize( const std::string& appName, const Width requested_width, const Height requested_height )
            -> bool override;

        /**
         * @brief Clean up platform resources.
         */
        auto Cleanup() -> void override;

        /**
         * @brief Process platform events.
         * @return True if the application should continue running, false if it should exit.
         */
        auto ProcessEvents() noexcept -> bool override;

        /**
         * @brief Check if the window has been resized.
         * @return True if the window has been resized, false otherwise.
         */
        auto HasWindowResized() noexcept -> bool override;

        /**
         * @brief Get the current window width.
         * @return The window width.
         */
        [[nodiscard]] constexpr auto GetWindowWidth() const noexcept -> Width override {
            return width;
        }

        /**
         * @brief Get the current window height.
         * @return The window height.
         */
        [[nodiscard]] constexpr auto GetWindowHeight() const noexcept -> Height override {
            return height;
        }

        /**
         * @brief Set a callback for framebuffer resize events.
         * @param callback The callback function to be called when the framebuffer is resized.
         */
        constexpr auto SetFramebufferResizeCallback( std::function< void( Width, Height ) > callback ) noexcept
            -> void override {
            framebuffer_resize_callback = std::move( callback );
        }

        /**
         * @brief Set a callback for window resize events.
         * @param callback The callback function to be called when the window is resized.
         */
        constexpr auto SetWindowResizeCallback( std::function< void( Width, Height ) > callback ) noexcept
            -> void override {
            window_resize_callback = std::move( callback );
        }

        /**
         * @brief Set a callback for mouse input events.
         * @param callback The callback function to be called when mouse input is received.
         */
        constexpr auto SetMouseCallback( std::function< void( XAxis, YAxis, std::uint32_t ) > callback ) noexcept
            -> void override {
            mouse_callback = std::move( callback );
        }

        /**
         * @brief Set a callback for keyboard input events.
         * @param callback The callback function to be called when keyboard input is received.
         */
        constexpr auto SetKeyboardCallback( std::function< void( std::uint32_t, bool ) > callback ) noexcept
            -> void override {
            keyboard_callback = std::move( callback );
        }

        /**
         * @brief Set a callback for character input events.
         * @param callback The callback function to be called when character input is received.
         */
        constexpr auto SetCharCallback( std::function< void( std::uint32_t ) > callback ) noexcept -> void override {
            char_callback = std::move( callback );
        }

        /**
         * @brief Set the window title.
         * @param title The new window title.
         */
        constexpr auto SetWindowTitle( const std::string& title ) noexcept -> void override {
            if ( not_equal( window, nullptr ) ) { glfwSetWindowTitle( window, title.c_str() ); }
        }

        /**
         * @brief Get the GLFW window.
         * @return The GLFW window.
         */
        [[nodiscard]] constexpr auto GetWindow() const noexcept -> GLFWwindow* {
            return window;
        }

        /**
         * @brief Get the GLSL version from the OpenGL context.
         * @return The shader version.
         */
        [[nodiscard]] constexpr auto static GetGLSLVersion() noexcept -> const char* {
            // TODO: use glGetString(GL_SHADING_LANGUAGE_VERSION) instead of hard coding it.
            return GLSL_Version.c_str();
        }
    };

    /**
     * @brief Factory function for creating a platform instance.
     * @param args Arguments to pass to the platform constructor.
     * @return A unique pointer to the platform instance.
     */
    template < typename... Args > auto constexpr CreatePlatform( Args&&... args ) -> std::unique_ptr< Platform > {
        return std::make_unique< Platform >( std::forward< Args >( args )... );
    }
} // namespace PeanutGL
