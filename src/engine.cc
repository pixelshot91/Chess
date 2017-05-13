#include "engine.hh"


Engine::Engine(std::vector<plugin::Listener*> listeners, unsigned short port, std::string pgn_path)
  : port_(port), pgn_path_(pgn_path), listeners_(listeners),  chessboard_(listeners)
{}

/*clients_({network_api::ServerNetworkAPI(port), network_api::ServerNetworkAPI(port)}),*/

int Engine::start()
{
  if (port_ == 0) {
    std::cerr << "Parsing..." << std::endl;
    Parser parser(pgn_path_);
    auto moves = parser.parse();

    for (auto m : moves) {
      std::cerr << "move description" << std::endl;
      std::cerr << *m << std::endl;
      std::cerr << "end move description" << std::endl;
      chessboard_.update(*m);
      chessboard_.print();
    }
    return 0;
  }
  else {
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
    for (int i = 0; i < 2; ++i) {
      clients_[i]->send("isready");
      if (clients_[i]->receive() != "readyok") return -1;
    }
    std::string received_moves;

    clients_[0]->send("ucinewgame");
    clients_[0]->send("go");
    std::string new_move = clients_[0]->receive();
    chessboard_.update(Parser::parse_move(new_move));
    received_moves += " " + new_move;

    clients_[1]->send("ucinewgame");
    clients_[1]->send("position startpos moves " + received_moves);
    clients_[1]->send("go");
    clients_[1]->receive();



    /*while {
    // EACH player play

    if(is_player_mat()) {
    Listener.on_player_mat();
    Listener.on_game_finished();
    return;
    }
    if(check()) Listener.on_player_check();
    }*/
  }
  return 0;
}
