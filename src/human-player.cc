#include "human-player.hh"

std::string HumanPlayer::player play_next_move(std::string received_move)
{
  std::string input;
  received_move=received_move;
  std::cout << "Wainting for input : ";
  std::cin >> input;
  return input;
}

