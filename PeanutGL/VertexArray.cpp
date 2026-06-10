#include "VertexArray.hpp"


#include <cassert>


namespace PeanutGL {

    template < typename T > class VertexArray< T, detail::VAO >;
    template < typename T > class VertexArray< T, detail::EmptyVAO >;
}; // namespace PeanutGL
