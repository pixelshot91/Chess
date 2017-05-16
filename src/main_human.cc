#include <iostream>
#include <string>
#include "human-player.hh"
#include "client.hh"

int main(int argc, char* argv[])
{
  argc = argc;
  std::string ip(argv[1]);
  std::string port(argv[2]);
  std::cout << "port is " << port << std::endl;
  auto player = HumanPlayer();
  Client client(ip, port, player);
  return client.start();
}
