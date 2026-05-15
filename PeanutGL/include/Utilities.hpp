#pragma once

#include "DebugSystem.hpp"

#include "quill/LogMacros.h"

#include <compare>
#include <concepts>
#include <cstdlib>
#include <fstream>
#include <functional>

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

    template < typename T >
    concept StrictCompareToInt = requires( int MyVal, T other ) {
        { MyVal <=> other } -> std::same_as< std::strong_ordering >;
    };

    template < typename T >
    concept StrictCompareToFloat = requires( int MyVal, T other ) {
        { MyVal <=> other } -> std::same_as< std::partial_ordering >;
    };

    struct Width {
        int val = 0;

        template < typename T >
            requires std::same_as< T, int >
        explicit constexpr Width( T inner ) noexcept
            : val{ inner } {};

        template < StrictCompareToInt T > auto operator<=>( const T& other ) const {
            return val <=> other;
        }

        // Delete the non const reference so the inner value can't accidently be changed.
        auto operator()() -> int& = delete;

        auto operator()() const -> int {
            return val;
        }
    };

    struct Height {
        int val = 0;

        template < typename T >
            requires std::same_as< T, int >
        explicit constexpr Height( T inner ) noexcept
            : val{ inner } {};

        template < StrictCompareToInt T > auto operator<=>( const T& other ) const {
            return val <=> other;
        }

        // Delete the non const reference so the inner value can't accidently be changed.
        auto operator()() -> int& = delete;

        auto operator()() const -> int {
            return val;
        }
    };

    struct XAxis {
        double val = 0.0;
        // Partial_ordering: == and != are not defined.

        template < typename T >
            requires std::same_as< T, double >
        explicit constexpr XAxis( T inner )
            : val{ inner } {};

        template < StrictCompareToFloat T > auto operator<=>( const T& other ) const {
            val <=> other;
        }

        // Delete the non const reference so the inner value can't accidently be changed.
        auto operator()() -> double& = delete;

        auto operator()() const -> double {
            return val;
        }

        friend auto operator<<( std::ostream& _os, const XAxis& self ) -> std::ostream& {
            _os << self.val;
            return _os;
        }
    };

    struct YAxis {
        double val = 0.0;

        template < typename T >
            requires std::same_as< T, double >
        explicit constexpr YAxis( T inner )
            : val{ inner } {};

        // Partial_ordering: == and != are not defined by the space ship operator because of floating point precision
        // things
        auto operator<=>( const YAxis& other ) const = default;

        // Delete the non const reference so the inner value can't accidently be changed.
        auto operator()() -> double& = delete;

        auto operator()() const -> double {
            return val;
        }

        friend auto operator<<( std::ostream& _os, const YAxis& self ) -> std::ostream& {
            _os << self.val;
            return _os;
        }
    };

    struct VertexBufferElement {
        unsigned int type{}; // GLenum type.
        int count{};
        unsigned char normalized{};

        template < typename T > constexpr auto static _size_of() -> unsigned int {
            return static_cast< unsigned int >( sizeof( T ) );
        };

        auto static size_of_enum_type( const unsigned int type ) -> int {
            switch ( type ) {
                case GL_FLOAT        : return _size_of< GLfloat >(); // OpenGL spec gaurantees 4.
                case GL_UNSIGNED_INT : return _size_of< GLuint >();  // OpenGL spec gaurantees 4.
                case GL_UNSIGNED_BYTE: return _size_of< GLbyte >();  // OpenGL spec gaurantees 1.
                default              : assert( false && "Could not get size of type for VertexBufferLayout." );
            }
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

    constexpr auto compile_error( const unsigned int shader, const std::string_view type ) noexcept -> bool {
        int success{};

        if ( type != "PROGRAM" ) {
            glGetShaderiv( shader, GL_COMPILE_STATUS, &success );

            if ( success == 0 ) {
                constexpr static int LOGSIZE{ 1024 };

                std::string info_log{};

                glGetShaderInfoLog( shader, LOGSIZE, nullptr, info_log.data() );

                LOG_ERROR( QuillPtr(), "Shader compilation failed.\n\t{}", info_log );

                return true;
            }
            return false;
        } else {
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
    }

    constexpr auto link_shaders(
        // NOLINTNEXTLINE
        const unsigned int VertexShader, const unsigned int FragmentShader ) noexcept -> unsigned int {
        unsigned int new_shader_program = glCreateProgram();

        glAttachShader( new_shader_program, VertexShader );

        glAttachShader( new_shader_program, FragmentShader );

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

    enum class State : std::uint8_t {
        Uninitialized,
        Initialized,
        Active,
        InActive,
        Destroyed
    };
} // namespace PeanutGL
