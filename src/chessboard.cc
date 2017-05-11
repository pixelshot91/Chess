#include "chessboard.hh"

ChessBoard::ChessBoard()
{
board_ = {
  {20, 30, 40, 50, 60, 70, 80, 90},
  {20, 30, 40, 50, 60, 70, 80, 90},
  {20, 30, 40, 50, 60, 70, 80, 90},
  {20, 30, 40, 50, 60, 70, 80, 90},
  {20, 30, 40, 50, 60, 70, 80, 90},
  {20, 30, 40, 50, 60, 70, 80, 90},
  {20, 30, 40, 50, 60, 70, 80, 90},
  {20, 30, 40, 50, 60, 70, 80, 90}
};
}

void ChessBoard::print_board()
{
/*
  for (int i = 0; i < siz_ar; ++i)
    cout << ar[i] << " ";
  cout << endl;
  */

  for (unsigned i = 0; i < board_.size(); ++i)
  {
    for (unsigned j = 0; j < board_[0].size(); j++)
      std::cout << std::hex << std::setfill('0') << std::setw(2) << board_[i][j] << " ";
    std::cout << std::endl;
  }
}
