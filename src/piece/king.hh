#pragma once

#include "piece.hh"

class King : public Piece
{
public:
  King(plugin::Color color, plugin::Position pos, bool has_moved);
};
