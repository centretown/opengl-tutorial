project(tutgl)

add_library(imguigl
    imgui.cpp
    imgui_draw.cpp
    imgui_demo.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_opengl3.cpp)

target_include_directories(imguigl
    PUBLIC . backends)
