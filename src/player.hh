#pragma once

#include <string>
#include <vector>
#include <memory>
#include "plugin/color.hh"
#include "move.hh"

class Player
{
public:
  Player(plugin::Color color);
  virtual std::string play_next_move(const std::string &received_move) = 0;

protected:
  const plugin::Color color_;
};
