#include "rule-checker.hh"

bool RuleChecker::check(uint64_t board[6], const Move& move)
{
  if (!isMoveAuthorized(board, move)) return false; // Based on the Piece Type and position of start and end cell
  return isMoveLegal(board, move);

}

bool RuleChecker::isMoveAuthorized(uint64_t board[6], const Move& move)
{
  if (move.move_type_get() == Move::Type::QUIET)
  {
    const QuietMove& quiet_move = reinterpret_cast<const QuietMove&>(move);
    auto d_file = static_cast<char>(quiet_move.end_get().file_get()) - static_cast<char>(quiet_move.start_get().file_get());
    auto d_rank = static_cast<char>(quiet_move.end_get().rank_get()) - static_cast<char>(quiet_move.start_get().rank_get());
    switch(quiet_move.piecetype_get()) {
      case plugin::PieceType::PAWN:

        break;
      case plugin::PieceType::BISHOP:
        return abs(d_file) == abs(d_rank);
        break;
    }
  }
}

bool RuleChecker::isMoveLegal(uint64_t board[6], Move m)
{
}
