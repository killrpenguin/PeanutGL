
set(AssetsRoot "${CMAKE_SOURCE_DIR}/Assets")
set(RenderdocOutputDir "${AssetsRoot}/Renderdoc/")

configure_file(
    "${CMAKE_SOURCE_DIR}/CMake/GeneratedConstants.h.in"
    "${CMAKE_SOURCE_DIR}/PeanutGL/include/GeneratedConstants.hpp"
)
