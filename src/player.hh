#pragma once

#include <string>
#include "plugin/color.hh"

class Player
{
  public:
    Player(plugin::Color color);
    virtual std::string play_next_move(const std::string& received_move) = 0;
  protected:
    const plugin::Color color_;
};
