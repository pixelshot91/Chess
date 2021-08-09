#include <iostream>
#include <string>
#include "human-player.hh"
#include "client.hh"

int main(int argc, char *argv[])
{
  argc = argc;
  std::string ip(argv[1]);
  std::string port(argv[2]);
  Client<HumanPlayer> client(ip, port);
  return client.start();
}
