#pragma once

#include "piece.hh"

class Bishop : public Piece
{
public:
  Bishop(plugin::Color color, plugin::Position pos, bool has_moved);
};
