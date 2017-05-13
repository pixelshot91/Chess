#pragma once

#include <string>
#include <boost/regex.hpp>

#include "move.hh"
#include "quiet-move.hh"

class Parser
{
public:
  Parser(std::string pgn_path);
  std::vector<Move*> parse();
  static Move& parse_move(std::string);
private:
  static QuietMove* generateMove(plugin::Color color, boost::smatch what);
  std::string pgn_path_;
};
