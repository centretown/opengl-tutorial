project(tutgl)

add_executable(tut10 main.cpp)

target_link_libraries(tut10
    PRIVATE
    ${LIBS}
    davegl)
