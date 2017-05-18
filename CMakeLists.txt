cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -std=c++14 -pedantic -ldl -lpthread")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
#set(BIN_NAME "chess")
set(BIN_ENGINE "chessengine")
set(BIN_HUMAN "human")
set(BIN_AI "ai")

set(SRC_engine src/main_engine.cc src/move.cc src/quiet-move.cc src/parser.cc src/adaptater.cc
  src/chessboard.cc src/rule-checker.cc src/engine.cc src/history.cc
  src/piece/pawn.cc src/piece/rook.cc src/piece/king.cc src/piece/queen.cc
  src/piece/knight.cc src/piece/bishop.cc src/piece/piece.cc
  src/plugin-auxiliary.cc)

set (SRC_TEST_ChessBoard tests/chessboard.cc src/move.cc src/quiet-move.cc
  src/chessboard.cc src/rule-checker.cc src/history.cc
  src/piece/pawn.cc src/piece/rook.cc src/piece/king.cc src/piece/queen.cc
  src/piece/knight.cc src/piece/bishop.cc src/piece/piece.cc)

set(SRC_human src/main_human.cc src/human-player.cc src/player.cc)

set(SRC_ai src/AI/main_ai.cc src/player.cc src/AI/AI.cc src/move.cc src/quiet-move.cc
  src/chessboard.cc src/rule-checker.cc src/plugin-auxiliary.cc src/parser.cc)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

include_directories(src)

add_executable(${BIN_ENGINE} ${SRC_engine})
add_executable(${BIN_HUMAN} ${SRC_human})
add_executable(${BIN_AI} ${SRC_ai})
add_executable("test_chessboard" EXCLUDE_FROM_ALL ${SRC_TEST_ChessBoard})

target_link_libraries(${BIN_ENGINE} boost_program_options)
target_link_libraries(${BIN_ENGINE} boost_regex)
target_link_libraries(${BIN_ENGINE} boost_system)

target_link_libraries(${BIN_HUMAN} boost_system)

target_link_libraries(${BIN_AI} boost_system)
target_link_libraries(${BIN_AI} boost_regex)
