#pragma once

#include <memory>
#include <string>

namespace network_api
{

  class ServerNetworkAPI
  {
  public:
    /**
    ** \brief Constructor. Waits for a client and initializes connection.
    **
    ** @param port: Listening port
    */
    explicit ServerNetworkAPI(unsigned short port);

    /**
    ** \brief Ack function. Sends the login to the server and gets the color.
    **
    ** @param login: Color of the client player (true if black)
    ** @return Player login
    */
    std::string acknowledge(bool black_player) const;

    void send(const std::string& line);
    std::string receive();

  protected:
    /**
    ** Pimpl idiom
    ** You don't need to bother about the implementation
    */
    class Implementation;
    std::unique_ptr<Implementation> pimpl_;
  };
}

#include "server-network-api.hxx"
