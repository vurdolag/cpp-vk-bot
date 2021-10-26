cmake_minimum_required(VERSION 3.20)
project(awaiteble)

set(CMAKE_CXX_STANDARD 14)

add_executable(awaiteble Coro.cpp Loop.cpp util.cpp  main.cpp)
