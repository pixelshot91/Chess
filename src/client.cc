

Client::Client(const std::string& ip, const std::string& port)
              : ip_(ip)
              , port_(port)
              {}

int Client::start()
{
  //checks?
  {

  }
  else
  {
    ClientNetworkAPI client = new ClientNetworkAPI(ip, port); // IP, PORT

    // connection confirmation from server?

    client->acknowledge(login);


    /* initialization */

    // receive uci from engine
    if (client->receive() == "uci")
      client->send("uciok");

    // receive isready from engine
    if (client->receive() == "isready")
      client->send("readyok");


    /* moves */
    std::string move;

    if (client->receive() == "ucinewgame")
    {
      if (client->receive() == "go")
      {
        move = /* get bestmove */;
        client->send("bestmove " + move);
      }
    }

    std::string opponent_moves = client->receive();
    if (client->receive == "go")
    {
      move = /* get bestmove */;
      client->send("bestmove " + move);
    }
  }
}
