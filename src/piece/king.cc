#include "king.hh"

King::King(Color color, Position pos, PieceType piece)
    : Piece(color, pos, has_moved, PieceType::KING)
{}
