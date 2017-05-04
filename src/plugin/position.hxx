#pragma once

#include "position.hh"

#include <iostream>

namespace plugin
{

  inline
  Position::Position(File file, Rank rank)
    : file_(file)
    , rank_(rank)
  {}

  inline
  bool
  operator==(const Position& pos1, const Position& pos2)
  {
    return pos1.file_get() == pos2.file_get()
           && pos1.rank_get() == pos2.rank_get();
}

  inline
  bool
  operator!=(const Position& pos1, const Position& pos2)
  {
    return !(pos1 == pos2);
  }

  inline
  File
  Position::file_get() const
  {
    return file_;
  }

  inline
  Rank
  Position::rank_get() const
  {
    return rank_;
  }

} // plugin
