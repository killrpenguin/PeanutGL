#pragma once

#include <compare>
#include <concepts>
#include <limits>

namespace PeanutGL {
    template < typename T >
    concept StrictCompareToInt = requires( int MyVal, T other ) {
        { MyVal <=> other } -> std::same_as< std::strong_ordering >;
    };

    template < typename T >
    concept StrictCompareToFloat = requires( int MyVal, T other ) {
        { MyVal <=> other } -> std::same_as< std::partial_ordering >;
    };

    template < typename T >
    concept SafeFloat = requires( float MyVal, T other ) {
        std::convertible_to< T, float > && std::numeric_limits< T >::is_iec559;
        { float{ other } }; // Fails on narrowing convertions.
    };

    template < typename T >
    concept PartiallyOrdered = requires( T lhs, T rhs ) {
        { lhs <=> rhs } -> std::convertible_to< std::partial_ordering >;
    };

    struct Width {
        int val{ 0 };

        template < typename T >
            requires std::same_as< T, int >
        explicit constexpr Width( T inner ) noexcept
            : val{ inner } {};

        template < StrictCompareToInt T > auto operator<=>( const T& other ) const {
            return val <=> other;
        }

        template < typename Self > auto operator()( this Self&& self ) {
            return std::forward< Self >( self ).val;
        }
    };

    struct Height {
        int val{ 0 };

        template < typename T >
            requires std::same_as< T, int >
        explicit constexpr Height( T inner ) noexcept
            : val{ inner } {};

        template < StrictCompareToInt T > auto operator<=>( const T& other ) const {
            return val <=> other;
        }

        template < typename Self > auto operator()( this Self&& self ) {
            return std::forward< Self >( self ).val;
        }
    };

    struct XAxis {
        double val{ 0.0 };

        template < typename T >
            requires std::same_as< T, double >
        explicit constexpr XAxis( T inner )
            : val{ inner } {};

        // Partial_ordering: == and != are not defined.
        template < StrictCompareToFloat T > auto operator<=>( const T& other ) const {
            val <=> other;
        }
        template < typename Self > auto operator()( this Self&& self ) {
            return std::forward< Self >( self ).val;
        }

        friend auto operator<<( std::ostream& _os, const XAxis& self ) -> std::ostream& {
            _os << self.val;
            return _os;
        }
    };

    struct YAxis {
        double val{ 0.0 };

        template < typename T >
            requires std::same_as< T, double >
        explicit constexpr YAxis( T inner )
            : val{ inner } {};

        // Partial_ordering: == and != are not defined by the space ship operator because of floating point precision
        auto operator<=>( const YAxis& other ) const -> std::partial_ordering = default;

        template < typename Self > auto operator()( this Self&& self ) {
            return std::forward< Self >( self ).val;
        }

        friend auto operator<<( std::ostream& _os, const YAxis& self ) -> std::ostream& {
            _os << self.val;
            return _os;
        }
    };

    struct CameraAspectRatio {
        constexpr static float DefaultValue{ 16.0F / 9.0F };

        CameraAspectRatio() noexcept = default;

        template < SafeFloat T >
        explicit constexpr CameraAspectRatio( const T val ) noexcept
            : val{ val } {};

        explicit operator double() const {
            return static_cast< double >( val );
        };

        explicit operator float() const {
            return val;
        };

        auto operator<=>( const CameraAspectRatio& other ) const -> std::partial_ordering = default;

        template < PartiallyOrdered T > auto operator<=>( T rhs ) const -> std::partial_ordering {
            return val <=> rhs;
        }

        template < SafeFloat T > auto& operator=( T rhs ) {
            this->val = std::forward< T >( rhs );
            return *this;
        }

        template < typename Self > auto operator()( this Self&& self ) {
            return std::forward< Self >( self ).val;
        }

      private:
        float val{ DefaultValue };
    };

    struct FieldOfView {
        constexpr static float DefaultValue{ 45.0F };

        FieldOfView() noexcept = default;

        template < SafeFloat T >
        explicit constexpr FieldOfView( const T val ) noexcept
            : val{ val } {};

        explicit operator double() const {
            return static_cast< double >( val );
        };

        explicit operator float() const {
            return val;
        };

        auto operator<=>( const FieldOfView& other ) const -> std::partial_ordering = default;

        template < PartiallyOrdered T > auto operator<=>( T rhs ) const -> std::partial_ordering {
            return val <=> rhs;
        }

        template < SafeFloat T > auto& operator=( T rhs ) {
            this->val = std::forward< T >( rhs );
            return *this;
        }

        template < typename Self > auto operator()( this Self&& self ) {
            return std::forward< Self >( self ).val;
        }

      private:
        float val{ DefaultValue };
    };

    struct Zoom {
        constexpr static float DefaultValue{ 45.0F };

        Zoom() noexcept = default;

        template < SafeFloat T >
        explicit constexpr Zoom( const T val ) noexcept
            : val{ val } {};

        explicit operator double() const {
            return static_cast< double >( val );
        };

        explicit operator float() const {
            return val;
        };

        auto operator<=>( const Zoom& other ) const -> std::partial_ordering = default;

        template < PartiallyOrdered T > auto operator<=>( T rhs ) const -> std::partial_ordering {
            return val <=> rhs;
        }

        template < SafeFloat T > auto& operator=( T rhs ) {
            this->val = std::forward< T >( rhs );
            return *this;
        }

        template < typename Self > auto operator()( this Self&& self ) {
            return std::forward< Self >( self ).val;
        }

      private:
        float val{ DefaultValue };
    };

    struct MouseSensitivity {
        constexpr static float DefaultValue{ 0.1F };

        MouseSensitivity() noexcept = default;

        template < SafeFloat T >
        explicit constexpr MouseSensitivity( const T val ) noexcept
            : val{ val } {};

        explicit operator double() const {
            return static_cast< double >( val );
        };

        explicit operator float() const {
            return val;
        };

        auto operator<=>( const MouseSensitivity& other ) const -> std::partial_ordering = default;

        template < PartiallyOrdered T > auto operator<=>( T rhs ) const -> std::partial_ordering {
            return val <=> rhs;
        }

        template < SafeFloat T > auto& operator=( T rhs ) {
            this->val = std::forward< T >( rhs );
            return *this;
        }

        template < typename Self > auto operator()( this Self&& self ) {
            return std::forward< Self >( self ).val;
        }

      private:
        float val{ DefaultValue };
    };

    struct MovementSpeed {
        constexpr static float DefaultValue{ 2.5F };

        MovementSpeed() noexcept = default;

        template < SafeFloat T >
        explicit constexpr MovementSpeed( const T val ) noexcept
            : val{ val } {};

        explicit operator double() const {
            return static_cast< double >( val );
        };

        explicit operator float() const {
            return val;
        };

        auto operator<=>( const MovementSpeed& other ) const -> std::partial_ordering = default;

        template < PartiallyOrdered T > auto operator<=>( T rhs ) const -> std::partial_ordering {
            return val <=> rhs;
        }

        template < SafeFloat T > auto& operator=( T rhs ) {
            this->val = std::forward< T >( rhs );
            return *this;
        }

        template < typename Self > auto operator()( this Self&& self ) {
            return std::forward< Self >( self ).val;
        }

      private:
        float val{ DefaultValue };
    };

    struct Yaw {
        constexpr static float DefaultValue{ -90.0F };

        Yaw() noexcept = default;

        template < SafeFloat T >
        explicit constexpr Yaw( const T val ) noexcept
            : val{ val } {};

        explicit operator double() const {
            return static_cast< double >( val );
        };

        explicit operator float() const {
            return val;
        };

        auto operator<=>( const Yaw& other ) const -> std::partial_ordering = default;

        template < PartiallyOrdered T > auto operator<=>( T rhs ) const -> std::partial_ordering {
            return val <=> rhs;
        }

        template < SafeFloat T > auto& operator=( T rhs ) {
            this->val = std::forward< T >( rhs );
            return *this;
        }

        template < typename Self > auto operator()( this Self&& self ) {
            return std::forward< Self >( self ).val;
        }

      private:
        float val{ DefaultValue };
    };

    struct Pitch {
        constexpr static float DefaultValue{ 0.0F };

        Pitch() noexcept = default;

        template < SafeFloat T >
        explicit constexpr Pitch( const T val ) noexcept
            : val{ val } {};

        explicit operator double() const {
            return static_cast< double >( val );
        };

        explicit operator float() const {
            return val;
        };

        auto operator<=>( const Pitch& other ) const -> std::partial_ordering = default;

        template < StrictCompareToFloat T > auto operator<=>( const T& other ) const -> std::partial_ordering {
            return val <=> other;
        }

        template < SafeFloat T > auto& operator=( T other ) {
            this->val = other;
            return *this;
        }

        template < typename Self > auto operator()( this Self&& self ) {
            return std::forward< Self >( self ).val;
        }

      private:
        float val{ DefaultValue };
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

    enum class State : std::uint8_t {
        Uninitialized,
        Initialized,
        Active,
        InActive,
        Destroyed
    };

} // namespace PeanutGL
