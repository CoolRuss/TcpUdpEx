cmake_minimum_required(VERSION 3.13)
project(TcpUdpEx)

set(CMAKE_CXX_STANDARD 14)

include_directories(includes)

add_executable(clientEx src/clientEx.cpp src/protocols.cpp)
add_executable(serverEx src/serverEx.cpp src/protocols.cpp)

