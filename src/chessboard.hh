#pragma once

#include "rule-checker.hh"

class ChessBoard
{
public:
  ChessBoard();
private:
  uint64_t bitboard_[6];
  RuleChecker checker_;
};
