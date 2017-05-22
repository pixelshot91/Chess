#include "human-player.hh"
#include <iostream>

HumanPlayer::HumanPlayer(plugin::Color c)
  : Player(c)
{}

std::string HumanPlayer::play_next_move(const std::string& received_move)
{
  std::string input;
  std::cout << received_move << std::endl;
  if (received_move != "") {
    auto pos = received_move.find_last_of(' ');
    std::string move = received_move.substr(pos + 1);
    auto opponent_move = Parser::parse_uci(move, !color_, board_);
    std::cout << "Your opponent move: " << *opponent_move << std::endl;
    board_.update(opponent_move);
  }
  while (true)
  {
    std::cout << "Wainting for input : ";

    std::cin >> input;
    try {
      auto player_move = Parser::parse_uci(input, color_, board_);
      if (board_.update(player_move) == -2) {
        std::cout << "Invalid move" << std::endl;
        continue;
      }
      std::cout << "Your move: " << *player_move << std::endl;
      break;
    }
    catch (std::invalid_argument& e)
    {
      std::cout << e.what() << std::endl;
    }
  }
  return input;
}

void HumanPlayer::set_scripted_moves( std::vector<std::shared_ptr<Move>> moves)
{
  moves = moves;
  std::cout << "Scripted move were given" << std::endl;
}
