#pragma once

#include "../plugin/colors.hh"
#include "../plugin/piece-type.hh"
#include "../move.hh"


using namespace plugin;

class Piece
{
  public :
    Piece(Color color, Position pos, PieceType piece);

  public :
    Color getColor();
    PieceType getType();
    const Position& getPosition();
    uint8_t getPopulation();
    const list<Move>& getMoves();

  private :
    Color color_;
    Position pos_;
    PieceType type_;

    static uint8_t population_;
    list<Move> moves_;
};
