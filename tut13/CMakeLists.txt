project(${CMAKE_PROJECT_NAME})

add_executable(tut13 main.cpp)

target_link_libraries(tut13 PRIVATE ${LIBS} davegl)
