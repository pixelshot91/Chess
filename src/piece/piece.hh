#pragma once

#include "../move.hh"
#include "../plugin/color.hh"
#include "../plugin/piece-type.hh"
#include "../plugin/position.hh"
#include <list>

class Piece
{
public:
  Piece(plugin::Color color, plugin::Position pos, bool has_moved,
        plugin::PieceType piece);

public:
  plugin::Color color_get();
  plugin::PieceType piecetype_get();
  const plugin::Position& position_get();
  // uint8_t population_get();
  // const list<Move>& moves_get();

private:
  plugin::Color color_;
  plugin::Position pos_;
  plugin::PieceType type_;
  bool has_moved_;

  static uint8_t population_;
  std::list<Move> moves_;
};
