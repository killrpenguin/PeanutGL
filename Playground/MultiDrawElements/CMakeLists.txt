add_executable(MultiDrawElements MultiDraw.cpp)

target_compile_options(MultiDrawElements PUBLIC
	$<$<CONFIG:Debug>: ${DEBUG_COMPILER_FLAGS}>
	$<$<CONFIG:Release>: ${RELEASE_COMPILER_FLAGS}>
	$<$<CXX_COMPILER_ID:Clang>: ${CLANG_COMPILER_FLAGS}>
)

target_compile_features(MultiDrawElements PRIVATE)

target_link_libraries(MultiDrawElements PRIVATE PeanutGL imgui glfw glm::glm glad readerwriterqueue quill::quill)

target_link_libraries(MultiDrawElements INTERFACE stb_image)

