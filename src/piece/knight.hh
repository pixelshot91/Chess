#pragma once

#include "piece.hh"

class Knight : public Piece
{
public:
  Knight(plugin::Color color, plugin::Position pos, bool has_moved);
};
