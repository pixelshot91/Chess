#include "chessboard.hh"
#include "rule-checker.hh"
#include "piece/pawn.hh"
#include "piece/king.hh"
#include "piece/queen.hh"
#include "piece/bishop.hh"
#include "piece/knight.hh"
#include "piece/rook.hh"

std::ostream& operator<<(std::ostream& o, const plugin::Position& p);
/*std::ostream& operator<<(std::ostream& o, const plugin::Position& p)
{
  o << (char)(static_cast<char>(p.file_get()) + 'A') << (char)(static_cast<char>(p.rank_get()) + '1');
  return o;
}*/

ChessBoard::ChessBoard()
{
}

void ChessBoard::update(Move& move) { // FIX ME PROMOtiOn
  if (!RuleChecker::check(*this, move)) 
    throw std::invalid_argument("invalid move - in ChessBoard update()");

  for (auto l : listeners_)
    l->on_piece_moved(quiet_move.piecetype_get(), quiet_move.start_get(), quiet_move.end_get());
  /* Update piece position*/
  if (move.move_type_get() == Move::Type::QUIET) 
  {
    const QuietMove& quiet_move = static_cast<const QuietMove&>(move);
    move_piece(quiet_move.start_get(), quiet_move.end_get());
  }
  else 
  {
    /* Rock */
    
    if (move.move_type_get() == Move::Type::KING_CASTLING)
    {
      for (auto l : listeners_)
        on_kingside_castling(move.color_get());
    }
    else
    {
      for (auto l : listeners_)
        on_queenside_castling(move.color_get());
    }
    move_piece(initial_king_position(move.color_get()), castling_king_end_position(move.color_get(), move.move_type_get() == Move::Type::KING_CASTLING));
    move_piece(initial_rook_position(move.color_get(), move.move_type_get() == Move::Type::KING_CASTLING)
        , castling_rook_end_position(move.color_get(), move.move_type_get() == Move::Type::KING_CASTLING));
  }
  if (RuleChecker::isCheckmate(*this, get_king_position(move.color_get())))
  {
    for (auto l : listeners_)
      on_player_check(move.color_get());
    //throw std::invalid_argument("CHECKMATE -- in chessboard update function");
  }
}

void ChessBoard::move_piece(plugin::Position start, plugin::Position end) {
  cell_t moving_piece = get_square(start);
  moving_piece |= 0b00001000;
  set_square(end, moving_piece);

  set_square(start, 0x7); //0b000001111
}

void ChessBoard::print() const
{
  for (int i = board_.size() - 1; i >= 0; --i)
  {
    for (unsigned j = 0; j < board_[0].size(); j++) {
      if (piecetype_get(plugin::Position((plugin::File)j,(plugin::Rank) i)) == std::experimental::nullopt)
        std::cout << "_ ";
      else
        std::cout << static_cast<char>(piecetype_get(plugin::Position((plugin::File)j,(plugin::Rank) i)).value()) << " ";

      //std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)board_[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

void ChessBoard::set_square(plugin::Position position, cell_t value) {
  board_[ 7 - static_cast<char>(position.rank_get())][static_cast<char>(position.file_get())] = value;
}

ChessBoard::cell_t ChessBoard::get_square(plugin::Position position) const
{
  char j = static_cast<char>(position.file_get());
  char i = 7 - static_cast<char>(position.rank_get());
  /*std::cout << "i = " << (unsigned)i << " j = " << (unsigned int)j << std::endl;
  std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)board_[i][j] << " ";
  std::cout << std::endl;*/
  return board_[i][j];
}

ChessBoard::cell_t ChessBoard::get_opt(plugin::Position position, cell_t mask) const
{
  return get_square(position) & mask;
}

bool ChessBoard::has_moved(plugin::Position position) const
{
  return (bool)get_opt(position, 0b00001000);
}

plugin::Color ChessBoard::color_get(plugin::Position position) const
{
  return static_cast<plugin::Color>(get_opt(position, 0b10000000) != 0);
}

bool ChessBoard::castleflag_get(plugin::Position position) const
{
  return (bool)get_opt(position, 0b00010000);
}

  std::experimental::optional<plugin::PieceType>
ChessBoard::piecetype_get(plugin::Position position) const
{
  cell_t type_b = get_opt(position, 0b00000111);
  if(type_b != 0b00000111)
    return plugin::piecetype_array()[type_b];
  return std::experimental::nullopt;
}

bool ChessBoard::is_attacked(plugin::Color color, plugin::Position current_cell) const
{
  //for (auto p : getPieces())
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
    {
      plugin::Position pos(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));
      if (piecetype_get(pos) != std::experimental::nullopt and color_get(pos) != color) {
        std::cout << " ATTACKER : " << pos << std::endl;
        try {
          if (RuleChecker::check(*this,
            QuietMove(static_cast<plugin::Color>(not static_cast<bool>(color)),
            pos, current_cell, piecetype_get(pos).value(), true, false))) {
            return true;
          }
        }
        catch (std::invalid_argument& e) {
          return false;
        }
      }
    }
  return false;
}

std::vector<Piece*> ChessBoard::get_piece(plugin::Color color)
{
  std::vector<Piece*> pieces;
  for(int i = 0; i < 8; i++)
    for(int j = 0; j < 8 ; j++)
    {
      plugin::Position position(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));
      if(color_get(position) == color)
      {
        std::experimental::optional<plugin::PieceType> piece_type = piecetype_get(position);
        if (piece_type == std::experimental::nullopt)
          continue;
        switch (piece_type.value())
        {
          case plugin::PieceType::KING:
            pieces.push_back(new King(color_get(position), position, has_moved(position)));
            break;

          case plugin::PieceType::QUEEN:
            pieces.push_back(new Queen(color_get(position), position, has_moved(position)));
            break;

          case plugin::PieceType::ROOK:
            pieces.push_back(new Rook(color_get(position), position, has_moved(position)));
            break;

          case plugin::PieceType::BISHOP:
            pieces.push_back(new Bishop(color_get(position), position, has_moved(position)));
            break;

          case plugin::PieceType::KNIGHT:
            pieces.push_back(new Knight(color_get(position), position, has_moved(position)));
            break;

          case plugin::PieceType::PAWN:
            pieces.push_back(new Pawn(color_get(position), position, has_moved(position)));
            break;

          default:
            exit(58);
            break;
        }
      }
    }
  return pieces;
}

//King(Color color, Position pos, bool has_moved,PieceType piece);

History ChessBoard::history_get() const {
  return history_;
}

plugin::Position ChessBoard::initial_king_position(plugin::Color c)
{
  if (c == plugin::Color::WHITE)
    return plugin::Position(plugin::File::E, plugin::Rank::ONE);
  return plugin::Position(plugin::File::E, plugin::Rank::EIGHT);
}

plugin::Position ChessBoard::initial_rook_position(plugin::Color c, bool king_side)
{
  return plugin::Position(king_side ? plugin::File::H : plugin::File::A, c == plugin::Color::WHITE ? plugin::Rank::ONE : plugin::Rank::EIGHT);
}

plugin::Position ChessBoard::castling_king_end_position(plugin::Color color, bool king_side)
{
  return plugin::Position(king_side ? plugin::File::G : plugin::File::C, color == plugin::Color::WHITE ? plugin::Rank::ONE : plugin::Rank::EIGHT);
}

plugin::Position ChessBoard::castling_rook_end_position(plugin::Color color, bool king_side)
{
  return plugin::Position(king_side ? plugin::File::F : plugin::File::D, color == plugin::Color::WHITE ? plugin::Rank::ONE : plugin::Rank::EIGHT);
}

plugin::Position ChessBoard::get_king_position(plugin::Color color)
{
  for (auto i = 0; i < 8; i++)
  {
    for (auto j = 0; j < 8; j++)
    {
      auto p = plugin::Position(static_cast<plugin::File>(j),
               static_cast<plugin::Rank>(i));
      if (piecetype_get(p) == plugin::PieceType::KING and color_get(p) ==
          color)
        return p;
    }
  }
}
