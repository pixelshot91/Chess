#include "knight.hh"

Knight::Knight(Color color, Position pos, PieceType piece)
    : Piece(color, pos,has_moved, PieceType::KNIGHT)
{}
