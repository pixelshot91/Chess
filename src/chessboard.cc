#include "chessboard.hh"

#include "rule-checker.hh"

ChessBoard::ChessBoard()
{
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

/*std::vector<Piece*> ChessBoard::get_piece()
{
  std::vector<Piece*> pieces;
  Position position;
  for(int i = 0; i < 8; i++)
    for(int j = 0; j < 8 ; j++)
    {
      position = Position(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));

    }
}*/

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
{}

plugin::Position ChessBoard::castling_end_position(plugin::Color color, bool king_side)
{}
