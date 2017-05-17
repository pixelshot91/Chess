#include <iostream>
#include <string>
#include "AI/AI.hh"
#include "client.hh"

int main(int argc, char* argv[])
{
  argc = argc;
  std::string ip(argv[1]);
  std::string port(argv[2]);
  std::string s_color(argv[3]);
  int color = std::stoi(s_color);
  std::cout << "port is " << port << std::endl;
  auto player = Ai(static_cast<plugin::Color> (color));
  Client client(ip, port, player);
  return client.start();
}
