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
    virtual bool isMoveAuthorized(Move) = 0;
    virtual bool isMoveLegal(Move) = 0;

    Color color_get();
    PieceType piecetype_get();
    const Position& position_get();
    //uint8_t population_get();
    //const list<Move>& moves_get();

  private :
    Color color_;
    Position pos_;
    PieceType type_;

    static uint8_t population_;
    list<Move> moves_;
};
