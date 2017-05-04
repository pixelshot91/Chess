#pragma once

#include <boost/asio.hpp>
#include <cstddef>
#include <string>

/*************************/
/* Network Configuration */
/*     Do not modify!    */
/*************************/

namespace network_api
{
  /**
  ** \brief Duration in seconds that a client has
  ** during the entire game to send moves
  */
  constexpr size_t ktimeout_dur = 300;

  /**
  ** \brief Size of the ack message
  */
  constexpr size_t kack_size = 5;

  /**
  ** \brief Maximum amount of data in bytes that
  ** can be received for the player's name
  */

  constexpr size_t kdata_max = 32;

  using socket_t = boost::asio::ip::tcp::socket;
  using buffer_t = boost::asio::streambuf;
}
