#include "../client.hh"
#include "AI.hh"

int main(int argc, char* argv[])
{
  argc = argc;
  std::string ip(argv[1]);
  std::string port(argv[2]);
  std::string pgn_path;
  if (std::string(argv[3]) != std::string(""))
    pgn_path = std::string(argv[3]);
  std::cout << "port is " << port << std::endl;

  Client<AI> client(ip, port, pgn_path);
  return client.start();
}
