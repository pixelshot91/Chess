cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_COMPILER "g++")
set(CMAKE_C_FLAGS "-Wall -Wextra -std=c++14 -pedantic -lboost_program_options")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(BIN_NAME "chess")

set(SRC src/main.cc)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

include_directories(src
  src/boost)

add_executable(${BIN_NAME} ${SRC})

target_link_libraries(${BIN_NAME} boost_program_options)

