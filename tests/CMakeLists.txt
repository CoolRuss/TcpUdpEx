cmake_minimum_required(VERSION 3.13)
project(TcpUdpEx-tests)

find_package(GTest REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include_directories(
    ../includes
    ${GTEST_INCLUDE_DIRS}
    /usr/include/gmock
)

#file(GLOB SOURCES src/*.cpp)

add_executable(TcpUdpEx-tests ./utests.cpp ../src/protocols.cpp)
#add_executable(serverEx ../src/serverEx.cpp ../src/protocols.cpp)

target_link_libraries(TcpUdpEx-tests ${GTEST_LIBRARIES} /usr/lib64/libgmock.so /usr/lib64/libgmock_main.so pthread)

enable_testing()
add_test(TcpUdpEx-tests "./TcpUdpEx-tests")
