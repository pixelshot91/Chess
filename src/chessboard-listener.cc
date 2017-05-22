#include "chessboard-listener.hh"
#include "adaptater.hh"
#include "plugin-auxiliary.hh"

LISTENER_EXPORT(ChessboardListener);

ChessboardListener::ChessboardListener()
  : adaptater_(nullptr)
{
}

void ChessboardListener::register_board(
  const ChessboardInterface& board_interface)
{
  adaptater_ = &board_interface;
}
void ChessboardListener::on_game_started()
{
  auto my_adaptater = reinterpret_cast<const Adaptater*>(adaptater_);
  my_adaptater->pretty_print();
}
void ChessboardListener::on_game_finished()
{
}

void ChessboardListener::on_piece_moved(const PieceType piece,
                                        const Position& from,
                                        const Position& to)
{
  if (adaptater_ == nullptr)
  {
    std::cout << "Board isn't registered" << std::endl;
  }
  auto my_adaptater = reinterpret_cast<const Adaptater*>(adaptater_);
  my_adaptater->pretty_print();
}

void ChessboardListener::on_piece_taken(const PieceType piece,
                                        const Position& at)
{
}
void ChessboardListener::on_piece_promoted(const PieceType piece,
                                           const Position& at)
{
}
void ChessboardListener::on_kingside_castling(const Color color)
{
}
void ChessboardListener::on_queenside_castling(const Color color)
{
}
void ChessboardListener::on_player_check(const Color color)
{
}
void ChessboardListener::on_player_mat(const Color color)
{
}
void ChessboardListener::on_player_pat(const Color color)
{
}
void ChessboardListener::on_player_timeout(const Color color)
{
}
void ChessboardListener::on_player_disqualified(const Color color)
{
}
void ChessboardListener::on_draw()
{
}
