project(${CMAKE_PROJECT_NAME})

add_executable(tut12 main.cpp)

target_link_libraries(tut12 PRIVATE ${LIBS} davegl)
