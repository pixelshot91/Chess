#pragma once

#include "network-api/client-network-api.hh"
#include "network-api/server-network-api.hh"
#include "player.hh"

class Client
{
public:
  Client(const std::string& ip, const std::string& port, Player& player);
  int start();

private:
  network_api::ClientNetworkAPI client_;
  std::string ip_;
  std::string port_;
  Player& player_;
};
