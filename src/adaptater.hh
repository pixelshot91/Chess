#pragma once

#include "chessboard.hh"
#include "plugin/chessboard-interface.hh"
#include "plugin/position.hh"

using namespace plugin;

class Adaptater : public ChessboardInterface
{
public:
  Adaptater(const ChessBoard& board);
  opt_piece_t operator[](const Position& position) const;

private:
  const ChessBoard& chessboard_;
};
