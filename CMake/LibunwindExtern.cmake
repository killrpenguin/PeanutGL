include(ExternalProject)

set(LIBUNWIND_DIR ${CMAKE_SOURCE_DIR}/vendor/libunwind)

find_program(LIBTOOL_EXECUTABLE NAMES libtool)

if(NOT LIBTOOL_EXECUTABLE)
    message(FATAL_ERROR "libtool not found. Please install libtool.")
endif()

ExternalProject_Add(Libunwind
    GIT_REPOSITORY https://github.com/libunwind/libunwind.git
    GIT_TAG 71f2540411d6fe309f844b2ef27f12819dce80e5
    SOURCE_DIR "${LIBUNWIND_DIR}"
    LOG_OUTPUT_ON_FAILURE ON

    LOG_PATCH ON
    PATCH_COMMAND autoreconf -i

    LOG_CONFIGURE ON
    CONFIGURE_COMMAND ${LIBUNWIND_DIR}/configure --prefix=<INSTALL_DIR> --disable-minidebuginfo --disable-zlibdebuginfo

    LOG_BUILD ON
    BUILD_COMMAND make LIBTOOLFLAGS=--silent

    LOG_INSTALL ON
    INSTALL_COMMAND make install
    EXCLUDE_FROM_ALL TRUE
    CONFIGURE_HANDLED_BY_BUILD TRUE
)
ExternalProject_Get_Property(Libunwind INSTALL_DIR)

set(LIBUNWIND_INCLUDE_DIR ${INSTALL_DIR}/include)
set(LIBUNWIND_LIB_DIR ${INSTALL_DIR}/lib)

set_property(TARGET Libunwind PROPERTY
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${LIBUNWIND_INCLUDE_DIR}"
)
