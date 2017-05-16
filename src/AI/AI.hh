#include <string>
#include "../Player.hh"

class AI : public Player
{
  AI(plugin::Color AI_color);
  private :
  std::string play_next_move(std::string received_move) override;
    ChessBoard board_;
    plugin::Color AI_color;

}
