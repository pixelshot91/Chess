#pragma once
#include <string>
#include "player.hh"
class HumanPlayer : public Player
{
private:
  std::string player play_next_move(std::string received_move) override;
};


