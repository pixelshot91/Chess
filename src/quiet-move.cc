#include "quiet-move.hh"

QuietMove::QuietMove(plugin::Position start, plugin::Position end, plugin::PieceType type)
  : Move(QUIET), start_(start), end_(end), piecetype_(type)
{
  std::cout << "Creating QuietMove" << std::endl;
}

plugin::PieceType QuietMove::piecetype_get() const {
  return piecetype_;
}

plugin::Position QuietMove::start_get() const {
  return start_;
}

plugin::Position QuietMove::end_get() const {
  return end_;
}

std::ostream& operator<<(std::ostream& o, const plugin::Position p)
{
  o << static_cast<char>(p.file_get()) << static_cast<char>(p.rank_get());
  return o;
}

void QuietMove::print(std::ostream& o) const {
  o << static_cast<char>(piecetype_get()) << " from " << start_get() << " to " << end_get();
}


