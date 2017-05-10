#pragma once

#include "plugin/chessboard-interface.hh"
#include "plugin/position.hh"
#include "chessboard.hh"

using namespace plugin;

class Adaptater : public ChessboardInterface
{
public:
  opt_piece_t operator[](const Position& position) const;
private:
  ChessBoard* chessboard_;
};
