add_executable(BasicText BasicText.cpp)

target_compile_options(BasicText PUBLIC
	$<$<CONFIG:Debug>: ${DEBUG_COMPILER_FLAGS}>
	$<$<CONFIG:Release>: ${RELEASE_COMPILER_FLAGS}>
    	$<$<CXX_COMPILER_ID:Clang>: ${CLANG_COMPILER_FLAGS}>
)

target_compile_features(BasicText PRIVATE)

target_link_libraries(BasicText PRIVATE PeanutGL imgui glfw glm::glm glad readerwriterqueue quill::quill)

target_link_libraries(BasicText INTERFACE stb_image)
