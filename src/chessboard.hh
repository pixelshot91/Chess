#pragma once

#include <vector>
#include <iostream>
#include <iomanip>
#include "rule-checker.hh"

class ChessBoard
{
  public:
    ChessBoard();
    PieceType piecetype(Position);
    bool moved(Position);
    Color color(Position);
    void print_board();
  private:
    std::vector<std::vector<uint64_t>> board_;
    RuleChecker checker_;
};
