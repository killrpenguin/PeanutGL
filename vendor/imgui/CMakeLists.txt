set(HEADER_LIST "imconfig.h" 
	"imgui.h" 
	"imgui_impl_glfw.h" 
	"imgui_impl_opengl3.h" 
	"imgui_impl_opengl3_loader.h" 
	"imgui_internal.h" 
	"imgui_stdlib.h" 
	"imstb_rectpack.h" 
	"imstb_textedit.h" 
	"imstb_truetype.h" 
)

add_library(imgui "imgui.cpp" 
	"imgui_demo.cpp" 
	"imgui_draw.cpp" 
	"imgui_impl_glfw.cpp" 
	"imgui_impl_opengl3.cpp" 
	"imgui_stdlib.cpp" 
	"imgui_tables.cpp" 
	"imgui_widgets.cpp" ${HEADER_LIST})

target_include_directories(imgui SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${glfw3_INCLUDE_DIRS})

target_link_libraries(imgui PRIVATE glfw glad)
