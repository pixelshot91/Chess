#include "rule-checker.hh"
#include "plugin-auxiliary.hh"

char operator~(plugin::Color c)
{
  return static_cast<bool>(c) ? 'B' : 'W';
}

std::ostream& operator<<(std::ostream& o, const plugin::Position& p)
{
  o << (char)(static_cast<char>(p.file_get()) + 'A')
    << (char)(static_cast<char>(p.rank_get()) + '1');
  return o;
}

bool RuleChecker::invalid_move(std::string reason)
{
  reason = reason;
  //std::cerr << "Invalid move : " << reason << std::endl;
  return false;
}

bool RuleChecker::is_move_valid(const ChessBoard& board, Move& move)
{
  if (!isMoveAuthorized(board, move))
    return false; // Based on the Piece Type and position of start and end cell
  if (!isMoveLegal(board, move))
    return false;
  //std::cerr << move << " is valid" << std::endl;
  if (move.move_type_get() == Move::Type::QUIET)
  {
    const QuietMove& quiet_move = static_cast<const QuietMove&>(move);
    auto end_pos = quiet_move.end_get();
    if (board.piecetype_get(end_pos) != std::experimental::nullopt and board.piecetype_get(end_pos).value() == plugin::PieceType::KING)
      return true;
  }
  //std::cerr << "checking that " << move << " doesn't cause a check to " << move.color_get() << std::endl;
  auto tmp = ChessBoard(board);
  tmp.apply_move(move);
  plugin::Position king_pos = tmp.get_king_position(move.color_get());
  //std::cerr << "king_pos is " << king_pos << std::endl;
  if (isCheck(tmp, king_pos))
    return false;
  return true;
}

bool RuleChecker::isMoveAuthorized(const ChessBoard& board, Move& move)
{
  if (move.move_type_get() == Move::Type::QUIET)
  {
    QuietMove& quiet_move = static_cast<QuietMove&>(move);
    if (quiet_move.end_get() == quiet_move.start_get())
      return invalid_move("No move has been made");
    if (quiet_move.is_promotion() and
        not(quiet_move.end_get().rank_get() == plugin::Rank::EIGHT or
          quiet_move.end_get().rank_get() == plugin::Rank::ONE))
      return invalid_move("illegal promotion");
    if (board.piecetype_get(quiet_move.start_get()) !=
        quiet_move.piecetype_get())
    {
      return invalid_move("The piece is not at its starting point");
      /*if (board.piecetype_get(quiet_move.start_get()) ==
        std::experimental::nullopt) {
        std::cerr << "Cell is empty" << std::endl;
        ;
        }
        std::cerr << "board at " << quiet_move.start_get() << " is " <<
        static_cast<char>(board.piecetype_get(quiet_move.start_get()).value()) <<
        " differ from move : " << static_cast<char>(quiet_move.piecetype_get()) <<
        std::endl;
        return false;*/
    }
    auto d_file = ~quiet_move.end_get().file_get() -
      static_cast<char>(quiet_move.start_get().file_get());
    auto d_rank = static_cast<char>(quiet_move.end_get().rank_get()) -
      static_cast<char>(quiet_move.start_get().rank_get());
    switch (quiet_move.piecetype_get())
    {
      case plugin::PieceType::PAWN:
        {
          char dir_rank = (move.color_get() == plugin::Color::WHITE) ? 1 : -1;
          if (abs(d_file) == 1 and d_rank == dir_rank)
          {
            quiet_move.set_as_attack(true);
            return true;
          }
          if (quiet_move.is_an_attack())
            return false;
          if (d_file == 0 and (d_rank == dir_rank or
                (d_rank == 2 * dir_rank and !board.has_moved(quiet_move.start_get()))))
            return true;
          return false;
        }
      case plugin::PieceType::BISHOP:
        return abs(d_file) == abs(d_rank);
        break;
      case plugin::PieceType::KNIGHT:
        return (abs(d_file) == 1 and abs(d_rank) == 2) or
          (abs(d_file) == 2 and abs(d_rank) == 1);
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

bool RuleChecker::isMoveLegal(const ChessBoard& board, Move& move)
{
  if (move.move_type_get() == Move::Type::QUIET)
  {
    QuietMove& quiet_move = static_cast<QuietMove&>(move);
    /*if (quiet_move.piecetype_get() == plugin::PieceType::KING and
      board.is_attacked(quiet_move.color_get(), quiet_move.end_get()))
      return invalid_move("Can't move King to check position");*/
    if (board.piecetype_get(quiet_move.end_get()) !=
        std::experimental::nullopt and
        quiet_move.color_get() == board.color_get(quiet_move.end_get()))
      return invalid_move("cant move to a cell containing the same color as "
          "the moving piece"); // same color

    if (quiet_move.piecetype_get() == plugin::PieceType::PAWN)
    {
      if (quiet_move.is_an_attack())
      {
        if (board.piecetype_get(quiet_move.end_get()) == std::experimental::nullopt and not quiet_move.is_a_test()) // en passant
        {
          //Move& last_move = board.history_get().last_get();
          if (board.last_move_get() == nullptr)
            return invalid_move("There was no last move");
          Move& last_move = *board.last_move_get();
          if (last_move.move_type_get() != Move::Type::QUIET) {
            return invalid_move("last move was not a quiet move");
          }
          const QuietMove& last_quiet_move =
            static_cast<const QuietMove&>(last_move);
          if (last_quiet_move.piecetype_get() != plugin::PieceType::PAWN)
            return invalid_move("The last piece to move was not a pawn");
          if (last_quiet_move.end_get().file_get() !=
              quiet_move.end_get().file_get())
            return invalid_move("The last moving pawn and the current pawn wont be in the same file");
          if (last_quiet_move.end_get().rank_get() !=
              quiet_move.start_get().rank_get())
            return invalid_move("The pawn is not one the same rank as the pawn to eat");
          if (last_quiet_move.color_get() == quiet_move.color_get() or
              abs(~last_quiet_move.end_get().rank_get() - ~last_quiet_move.start_get().rank_get()) != 2)
            return invalid_move("The pawn cannot move in diagonal if its not a en passant attack");
        }
      }
      else if (board.piecetype_get(quiet_move.end_get()) != std::experimental::nullopt)
        return invalid_move("The pawn cant move to an occupied cell");
    } // END En passant
    else if (board.piecetype_get(quiet_move.end_get()) != std::experimental::nullopt) // Not an attack -> the cell must be empty
      quiet_move.set_as_attack(true);
    if (quiet_move.piecetype_get() != plugin::PieceType::KNIGHT)
      // In all case check that the piece doesn't go through another peice (except for the Knight)
    { // Piece in the path
      auto d_file = static_cast<char>(quiet_move.end_get().file_get()) -
        static_cast<char>(quiet_move.start_get().file_get());
      auto d_rank = static_cast<char>(quiet_move.end_get().rank_get()) -
        static_cast<char>(quiet_move.start_get().rank_get());
      char dir_x = (d_file > 0) ? 1 : (d_file == 0) ? 0 : -1;
      char dir_y = (d_rank > 0) ? 1 : (d_rank == 0) ? 0 : -1;
      char file =
        static_cast<char>(quiet_move.start_get().file_get()) + dir_x;
      char rank =
        static_cast<char>(quiet_move.start_get().rank_get()) + dir_y;
      for (;
          not(file == static_cast<char>(quiet_move.end_get().file_get()) and
            rank == static_cast<char>(quiet_move.end_get().rank_get()));
          file += dir_x, rank += dir_y)
      {
        if (board.piecetype_get(plugin::Position(
                static_cast<plugin::File>(file),
                static_cast<plugin::Rank>(rank))) != std::experimental::nullopt)
          return invalid_move("Cant move through piece");
      }
    }

  } // END Quiet Move
  else // Special Move  // CHECKING IS CASTLING POSSIBLE
  {
    plugin::Position king_pos =
      ChessBoard::initial_king_position(move.color_get());
    if (board.piecetype_get(king_pos) != plugin::PieceType::KING or board.has_moved(king_pos))
      return invalid_move("king has already moved");

    plugin::Position rook_pos = ChessBoard::initial_rook_position(
        move.color_get(), move.move_type_get() == Move::Type::KING_CASTLING);
    if (board.piecetype_get(rook_pos) != plugin::PieceType::ROOK or board.has_moved(rook_pos))
      return invalid_move("rook has already moved");

    char d_file =
      static_cast<char>(rook_pos.file_get()) -
      static_cast<char>(king_pos.file_get()); // No piece between King and Rook
    char dir_x = (d_file > 0) ? 1 : -1;
    for (char file = static_cast<char>(king_pos.file_get()) + dir_x;
        file + dir_x != static_cast<char>(rook_pos.file_get()); file += dir_x)
    {
      /* std::cerr << "checking : "
         << plugin::Position(
         static_cast<plugin::File>(file),
         static_cast<plugin::Rank>(king_pos.rank_get()))
         << std::endl;*/
      if (board.piecetype_get(plugin::Position(static_cast<plugin::File>(file),
              king_pos.rank_get())) !=
          std::experimental::nullopt)
        return invalid_move("Cell between king and rook are not empty");
    }
    plugin::Position king_end_position = ChessBoard::castling_king_end_position(
        move.color_get(), move.move_type_get() == Move::Type::KING_CASTLING);

    for (char file = static_cast<char>(king_pos.file_get());
        file != static_cast<char>(king_end_position.file_get()); file += dir_x)
    {
      /*std::cerr << "checking CHECK at "
        << plugin::Position(static_cast<plugin::File>(file),
        king_pos.rank_get())
        << std::endl;*/
      if (board.is_attacked(move.color_get(),
            plugin::Position(static_cast<plugin::File>(file),
              king_pos.rank_get()))) // first
        // parameter
        // is the
        // one who
        // is under
        // attack
        return invalid_move("The king go through a cell in check");
    }
  }
  return true;
}

bool RuleChecker::no_possible_move(const ChessBoard& board, plugin::Color color)
{
  for (int i = 0; i < 8; ++i)
  {
    for (int j = 0; j < 8; ++j)
    {
      plugin::Position start_pos(static_cast<plugin::File>(i),
          static_cast<plugin::Rank>(j));
      if (board.piecetype_get(start_pos) != std::experimental::nullopt and
          board.color_get(start_pos) == color)
      {
        for (int x = 0; x < 8; ++x)
        {
          for (int y = 0; y < 8; ++y)
          {
            if (i == x and j == y)
              continue;
            plugin::Position end_pos(static_cast<plugin::File>(x),
                static_cast<plugin::Rank>(y));
            for (int attack = 0;
                attack <= 0 /*(board.piecetype_get(start_pos).value() ==
                              plugin::PieceType::PAWN)*/;
                ++attack)
            {
              QuietMove quiet_move(color, start_pos, end_pos,
                  board.piecetype_get(start_pos).value(),
                  attack, false);
              if (RuleChecker::is_move_valid(board, quiet_move)) {
                //std::cerr << "A move is still valid: "<< quiet_move << std::endl;
                return false;
              }
            }
          }
        }
      }
    }
  }
  Move king_castling(Move::Type::KING_CASTLING, color);
  if (RuleChecker::is_move_valid(board, king_castling))
    return false;
  Move queen_castling(Move::Type::QUEEN_CASTLING, color);
  if (RuleChecker::is_move_valid(board, queen_castling))
    return false;
  return true;
}

std::vector<std::shared_ptr<Move>> RuleChecker::possible_moves(const ChessBoard& board, plugin::Color color)
{
  std::vector<std::shared_ptr<Move>> moves;
  for (int i = 0; i < 8; ++i)
  {
    for (int j = 0; j < 8; ++j)
    {
      plugin::Position start_pos(static_cast<plugin::File>(i),
          static_cast<plugin::Rank>(j));
      if (board.piecetype_get(start_pos) != std::experimental::nullopt and
          board.color_get(start_pos) == color)
      {
        for (char x = 0; x < 8; ++x)
        {
          for (char y = 0; y < 8; ++y)
          {
            if (~start_pos.file_get() == x and ~start_pos.rank_get() == y)
              continue;
            plugin::Position end_pos(static_cast<plugin::File>(x),
                static_cast<plugin::Rank>(y));
            for (int attack = 0; attack <= 0; ++attack)
            {
              if (board.piecetype_get(start_pos) == plugin::PieceType::PAWN)
              {
                QuietMove quiet_move(color, start_pos, end_pos,
                    board.piecetype_get(start_pos).value(),
                    attack, false, 1);
                if (RuleChecker::is_move_valid(board, quiet_move))
                  moves.push_back(std::make_shared<QuietMove>(quiet_move));
              }
              QuietMove quiet_move(color, start_pos, end_pos,
                  board.piecetype_get(start_pos).value(),
                  attack, false);
              if (RuleChecker::is_move_valid(board, quiet_move))
                moves.push_back(std::make_shared<QuietMove>(quiet_move));
            }
          }
        }
      }
    }
  }
  Move king_castling(Move::Type::KING_CASTLING, color);
  if (RuleChecker::is_move_valid(board, king_castling))
    moves.push_back(std::make_shared<Move>(Move::Type::KING_CASTLING, color));
  Move queen_castling(Move::Type::QUEEN_CASTLING, color);
  if (RuleChecker::is_move_valid(board, queen_castling))
    moves.push_back(std::make_shared<Move>(Move::Type::QUEEN_CASTLING, color));
  //std::cerr << "RuleChecker::possible_moves genereated " << moves.size() << " moves at " << start_pos << std::endl;
  return moves;

}

// QuietMove(plugin::Color color, plugin::Position start, plugin::Position end,
// plugin::PieceType, bool attack, bool promotion);

/*bool RuleChecker::isCheckmate(const ChessBoard& board,
  plugin::Position king_pos)
  {

  plugin::Color color = board.color_get(king_pos);
  for (int i = 0; i < 8; ++i)
  {
  for (int j = 0; j < 8; ++j)
  {
  plugin::Position start_pos(static_cast<plugin::File>(i),
  static_cast<plugin::Rank>(j));
  if (board.piecetype_get(start_pos) != std::experimental::nullopt and
  board.color_get(start_pos) == color)
  {
  for (int x = 0; x < 8; ++x)
  {
  for (int y = 0; y < 8; ++y)
  {
  if (i == x and j == y)
  continue;
  plugin::Position end_pos(static_cast<plugin::File>(x),
  static_cast<plugin::Rank>(y));
  for (int attack = 0;
  attack <= (board.piecetype_get(start_pos).value() ==
  plugin::PieceType::PAWN);
  ++attack)
  {
  QuietMove quiet_move(color, start_pos, end_pos,
  board.piecetype_get(start_pos).value(),
  attack, false);
  if (RuleChecker::clear_check(board, quiet_move))
  return false;
  }
  }
  }
  }
  }
  }
  return true;
  }

  bool RuleChecker::clear_check(const ChessBoard& board, Move& move)
  {
  if (!RuleChecker::is_move_valid(board, move)) return false;

  ChessBoard tmp(board);
//tmp.print();

tmp.apply_move(move);
//tmp.print();

if (isCheck(tmp, tmp.get_king_position(move.color_get())))
return false;
return true;
}*/

bool RuleChecker::isCheck(const ChessBoard& board, plugin::Position king_pos)
{
  return board.is_attacked(board.color_get(king_pos), king_pos);
}

/*bool RuleChecker::is_a_draw(const ChessBoard& board)
  {
  return false;
  }*/

bool RuleChecker::three_fold_repetition(const std::vector<ChessBoard::board_t> permanent, const std::vector<ChessBoard*> temp)
{
  int counter = 0;
  const ChessBoard::board_t& last_board = temp.back()->board_get();
  for (auto it = temp.rbegin() + 1; it != temp.rend(); ++it) {
    if ((*it)->board_get() == last_board) {
      if (counter == 1)
        return true;
      ++counter;
    }
  }
  for (auto it = permanent.rbegin(); it != permanent.rend(); ++it) {
    if (*it == last_board) {
      if (counter == 1)
        return true;
      ++counter;
    }
  }
  return false;
}

