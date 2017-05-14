#include "human-player.hh"

#include <iostream>

HumanPlayer::HumanPlayer(const std::string& ip, const std::string& port)
  : client_(ip, port), ip_(ip), port_(port)
{}

int HumanPlayer::start()
{

  // connection confirmation from server?

  plugin::Color color = static_cast<plugin::Color>(client_.acknowledge("nicolas.roger"));

  /* initialization */

  // receive uci from engine
  if (client_.receive() != "uci") return -1;
  client_.send("uciok");

  // receive isready from engine
  if (client_.receive() != "isready") return -1;
  client_.send("readyok");


  /* moves */
  std::string move;

  if (client_.receive() != "ucinewgame") return -1;
  bool first_time = true;

  while (true) {
    if (!first_time or color == plugin::Color::BLACK ) {
      std::cerr << "received move" << client_.receive() << std::endl;
    }
    first_time = false;

    if (client_.receive() != "go") return -1;

    std::string input;
    std::cout << "Wainting for input : ";
    std::cin >> input;
    std::cerr << "input is " << input << std::endl;
    //move = /* get bestmove */;
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
