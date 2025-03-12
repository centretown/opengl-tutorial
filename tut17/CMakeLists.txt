project(${CMAKE_PROJECT_NAME})

add_executable(tut17 main.cpp)

target_link_libraries(tut17 PRIVATE ${LIBS} davegl)
