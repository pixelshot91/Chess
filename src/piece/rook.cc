#include "rook.hh"

Rook::Rook(plugin::Color color, plugin::Position pos, bool has_moved)
    : Piece(color, pos,has_moved, plugin::PieceType::ROOK)
{}
