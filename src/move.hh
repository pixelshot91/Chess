#pragma once

#include "plugin/color.hh"
#include "plugin/piece-type.hh"
#include "plugin/position.hh"
#include "plugin-auxiliary.hh"
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
  virtual ~Move() = default;
  Type move_type_get() const {
    return move_type_;
  }
  plugin::Color color_get() const {
    return color_;
  }
  virtual void print(std::ostream& o) const;
  virtual std::string to_lan() const;
  virtual std::string to_an() const;
  virtual bool operator<(const Move&) const;

protected:
  Type move_type_;
  plugin::Color color_;
public:
  int priority_;
};

std::ostream& operator<<(std::ostream& o, const Move& m);
