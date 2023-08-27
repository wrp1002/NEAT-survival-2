cmake_minimum_required(VERSION 3.5)

project(NEAT-survival-2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_minimum_required(VERSION 3.12)

file(GLOB_RECURSE src CONFIGURE_DEPENDS "./src/*.h" "./src/*.cpp")

add_executable(
    NEAT-survival-2
    ${src}
)


target_link_libraries (NEAT-survival-2 allegro allegro_font allegro_ttf allegro_primitives allegro_color allegro_dialog fmt box2d)

install(TARGETS NEAT-survival-2
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
