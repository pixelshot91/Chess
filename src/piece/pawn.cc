#include "pawn.hh"

Pawn::Pawn(plugin::Color color, plugin::Position pos, bool has_moved)
  : Piece(color, pos, has_moved, plugin::PieceType::PAWN)
{
}
