project(${CMAKE_PROJECT_NAME})

add_executable(test main.cpp)

include_directories("../include")

target_link_libraries(test
    PRIVATE
    ${LIBS}
    davegl)
