#pragma once

#include "move.hh"

#include <vector>

class History
{
public:
  History() = default;
  void add(Move& move);
  Move& last_get();

private:
  std::vector<Move*> data_;
};
