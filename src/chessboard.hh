#pragma once

#include "rule-checker.hh"

class ChessBoard
{
public:
  ChessBoard();
  PieceType piecetype(Position)
  bool moved(Position);
  Color color(Position);

  
private:
  uint64_t bitboard_[6];
  RuleChecker checker_;
};
