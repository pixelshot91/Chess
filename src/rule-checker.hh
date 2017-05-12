#pragma once

#include "move.hh"
#include "quiet-move.hh"
#include "chessboard.hh"

class RuleChecker
{
public:
  RuleChecker() = default;
  static bool check(const ChessBoard& board, const Move& m);
private:
  static bool isMoveAuthorized(const ChessBoard& board, const Move& m);
  static bool isMoveLegal(const ChessBoard& board, const Move& m);
};
