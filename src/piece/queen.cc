#include "queen.hh"

Queen::Queen(plugin::Color color, plugin::Position pos, bool has_moved)
  : Piece(color, pos, has_moved, plugin::PieceType::QUEEN)
{
}
