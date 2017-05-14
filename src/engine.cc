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

/*clients_({network_api::ServerNetworkAPI(port),
 * network_api::ServerNetworkAPI(port)}),*/

int Engine::start()
{
  Adaptater adaptater(chessboard_);
  for (auto l : listeners_)
    l->register_board(adaptater);
  for (auto l : listeners_)
    l->on_game_started();
  if (port_ == 0)
  {
    std::cerr << "Parsing..." << std::endl;
    Parser parser(pgn_path_);
    auto moves = parser.parse();

    for (auto m : moves)
    {
      std::cerr << "move description" << std::endl;
      std::cerr << *m << std::endl;
      std::cerr << "end move description" << std::endl;
      if (chessboard_.update(*m) == -1)
        break;
      // chessboard_.print();
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
      std::cerr << "The connection is established" << std::endl;
      login[i] = clients_[i]->acknowledge(static_cast<bool>(i));
      std::cerr << "LOGIN is " << login[i] << std::endl;
    }

    for (int i = 0; i < 2; ++i)
    { // Initialization
      clients_[i]->send("uci");
      std::cerr << "sending uci" << std::endl;
      if (clients_[i]->receive() != "uciok")
        return -1;
    }
    for (int i = 0; i < 2; ++i)
    {
      std::cerr << "sending isready" << std::endl;
      clients_[i]->send("isready");
      if (clients_[i]->receive() != "readyok")
        return -1;
    }

    std::cerr << "Initialization OK" << std::endl;
    /* Moves */

    std::string total_moves;
    std::string client_move;

    // plugin::Color color = plugin::Color::WHITE;
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

      client_move = clients_[color]->receive(); //.substr(9);
      std::cerr << "bestmove = " << client_move << std::endl;
      if (chessboard_.update(Parser::parse_move(
            client_move, static_cast<plugin::Color>(color))) == -1)
      {
        std::cout << "invalid move : You are disqualified";
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
