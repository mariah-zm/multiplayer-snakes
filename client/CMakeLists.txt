cmake_minimum_required(VERSION 3.16)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lncurses -lpthread")

project(client)

set(SOURCE_FILES )

add_executable(snakes-client main.c ${SOURCE_FILES})