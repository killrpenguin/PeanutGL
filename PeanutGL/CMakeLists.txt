set(INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(HEADER_LIST 
	"${INCLUDE_PATH}/RenderDocSystem.hpp"
	"${INCLUDE_PATH}/DebugSystem.hpp"
	"${INCLUDE_PATH}/CrashReporter.hpp"
	"${INCLUDE_PATH}/ShaderProgram.hpp"
	"${INCLUDE_PATH}/Shader.hpp"
	"${INCLUDE_PATH}/MeshResource.hpp"
	"${INCLUDE_PATH}/ResourceBase.hpp"
	"${INCLUDE_PATH}/GeneratedConstants.hpp"
	"${INCLUDE_PATH}/Application.hpp"
	"${INCLUDE_PATH}/Utilities.hpp"
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
	"${INCLUDE_PATH}/ResourceManager.hpp"
)

set(SRC_LIST 
	"RenderDocSystem.cpp"
	"ResourceBase.cpp"
	"PhysicsSystem.cpp"
	"Engine.cpp"
	"Entity.cpp"
	"Component.cpp"
	"MeshComponent.cpp"
	"CameraComponent.cpp"
	"Platform.cpp"
	"ImGuiSystem.cpp"
	"Renderer.cpp"
	"ResourceManager.cpp"
)

add_library(PeanutGL ${SRC_LIST} ${HEADER_LIST})

target_include_directories(PeanutGL SYSTEM PUBLIC
	$<BUILD_INTERFACE:${renderdoc_api_SOURCE_DIR}>
	$<INSTALL_INTERFACE:${renderdoc_api_SOURCE_DIR}>
)

target_precompile_headers(PeanutGL PUBLIC "${INCLUDE_PATH}/pch.hpp")

set_target_properties(PeanutGL PROPERTIES UNITY_BUILD ON)

target_compile_options(PeanutGL PUBLIC
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

target_include_directories(PeanutGL PUBLIC include)

target_link_libraries(PeanutGL PUBLIC imgui glfw glm::glm glad quill::quill PRIVATE 
	stb_image 
	readerwriterqueue)
