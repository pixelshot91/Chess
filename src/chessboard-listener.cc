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
  for (int i = 8; i > 0; --i)
  {
    for (unsigned j = 0; j < 8; j++) {
     auto piece = adapter_[plugin::Position((plugin::File)j,(plugin::Rank) i)];
      auto background = "";
      if (piece == nullopt)
        std::cout << background + " ";
      else {
        if(piece.get<plugin::Color> == plugin::color:WHITE)
        {
          std::cout << background + "Hello, ф or \u0444!";
        }
        else
        {
          std::cout << background + "Hello, ф or \u0444!";
        }
      }
    }
    std::cout << std::endl;
  }
}

//std::cout << "Hello, ф or \u0444!\n";
/*
 \e[07m
 \e|00m
 */
/*
 for (int i = board_.size() - 1; i >= 0; --i)
  {
    for (unsigned j = 0; j < board_[0].size(); j++) {
      if (piecetype_get(plugin::Position((plugin::File)j,(plugin::Rank) i)) == std::experimental::nullopt)
        std::cerr << "_ ";
      else
        std::cerr << static_cast<char>(piecetype_get(plugin::Position((plugin::File)j,(plugin::Rank) i)).value()) << " ";

      //std::cerr << std::hex << std::setfill('0') << std::setw(2) << (int)board_[i][j] << " ";
    }
    std::cerr << std::endl;
  }
  */





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
