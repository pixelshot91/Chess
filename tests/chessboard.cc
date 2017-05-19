#include "chessboard.hh"

int main()
{


  ChessBoard b;

  for(int i; i < 8; i++)
  {
    //PIECE TYPE
    if (b.piecetype_get(plugin::Position(static_cast<plugin::File> (i), plugin::Rank::ONE)) != pieceline[i])
      std::cout<< "error piece WHITE"<< std::endl;
    if (b.piecetype_get(plugin::Position(static_cast<plugin::File> (i), plugin::Rank::EIGHT)) != pieceline[i])
      std::cout<< "error piece BLACK"<< std::endl;
    if (b.piecetype_get(plugin::Position(static_cast<plugin::File> (i), plugin::Rank::SEVEN)) != pawnline[i])
      std::cout<< "error pawn BLACK"<< std::endl;
    if (b.piecetype_get(plugin::Position(static_cast<plugin::File> (i), plugin::Rank::TWO)) != pawnline[i])
      std::cout<< "error pawn WHITE"<< std::endl;

    for(int j = 2; j < 6 ; j++)
    {
      //PIECE TYPE
      if (b.piecetype_get(plugin::Position(static_cast<plugin::File> (i), static_cast<plugin::Rank> (j) )) != std::experimental::nullopt)
        std::cout<< "error not empty piece"<< std::endl;
    }
    std::cout<< "new Piece" << std::endl;
  }

  for(int i; i < 8; i++)
  {
    for(int k = 0; k < 2; k++)
    {
      if (b.color_get(plugin::Position(static_cast<plugin::File> (i), static_cast<plugin::Rank> (k) != plugin::color::BLACK )))
        std::cout<< "color fail"<< std::endl;
    }
    for(int j = 6; j < 8; j++)
    {
      if (b.color_get(plugin::Position(static_cast<plugin::File> (i), static_cast<plugin::Rank> (k) != plugin::color::WHITE)))
        std::cout<< "color fail"<< std::endl;

    }
  }


  //std::cout << static_cast<char>(b.piecetype_get(plugin::Position(plugin::File::A, plugin::Rank::ONE))) << std::endl;
  return 0;
}
