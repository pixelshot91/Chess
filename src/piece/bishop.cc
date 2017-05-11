#include "bishop.hh"

Bishop::Bishop(Color color, Position pos, PieceType piece)
    : Piece(color, pos,has_moved, PieceType::BISHOP)
{}
