project(${CMAKE_PROJECT_NAME})

#set(CMAKE_CXX_FLAGS -DUSE_OPEN_GLES)

add_executable(tut18 main.cpp ../src/model.cpp ../src/mesh.cpp)
include_directories(${ASSIMP_INCLUDE_DIR})

target_link_libraries(tut18
    PRIVATE
    ${LIBS}
    assimp
    davegl)
