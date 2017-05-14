#pragma once

#include "move.hh"

class QuietMove : public Move
{
public:
  QuietMove(plugin::Color color, plugin::Position start, plugin::Position end,
            plugin::PieceType, bool attack, bool promotion);
  plugin::PieceType piecetype_get() const;
  plugin::Position start_get() const;
  plugin::Position end_get() const;
  bool is_an_attack() const;
  bool is_promotion() const;
  bool is_a_test() const;

  void print(std::ostream& o) const override;

private:
  plugin::Position start_;
  plugin::Position end_;
  plugin::PieceType piecetype_;
  bool attack_;
  bool promotion_;
  bool test_;
};
std::ostream& operator<<(std::ostream& o, const QuietMove& m);
