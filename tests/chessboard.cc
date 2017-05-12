#include "chessboard.hh"

int main()
{
  ChessBoard b;
  if (b.piecetype_get(plugin::Position(plugin::File::A, plugin::Rank::ONE)) != plugin::PieceType::ROOK)
    return -1;

  //std::cout << static_cast<char>(b.piecetype_get(plugin::Position(plugin::File::A, plugin::Rank::ONE))) << std::endl;
  return 0;
}
