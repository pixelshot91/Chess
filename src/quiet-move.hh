#pragma once

#include "move.hh"

class QuietMove : public Move
{
public:
  QuietMove(plugin::Color color, plugin::Position start, plugin::Position end,
            plugin::PieceType, bool attack = false, bool test = false, char promotion_piecetype = - 1);
  plugin::PieceType piecetype_get() const {
    return piecetype_;
  }
  char promotion_piecetype_get() const {
    return promotion_piecetype_;
  }
  plugin::Position start_get() const {
    return start_;
  }
  plugin::Position end_get() const {
    return end_;
  }
  bool is_an_attack() const {
    return attack_;
  }
  void set_as_attack(bool attack) {
    attack_ = attack;
    priority_ = 1;
  }
  bool is_promotion() const {
    return promotion_piecetype_ != -1;
  }
  bool is_a_test() const {
    return test_;
  }

  void print(std::ostream& o) const override;
  std::string to_lan() const override;
  std::string to_an() const override;

  bool operator<(const Move&) const;

private:
  plugin::Position start_;
  plugin::Position end_;
  plugin::PieceType piecetype_;
  bool attack_;
  bool test_;
  char promotion_piecetype_;
};
