#pragma once

#include "move.hh"
#include "quiet-move.hh"
#include "chessboard.hh"

class RuleChecker
{
public:
  RuleChecker() = default;
  static bool is_move_valid(const ChessBoard& board, const Move& m);
private:
//private:
  static bool isMoveAuthorized(const ChessBoard& board, const Move& m);
  static bool isMoveLegal(const ChessBoard& board, const Move& m);

  static bool invalid_move(std::string reason);

public:
  static bool isStalemate(const ChessBoard& board, plugin::Color color);
  static bool isCheck(const ChessBoard& board, plugin::Position position);
  static bool isCheckmate(const ChessBoard& board, plugin::Position
  king_pos);
  static bool is_a_draw(const ChessBoard& board);
};
