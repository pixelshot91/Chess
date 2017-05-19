#pragma once

#include "network-api/client-network-api.hh"
#include "network-api/server-network-api.hh"
#include "player.hh"

template <typename T>
class Client
{
public:
  using player_t = T;
  Client(const std::string& ip, const std::string& port, const std::string& pgn_path = "");
  int start();

private:
  network_api::ClientNetworkAPI client_;
  std::string ip_;
  std::string port_;
  std::string pgn_path_;
};

#include "client.hxx"
