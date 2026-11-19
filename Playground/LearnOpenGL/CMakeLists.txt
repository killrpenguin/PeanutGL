add_executable(LearnOpenGL LearnOpenGL.cpp)

target_compile_options(LearnOpenGL PUBLIC
	$<$<CONFIG:Debug>: ${DEBUG_COMPILER_FLAGS}>
	$<$<CONFIG:Release>: ${RELEASE_COMPILER_FLAGS}>
	$<$<CXX_COMPILER_ID:Clang>: ${CLANG_COMPILER_FLAGS}>
)

target_compile_features(LearnOpenGL PRIVATE)

target_link_libraries(LearnOpenGL PRIVATE PeanutGL imgui glfw glm::glm glad readerwriterqueue quill::quill)

target_link_libraries(LearnOpenGL INTERFACE stb_image)

