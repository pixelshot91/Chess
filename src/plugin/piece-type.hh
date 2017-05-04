#pragma once

#include <array>

namespace plugin
{

  /* PieceType is an enum representing every possible
   * piece type present on the board. The char associated
   * with each value is the ascii char representing the
   * piece on the board */
  enum class PieceType : char
  {
    KING = 'K',
    QUEEN = 'Q',
    ROOK = 'R',
    BISHOP = 'B',
    KNIGHT = 'N',
    PAWN = 'P'
  };

  /* can be useful to iterate over PieceTypes */
  constexpr
  std::array<PieceType, 6> piecetype_array()
  {
    return { PieceType::KING,
             PieceType::QUEEN,
             PieceType::ROOK,
             PieceType::BISHOP,
             PieceType::KNIGHT,
             PieceType::PAWN };
  }

} // plugin
