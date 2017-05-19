#pragma once
#include <string>
#include "player.hh"
class HumanPlayer : public Player
{
public:
  HumanPlayer(plugin::Color c);
  std::string play_next_move(const std::string& received_move) override;
  void set_scripted_moves( std::vector<std::shared_ptr<Move>> moves) override;
};


