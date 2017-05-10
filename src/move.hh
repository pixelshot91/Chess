#pragma once

#include "plugin/position.hh"
#include <iostream>
#include "plugin/piece-type.hh"


class Move {
public:
  enum Type {
    QUIET,
    CASTLING
  };
  
  Move(Type move_type);
  Type move_type_get() const;
  virtual void print(std::ostream& o) const;
private:
  Type move_type_;
};

std::ostream& operator<<(std::ostream& o, const Move& m);
