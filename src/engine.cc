#include "engine.hh"

#include "adaptater.hh"

Engine::Engine(std::vector<plugin::Listener*> listeners, unsigned short port, std::string pgn_path)
  : port_(port), pgn_path_(pgn_path), listeners_(listeners),  chessboard_(listeners)
{}

/*clients_({network_api::ServerNetworkAPI(port), network_api::ServerNetworkAPI(port)}),*/

int Engine::start()
{
  if (port_ == 0) {
    Adaptater adaptater(chessboard_);
    for (auto l : listeners_)
      l->register_board(adaptater);
    std::cerr << "Parsing..." << std::endl;
    Parser parser(pgn_path_);
    auto moves = parser.parse();
    
    for (auto l : listeners_)
      l->on_game_started();

    for (auto m : moves) {
      std::cerr << "move description" << std::endl;
      std::cerr << *m << std::endl;
      std::cerr << "end move description" << std::endl;
      if (chessboard_.update(*m) == -1)
        break;
      //chessboard_.print();
    }
    for (auto l : listeners_)
      l->on_game_finished();
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
      if (clients_[i]->receive() != "uciok") return -1;
    }
    for (int i = 0; i < 2; ++i) {
      clients_[i]->send("isready");
      if (clients_[i]->receive() != "readyok") return -1;
    }


    /* Moves */

    std::string total_moves;
    std::string client_move;

    /* engine -> client_1 */
    clients_[0]->send("ucinewgame");
    clients_[0]->send("go");

    client_move = clients_[0]->receive().substr(9);
    chessboard_.update(Parser::parse_move(client_move));
    total_moves += " " + client_move;

    /* engine -> client_2 */
    clients_[1]->send("ucinewgame");
    clients_[1]->send("position startpos moves " + total_moves);
    clients_[1]->send("go");
    client_move = clients_[1]->receive().substr(9);

    /* engine -> client_1 */
    clients_[0]->send(total_moves);
    clients_[0]->send("go");
    client_move = clients_[0]->receive().substr(9);

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
