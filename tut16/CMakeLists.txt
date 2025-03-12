project(${CMAKE_PROJECT_NAME})

add_executable(tut16 main.cpp)

target_link_libraries(tut16 PRIVATE ${LIBS} davegl)
