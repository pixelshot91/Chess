#pragma once

#include <memory>
#include <string>

namespace network_api
{

  class ClientNetworkAPI
  {
  public:
    /**
    ** \brief Constructor. Initializes connection with the server.
    **
    ** @param ip: The IP address of the server.
    ** @param port: The port of the server.
    */
    ClientNetworkAPI(const std::string& ip, const std::string& port);

    /**
    ** \brief Ack function. Sends the login to the server and gets the color.
    **
    ** @param login: Login of the group leader
    ** @return Player color (true if black)
    */
    bool acknowledge(const std::string& login) const;

    /**
    ** \brief Gets the last opponent's move as a PCN string from the server.
    */
    std::string receive();

    /**
    ** \brief Sends a PCN move to the server
    */
    void send(const std::string& line);

  private:
    /**
    ** Pimpl idiom
    ** You don't need to bother about the implementation
    */
    class Implementation;
    std::unique_ptr<Implementation> pimpl_;
  };

}

#include "client-network-api.hxx"
