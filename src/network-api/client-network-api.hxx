#pragma once

#include "client-network-api.hh"

#include <array>
#include <boost/asio.hpp>

#include "common.hh"

/**
** \brief Implementation file for the client API
** Do not modify!
** You do not have to read or understand it.
*/

namespace network_api
{

  /* Namespace aliases */
  namespace asio = boost::asio;
  using boost::asio::ip::tcp;

  /* Implementation */
  class ClientNetworkAPI::Implementation
  {
  public:
    Implementation(const std::string& ip, const std::string& port)
        : socket_{io_service_}
    {
      tcp::resolver resolver{io_service_};
      tcp::resolver::query query{ip, port};
      auto point = resolver.resolve(query);

      boost::asio::connect(socket_, point);
    }

    tcp::socket& socket_get()
    {
      return socket_;
    }

    boost::asio::streambuf& buf_get()
    {
      return buf_;
    }

  private:
    asio::io_service io_service_;
    tcp::socket socket_;
    boost::asio::streambuf buf_;
  };
  /* *** */

  /*
   * Client API implementation
   */
  inline ClientNetworkAPI::ClientNetworkAPI(const std::string& ip,
                                            const std::string& port)
      : pimpl_(std::make_unique<Implementation>(ip, port))
  {
  }

  inline bool ClientNetworkAPI::acknowledge(const std::string& login) const
  {
    std::array<char, kack_size> data;
    auto buffer = boost::asio::buffer(data);
    pimpl_->socket_get().read_some(buffer);

    const std::string side_str{data.begin(), data.begin() + kack_size};
    asio::write(pimpl_->socket_get(), asio::buffer(login));

    return side_str == "BLACK";
  }

  inline std::string ClientNetworkAPI::receive()
  {
    asio::read_until(pimpl_->socket_get(), pimpl_->buf_get(), "\n");

    std::istream str(&pimpl_->buf_get());

    std::string line;
    if (!str || !std::getline(str, line))
    {
      throw std::runtime_error("IO error");
    }

    return line;

  }

  inline void ClientNetworkAPI::send(const std::string& line)
  {
    asio::write(pimpl_->socket_get(), asio::buffer(line));
    asio::write(pimpl_->socket_get(), asio::buffer(std::string("\n")));
  }

  /* *** */
}
