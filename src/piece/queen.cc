#include "queen.hh"

Queen::Queen(Color color, Position pos, PieceType piece)
    : Piece(color, pos,has_moved, PieceType::QUEEN)
{}
