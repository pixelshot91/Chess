#include "chessboard.hh"
#include "rule-checker.hh"
#include "plugin-auxiliary.hh"

std::ostream& operator<<(std::ostream& o, const plugin::Position& p);

ChessBoard::ChessBoard()
  : last_move_(nullptr)
{}

ChessBoard::ChessBoard(std::vector<plugin::Listener*> listeners)
  : last_move_(nullptr), listeners_(listeners)
{
}

ChessBoard::ChessBoard(const ChessBoard& board)
  : board_(board.board_)
{
}

int ChessBoard::update(std::shared_ptr<Move> move_ptr)
{
  Move& move = *move_ptr;
  //std::cerr << "move is " << *move_ptr << std::endl;
  if (!RuleChecker::is_move_valid(*this, move))
  {
    for (auto l : listeners_)
      l->on_player_disqualified(move.color_get()); // Disqualified
    return -2;
  }

  plugin::PieceType piecetype_eaten = plugin::PieceType::KING;
  plugin::Position position_piece_eaten_en_passant(plugin::File::A, plugin::Rank::ONE);
  bool en_passant = false;
  if (move.move_type_get() == Move::Type::QUIET)
  {
    const QuietMove& quiet_move = static_cast<const QuietMove&>(move);
    if (quiet_move.is_an_attack())
    {
      if (piecetype_get(quiet_move.end_get()) == std::experimental::nullopt) {
        position_piece_eaten_en_passant = plugin::Position(
            quiet_move.end_get().file_get(), quiet_move.start_get().rank_get());
        piecetype_eaten = piecetype_get(position_piece_eaten_en_passant)
          .value();
        en_passant = true;
      }
      else
        piecetype_eaten = piecetype_get(quiet_move.end_get()).value();
    }
  }
  previous_states_.push_back(board_);
  apply_move(move);
  if (en_passant)
    set_square(position_piece_eaten_en_passant, 0x7);
  if (RuleChecker::isCheck(*this, get_king_position(move.color_get())))
  {
    for (auto l : listeners_)
      l->on_player_disqualified(move.color_get()); // Disqualified
    return -1;
  }
  last_move_ = move_ptr;


  /* Update piece position*/
  if (move.move_type_get() == Move::Type::QUIET)
  {
    const QuietMove& quiet_move = static_cast<const QuietMove&>(move);
    if (quiet_move.piecetype_get() == plugin::PieceType::PAWN or quiet_move.is_an_attack())
      inactive_turn = 0;
    else
      ++inactive_turn;
    for (auto l : listeners_)
      l->on_piece_moved(quiet_move.piecetype_get(), quiet_move.start_get(),
          quiet_move.end_get());
    if (quiet_move.is_an_attack()) {// Piece Taken
      if (en_passant)
        for (auto l : listeners_)
          l->on_piece_taken(piecetype_eaten, position_piece_eaten_en_passant);
      else
        for (auto l : listeners_)
          l->on_piece_taken(piecetype_eaten, quiet_move.end_get());
    }
    if (quiet_move.is_promotion())
      for (auto l : listeners_)
        l->on_piece_promoted(plugin::piecetype_array()[quiet_move.promotion_piecetype_get()], quiet_move.end_get());
  }
  else /* Castling */
  {
    ++inactive_turn;
    plugin::Position king_start_position =
      initial_king_position(move.color_get());
    plugin::Position king_end_position = castling_king_end_position(
        move.color_get(), move.move_type_get() == Move::Type::KING_CASTLING);

    for (auto l : listeners_)
      l->on_piece_moved(plugin::PieceType::KING, king_start_position,
          king_end_position);
    if (move.move_type_get() == Move::Type::KING_CASTLING)
      for (auto l : listeners_)
        l->on_kingside_castling(move.color_get());
    else
      for (auto l : listeners_)
        l->on_queenside_castling(move.color_get());
  }
  plugin::Color opponent_color =
    static_cast<plugin::Color>(not static_cast<bool>(move.color_get()));
  plugin::Position opponent_king_position = get_king_position(opponent_color);
  bool no_possible_move = RuleChecker::no_possible_move(*this, opponent_color);
  bool in_check = RuleChecker::isCheck(*this, opponent_king_position);

  if (no_possible_move and in_check)
  {
    for (auto l : listeners_)
      l->on_player_mat(opponent_color); // Checkmate
    return -1;
  }
  if (in_check)
  {
    for (auto l : listeners_)
      l->on_player_check(opponent_color); // Check
  }
  if (no_possible_move)
  {
    for (auto l : listeners_)
      l->on_player_pat(opponent_color);
    for (auto l : listeners_)
      l->on_draw();
    return -1;
  }

  if (three_fold_repetition() or inactive_turn == 50) {
    for (auto l : listeners_)
      l->on_draw();
    return -1;
  }
  return 0;
}

bool ChessBoard::three_fold_repetition()
{
  int counter = 0;
  for (auto previous : previous_states_) {
    if (previous == board_) {
      if (counter == 1)
        return true;
      ++counter;
    }
  }
  return false;

}

void ChessBoard::apply_move(const Move& move)
{
  if (move.move_type_get() == Move::Type::QUIET)
  {
    const QuietMove& quiet_move = static_cast<const QuietMove&>(move);
    if (quiet_move.is_promotion()) {
      set_square(quiet_move.end_get(), (static_cast<bool>(move.color_get()) << 7) | 0x8 | quiet_move.promotion_piecetype_get());
      set_square(quiet_move.start_get(), 0x7); // 0b000001111
    }
    else
      move_piece(quiet_move.start_get(), quiet_move.end_get());
  }
  else /* Castling */
  {
    //Moving King
    move_piece(initial_king_position(move.color_get()),
        castling_king_end_position(move.color_get(),
          move.move_type_get() ==
          Move::Type::KING_CASTLING));
    // Moving Rook
    move_piece(
        initial_rook_position(move.color_get(),
          move.move_type_get() == Move::Type::KING_CASTLING),
        castling_rook_end_position(
          move.color_get(), move.move_type_get() == Move::Type::KING_CASTLING));
  }
}

void ChessBoard::move_piece(plugin::Position start, plugin::Position end)
{
  cell_t moving_piece = get_square(start);
  moving_piece |= 0b00001000;
  set_square(end, moving_piece);

  set_square(start, 0x7); // 0b000001111
}

void ChessBoard::print() const
{
  for (int i = board_.size() - 1; i >= 0; --i)
  {
    for (unsigned j = 0; j < board_[0].size(); j++)
    {
      if (piecetype_get(plugin::Position((plugin::File)j, (plugin::Rank)i)) ==
          std::experimental::nullopt)
        std::cerr << "_ ";
      else
        std::cerr << static_cast<char>(
            piecetype_get(
              plugin::Position((plugin::File)j, (plugin::Rank)i))
            .value())
          << " ";

    }
    std::cerr << std::endl;
  }
}

void ChessBoard::pretty_print() const {
  char chars[5];
  std::cout << "  a  b  c  d  e  f  g  h" << std::endl;
  for (int i = 7; i >= 0; --i)
  {
    std::cout << i + 1;
    for (unsigned j = 0; j < 8; j++)
    {
      auto pos = plugin::Position((plugin::File)j, (plugin::Rank)i);
      auto piece = piecetype_get(pos);
      if ((i + j) % 2 == 0)
        //std::cout << "\x1B[30;48;5;95m";
        std::cout << "\x1B[07m";
      std::cout << " ";
      if (piece == std::experimental::nullopt)
        std::cout << " ";
      else
      {
        bool color = color_get(pos) == plugin::Color::BLACK;
        if ((i + j) % 2)
          color = !color;

        auxiliary::GetUnicodeChar(
            0x2654 + color * 6 +
            auxiliary::PieceTypeToInt(piece.value()), chars);
        std::cout << chars;
      }
      std::cout << " \x1B[00m";
    }
    std::cout << i + 1 << std::endl;
  }
  std::cout << "  a  b  c  d  e  f  g  h" << std::endl;
}

void ChessBoard::set_square(plugin::Position position, cell_t value)
{
  board_[7 - static_cast<char>(position.rank_get())]
    [static_cast<char>(position.file_get())] = value;
}

ChessBoard::cell_t ChessBoard::get_square(plugin::Position position) const
{
  char j = static_cast<char>(position.file_get());
  char i = 7 - static_cast<char>(position.rank_get());
  return board_[i][j];
}

const ChessBoard::board_t& ChessBoard::board_get() const
{
  return board_;
}

ChessBoard::cell_t ChessBoard::get_opt(plugin::Position position,
    cell_t mask) const
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
  if (type_b != 0b00000111)
    return plugin::piecetype_array()[type_b];
  return std::experimental::nullopt;
}

// Color : the color of the cell that is attacked
bool ChessBoard::is_attacked(plugin::Color color,
    plugin::Position current_cell) const
{
  /*auto moves = get_possible_actions(!color);
  for (auto move : moves) {
    if (move->move_type_get() == Move::Type::QUIET)
    {
      QuietMove& quiet_move = static_cast<QuietMove&>(*move);
      if (quiet_move.end_get() == current_cell)
        return true;
    }
  }
  return false;*/

  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
    {
      plugin::Position pos(static_cast<plugin::File>(i),
          static_cast<plugin::Rank>(j));
      if (piecetype_get(pos) != std::experimental::nullopt and
          color_get(pos) != color)
      {
        QuietMove move(!color, pos,
                current_cell, piecetype_get(pos).value(), true, true);
        if (RuleChecker::is_move_valid(*this, move))
        {
          return true;
        }
      }
    }
  return false;
}

std::vector<std::shared_ptr<Move>> ChessBoard::get_possible_actions(plugin::Color playing_color) const
{
  std::vector<std::shared_ptr<Move>> moves;
  for (char i = 0; i < 8; ++i)
  {
    for (char j = 0; j < 8; ++j)
    {
      auto file = static_cast<plugin::File>(i);
      auto rank = static_cast<plugin::Rank>(j);
      plugin::Position position(file, rank);
      char dir = (playing_color == plugin::Color::WHITE) ? 1 : -1;
      auto color_piece = color_get(position);
      if (color_piece != playing_color)
        continue;
      if (piecetype_get(position) == std::experimental::nullopt)
        continue;//return moves;
      plugin::PieceType piece_type = piecetype_get(position).value();

      switch (piece_type)
      {
        case plugin::PieceType::KING:
          {
            for (int i = -1; i <= 1; i++)
            {
              for (int j = -1; j <= 1; j++)
              {
                if (i + ~file < 0 or 7 < ~file + i or ~rank + j < 0 or 7 < ~rank +j)
                  continue;
                plugin::Position end_pos(static_cast<plugin::File>(~file + i), static_cast<plugin::Rank>(~rank + j));
                push_move(moves, QuietMove(color_piece, position, end_pos, piece_type));
              }
            }
            Move KingCastle(Move::Type::KING_CASTLING,color_piece);
            if(RuleChecker::is_move_valid(*this, KingCastle))
              moves.push_back(std::make_shared<Move>(KingCastle));
            Move QueenCastle(Move::Type::QUEEN_CASTLING,color_piece);
            if(RuleChecker::is_move_valid(*this, QueenCastle))
              moves.push_back(std::make_shared<Move>(QueenCastle));
            break;
          }

        case plugin::PieceType::QUEEN:
          {
            for (int i = 0; i < 8; i++)
            {
              if (i != ~file) {
                plugin::Position end_pos(static_cast<plugin::File>(i), rank);
                push_move(moves, QuietMove(color_piece, position, end_pos, piece_type));
              }
              if (i != ~rank) {
                plugin::Position end_pos(file, static_cast<plugin::Rank>(i));
                push_move(moves, QuietMove(color_piece, position, end_pos, piece_type));
              }
            }
            for (int j = 0; j < 8; j++)
            {
              auto offset = abs(j - ~file);
              if (offset == 0)
                continue;
              for (int coef = -1; coef <= 1; coef += 2)
              {
                auto end_rank = ~rank + coef * (j - ~file);
                if (end_rank < 0 or 7 < end_rank)
                  continue;
                plugin::Position end_pos(static_cast<plugin::File>(j),
                    static_cast<plugin::Rank>(end_rank));
                push_move(moves, QuietMove(color_piece, position, end_pos, piece_type));
              }
            }
            break;
          }

        case plugin::PieceType::ROOK:
          for (int i = 0; i < 8; i++)
          {
            if (i != ~file) {
              plugin::Position end_pos(static_cast<plugin::File>(i), rank);
              push_move(moves, QuietMove(color_piece, position, end_pos, piece_type));
            }
            if (i != ~rank) {
              plugin::Position end_pos(file, static_cast<plugin::Rank>(i));
              push_move(moves, QuietMove(color_piece, position, end_pos, piece_type));
            }
          }
            break;

        case plugin::PieceType::BISHOP:
          {
            for (int j = 0; j < 8; j++)
            {
              auto offset = abs(j - ~file);
              if (offset == 0)
                continue;
              for (int coef = -1; coef <= 1; coef += 2)
              {
                auto end_rank = ~rank + coef * (j - ~file);
                if (end_rank < 0 or 7 < end_rank)
                  continue;
                plugin::Position end_pos(static_cast<plugin::File>(j),
                    static_cast<plugin::Rank>(end_rank));
                push_move(moves, QuietMove(color_piece, position, end_pos, piece_type));
              }
            }
            break;
          }

        case plugin::PieceType::KNIGHT:
          {
            int x[2] = {1, 2};
            int y[2] = {2, 1};

            for (int j = 0; j < 2; ++j) {
              auto x1 = x[j];
              auto y1 = y[j];
              for(int i = 0; i < 4; i++)
              {
                if (0 <= x1 + ~file and ~file + x1 < 8 and 0 <= ~rank + y1 and ~rank +y1 < 8)
                {
                  plugin::Position pos_1(static_cast<plugin::File>(x1 +  ~file),
                      static_cast<plugin::Rank>(y1 + ~rank));
                  push_move(moves, QuietMove(color_piece, position, pos_1, piece_type));
                }

                int temp;
                temp = x1;
                x1 = -y1;
                y1 = temp;
              }
            }
            break;
          }

        case plugin::PieceType::PAWN:
          if (~position.rank_get() <= 0 or 7 <= ~position.rank_get())
            break;
          {
            plugin::Position front(file, static_cast<plugin::Rank>(~rank + dir)); // Simple move
            push_move(moves, QuietMove(color_piece, position, front, piece_type, false));
          }
          {
            plugin::Position double_front(file, static_cast<plugin::Rank>(~rank + 2*dir)); // Double move
            push_move(moves, QuietMove(color_piece, position, double_front, piece_type, false));
          }
          for (char i = -1; i <= 1; i += 2) { // Attack
            if (~file + i < 0 or 7 < ~file + i)
              continue;
            plugin::Position diag_left(static_cast<plugin::File>(~file + i),
                static_cast<plugin::Rank>(~rank + dir));
            push_move(moves, QuietMove(color_piece, position, diag_left, piece_type, true));

          }
          break;

        default:
          throw "Impossible";
          break;
      }
    }
  }

  return moves;
}

void ChessBoard::push_move(std::vector<std::shared_ptr<Move>>& moves, QuietMove move) const
{
  if (RuleChecker::is_move_valid(*this, move))
    moves.push_back(std::make_shared<QuietMove>(move));
}


const std::shared_ptr<Move> ChessBoard::last_move_get() const {
  return last_move_;
}

plugin::Position ChessBoard::initial_king_position(plugin::Color c)
{
  if (c == plugin::Color::WHITE)
    return plugin::Position(plugin::File::E, plugin::Rank::ONE);
  return plugin::Position(plugin::File::E, plugin::Rank::EIGHT);
}

plugin::Position ChessBoard::initial_rook_position(plugin::Color c,
    bool king_side)
{
  return plugin::Position(king_side ? plugin::File::H : plugin::File::A,
      c == plugin::Color::WHITE ? plugin::Rank::ONE
      : plugin::Rank::EIGHT);
}

plugin::Position ChessBoard::castling_king_end_position(plugin::Color color,
    bool king_side)
{
  return plugin::Position(king_side ? plugin::File::G : plugin::File::C,
      color == plugin::Color::WHITE ? plugin::Rank::ONE
      : plugin::Rank::EIGHT);
}

plugin::Position ChessBoard::castling_rook_end_position(plugin::Color color,
    bool king_side)
{
  return plugin::Position(king_side ? plugin::File::F : plugin::File::D,
      color == plugin::Color::WHITE ? plugin::Rank::ONE
      : plugin::Rank::EIGHT);
}

plugin::Position ChessBoard::get_king_position(plugin::Color color) const
{
  for (auto i = 0; i < 8; i++)
  {
    for (auto j = 0; j < 8; j++)
    {
      auto p = plugin::Position(static_cast<plugin::File>(j),
          static_cast<plugin::Rank>(i));
      if (piecetype_get(p) != std::experimental::nullopt and piecetype_get(p).value() ==  plugin::PieceType::KING and color_get(p) == color) {
        return p;
      }
    }
  }
  throw std::invalid_argument("There is no king !");
  std::cerr << "There is no king in the following chessbaord" << std::endl;
  pretty_print();
  std::cerr << "Up" << std::endl;
}
