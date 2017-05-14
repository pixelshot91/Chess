#include "piece.hh"

Piece::Piece(plugin::Color color, plugin::Position pos, bool has_moved,
             plugin::PieceType type)
  : color_(color)
  , pos_(pos)
  , type_(type)
  , has_moved_(has_moved)
{
}

plugin::Color Piece::color_get()
{
  return color_;
}

plugin::PieceType Piece::piecetype_get()
{
  return type_;
}

const plugin::Position& Piece::position_get()
{
  return pos_;
}

/*
uint8_t Piece::getPopulation()
{
  return population_;
}

const list<Move>& Piece::getMoves()
{
  return moves_;
}
*/
