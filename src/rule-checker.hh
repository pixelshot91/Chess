#pragma once

#include "chessboard.hh"
#include "move.hh"
#include "quiet-move.hh"

class RuleChecker
{
public:
  RuleChecker() = default;
  static bool is_move_valid(const ChessBoard& board, Move& m);
private:
  static bool isMoveAuthorized(const ChessBoard& board,Move& m);
  static bool isMoveLegal(const ChessBoard& board, Move& m);

  static inline bool invalid_move(std::string reason);

public:
  static bool no_possible_move(const ChessBoard& board, plugin::Color color);
  static std::vector<std::shared_ptr<Move>> possible_moves(const ChessBoard& board, plugin::Color color);
  static bool isCheck(const ChessBoard& board, plugin::Position position);
  static bool three_fold_repetition(const std::vector<ChessBoard::board_t> permanent, const std::vector<ChessBoard*> temp);
};
