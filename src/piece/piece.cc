#include "piece.hh"

Piece::Piece(Color color, Position pos, bool has_moved, PieceType type)
  : color_(color)
  , pos_(pos)
  , type_(type)
  , has_moved_(has_moved)
  {};

Color Piece::getColor()
{
  return color_;
};

PieceType Piece::getType()
{
  return type_;
}

const Position& Piece::getPosition()
{
  return pos_;
}

uint8_t Piece::getPopulation()
{
  return population_;
}

const list<Move>& Piece::getMoves()
{
  return moves_;
}
