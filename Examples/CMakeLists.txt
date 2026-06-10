add_executable(Triangle HelloTriangle.cpp)

target_compile_options(Triangle PUBLIC
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CXX_COMPILER_ID:Clang>: -Wall
				-Wdeprecated
				-Wextra
				-Wcast-align
				-Wdouble-promotion
				-Wimplicit-fallthrough
				-Wmisleading-indentation
				-Wnon-virtual-dtor
				-Wnull-dereference
				-Wold-style-cast
				-Woverloaded-virtual
				-Wpedantic
				-Wshadow
				-Wundef
				-Wuninitialized
				-std=c++2c>
)

target_compile_features(Triangle PRIVATE)

target_link_libraries(Triangle PRIVATE "${LIBUNWIND_LIB_DIR}/libunwind.a")

target_link_libraries(Triangle PRIVATE PeanutGL imgui glfw glad readerwriterqueue quill::quill)

target_link_libraries(Triangle INTERFACE stb_image)



add_executable(BasicText BasicText.cpp)

target_compile_options(BasicText PUBLIC
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CXX_COMPILER_ID:Clang>: -Wall
				-Wdeprecated
				-Wextra
				-Wcast-align
				-Wdouble-promotion
				-Wimplicit-fallthrough
				-Wmisleading-indentation
				-Wnon-virtual-dtor
				-Wnull-dereference
				-Wold-style-cast
				-Woverloaded-virtual
				-Wpedantic
				-Wshadow
				-Wundef
				-Wuninitialized
				-std=c++2c>
)

target_compile_features(BasicText PRIVATE)

target_link_libraries(BasicText PRIVATE "${LIBUNWIND_LIB_DIR}/libunwind.a")

target_link_libraries(BasicText PRIVATE PeanutGL imgui glfw glad readerwriterqueue quill::quill)

target_link_libraries(BasicText INTERFACE stb_image)
