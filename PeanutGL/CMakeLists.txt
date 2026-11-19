set(INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(HEADER_LIST 
	"${INCLUDE_PATH}/BufferResource.hpp"
	"${INCLUDE_PATH}/UniformBufferResource.hpp"
	"${INCLUDE_PATH}/ResourceManager.hpp"
	"${INCLUDE_PATH}/MemoryBarrierResource.hpp"
	"${INCLUDE_PATH}/MaterialResource.hpp"
	"${INCLUDE_PATH}/RenderDocSystem.hpp"
	"${INCLUDE_PATH}/DebugSystem.hpp"
	"${INCLUDE_PATH}/CrashReporterSystem.hpp"
	"${INCLUDE_PATH}/ShaderProgram.hpp"
	"${INCLUDE_PATH}/Shader.hpp"
	"${INCLUDE_PATH}/GeneratedConstants.hpp"
	"${INCLUDE_PATH}/VertexArray.hpp"
	"${INCLUDE_PATH}/Utilities.hpp"
	"${INCLUDE_PATH}/UtilityVerticies.hpp"
	"${INCLUDE_PATH}/UtilityTypes.hpp"
	"${INCLUDE_PATH}/PhysicsSystem.hpp"
	"${INCLUDE_PATH}/Engine.hpp"
	"${INCLUDE_PATH}/EntryPoint.hpp"
	"${INCLUDE_PATH}/Component.hpp"
	"${INCLUDE_PATH}/Platform.hpp"
	"${INCLUDE_PATH}/ImGuiSystem.hpp"
	"${INCLUDE_PATH}/Renderer.hpp"
	"${INCLUDE_PATH}/CameraComponent.hpp"
	"${INCLUDE_PATH}/TransformComponent.hpp"
	"${INCLUDE_PATH}/MeshComponent.hpp"
	"${INCLUDE_PATH}/Entity.hpp"
	"${INCLUDE_PATH}/ModelComponent.hpp"
	"${INCLUDE_PATH}/ProjectionComponent.hpp"
	"${INCLUDE_PATH}/ViewComponent.hpp"
	"${INCLUDE_PATH}/RotationComponent.hpp"
)

set(SRC_LIST 
	"STBImage.cpp"
	"ResourceManager.cpp"
	"VertexArray.cpp"
	"RenderDocSystem.cpp"
	"ImGuiSystem.cpp"
	"PhysicsSystem.cpp"
	"Engine.cpp"
	"Entity.cpp"
	"Platform.cpp"
	"Renderer.cpp"
	"Component.cpp"
	"ModelComponent.cpp"
	"ViewComponent.cpp"
	"ProjectionComponent.cpp"
	"TransformComponent.cpp"
	"MeshComponent.cpp"
	"CameraComponent.cpp"
	"BufferResource.cpp"
	"UniformBufferResource.cpp"
	"RotationComponent.cpp"
)

set_source_files_properties(STBImage.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION TRUE)

add_library(PeanutGL ${SRC_LIST} ${HEADER_LIST})

# Ensure libunwind builds before PeanutGL.
# add_dependencies(PeanutGL Libunwind)

target_include_directories(PeanutGL SYSTEM PUBLIC
	$<BUILD_INTERFACE:${renderdoc_api_SOURCE_DIR}>
	$<INSTALL_INTERFACE:${renderdoc_api_SOURCE_DIR}>
)

target_precompile_headers(PeanutGL PUBLIC "${INCLUDE_PATH}/pch.hpp")

set_target_properties(PeanutGL PROPERTIES UNITY_BUILD ON)

target_compile_options(PeanutGL PUBLIC
    $<$<CONFIG:Debug>: ${DEBUG_COMPILER_FLAGS}>
    $<$<CONFIG:Release>: ${RELEASE_COMPILER_FLAGS}>
    $<$<CXX_COMPILER_ID:Clang>: ${CLANG_COMPILER_FLAGS}>
)

target_include_directories(PeanutGL PUBLIC include)

# target_include_directories(PeanutGL SYSTEM PRIVATE "${LIBUNWIND_INCLUDE_DIR}")

# target_link_libraries(PeanutGL PRIVATE "${LIBUNWIND_LIB_DIR}/libunwind.a")

target_link_libraries(PeanutGL PUBLIC imgui glfw glm::glm glad quill::quill stb_image PRIVATE 
	readerwriterqueue)
