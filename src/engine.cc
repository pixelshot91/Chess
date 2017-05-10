#include "engine.hh"


Engine::Engine(std::vector<plugin::Listener*> listeners, unsigned short port, ChessBoard chessboard)
  : listeners_(listeners), clients_({network_api::ServerNetworkAPI(port), network_api::ServerNetworkAPI(port)}), chessboard_(chessboard)
{}

int Engine::start()
{
  return 0;
}
