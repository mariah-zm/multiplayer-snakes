cmake_minimum_required(VERSION 3.16)

set(CMAKE_BUILD_TYPE Debug)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lncurses")

project(client)

set(SOURCE_FILES window/window.c gameclient/gameclient.c ../core/core.c)

add_executable(snakes-client main.c ${SOURCE_FILES})

find_package(Threads REQUIRED)
target_link_libraries(snakes-client PRIVATE Threads::Threads curses)