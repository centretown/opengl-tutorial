project(tutgl)

add_executable(tut10 main.cpp ../glad/glad.c)

target_link_libraries(tut10
    PRIVATE
    ${LIBS}
    davegl)
