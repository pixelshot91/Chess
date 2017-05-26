#pragma once

#include "client.hh"
#include "plugin/color.hh"
#include "player.hh"
#include "human-player.hh"
#include "parser.hh"
#include "move.hh"
#include <iostream>

template <typename T>
Client<T>::Client(const std::string& ip, const std::string& port, const std::string& pgn_path)
  : client_(ip, port)
  , ip_(ip)
  , port_(port)
  , pgn_path_(pgn_path)
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
  if (pgn_path_ != "") {
    std::cerr << "reading file : " << pgn_path_ << std::endl;
    Parser parser(pgn_path_);
    moves = parser.parse();
    if (color == plugin::Color::BLACK)
      moves.erase(moves.begin());
    std::cerr << "Scripted moved :" << std::endl;
    for (auto m : moves)
      std::cerr << *m << std::endl;
    player.set_scripted_moves(moves);
  }
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
    try {
    if (!first_time or color == plugin::Color::BLACK)
    {
      received_move = client_.receive();
      //std::cerr << "received move" << received_move << std::endl;
    }
    first_time = false;

    if (client_.receive() != "go")
      return -1;

    std::string input = player.play_next_move(received_move);
    client_.send("bestmove " + input);
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
      std::cerr << "Program terminated" << std::endl;
      exit(0);
    }
  }
}
