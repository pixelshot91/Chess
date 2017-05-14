#pragma once

#include <vector>

#include "plugin/listener.hh"
#include "chessboard.hh"
#include "parser.hh"
#include "network-api/server-network-api.hh"

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
  //network_api::ServerNetworkAPI clients_[2];
  ChessBoard chessboard_;
};
