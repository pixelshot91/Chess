#include "rule-checker.hh"

bool RuleChecker::check(plugin::ChessBoard board, const Move& move)
{
  if (move.end_get() == move.start_get()) return false;
  if (move.is_promotion() and not (move.end_get().rank_get() == File::EIGHT or move.end_get().rank_get() == File::ONE) return false;
  if (board[move.start_get()].first != move.piecetype_get()) return false;
  if (!isMoveAuthorized(board, move)) return false; // Based on the Piece Type and position of start and end cell
  return isMoveLegal(board, move);

}

bool RuleChecker::isMoveAuthorized(plugin::ChessBoard board, const Move& move)
{
  if (move.move_type_get() == Move::Type::QUIET)
  {
    const QuietMove& quiet_move = reinterpret_cast<const QuietMove&>(move);
    auto d_file = static_cast<char>(quiet_move.end_get().file_get()) - static_cast<char>(quiet_move.start_get().file_get());
    auto d_rank = static_cast<char>(quiet_move.end_get().rank_get()) - static_cast<char>(quiet_move.start_get().rank_get());
    switch(quiet_move.piecetype_get()) {
      case plugin::PieceType::PAWN:
        char dir_rank = (move.color_get() == WHITE) ? 1 : -1;
        if (move.is_an_attack())
          return abs(d_file) == 1 and d_rank == dir_rank;
        else
          return d_file == 0 and (d_rank == dir_rank or (d_rank == 2 * dir_rank and board.moved(Position(file, rank))));
      case plugin::PieceType::BISHOP:
        return abs(d_file) == abs(d_rank);
        break;
      case plugin::PieceType::KNIGHT:
        return abs(d_file) == 1 and abs(d_rank) == 2
          or abs(d_file) == 2 and abs(d_rank) == 1;
        break;
      case plugin::PieceType::KING:
        return abs(d_file) <= 1 and abs(d_rank) <= 1;
      case plugin::PieceType::ROOK:
        return d_file == 0 or d_rank == 0;
      case plugin::PieceType::QUEEN:
        return d_file == 0 or d_rank == 0 or abs(d_file) == abs(d_rank);
    }
  }
  return true;
}

bool RuleChecker::isMoveLegal(plugin::ChessBoard board, Move move)
{
  if (move.move_type_get() == Move::Type::QUIET)
  {
    if (move.piecetype_get() == plugin::PieceType::KING and board.is_attacked(move.color_get(), move.end_get())) return false;
      
    if (move.piecetype_get() == plugin::PieceType::PAWN and move.is_an_attack()) {
      if (board.piecetype_get(move.end_get()) != nullopt_t) // case non vide
        return  move.color_get() != board.color(move.end_get()); // different color
      else // en passant ?
        Move last_move = board.history.last();
      return last_move == Move::Type::QUIET and last_move.piecetype_get() == plugin::PieceType::PAWN and last_move.end_get().file_get() == move.end_get().file_get() and last_move.end_get().rank_get() == move.start_get().rank_get() and last_move.color_get() != move.color_get() and abs(last_move.end_get().rank_get() - last_move.start_get().rank_get()) == 2;
    }
    else {
      if (not move.is_a_test() and move.is_an_attack() and board.piecetype_get(move.end_get()) == nullopt_t) return false;
      if (board.piecetype_get(move.end_get()) != nullopt_t and move.color_get() == board.color(move.end_get())) return false; // same color

      if (move.piecetype_get() != plugin::PieceType::KNIGHT) { // Piece in the path
        auto d_file = static_cast<char>(quiet_move.end_get().file_get()) - static_cast<char>(quiet_move.start_get().file_get());
        auto d_rank = static_cast<char>(quiet_move.end_get().rank_get()) - static_cast<char>(quiet_move.start_get().rank_get());
        char dir_x = (d_file > 0) ? 1 : (d_file == 0) ? 0 : -1;
        char dir_y = (d_rank > 0) ? 1 : (d_rank == 0) ? 0 : -1;

        for (Position pos = move.start_get(); pos != move.end_get(); pos.x += dir_x, pos.y += dir_y)
          if (board.piecetype(pos) != nullopt_t)
            return false;
      }
    }
  }
  else // Special Move
  {
    Position king_pos = ChessBoard::initial_king_position(move.color_get());
    if (board.moved(king_pos)) return false;
    Position rook_pos = ChessBoard::initial_rook_position(move.color_get(), move.move_type_get() == Move::Type::KING_CASTLING);
    if (board.moved(rook_pos)) return false;

    auto d_file = static_cast<char>(king_pos.file_get()) - static_cast<char>(rook_pos.file_get()); // No piece between King and Rook
    char dir_x = (d_file > 0) ? 1 : -1;
    for (char file = king_pos.file_get() + dir_x ; file + dir_x != rook.file_get(); pos.x += dir_x)
      if (board.piecetype(pos) != nullopt_t)
        return false;
    Position king_end_position = ChessBoard::castling_end_position(color, move.move_type_get() == Move::Type::KING_CASTLING);

    for (char file = king_pos.file_get(); file != king_end_position.file_get(); pos.x += dir_x)
      if (board.is_attacked(move.color_get(), Position(file, king_pos.rank_get())) // first parameter is the one who is under attack
          return false;

  }
}
