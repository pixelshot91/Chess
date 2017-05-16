#include "plugin-auxiliary.hh"

namespace auxiliary {
  char PieceTypeToInt(plugin::PieceType type)
  {
    char i;
    for (i = 0; plugin::piecetype_array()[i] != type; ++i)
      continue;
    if (i >= 6)
      throw std::invalid_argument("Not a piece Symbol");
    return i;
  }

  std::string to_lan(plugin::Position pos)
  {
    std::string result;
    result += static_cast<char>(pos.file_get()) + 'a';
    result += static_cast<char>(pos.rank_get()) + '1';
    return result;
    //return std::string((char)(static_cast<char>(pos.file_get()) + 'a')) + std::string(static_cast<char>(pos.rank_get()) + '1');
  }
}
