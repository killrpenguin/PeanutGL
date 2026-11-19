#include "glad/gl.h"
#include <GLFW/glfw3.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <print>
#include <string>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
namespace {
    auto framebuffer_size_callback( GLFWwindow* /* window */, int width, int height ) -> void {
        glViewport( 0, 0, width, height );
    }

    auto create_window( int width, int height, const char* title ) noexcept -> GLFWAPI GLFWwindow* {
        glfwSetErrorCallback( []( int /*error_code*/, const char* description ) {
            std::cerr << description;
            std::exit( EXIT_FAILURE );
        } );

        glfwInit();
        constexpr std::size_t MajorVer{ 4 };
        constexpr std::size_t MinorVer{ 6 };
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, MajorVer );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, MinorVer );
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE );
        glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, 1 );

        GLFWwindow* window = nullptr;

        if ( window = glfwCreateWindow( width, height, title, nullptr, nullptr ); window == nullptr ) {
            try {
                std::println( std::cerr, "Failed to create GLFW window" );
            } catch ( ... ) { std::cerr << "Caught an unknown exception."; }

            glfwTerminate();
            std::exit( EXIT_FAILURE );
        }

        glfwMakeContextCurrent( window );

        glfwSetFramebufferSizeCallback( window, framebuffer_size_callback );

        if ( gladLoadGL( glfwGetProcAddress ) == 0 ) {
            try {
                std::println( std::cerr, "Failed to initialize GLAD" );
            } catch ( ... ) { std::cerr << "Caught an unknown exception."; }

            glfwDestroyWindow( window );
            glfwTerminate();
            std::exit( EXIT_FAILURE );
        }

        glEnable( GL_DEBUG_OUTPUT );
        glDebugMessageCallback(
            []( GLenum /* source */, GLenum /* type */, GLuint /* id */, GLenum /* severity */, GLsizei /* length */,
                const GLchar* message, const void* /* userParam */ ) -> void {
                try {
                    std::println( "OpenGL Debug: {}\n", message );
                } catch ( ... ) { std::cerr << "Caught an unknown exception."; }
            },
            nullptr );

        return window;
    }

    struct DrawCommand {
        GLuint count{};
        GLuint instance_count{};
        GLuint first_index{};
        GLuint base_vertex{};
        GLuint base_instance{};
    };

} // namespace
int main() {
    using vertex_type  = float;
    using indices_type = std::uint32_t;

    GLFWwindow* window{ create_window( 1024, 1024, "MultiDrawIndirectPractice" ) };

    [[maybe_unused]] const std::string vertex_shader_source{
        R"(
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;

void main() {
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
})"
    };

    [[maybe_unused]] const std::string fragment_shader_source{
        R"(
#version 460 core
out vec4 FragColor;
in vec3 ourColor;

void main() {
    FragColor = vec4(ourColor, 1.0f);
})"
    };

    // clang-format off
	constexpr std::array< vertex_type, 18 > vertices {
        -0.5F, -0.5F, 0.0F, 1.0F, 0.0F, 0.0F,
		0.5F, -0.5F, 0.0F, 0.0F, 1.0F, 0.0F,
		0.0F, 0.5F, 0.0F, 0.0F, 0.0F, 1.0F
    };
	constexpr std::array<indices_type, 3> incides {0, 1, 2};
    // clang-format on

    constexpr GLuint cmd_count{ static_cast< GLuint >( incides.size() ) };

    constexpr DrawCommand cmd{
        .count = cmd_count, .instance_count = 1, .first_index = 0, .base_vertex = 0, .base_instance = 0
    };

    unsigned int VAO{};
    glCreateVertexArrays( 1, &VAO );

    glEnableVertexArrayAttrib( VAO, 0 );

    constexpr std::size_t FIRST_INDEX_OFFSET{ 0 };
    glVertexArrayAttribFormat( VAO, 0, 3, GL_FLOAT, GL_FALSE, FIRST_INDEX_OFFSET );
    glVertexArrayAttribBinding( VAO, 0, 0 );

    glEnableVertexArrayAttrib( VAO, 1 );

    constexpr std::size_t SECOND_INDEX_OFFSET{ 3 * sizeof( vertex_type ) };
    glVertexArrayAttribFormat( VAO, 1, 3, GL_FLOAT, GL_FALSE, SECOND_INDEX_OFFSET );
    glVertexArrayAttribBinding( VAO, 1, 0 );

    unsigned int VBO{};
    constexpr std::size_t VBO_SIZE{ vertices.size() * sizeof( vertex_type ) };

    glCreateBuffers( 1, &VBO );
    glNamedBufferStorage( VBO, VBO_SIZE, vertices.data(), 0 );

    unsigned int EBO{};
    constexpr std::size_t EBO_SIZE{ vertices.size() * sizeof( indices_type ) };

    glCreateBuffers( 1, &EBO );
    glNamedBufferStorage( EBO, EBO_SIZE, incides.data(), 0 );

    unsigned int DRAW_BUFFER{};
    constexpr std::size_t CMD_SIZE{ sizeof( DrawCommand ) };

    glCreateBuffers( 1, &DRAW_BUFFER );
    glNamedBufferStorage( DRAW_BUFFER, CMD_SIZE, &cmd, 0 );

    constexpr std::size_t STRIDE{ 6 * sizeof( vertex_type ) };

    while ( glfwWindowShouldClose( window ) == 0 ) {
        if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) { glfwSetWindowShouldClose( window, 1 ); }

        glClearColor( 0.0F, 0.0F, 0.5F, 1.0F );
        glClear( GL_COLOR_BUFFER_BIT );

        /*
          glUseProgram( no program defined yet.);
        */

        glBindVertexArray( VAO );
        glVertexArrayVertexBuffer( VAO, 0, VBO, 0, STRIDE );
        glVertexArrayElementBuffer( VAO, EBO );

        glBindBuffer( GL_DRAW_INDIRECT_BUFFER, DRAW_BUFFER );

        glDrawElementsIndirect( GL_TRIANGLES, GL_UNSIGNED_INT, static_cast< void* >( nullptr ) );

        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    glDeleteVertexArrays( 1, &VAO );
    glDeleteBuffers( 1, &VBO );
    glDeleteBuffers( 1, &EBO );
    glDeleteBuffers( 1, &DRAW_BUFFER );

    return EXIT_SUCCESS;
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
