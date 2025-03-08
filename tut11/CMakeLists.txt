project(tutgl)

add_executable(tut11 main.cpp ../src/model.cpp ../src/mesh.cpp)

find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

include_directories(${ASSIMP_INCLUDE_DIR})

target_link_libraries(tut11
    PRIVATE
    ${LIBS}
    assimp
    davegl)
