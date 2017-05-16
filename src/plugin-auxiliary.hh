#pragma once

#include "plugin/position.hh"
#include "plugin/piece-type.hh"

namespace auxiliary {
  char PieceTypeToInt(plugin::PieceType type);
  std::string to_lan(plugin::Position pos);
}
