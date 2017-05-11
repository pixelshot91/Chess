#include "chessboard.hh"
#include "rule-checker.hh"

ChessBoard::ChessBoard()
{
}

void ChessBoard::update(Move& move) { // FIX ME PROMOtiOn
  //if (!RuleChecker::check(*this, move)) throw invalid_move;
  
  if (move.move_type_get() == Move::Type::QUIET) {
    const QuietMove& quiet_move = reinterpret_cast<const QuietMove&>(move);
    move_piece(quiet_move.start_get(), quiet_move.end_get());
  }
  else {
    move_piece(initial_king_position(move.color_get()), castling_king_end_position(move.color_get(), move.move_type_get() == Move::Type::KING_CASTLING));
    move_piece(initial_rook_position(move.color_get(), move.move_type_get() == Move::Type::KING_CASTLING)
        , castling_rook_end_position(move.color_get(), move.move_type_get() == Move::Type::KING_CASTLING));
  }
}

void ChessBoard::move_piece(plugin::Position start, plugin::Position end) {
  cell_t moving_piece = get_square(start);
  moving_piece |= 0b00001000;
  set_square(end, moving_piece);

  set_square(start, 0x7); //0b000001111
}

void ChessBoard::print_board()
{
  for (unsigned i = 0; i < board_.size(); ++i)
  {
    for (unsigned j = 0; j < board_[0].size(); j++)
      std::cout << std::hex << std::setfill('0') << std::setw(2) << board_[i][j] << " ";
    std::cout << std::endl;
  }
}

void ChessBoard::set_square(plugin::Position position, cell_t value) {
  board_[static_cast<char>(position.file_get())][static_cast<char>(position.rank_get())] = value;
}

ChessBoard::cell_t ChessBoard::get_square(plugin::Position position)
{
  int j = (int)position.file_get();
  int i = (int)position.rank_get();
  std::cout << std::hex << std::setfill('0') << std::setw(2) << board_[i][j] << " ";
  std::cout << std::endl;
  return board_[i][j];
}

ChessBoard::cell_t ChessBoard::get_opt(plugin::Position position, cell_t mask)
{
  return get_square(position) & mask;
}

bool ChessBoard::has_moved(plugin::Position position)
{
  return (bool)get_opt(position, 0b00001000);
}

plugin::Color ChessBoard::color_get(plugin::Position position)
{
  return static_cast<plugin::Color>(get_opt(position, 0b10000000));
}

bool ChessBoard::castleflag_get(plugin::Position position)
{
  return (bool)get_opt(position, 0b00010000);
}

  std::experimental::optional<plugin::PieceType>
ChessBoard::piecetype_get(plugin::Position position)
{
  cell_t type_b = get_opt(position, 0b00000111);
  if(type_b != 0b00000111)
    return plugin::piecetype_array()[type_b];
  return std::experimental::nullopt;
}

bool ChessBoard::is_attacked(plugin::Color color, plugin::Position current_cell)
{
  //for (auto p : getPieces())
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
    {
      plugin::Position pos(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));
      if (piecetype_get(pos) != std::experimental::nullopt) {
        if (RuleChecker::check(*this,
              QuietMove(static_cast<plugin::Color>(not static_cast<bool>(color)),
                pos, current_cell, piecetype_get(pos).value(), true, false)))
          return true;
      }
    }
  return false;
}

std::vector<Piece*> ChessBoard::get_piece(Color color)
{
  std::vector<Piece*> pieces;
  Position position;
  for(int i = 0; i < 8; i++)
    for(int j = 0; j < 8 ; j++)
    {
      position = Position(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));
      if(color_get(position) == color)
      {
        switch (piecetype_get(position))
        {
          case plugin::PieceType::KING:
            pieces.push_back(&King(color_get(position), position, has_moved(position),piece_typeget(position)));
            break;

          case plugin::PieceType::QUEEN:
            pieces.push_back(&Queen(color_get(position), position, has_moved(position),piece_typeget(position)));
            break;

          case plugin::PieceType::ROOK:
            pieces.push_back(&Rook(color_get(position), position, has_moved(position),piece_typeget(position)));
            break;

          case plugin::PieceType::BISHOP:
            pieces.push_back(&Bishop(color_get(position), position, has_moved(position),piece_typeget(position)));
            break;

          case plugin::PieceType::KNIGHT:
            pieces.push_back(&Knight(color_get(position), position, has_moved(position),piece_typeget(position)));
            break;

          case plugin::PieceType::PAWN:
            pieces.push_back(&Pawn(color_get(position), position, has_moved(position),piece_typeget(position)));
            break;

          case std::experimental::nullopt:
            break;

          default:
            exit(58);
            break;
        }
      }
    }
  return pieces;
}

History ChessBoard::history_get() const {
  return history_;
}

plugin::Position ChessBoard::initial_king_position(plugin::Color c)
{
  if (c == plugin::Color::WHITE)
    return plugin::Position(plugin::File::E, plugin::Rank::ONE);
  return plugin::Position(plugin::File::E, plugin::Rank::EIGHT);
}

plugin::Position ChessBoard::initial_rook_position(plugin::Color c, bool king_side)
{
  return plugin::Position(king_side ? plugin::File::H : plugin::File::A, c == plugin::Color::WHITE ? plugin::Rank::ONE : plugin::Rank::EIGHT);
}

plugin::Position ChessBoard::castling_king_end_position(plugin::Color color, bool king_side)
{
  return plugin::Position(king_side ? plugin::File::G : plugin::File::C, color == plugin::Color::WHITE ? plugin::Rank::ONE : plugin::Rank::EIGHT);
}

plugin::Position ChessBoard::castling_rook_end_position(plugin::Color color, bool king_side)
{
  return plugin::Position(king_side ? plugin::File::F : plugin::File::D, color == plugin::Color::WHITE ? plugin::Rank::ONE : plugin::Rank::EIGHT);
}
