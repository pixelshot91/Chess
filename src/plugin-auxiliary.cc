#include "plugin-auxiliary.hh"

char PieceTypeToInt(plugin::PieceType type)
{
  char i;
  for (i = 0; plugin::piecetype_array()[i] != type; ++i)
    continue;
  if (i >= 6)
    throw std::invalid_argument("Not a piece Symbol");
  return i;
}


