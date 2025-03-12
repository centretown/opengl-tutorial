project(${CMAKE_PROJECT_NAME})

add_executable(tut14 main.cpp)

target_link_libraries(tut14 PRIVATE ${LIBS} davegl)
