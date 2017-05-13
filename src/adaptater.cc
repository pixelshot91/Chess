#include "adaptater.hh"

Adaptater::opt_piece_t Adaptater::operator[](const Position& position) const
{
  if (chessboard_->piecetype_get(position) == std::experimental::nullopt)
    return std::experimental::nullopt;
  return side_piece_t(chessboard_->piecetype_get(position).value(), chessboard_->color_get(position));
}
