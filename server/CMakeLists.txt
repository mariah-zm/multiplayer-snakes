cmake_minimum_required(VERSION 3.16)

set(CMAKE_BUILD_TYPE Debug)

project(server)

set(SOURCE_FILES game/game.c)

add_executable(snakes-server main.c ${SOURCE_FILES})