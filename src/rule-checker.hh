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

public:
  static bool isStalemate(const ChessBoard& board, plugin::Position
  king_pos);
  static bool isCheckmate(const ChessBoard& board, plugin::Position
  king_pos);
};
