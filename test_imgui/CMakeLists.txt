project(${CMAKE_PROJECT_NAME})

#set(CMAKE_CXX_FLAGS -DUSE_OPEN_GLES)

set(IMGUI_DIR ../external/imgui)

add_executable(examp main.cpp)

include_directories(${IMGUI_DIR})

target_link_libraries(examp
    PRIVATE
    ${LIBS}
    imguigl)
