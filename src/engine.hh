#pragma once

#include <vector>

#include "chessboard.hh"
#include "network-api/server-network-api.hh"
#include "parser.hh"
#include "plugin/listener.hh"

class Engine
{
public:
  Engine(std::vector<plugin::Listener*> listeners, unsigned short port);
  Engine(std::vector<plugin::Listener*> listeners, std::string pgn_path);
  int start();

private:
  unsigned short port_;
  std::string pgn_path_;
  std::vector<plugin::Listener*> listeners_;
  ChessBoard chessboard_;
};
