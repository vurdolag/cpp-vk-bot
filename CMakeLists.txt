cmake_minimum_required(VERSION 3.16)
project(awaiteble)

set(CMAKE_CXX_STANDARD 17)


set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CSS_STANDARD_LIBRARIES}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")

#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++  -pthread")


include_directories(include/)
include_directories(src/)


add_executable(awaiteble
        src/Coro.cpp
        src/Loop.cpp
        src/util.cpp
        src/main.cpp
        src/RequestSession.cpp
        src/Session.cpp
        src/Methods.cpp)


find_library(LIB libcurl.dll lib)
target_link_libraries(awaiteble LINK_PUBLIC ${LIB})