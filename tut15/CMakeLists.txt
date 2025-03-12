project(${CMAKE_PROJECT_NAME})

add_executable(tut15 main.cpp)

target_link_libraries(tut15 PRIVATE ${LIBS} davegl)
