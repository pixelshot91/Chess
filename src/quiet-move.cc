#include "quiet-move.hh"

QuietMove::QuietMove(plugin::Color color, plugin::Position start,
                     plugin::Position end, plugin::PieceType type, bool attack,
                     bool test, char promotion_piecetype)
  : Move(QUIET, color)
  , start_(start)
  , end_(end)
  , piecetype_(type)
  , attack_(attack)
  , test_(test)
  , promotion_piecetype_(promotion_piecetype)
{
}

plugin::PieceType QuietMove::piecetype_get() const
{
  return piecetype_;
}

char QuietMove::promotion_piecetype_get() const
{
  return promotion_piecetype_;
}

plugin::Position QuietMove::start_get() const
{
  return start_;
}

plugin::Position QuietMove::end_get() const
{
  return end_;
}

bool QuietMove::is_an_attack() const
{
  return attack_;
}

bool QuietMove::is_promotion() const
{
  return promotion_piecetype_ != -1;
}

bool QuietMove::is_a_test() const
{
  return test_;
}

std::string QuietMove::to_lan() const
{
  std::string result;
  if (piecetype_get() != plugin::PieceType::PAWN)
    result += static_cast<char>(piecetype_get());
  result += auxiliary::to_lan(start_get());
  result += attack_ ? 'x' : '-';
  result += auxiliary::to_lan(end_get());
  if (promotion_piecetype_ != -1)
  {
    result += '=';
    result += static_cast<char>(plugin::piecetype_array()[promotion_piecetype_]);
  }
  return result;
}

std::string QuietMove::to_an() const
{
  std::string result;
  result += auxiliary::to_lan(start_get());
  result += auxiliary::to_lan(end_get());
  if (promotion_piecetype_ != -1)
    result += static_cast<char>(plugin::piecetype_array()[promotion_piecetype_]);
  return result;
}


void QuietMove::print(std::ostream& o) const
{
  o << color_ << " " << piecetype_ << (attack_ ? " attack" : " move")
    << " from " << start_get() << " to " << end_get();
  if (promotion_piecetype_ != -1)
    o << " and is promoted to " << plugin::piecetype_array()[promotion_piecetype_];
}
