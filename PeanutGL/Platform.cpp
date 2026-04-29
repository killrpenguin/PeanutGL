#include "Platform.hpp"
#include "DebugSystem.hpp"
#include "Utilities.hpp"

// clang-format off
#include "glad/gl.h"
#include <GLFW/glfw3.h>
// clang-format on

#include "quill/LogMacros.h"

#include <cstdint>
#include <exception>
#include <functional>
#include <print>
#include <stdexcept>
#include <string>

namespace PeanutGL {

    auto Platform::Initialize( const std::string& appName, const Width requested_width, const Height requested_height )
        -> bool {
        constexpr static int MAJOR_VER{ 4 };
        constexpr static int MINOR_VER{ 6 };

        // Set this callback before calling glfwInit() to capture better error information on failure.
        glfwSetErrorCallback( GLFWErrorCallback );

        if ( equal( glfwInit(), GLFW_FALSE ) ) { throw std::runtime_error( "Failed to initialize GLFW" ); }

        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, MAJOR_VER );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, MINOR_VER );
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
        glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );

        glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE );

        window = glfwCreateWindow( requested_width.val, requested_height.val, appName.c_str(), nullptr, nullptr );

        if ( equal( window, nullptr ) ) { throw std::runtime_error( "Failed to create GLFW window." ); }

        glfwSetWindowUserPointer( window, this );

        glfwMakeContextCurrent( window );

        glfwSetFramebufferSizeCallback( window, FramebufferResizeCallback );
        glfwSetWindowSizeCallback( window, WindowResizeCallback );
        glfwSetCursorPosCallback( window, MousePositionCallback );
        glfwSetMouseButtonCallback( window, MouseButtonCallback );
        glfwSetKeyCallback( window, KeyCallback );
        glfwSetCharCallback( window, CharCallback );

        if ( equal( gladLoadGL( glfwGetProcAddress ), 0 ) ) {
            throw std::runtime_error( std::string( "Failed to initialize GLAD." ) );
        }

        glEnable( GL_DEBUG_OUTPUT );
        glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
        glDebugMessageCallback( Platform::GLDebugMessageCallback, nullptr );

        width  = requested_width;
        height = requested_height;

        return true;
    }

    auto Platform::Cleanup() -> void {
        if ( not_equal( window, nullptr ) ) {
            glfwDestroyWindow( window );
            window = nullptr;
        }
        glfwTerminate();
    }

    auto Platform::ProcessEvents() noexcept -> bool {
        glfwPollEvents();
        return equal( glfwWindowShouldClose( window ), GLFW_FALSE );
    }

    auto Platform::HasWindowResized() noexcept -> bool {
        const bool resized{ window_resized };
        window_resized = false;
        return resized;
    }

    // NOLINTBEGIN
    void GLAPIENTRY Platform::GLDebugMessageCallback(
        GLenum source, GLenum type, [[maybe_unused]] GLuint id, GLenum severity, [[maybe_unused]] GLsizei length,
        const GLchar* message, [[maybe_unused]] const void* userParam ) {
        const auto source_str = [source]() {
            switch ( source ) {
                case GL_DEBUG_SOURCE_API            : return "API";
                case GL_DEBUG_SOURCE_WINDOW_SYSTEM  : return "WINDOW SYSTEM";
                case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
                case GL_DEBUG_SOURCE_THIRD_PARTY    : return "THIRD PARTY";
                case GL_DEBUG_SOURCE_APPLICATION    : return "APPLICATION";
                case GL_DEBUG_SOURCE_OTHER          : return "OTHER";
            }
            return "";
        }();

        const auto type_str = [type]() {
            switch ( type ) {
                case GL_DEBUG_TYPE_ERROR              : return "ERROR";
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR : return "UNDEFINED_BEHAVIOR";
                case GL_DEBUG_TYPE_PORTABILITY        : return "PORTABILITY";
                case GL_DEBUG_TYPE_PERFORMANCE        : return "PERFORMANCE";
                case GL_DEBUG_TYPE_MARKER             : return "MARKER";
                case GL_DEBUG_TYPE_OTHER              : return "OTHER";
            }
            return "";
        }();

        const auto severity_str = [severity]() {
            switch ( severity ) {
                case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
                case GL_DEBUG_SEVERITY_LOW         : return "LOW";
                case GL_DEBUG_SEVERITY_MEDIUM      : return "MEDIUM";
                case GL_DEBUG_SEVERITY_HIGH        : return "HIGH";
            }
            return "";
        }();
        LOG_ERROR(
            QuillPtr(), "GL CALLBACK: {}\n\tSource: {}\n\tType: {}\n\tSeverity: {}\n\tMessage: {}\n",
            ( type == GL_DEBUG_TYPE_ERROR ? "\n\t** GL ERROR **" : "" ), source_str, type_str, severity_str, message );
    }

    void Platform::GLFWErrorCallback( int error_code, const char* description ) {
        LOG_ERROR(
            QuillPtr(), "** GLFW INTERNAL ERROR ** \n\t ErrorCode: {}\n\t Description: {}", error_code, description );
    }

    void Platform::FramebufferResizeCallback( GLFWwindow* window, int width, int height ) {
        auto* platform = static_cast< Platform* >( glfwGetWindowUserPointer( window ) );

        platform->width          = Width{ width };
        platform->height         = Height{ height };
        platform->window_resized = true;

        if ( platform->framebuffer_resize_callback ) {
            platform->framebuffer_resize_callback( platform->GetWindowWidth(), platform->GetWindowHeight() );
        }
    }

    void Platform::WindowResizeCallback( GLFWwindow* window, int width, int height ) {
        auto* platform = static_cast< Platform* >( glfwGetWindowUserPointer( window ) );

        platform->width          = Width{ width };
        platform->height         = Height{ height };
        platform->window_resized = true;

        if ( platform->window_resize_callback ) {
            platform->window_resize_callback( platform->GetWindowWidth(), platform->GetWindowHeight() );
        }
    }

    void Platform::MousePositionCallback( GLFWwindow* window, double xpos, double ypos ) {
        auto* platform = static_cast< Platform* >( glfwGetWindowUserPointer( window ) );

        if ( platform->mouse_callback ) {
            const XAxis mouseX{ xpos };
            const YAxis mouseY{ ypos };

            std::uint32_t buttons = 0;

            if ( equal( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ), GLFW_PRESS ) ) { buttons |= 0x01; }

            if ( equal( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT ), GLFW_PRESS ) ) { buttons |= 0x02; }

            if ( equal( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_MIDDLE ), GLFW_PRESS ) ) { buttons |= 0x04; }

            platform->mouse_callback( mouseX, mouseY, buttons );
        }
    }

    void Platform::MouseButtonCallback(
        GLFWwindow* window, [[maybe_unused]] int button, [[maybe_unused]] int action, [[maybe_unused]] int mods ) {
        auto* platform = static_cast< Platform* >( glfwGetWindowUserPointer( window ) );

        if ( platform->mouse_callback ) {
            double xpos{};
            double ypos{};

            glfwGetCursorPos( window, &xpos, &ypos );

            std::uint32_t buttons{ 0 };

            const XAxis mouseX{ xpos };
            const YAxis mouseY{ ypos };

            if ( equal( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ), GLFW_PRESS ) ) { buttons |= 0x01; }

            if ( equal( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT ), GLFW_PRESS ) ) { buttons |= 0x02; }

            if ( equal( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_MIDDLE ), GLFW_PRESS ) ) { buttons |= 0x04; }

            platform->mouse_callback( mouseX, mouseY, buttons );
        }
    }

    void Platform::KeyCallback(
        GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods ) {
        auto* platform = static_cast< Platform* >( glfwGetWindowUserPointer( window ) );

        if ( platform->keyboard_callback ) { platform->keyboard_callback( key, not_equal( action, GLFW_RELEASE ) ); }
    }

    void Platform::CharCallback( GLFWwindow* window, [[maybe_unused]] unsigned int codepoint ) {
        auto* platform = static_cast< Platform* >( glfwGetWindowUserPointer( window ) );

        if ( platform->char_callback ) { platform->char_callback( codepoint ); }
    }
    // NOLINTEND
}; // namespace PeanutGL
