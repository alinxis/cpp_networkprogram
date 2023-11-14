cmake_minimum_required(VERSION 3.26)
project(test)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC CONFIGURE_DEPENDS include/*.h src/*.cc)

add_executable(test ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC include)
