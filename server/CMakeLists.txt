cmake_minimum_required(VERSION 3.16)

set(CMAKE_BUILD_TYPE Debug)
set(THREADS_PREFER_PTHREAD_FLAG ON)

project(server)

set(SOURCE_FILES models/game.c models/snake.c 
                gameserver/gameserver.c ../core/core.c)

add_executable(snakes-server main.c ${SOURCE_FILES})

find_package(Threads REQUIRED)
target_link_libraries(snakes-server PRIVATE Threads::Threads )