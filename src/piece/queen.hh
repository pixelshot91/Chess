#pragma once

#include "piece.hh"

class Queen : public Piece
{
  public:
  Queen(plugin::Color color, plugin::Position pos, bool has_moved);
};
