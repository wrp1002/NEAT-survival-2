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


find_package(PkgConfig REQUIRED)

pkg_check_modules(ALLEGRO REQUIRED
    allegro-5
    allegro_font-5
    allegro_ttf-5
    allegro_primitives-5
    allegro_color-5
    allegro_dialog-5
)

target_include_directories(NEAT-survival-2 PRIVATE ${ALLEGRO_INCLUDE_DIRS})

target_link_libraries(
    NEAT-survival-2
    ${ALLEGRO_LIBRARIES}
    fmt
    box2d
)

install(TARGETS NEAT-survival-2
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
