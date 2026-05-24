#include "VertexArray.hpp"
#include "DebugSystem.hpp"
#include "Utilities.hpp"

#include <cassert>
#include <quill/LogMacros.h>

namespace PeanutGL {

    VertexArray::~VertexArray() noexcept {
        Unload();
    }

    auto VertexArray::Load() noexcept -> bool {
        glCreateVertexArrays( 1, &VAO );

        return true;
    }

    auto VertexArray::Unload() noexcept -> void {
        glDeleteVertexArrays( 1, &VAO );
    }

    auto VertexArray::SetLayout( const std::span< const VertexBufferElement > elements ) noexcept -> void {
        if ( equal( elements.size(), 0 ) ) {
            LOG_ERROR( QuillPtr(), "Layout can not contain 0 elements." );
            return;
        }

        unsigned int offset{ 0 };
        for ( const auto [attr_index, element] : std::views::enumerate( std::views::as_const( elements ) ) ) {
            glEnableVertexArrayAttrib( VAO, attr_index );

            glVertexArrayAttribFormat(
                VAO, attr_index, VertexBufferElement::size_of_enum_type( element.type ), element.type,
                element.normalized, offset );

            glVertexArrayAttribBinding( VAO, attr_index, 0 );

            offset += element.count * VertexBufferElement::size_of_enum_type( element.type );
        }
        // assert(
        //     not_equal( stride(), 0 ) &&
        //     "The stride of the vertex array object should never be 0 after attributes are enabled." );

        glVertexArrayVertexBuffer( VAO, 0, vbo_name, 0, stride() );

        glVertexArrayElementBuffer( VAO, ebo_name );

        glBindVertexArray( VAO );
    }

}; // namespace PeanutGL
