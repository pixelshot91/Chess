#pragma once

#include <boost/regex.hpp>
#include <string>
#include <memory>

#include "move.hh"
#include "quiet-move.hh"
#include "chessboard.hh"

class Parser
{
public:
  using moves_t = std::vector<std::shared_ptr<Move>>;
  Parser(std::string pgn_path);
  moves_t parse();
  static std::shared_ptr<Move> parse_move(std::string, plugin::Color color, bool pgn_check = false);
  static std::shared_ptr<Move> parse_uci(std::string, plugin::Color color, const ChessBoard& board);

private:
  static std::shared_ptr<QuietMove> generateMove(plugin::Color color, boost::smatch what);
  std::string pgn_path_;
};
