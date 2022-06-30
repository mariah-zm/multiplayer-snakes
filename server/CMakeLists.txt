cmake_minimum_required(VERSION 3.16)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")

project(server)

set(SOURCE_FILES models/game.c models/snake.c gameserver/gameserver.c 
        ../core/core.c servercore/servercore.c)

add_executable(snakes-server main.c ${SOURCE_FILES})