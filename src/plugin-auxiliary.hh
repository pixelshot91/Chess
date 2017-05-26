#pragma once

#include "plugin/color.hh"
#include "plugin/position.hh"
#include "plugin/piece-type.hh"
#include <chrono>
#include <iostream>
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::steady_clock;

struct scoped_timer {
  scoped_timer(double& s) : seconds(s), t0(steady_clock::now()) {}

  ~scoped_timer()
 {
   std::chrono::duration<double> diff = steady_clock::now() - t0;
 seconds = diff.count();
 }

 double&                       seconds;
 steady_clock::time_point      t0;
};

char operator~(plugin::File f);
char operator~(plugin::Rank r);
plugin::Color operator!(plugin::Color color);

std::ostream& operator<<(std::ostream& o, const plugin::Color c);
std::ostream& operator<<(std::ostream& o, const plugin::PieceType type);
std::ostream& operator<<(std::ostream& o, const plugin::Position p);

namespace auxiliary {

  constexpr char PieceTypeToInt(plugin::PieceType type) {
    switch (type)
    {
      case plugin::PieceType::KING:
        return 0;
      case plugin::PieceType::QUEEN:
        return 1;
      case plugin::PieceType::ROOK:
        return 2;
      case plugin::PieceType::BISHOP:
        return 3;
      case plugin::PieceType::KNIGHT:
        return 4;
      case plugin::PieceType::PAWN:
        return 5;
      default:
        throw std::invalid_argument("Not a piece Symbol");
    }
  }

  std::string to_lan(plugin::Position pos);

  int distance(plugin::Position& pos1, plugin::Position& pos2);
  void GetUnicodeChar(unsigned int code, char chars[5]);
}
