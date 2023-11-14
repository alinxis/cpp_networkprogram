cmake_minimum_required(VERSION 3.26)
project(cppNet)

set(CMAKE_CXX_STANDARD 17)
file(GLOB SRC CONFIGURE_DEPENDS include/*.h src/*.cc)

add_executable(cppNet ${SRC})

target_include_directories(cppNet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
