#pragma once

class King : public Piece
{
  King(Color color, Position pos, PieceType piece);
    : Piece(color, pos, PieceType::KING);
}
