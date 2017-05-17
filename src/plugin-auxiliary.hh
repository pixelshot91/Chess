#pragma once

#include "plugin/color.hh"
#include "plugin/position.hh"
#include "plugin/piece-type.hh"

char operator~(plugin::File f);
char operator~(plugin::Rank r);
plugin::Color operator!(plugin::Color color);

std::ostream& operator<<(std::ostream& o, const plugin::Color c);

namespace auxiliary {

  char PieceTypeToInt(plugin::PieceType type);
  std::string to_lan(plugin::Position pos);

  int distance(plugin::Position& pos1, plugin::Position& pos2);
  void GetUnicodeChar(unsigned int code, char chars[5]);
}
