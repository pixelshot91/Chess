#include "chessboard.hh"

int main()
{
  ChessBoard b;
  std::cout << static_cast<char>(b.piecetype_get(Position(plugin::File::A, plugin::Rank::ONE))) << std::endl;
}
