#include "move.hh"

Move::Move(Position start, Position end, PieceType type)
  : start_(start), end_(end), type_(type)
{}

PieceType Move::type_get() const {
  return type_;
}

Position Move::start_get() const {
  return start_;
}

Position Move::end_get() const {
  return end_;
}

std::ostream& operator<<(std::ostream& o, const Position p)
{
  o << static_cast<char>(p.file_get()) << static_cast<char>(p.rank_get());
  return o;
}

std::ostream& operator<<(std::ostream& o, const Move& m)
{
  o << static_cast<char>(m.type_get()) << " from " << m.start_get() << " to " << m.end_get();
  return o;
}
