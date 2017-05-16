#include "client.hh"
#include "plugin/color.hh"
#include "player.hh"
#include "human-player.hh"
#include <iostream>

Client::Client(const std::string& ip, const std::string& port, Player& player)
  : client_(ip, port)
  , ip_(ip)
  , port_(port)
  , player_(player)
{
}

int Client::start()
{
  // connection confirmation from server?

  plugin::Color color =
    static_cast<plugin::Color>(client_.acknowledge("nicolas.roger"));

  /* initialization */

  // receive uci from engine
  if (client_.receive() != "uci")
    return -1;
  client_.send("uciok");

  // receive isready from engine
  if (client_.receive() != "isready")
    return -1;
  client_.send("readyok");

  /* moves */
  std::string move;

  if (client_.receive() != "ucinewgame")
    return -1;
  bool first_time = true;

  while (true)
  {
    std::string received_move;
    if (!first_time or color == plugin::Color::BLACK)
    {
      received_move = client_.receive();
      std::cerr << "received move" << received_move << std::endl;
    }
    first_time = false;

    if (client_.receive() != "go")
      return -1;

    //std::cout << "Wainting for input : ";
    std::string input = player_.play_next_move(received_move);
    //std::cerr << "input is " << input << std::endl;
    // move = /* get bestmove */;
    client_.send("bestmove " + input);
  }

  /*std::string opponent_moves = client_.receive();
  std::cerr << "move received " << opponent_moves << std::endl;
  if (client_.receive() == "go")
  {*/
  /*move = i;
    client_.send("bestmove " + move);*/
  //}
}
