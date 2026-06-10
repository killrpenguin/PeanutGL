#pragma once

#include "DebugSystem.hpp"
#include "UtilityTypes.hpp"
#include "quill/LogMacros.h"

#include <cstdlib>
#include <fstream>
#include <functional>
#include <type_traits>

namespace PeanutGL {
    namespace ranges = std::ranges;

    constexpr auto not_equal{ ranges::not_equal_to{} };
    constexpr auto equal{ ranges::equal_to{} };
    constexpr auto greater{ ranges::greater{} };

    class FatalSignalError : std::exception {
        std::string message{};
        int code{};

      public:
        explicit FatalSignalError( std::string_view msg, int val = EXIT_FAILURE )
            : message{ msg }, code{ val } {
        }
        const char* what() const noexcept override {
            return "";
        }
        auto operator()() -> int& = delete;
        auto operator()() const -> int {
            return code;
        }
    };

    template < std::uint32_t... Flags > constexpr auto safe_or_assign( const int lhs ) -> int {
        auto unsigned_lhs = static_cast< uint32_t >( lhs );
        const std::uint32_t rhs{ ( Flags | ... ) };
        unsigned_lhs |= rhs;
        return static_cast< int >( unsigned_lhs );
    }

    constexpr auto read_file( const std::string& file ) -> std::string {
        std::ifstream input_file{ file };

        if ( !input_file.is_open() ) {
            LOG_ERROR( QuillPtr(), "File doesn't exist: {}", file );
            return {};
        }

        std::string shader_source(
            ( std::istreambuf_iterator< char >( input_file ) ), std::istreambuf_iterator< char >() );

        input_file.close();
        return shader_source;
    }

    namespace detail {
        struct Program {};
        struct Linker {};

        using ErrorType = std::variant< Program, Linker >;

        constexpr ErrorType Link    = Linker();
        constexpr ErrorType Default = Program();
        // namespace detail

        struct ErrorTypeVisitor {
            unsigned int shader{};
            int success{};

            auto operator()( const detail::Program /*unnamed*/ ) noexcept -> bool {
                assert( shader != 0 && "Tried to check errors for invalid shader." );
                glGetShaderiv( shader, GL_COMPILE_STATUS, &success );

                if ( success == 0 ) {
                    constexpr static int LOGSIZE{ 1024 };

                    std::string info_log{};

                    glGetShaderInfoLog( shader, LOGSIZE, nullptr, info_log.data() );

                    LOG_ERROR( QuillPtr(), "Shader compilation failed.\n\t{}", info_log );

                    return true;
                }
                return false;
            }
            auto operator()( const detail::Linker /*unnamed*/ ) noexcept -> bool {
                assert( shader != 0 && "Tried to check errors for invalid shader." );
                glGetProgramiv( shader, GL_LINK_STATUS, &success );
                if ( success == 0 ) {
                    constexpr static int LOGSIZE{ 1024 };

                    std::string info_log{};

                    glGetProgramInfoLog( shader, LOGSIZE, nullptr, info_log.data() );
                    LOG_ERROR( QuillPtr(), "Shader linking failed.\n\t", info_log );

                    return true;
                }
                return false;
            }
        };
    } // namespace detail

    constexpr auto compile_error( const unsigned int shader, const detail::ErrorType type = detail::Default ) noexcept
        -> bool {
        int success{};
        return std::visit( detail::ErrorTypeVisitor{ .shader = shader, .success = success }, type );
    }

    template < typename... Args >
        requires( std::same_as< Args, unsigned int > || ... )
    constexpr auto link_shaders( Args... args ) noexcept -> unsigned int {
        unsigned int new_shader_program = glCreateProgram();

        ( glAttachShader( new_shader_program, std::forward< Args >( args ) ), ... );

        glLinkProgram( new_shader_program );

        return new_shader_program;
    }

    constexpr auto compile_shader( GLenum shader_type, const std::string& shader_source ) noexcept -> unsigned int {
        unsigned int shader{ glCreateShader( shader_type ) };

        const std::array< const char*, 1 > ptr_array{ shader_source.c_str() };

        glShaderSource( shader, 1, ptr_array.data(), nullptr );
        glCompileShader( shader );

        return shader;
    }

} // namespace PeanutGL
