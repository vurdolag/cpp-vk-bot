cmake_minimum_required(VERSION 3.16)
project(awaiteble)

set(CMAKE_CXX_STANDARD 14)

add_executable(awaiteble Coro.cpp Loop.cpp util.cpp  main.cpp)
