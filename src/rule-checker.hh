#pragma once

#include "move.hh"
#include "quiet-move.hh"

class RuleChecker
{
public:
  RuleChecker() = default;
  static bool check(plugin::ChessBoard board, const Move& m);
private:
  static bool isMoveAuthorized(plugin::ChessBoard board, const Move& m);
  static bool isMoveLegal(plugin::ChessBoard board, Move m);
};
