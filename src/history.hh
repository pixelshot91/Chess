#pragma once

#include "move.hh"

#include <vector>

class History
{
public:
  History() = default;
  //add();
  Move& last_get();
private:
  std::vector<Move*> data_;
};
