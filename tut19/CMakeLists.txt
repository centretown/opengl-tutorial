project(${CMAKE_PROJECT_NAME})

#set(CMAKE_CXX_FLAGS -DUSE_OPEN_GLES)

add_executable(tut19 main.cpp ../src/model.cpp ../src/mesh.cpp)
include_directories(
${IMGUI_DIR}
${ASSIMP_INCLUDE_DIR})

target_link_libraries(tut19
    PRIVATE
    ${LIBS}
    assimp
    davegl
    imguigl)
