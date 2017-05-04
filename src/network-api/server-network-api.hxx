#pragma once

#include "server-network-api.hh"

#include <array>
#include <boost/asio.hpp>

#include "common.hh"

/**
** \brief Implementation file for the server API
** Do not modify!
** You do not have to read or understand it.
*/

namespace network_api
{

  /* Namespace aliases */
  namespace asio = boost::asio;
  using asio::ip::tcp;

  using duration_t = asio::deadline_timer::duration_type;

  namespace detail
  {
    using timer_t = asio::deadline_timer;
    using buff_t = asio::streambuf;
    using err_t = boost::system::error_code;

    inline void
    timed_read(tcp::socket& socket, duration_t& duration, buff_t& buff)
    {
      /* Initialize the timer */
      timer_t timer{socket.get_io_service()};
      timer.expires_from_now(duration);
      timer.async_wait([](const err_t& err)
      {
        if (err != asio::error::operation_aborted)
          throw std::runtime_error("Timeout");
      });

      bool complete = false;

      /* Read asynchronously */
      auto reader = [&](const err_t& err, size_t) 
      {
        if (err)
        {
          throw std::runtime_error("IO error: " + err.message());
        }

        complete = true;
      };

      async_read_until(socket, buff, "\n", reader);

      auto& io_service = socket.get_io_service();
      io_service.reset();

      /* Skip ack frames produced by the synchronous write */
      while (io_service.run_one() && !complete)
      {
      }

      /* Save the duration of the transaction */
      duration = timer.expires_from_now();
      timer.cancel();
    }
  }

  /* Implementation */
  class ServerNetworkAPI::Implementation
  {
  public:
    explicit Implementation(unsigned short port)
        : socket_{io_service_},
          time_left_{boost::posix_time::seconds(ktimeout_dur)}
    {
      tcp::endpoint end_point{tcp::v4(), port};
      tcp::acceptor acceptor{io_service_, end_point};
      acceptor.accept(socket_);
    }

    tcp::socket& socket_get()
    {
      return socket_;
    }

    boost::asio::streambuf& buf_get()
    {
      return buf_;
    }

    duration_t& time_left_get()
    {
      return time_left_;
    }

  private:
    asio::io_service io_service_;
    tcp::socket socket_;
    duration_t time_left_;
    boost::asio::streambuf buf_;
  };
  /* *** *** *** */

  /* Server Network Implementation */
  inline ServerNetworkAPI::ServerNetworkAPI(unsigned short port)
      : pimpl_(std::make_unique<Implementation>(port))
  {
  }

  inline std::string ServerNetworkAPI::acknowledge(bool black_player) const
  {
    const std::string message = black_player ? "BLACK" : "WHITE";

    asio::write(pimpl_->socket_get(), asio::buffer(message));
    std::array<char, kdata_max> data;

    auto buffer = asio::buffer(data);
    size_t length = pimpl_->socket_get().read_some(buffer);

    return {data.begin(), data.begin() + length};
  }

  inline void ServerNetworkAPI::send(const std::string& line)
  {
    asio::write(pimpl_->socket_get(), asio::buffer(line));
    asio::write(pimpl_->socket_get(), asio::buffer(std::string("\n")));
  }

  inline std::string ServerNetworkAPI::receive()
  {
    detail::timed_read(
        pimpl_->socket_get(), pimpl_->time_left_get(), pimpl_->buf_get());

    std::istream str(&pimpl_->buf_get());

    std::string line;
    if (!std::getline(str, line))
    {
      throw std::runtime_error("IO error");
    }

    return line;
  }
  /* *** */
}
