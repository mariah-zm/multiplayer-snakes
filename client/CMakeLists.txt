cmake_minimum_required(VERSION 3.16)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lncurses -lpthread")

project(client)

set(SOURCE_FILES window/window.c gameclient/gameclient.c ../core/core.c)

add_executable(snakes-client main.c ${SOURCE_FILES})

target_link_libraries(snakes-client curses)