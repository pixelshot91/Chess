#include "rook.hh"

Rook::Rook(Color color, Position pos, PieceType piece)
    : Piece(color, pos,has_moved, PieceType::ROOK)
{}
