#include "move.hh"

#include "chessboard.hh"

Move::Move(Type move_type, plugin::Color color)
  : move_type_(move_type)
  , color_(color)
  , priority_(0)
{
}

/*Move::Type Move::move_type_get() const
{
  return move_type_;
}

plugin::Color Move::color_get() const
{
  return color_;
}*/

void Move::print(std::ostream& o) const
{
  o << color_ << " ";
  if (move_type_ == KING_CASTLING)
    o << "King castling";
  else
    o << "Queen castling";
}

std::string Move::to_lan() const
{
  if (move_type_ == KING_CASTLING)
    return "O-O";
  return "O-O-O";
}

std::string Move::to_an() const
{
  std::string result;
  result += auxiliary::to_lan(ChessBoard::initial_king_position(color_));
  result += auxiliary::to_lan(ChessBoard::castling_king_end_position(color_, move_type_ == KING_CASTLING));
  return result;
}

bool Move::operator<(const Move& m) const
{
  return priority_ > m.priority_;
}


std::ostream& operator<<(std::ostream& o, const Move& m)
{
  m.print(o);
  //o << " " << m.to_lan();
  return o;
}
