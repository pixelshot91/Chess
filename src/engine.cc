#include "engine.hh"

#include "adaptater.hh"

Engine::Engine(std::vector<plugin::Listener*> listeners, unsigned short port)
  : port_(port)
  , pgn_path_("")
  , listeners_(listeners)
  , chessboard_(listeners)
{
}

Engine::Engine(std::vector<plugin::Listener*> listeners, std::string pgn_path)
  : port_(0)
  , pgn_path_(pgn_path)
  , listeners_(listeners)
  , chessboard_(listeners)
{
}

int Engine::start()
{
  Adaptater adaptater(chessboard_);
  for (auto l : listeners_)
    l->register_board(adaptater);
  for (auto l : listeners_)
    l->on_game_started();
  if (port_ == 0)
  {
    Parser parser(pgn_path_);
    auto moves = parser.parse();

    for (auto m : moves)
    {
      //std::cerr << *m << std::endl;
      if (chessboard_.update(m) != 0)
        break;
    }
    for (auto l : listeners_)
      l->on_game_finished();
    return 0;
  }
  else
  {
    network_api::ServerNetworkAPI* clients_[2];
    std::string login[2];
    for (int i = 0; i < 2; ++i)
    { // Connection
      clients_[i] = new network_api::ServerNetworkAPI(port_);
      login[i] = clients_[i]->acknowledge(static_cast<bool>(i));
    }

    for (int i = 0; i < 2; ++i)
    { // Initialization
      clients_[i]->send("uci");
      if (clients_[i]->receive() != "uciok")
        return -1;
    }
    for (int i = 0; i < 2; ++i)
    {
      clients_[i]->send("isready");
      if (clients_[i]->receive() != "readyok")
        return -1;
    }

    /* Moves */

    std::string total_moves;
    std::string client_move;

    int color = 0;
    /* engine -> client_1 */
    bool first_time = true;
    while (true)
    {
      if (first_time)
      {
        clients_[color]->send("ucinewgame");
        if (color == static_cast<bool>(plugin::Color::BLACK))
          first_time = false;
      }
      if (not first_time)
        clients_[color]->send("position startpos moves " + total_moves);

      clients_[color]->send("go");

      try {
      client_move = clients_[color]->receive().substr(9);
      }
      catch (std::runtime_error& e)
      {
        for (auto l : listeners_)
          l->on_player_timeout(static_cast<plugin::Color>(color));
      }
      //std::cerr << "bestmove = " << client_move << std::endl;
      auto best_move = Parser::parse_uci(
            client_move, static_cast<plugin::Color>(color), chessboard_);
      if (chessboard_.update(best_move) != 0)
      {
        for (auto l : listeners_)
          l->on_game_finished();
        return -1;
      }
      total_moves += " " + client_move;
      color = !color;
    }
    return 0;
  }
}
