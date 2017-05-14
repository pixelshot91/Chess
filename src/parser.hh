#pragma once

#include <boost/regex.hpp>
#include <string>

#include "move.hh"
#include "quiet-move.hh"

class Parser
{
public:
  Parser(std::string pgn_path);
  std::vector<Move*> parse();
  static Move& parse_move(std::string, plugin::Color color);

private:
  static QuietMove* generateMove(plugin::Color color, boost::smatch what);
  std::string pgn_path_;
};
