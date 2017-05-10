#pragma once

#include "plugin/position.hh"
#include "plugin/piece-type.hh"

using namespace plugin;

class Move {
public:
  Move(Position start, Position end, PieceType);
  PieceType type_get() const;
  Position start_get() const;
  Position end_get() const;
private:
  Position start_;
  Position end_;
  PieceType type_;
};

std::ostream& operator<<(std::ostream& o, const Move& m);
