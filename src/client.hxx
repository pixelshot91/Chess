#pragma once

#include "client.hh"
#include "plugin/color.hh"
#include "player.hh"
#include "human-player.hh"
#include "parser.hh"
#include "move.hh"
#include <iostream>

template <typename T>
Client<T>::Client(const std::string &ip, const std::string &port)
    : client_(ip, port), ip_(ip), port_(port)
{
}

template <typename T>
int Client<T>::start()
{
  // connection confirmation from server?

  plugin::Color color =
      static_cast<plugin::Color>(client_.acknowledge("nicolas.roger"));
  player_t player(color);
  std::vector<std::shared_ptr<Move>> moves;
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
    try
    {

      received_move = client_.receive();
      std::cerr << "received move" << received_move << std::endl;

      if (client_.receive() != "go")
        return -1;

      std::string input = player.play_next_move(received_move);
      std::cerr << "Senging 'bestmove " + input + "'" << std::endl;
      client_.send("bestmove " + input);
    }
    catch (std::exception &e)
    {
      std::cerr << e.what() << std::endl;
      std::cerr << "Program terminated" << std::endl;
      exit(0);
    }
  }
}
