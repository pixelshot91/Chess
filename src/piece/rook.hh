#pragma once

#include "piece.hh"

class Rook : public Piece
{
  public:
  Rook(plugin::Color color, plugin::Position pos, bool has_moved);
};
