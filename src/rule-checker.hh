#pragma once

#include "move.hh"
#include "quiet-move.hh"
#include "chessboard.hh"

class RuleChecker
{
public:
  RuleChecker() = default;
  static bool check(ChessBoard board, const Move& m);
private:
  static bool isMoveAuthorized(ChessBoard board, const Move& m);
  static bool isMoveLegal(ChessBoard board, Move m);
};
