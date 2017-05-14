#include <string>
#include <iostream>

#include "human-player.hh"

int main(int argc, char* argv[])
{
  std::string ip(argv[1]);
  std::string port(argv[2]);
  std::cout << "port is " << port << std::endl;
  HumanPlayer player(ip, port);
  return player.start();
}
