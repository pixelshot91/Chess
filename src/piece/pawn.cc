#include "pawn.hh"

Pawn::Pawn(Color color, Position pos, PieceType piece)
    : Piece(color, pos,has_moved, PieceType::PAWN)
{}
