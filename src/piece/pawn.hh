#pragma once

#include "piece.hh"

class Pawn : public Piece
{
public:
  Pawn(plugin::Color color, plugin::Position pos, bool has_moved);
};
