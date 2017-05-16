class Client
{
public:
  Client(const std::string& ip, const std::string& port);
  int start();

private:
  network_api::ClientNetworkAPI client_;
  std::string ip_;
  std::string port_;
  Player player_;
};
