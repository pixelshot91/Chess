#include "engine.hh"


Engine::Engine(std::vector<plugin::Listener*> listeners, unsigned short port, ChessBoard chessboard)
  : port_(port), listeners_(listeners), /*clients_({network_api::ServerNetworkAPI(port), network_api::ServerNetworkAPI(port)}),*/ chessboard_(chessboard)
{}

int Engine::start()
{
  network_api::ServerNetworkAPI* clients_[2];
  std::string login[2];
  for (int i = 0; i < 2; ++i) { // Connection
    clients_[i] = new network_api::ServerNetworkAPI(port_);
    login[i] = clients_[i]->acknowledge(i);
  }

  for (int i = 0; i < 2; ++i) { // Initialization
    clients_[i]->send("uci");
    clients_[i]->receive();
  }


  /*while {
    // EACH player play

    if(is_player_mat()) {
      Listener.on_player_mat();
      Listener.on_game_finished();
      return;
    }
    if(check()) Listener.on_player_check();
  }*/

  return 0;
}
