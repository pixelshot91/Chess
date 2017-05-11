#include "engine.hh"


Engine::Engine(std::vector<plugin::Listener*> listeners, unsigned short port, ChessBoard chessboard)
  : listeners_(listeners), clients_({network_api::ServerNetworkAPI(port), network_api::ServerNetworkAPI(port)}), chessboard_(chessboard)
{}

int Engine::start()
{
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
