#pragma once

#include "plugin/color.hh"
#include "plugin/piece-type.hh"
#include "plugin/position.hh"
#include <iostream>

class Move
{
public:
  enum Type
  {
    QUIET,
    KING_CASTLING,
    QUEEN_CASTLING
  };

  Move(Type move_type, plugin::Color color);
  Type move_type_get() const;
  plugin::Color color_get() const;
  virtual void print(std::ostream& o) const;
  virtual std::string to_lan() const;

protected:
  Type move_type_;
  plugin::Color color_;
};

std::ostream& operator<<(std::ostream& o, const Move& m);
