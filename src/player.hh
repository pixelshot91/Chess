#pragma once

class Player
{
  public:
    virtual std::string play_next_move(const std::string& received_move) = 0;
};
