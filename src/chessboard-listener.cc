#include "chessboard-listener.hh"

LISTENER_EXPORT(ChessboardListener);

ChessboardListener::ChessboardListener()
  : adaptater_(nullptr)
{}

void ChessboardListener::register_board(const ChessboardInterface& board_interface)
{
  adaptater_ = &board_interface;
}
void ChessboardListener::on_game_started()
{}
void ChessboardListener::on_game_finished()
{}

void GetUnicodeChar(unsigned int code, char chars[5]) {
    if (code <= 0x7F) {
        chars[0] = (code & 0x7F); chars[1] = '\0';
    } else if (code <= 0x7FF) {
        // one continuation byte
        chars[1] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[0] = 0xC0 | (code & 0x1F); chars[2] = '\0';
    } else if (code <= 0xFFFF) {
        // two continuation bytes
        chars[2] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[1] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[0] = 0xE0 | (code & 0xF); chars[3] = '\0';
    } else if (code <= 0x10FFFF) {
        // three continuation bytes
        chars[3] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[2] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[1] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[0] = 0xF0 | (code & 0x7); chars[4] = '\0';
    } else {
        // unicode replacement character
        chars[2] = 0xEF; chars[1] = 0xBF; chars[0] = 0xBD;
        chars[3] = '\0';
    }
}

char PieceTypeToInt(plugin::PieceType type)
{
  char i;
  for (i = 0; plugin::piecetype_array()[i] != type; ++i)
    continue;
  return i;
}

void ChessboardListener::on_piece_moved(const PieceType piece, const Position& from, const Position& to)
{
  if (adaptater_ == nullptr) {
    std::cout << "Board isn't registered" << std::endl;
  }
  char chars[5];
  for (int i = 7; i >= 0; --i)
  {
    for (unsigned j = 0; j < 8; j++) {
      auto piece = adaptater_->operator[](plugin::Position((plugin::File)j,(plugin::Rank) i));
      if((i + j)% 2 == 0) std::cout << "\e[07m";
      std::cout << " ";
      if (piece == std::experimental::nullopt)
        std::cout << " ";
      else {
        bool color = std::get<plugin::Color>(piece.value()) == plugin::Color::BLACK;
        if((i + j)% 2)
          color = !color;

          GetUnicodeChar(0x2654 + color * 6
          + PieceTypeToInt(std::get<plugin::PieceType>(piece.value())), chars);
          std::cout << chars;
        }
      std::cout << " \e[00m";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
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
