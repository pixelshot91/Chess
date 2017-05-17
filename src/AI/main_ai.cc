#include "../client.hh"
#include "AI.hh"

int main(int argc, char* argv[])
{
  argc = argc;
  std::string ip(argv[1]);
  std::string port(argv[2]);
  std::cout << "port is " << port << std::endl;

  Client<AI> client(ip, port);
  return client.start();
}
