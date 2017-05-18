#include "move.hh"

Move::Move(Type move_type, plugin::Color color)
  : move_type_(move_type)
  , color_(color)
{
}

Move::Type Move::move_type_get() const
{
  return move_type_;
}

plugin::Color Move::color_get() const
{
  return color_;
}

void Move::print(std::ostream& o) const
{
  o << color_;
  if (move_type_ == KING_CASTLING)
    o << "King castling" << std::endl;
  else
    o << "Queen castling" << std::endl;
}

std::string Move::to_lan() const
{
  if (move_type_ == KING_CASTLING)
    return "O-O";
  return "O-O-O";
}


std::ostream& operator<<(std::ostream& o, const Move& m)
{
  m.print(o);
  //o << " " << m.to_lan();
  return o;
}
