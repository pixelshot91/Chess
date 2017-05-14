#pragma once
#include "network-api/client-network-api.hh"
#include "plugin/color.hh"

class HumanPlayer
{
  public:
  HumanPlayer(const std::string& ip, const std::string& port);
  int start();

  private:
  network_api::ClientNetworkAPI client_;
  std::string ip_;
  std::string port_;
};
