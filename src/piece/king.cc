#include "king.hh"

King::King(plugin::Color color, plugin::Position pos, bool has_moved)
    : Piece(color, pos, has_moved, plugin::PieceType::KING)
{}
