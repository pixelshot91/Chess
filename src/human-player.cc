#include "human-player.hh"
#include <iostream>
std::string HumanPlayer::play_next_move(const std::string& received_move)
{
  std::string input;
  std::cout << received_move << std::endl <<"Wainting for input : ";
  std::cin >> input;
  return input;
}

