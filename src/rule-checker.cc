#include "rule-checker.hh"

char operator~(plugin::File f)
{
  return static_cast<char>(f);
}

char operator~(plugin::Rank r)
{
  return static_cast<char>(r);
}

char operator~(plugin::Color c)
{
  return static_cast<bool>(c) ? 'B' : 'W';
}

std::ostream& operator<<(std::ostream& o, const plugin::Position& p)
{
  o << (char)(static_cast<char>(p.file_get()) + 'A') << (char)(static_cast<char>(p.rank_get()) + '1');
  return o;
}

bool RuleChecker::check(const ChessBoard& board, const Move& move)
{
  if (!isMoveAuthorized(board, move)) return false; // Based on the Piece Type and position of start and end cell
  std::cout << "Move AUTHORIZED" << std::endl;
  return isMoveLegal(board, move);

}

bool RuleChecker::isMoveAuthorized(const ChessBoard& board, const Move& move)
{
  if (move.move_type_get() == Move::Type::QUIET)
  {
    const QuietMove& quiet_move = static_cast<const QuietMove&>(move);
    if (quiet_move.end_get() == quiet_move.start_get()) throw std::invalid_argument("No move has been made");
    if (quiet_move.is_promotion() and not (quiet_move.end_get().rank_get() == plugin::Rank::EIGHT or quiet_move.end_get().rank_get() == plugin::Rank::ONE)) throw std::invalid_argument("illegal promotion");
    if (board.piecetype_get(quiet_move.start_get()) != quiet_move.piecetype_get()) {
      throw std::invalid_argument("The piece is not at its starting point");
      /*if (board.piecetype_get(quiet_move.start_get()) == std::experimental::nullopt) {
        std::cerr << "Cell is empty" << std::endl;
        ;
      }
      std::cerr << "board at " << quiet_move.start_get() << " is " << static_cast<char>(board.piecetype_get(quiet_move.start_get()).value()) << " differ from move : " << static_cast<char>(quiet_move.piecetype_get()) << std::endl;
      return false;*/
    }
    auto d_file = ~quiet_move.end_get().file_get() - static_cast<char>(quiet_move.start_get().file_get());
    auto d_rank = static_cast<char>(quiet_move.end_get().rank_get()) - static_cast<char>(quiet_move.start_get().rank_get());
    switch(quiet_move.piecetype_get()) {
      case plugin::PieceType::PAWN:
        {
          char dir_rank = (move.color_get() == plugin::Color::WHITE) ? 1 : -1;
          if (quiet_move.is_an_attack())
            return abs(d_file) == 1 and d_rank == dir_rank;
          else {
            return d_file == 0 and (d_rank == dir_rank or (d_rank == 2 * dir_rank and !board.has_moved(quiet_move.start_get())));
          }
        }
      case plugin::PieceType::BISHOP:
        return abs(d_file) == abs(d_rank);
        break;
      case plugin::PieceType::KNIGHT:
        return (abs(d_file) == 1 and abs(d_rank) == 2)
          or (abs(d_file) == 2 and abs(d_rank) == 1);
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

bool RuleChecker::isMoveLegal(const ChessBoard& board, const Move& move)
{
  if (move.move_type_get() == Move::Type::QUIET)
  {
    const QuietMove& quiet_move = static_cast<const QuietMove&>(move);
    if (quiet_move.piecetype_get() == plugin::PieceType::KING and board.is_attacked(quiet_move.color_get(), quiet_move.end_get())) throw std::invalid_argument("Can't move King to check position");
      
    if (quiet_move.piecetype_get() == plugin::PieceType::PAWN and quiet_move.is_an_attack()) {
      if (board.piecetype_get(quiet_move.end_get()) != std::experimental::nullopt) // case non vide
        return  quiet_move.color_get() != board.color_get(quiet_move.end_get()); // different color
      else { // en passant ?
        Move& last_move = board.history_get().last_get();
        if (last_move.move_type_get() != Move::Type::QUIET) return false;
        const QuietMove& last_quiet_move = static_cast<const QuietMove&>(last_move);
        return last_move.move_type_get() == Move::Type::QUIET and last_quiet_move.piecetype_get() == plugin::PieceType::PAWN and last_quiet_move.end_get().file_get() == quiet_move.end_get().file_get() and last_quiet_move.end_get().rank_get() == quiet_move.start_get().rank_get() and last_quiet_move.color_get() != quiet_move.color_get() and abs(static_cast<char>(last_quiet_move.end_get().rank_get()) - static_cast<char>(last_quiet_move.start_get().rank_get())) == 2;
      }
    }
    else {
      if (not quiet_move.is_a_test() and quiet_move.is_an_attack() and board.piecetype_get(quiet_move.end_get()) == std::experimental::nullopt) throw std::invalid_argument("can't attack an empty cell");
      if (board.piecetype_get(quiet_move.end_get()) != std::experimental::nullopt and quiet_move.color_get() == board.color_get(quiet_move.end_get())) {
        std::cout << "color " << ~board.color_get(quiet_move.end_get()) << std::endl;
        throw std::invalid_argument("cant move to a cell containing the same color as the moving piece"); // same color
      }
      if (quiet_move.piecetype_get() != plugin::PieceType::KNIGHT) { // Piece in the path

        std::cout << " MOVE THROUGH PIECE " << std::endl;
        auto d_file = static_cast<char>(quiet_move.end_get().file_get()) - static_cast<char>(quiet_move.start_get().file_get());
        auto d_rank = static_cast<char>(quiet_move.end_get().rank_get()) - static_cast<char>(quiet_move.start_get().rank_get());
        char dir_x = (d_file > 0) ? 1 : (d_file == 0) ? 0 : -1;
        char dir_y = (d_rank > 0) ? 1 : (d_rank == 0) ? 0 : -1;
        char file = static_cast<char>(quiet_move.start_get().file_get()) + dir_x;
        char rank = static_cast<char>(quiet_move.start_get().rank_get()) + dir_y;
        std::cout << "file " << (int)file << " rank " << (int) rank << std::endl;
        for (; not (file == static_cast<char>(quiet_move.end_get().file_get()) and rank == static_cast<char>(quiet_move.end_get().rank_get())); file += dir_x, rank += dir_y) {
          std::cout << "checking : " << plugin::Position(static_cast<plugin::File>(file), static_cast<plugin::Rank>(rank)) << std::endl;
          if (board.piecetype_get(plugin::Position(static_cast<plugin::File>(file), static_cast<plugin::Rank>(rank))) != std::experimental::nullopt)
            throw std::invalid_argument("Cant move through piece");
        }
      }
    }
  }
  else // Special Move
  {
    plugin::Position king_pos = ChessBoard::initial_king_position(move.color_get());
    if (board.has_moved(king_pos)) throw std::invalid_argument("king has already moved");
    plugin::Position rook_pos = ChessBoard::initial_rook_position(move.color_get(), move.move_type_get() == Move::Type::KING_CASTLING);
    if (board.has_moved(rook_pos)) throw std::invalid_argument("rook has already moved");

    std::cout << "king pos " << king_pos << " rook pos " << rook_pos << std::endl;

    char d_file = static_cast<char>(rook_pos.file_get()) - static_cast<char>(king_pos.file_get()); // No piece between King and Rook
    char dir_x = (d_file > 0) ? 1 : -1;
    std::cout << "d_file = " << (int)d_file << std::endl;
    std::cout << "dir_x " << (int) dir_x << std::endl;
    for (char file = static_cast<char>(king_pos.file_get()) + dir_x; file + dir_x != static_cast<char>(rook_pos.file_get()); file += dir_x) {
      std::cout << "checking : " << plugin::Position(static_cast<plugin::File>(file), static_cast<plugin::Rank>(king_pos.rank_get())) << std::endl;
      if (board.piecetype_get(plugin::Position(static_cast<plugin::File>(file), king_pos.rank_get())) != std::experimental::nullopt)
        throw std::invalid_argument("Cell between king and rook are not empty");
    }
    plugin::Position king_end_position = ChessBoard::castling_king_end_position(move.color_get(), move.move_type_get() == Move::Type::KING_CASTLING);

    for (char file = static_cast<char>(king_pos.file_get()); file != static_cast<char>(king_end_position.file_get()); file += dir_x) {
      std::cout << "checking CHECK at " << plugin::Position(static_cast<plugin::File>(file), king_pos.rank_get()) << std::endl;
      if (board.is_attacked(move.color_get(), plugin::Position(static_cast<plugin::File>(file), king_pos.rank_get()))) // first parameter is the one who is under attack
        throw std::invalid_argument("The king go through a cell in check");
    }

  }
  return true;
}
