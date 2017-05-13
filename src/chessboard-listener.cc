#include "chessboard-listener.hh"

LISTENER_EXPORT(ChessboardListener);

ChessboardListener::ChessboardListener()
{}

void ChessboardListener::register_board(const ChessboardInterface& board_interface)
{
  adaptater_ = &board_interface;
}
void ChessboardListener::on_game_started()
{}
void ChessboardListener::on_game_finished()
{}
void ChessboardListener::on_piece_moved(const PieceType piece, const Position& from, const Position& to)
{
  std::cout << "A piece has moooooveed" << std::endl;
}
void ChessboardListener::on_piece_taken(const PieceType piece, const Position& at)
{}
void ChessboardListener::on_piece_promoted(const PieceType piece, const Position& at)
{}
void ChessboardListener::on_kingside_castling(const Color color)
{}
void ChessboardListener::on_queenside_castling(const Color color)
{}
void ChessboardListener::on_player_check(const Color color)
{}
void ChessboardListener::on_player_mat(const Color color)
{}
void ChessboardListener::on_player_pat(const Color color)
{}
void ChessboardListener::on_player_timeout(const Color color)
{}
void ChessboardListener::on_player_disqualified(const Color color)
{}
void ChessboardListener::on_draw()
{}
