cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++14 -pedantic -ldl")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
#set(BIN_NAME "chess")
set(BIN_ENGINE "chessengine")
set(BIN_HUMAN "human")

set(SRC_engine src/main_engine.cc src/move.cc src/quiet-move.cc src/parser.cc src/adaptater.cc
  src/chessboard.cc src/rule-checker.cc src/engine.cc src/history.cc
  src/piece/pawn.cc src/piece/rook.cc src/piece/king.cc src/piece/queen.cc
  src/piece/knight.cc src/piece/bishop.cc src/piece/piece.cc)

set (SRC_TEST_ChessBoard tests/chessboard.cc src/move.cc src/quiet-move.cc
  src/chessboard.cc src/rule-checker.cc src/history.cc
  src/piece/pawn.cc src/piece/rook.cc src/piece/king.cc src/piece/queen.cc
  src/piece/knight.cc src/piece/bishop.cc src/piece/piece.cc)

set(SRC_human src/main_human.cc)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

include_directories(src
  src/boost)

add_executable(${BIN_ENGINE} ${SRC_engine})
add_executable(${BIN_HUMAN} ${SRC_human})
add_executable("test_chessboard" EXCLUDE_FROM_ALL ${SRC_TEST_ChessBoard})

target_link_libraries(${BIN_ENGINE} boost_program_options)
target_link_libraries(${BIN_ENGINE} boost_regex)
target_link_libraries(${BIN_ENGINE} boost_system)

