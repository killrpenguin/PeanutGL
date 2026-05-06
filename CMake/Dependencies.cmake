include(FetchContent)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

FetchContent_Declare(
	glfw3
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG master
	EXCLUDE_FROM_ALL
	SYSTEM
	SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/glfw"
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw3)

FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG master
	EXCLUDE_FROM_ALL
	SYSTEM
	SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/glm"
)

FetchContent_MakeAvailable(glm)

FetchContent_Declare(
        quill
        GIT_REPOSITORY    https://github.com/odygrd/quill.git
        GIT_TAG           master
	EXCLUDE_FROM_ALL
	SYSTEM
	SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/quill"
)

FetchContent_MakeAvailable(quill)

FetchContent_Declare(
        readerwriterqueue
        GIT_REPOSITORY    https://github.com/cameron314/readerwriterqueue
        GIT_TAG           master
	EXCLUDE_FROM_ALL
	SYSTEM
	SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/moodycamelqueue"
)

FetchContent_MakeAvailable(readerwriterqueue)

FetchContent_Declare(renderdoc_api
	URL https://raw.githubusercontent.com/baldurk/renderdoc/refs/heads/v1.x/renderdoc/api/app/renderdoc_app.h
	DOWNLOAD_NO_EXTRACT TRUE
        EXCLUDE_FROM_ALL
 	SYSTEM
 	GIT_PROGRESS TRUE
 	SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/renderdoc_api"
)

FetchContent_MakeAvailable(renderdoc_api)

FetchContent_Declare(stb
	GIT_REPOSITORY https://github.com/nothings/stb.git
	GIT_TAG master
	EXCLUDE_FROM_ALL
	SYSTEM
	GIT_PROGRESS TRUE
	SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/stb_image"
)

FetchContent_MakeAvailable(stb)

add_library(stb_image INTERFACE)

target_include_directories(stb_image SYSTEM INTERFACE ${stb_SOURCE_DIR})

add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/glad" EXCLUDE_FROM_ALL SYSTEM)

add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/imgui" EXCLUDE_FROM_ALL SYSTEM)
