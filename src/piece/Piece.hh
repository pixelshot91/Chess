#pragma once

#include "../plugin/colors.hh"
#include "../plugin/piece-type.hh"
#include "../move.hh"


using namespace plugin;

class Piece
{
  public :
    Piece(Color color_, Position pos_, PieceType piece-type_);

  public :
    const Color getColor();
    const PieceType getType();
    const Position getPosition();
    const uint8_t getPopulation();
    const list<Move> getMoves();

  private :
    Color color_;
    Position pos_;
    PieceType piece-type_;

    static uint8_t population_;
};
