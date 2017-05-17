#include "eval_boards.hh"


EvalBoard::EvalBoard();


std::vector<std::vector<int>> EvalBoard::get_pawn_board() 
{ 
  return pawn_weight_board;
}
 
std::vector<std::vector<int>> EvalBoard::get_bishop_board() 
{ 
  return bishop_weight_board;
}

std::vector<std::vector<int>> EvalBoard::get_knight_board()
{
  return knight_weight_board;
}

std::vector<std::vector<int>> EvalBoard::get_king_end_board()
{
  return king_end_weight_board;
}

std::vector<std::vector<int>> EvalBoard::get_king_middle_board()
{
  return king_middle_weight_board;
}

std::vector<std::vector<int>> EvalBoard::get_rook_board()
{
  return rook_weight_board;
}

std::vector<std::vector<int>> EvalBoard::get_queen_board()
{
  return queen_weight_board;
}
