#pragma once

#include "move.hh"

class QuietMove : public Move
{
public:
  QuietMove(plugin::Position start, plugin::Position end, plugin::PieceType);
  plugin::PieceType piecetype_get() const;
  plugin::Position start_get() const;
  plugin::Position end_get() const;

  void print(std::ostream& o) const override;
private:
  plugin::Position start_;
  plugin::Position end_;
  plugin::PieceType piecetype_;
};
std::ostream& operator<<(std::ostream& o, const QuietMove& m);
