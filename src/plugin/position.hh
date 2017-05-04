#pragma once

namespace plugin
{

  /* The file enum represent the colomns
   * on the board */
  enum class File
  {
    A = 0,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
  };

  /* The rank enum represent the lines
   * on the board */
  enum class Rank
  {
    ONE = 0,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
  };

  /* Position represent a coordinate on the board */
  class Position final
  {
  public:
    Position(File file, Rank rank);

    File file_get() const;
    Rank rank_get() const;

  private:
    File file_;
    Rank rank_;
  };

  bool
  operator==(const Position& pos1, const Position& pos2);

  bool
  operator!=(const Position& pos1, const Position& pos2);

} // plugin

#include "position.hxx"
