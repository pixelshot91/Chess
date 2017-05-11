#include "move.hh"

Move::Move(Type move_type, plugin::Color color)
  : move_type_(move_type), color_(color)
{}

Move::Type Move::move_type_get() const {
  return move_type_;
}

plugin::Color Move::color_get() const {
  return color_;
}

void Move::print(std::ostream& o) const {
  o << "move";//static_cast<char>(m.type_get()) << " from " << m.start_get() << " to " << m.end_get();
}

std::ostream& operator<<(std::ostream& o, const Move& m)
{
  m.print(o);
  return o;
}
