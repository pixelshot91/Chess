#include "chessboard.hh"
#include "rule-checker.hh"
#include "plugin-auxiliary.hh"
#include <chrono>
#include <thread>
/*std::ostream& operator<<(std::ostream& o, const plugin::Position& p);*/

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

  animate(move);

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

short ChessBoard::apply_move(const Move& move)
{
  if (move.move_type_get() == Move::Type::QUIET)
  {
    const QuietMove& quiet_move = static_cast<const QuietMove&>(move);
    cell_t source_square = get_square(quiet_move.start_get());
    //std::cerr << "source_square :" << std::hex << source_square << std::endl;
    cell_t destination_square = get_square(quiet_move.end_get());
    if (quiet_move.is_promotion()) {
      set_square(quiet_move.end_get(), (static_cast<bool>(move.color_get()) << 7) | 0x8 | quiet_move.promotion_piecetype_get());
      set_square(quiet_move.start_get(), 0x7); // 0b000001111
    }
    else
      move_piece(quiet_move.start_get(), quiet_move.end_get());
    short ret = (static_cast<short>(source_square) << 8) | destination_square;
    //std::cerr << "generating token : " << std::hex << ret << std::endl;
    return ret;
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
    return 0;
  }
}

void ChessBoard::undo_move(const Move& move, short token)
{
  //std::cerr << "Undoing " << move << std::endl;
  if (move.move_type_get() == Move::Type::QUIET)
  {
    const QuietMove& quiet_move = static_cast<const QuietMove&>(move);
    if (quiet_move.is_promotion()) {
      set_square(quiet_move.start_get(), (static_cast<bool>(move.color_get()) << 7)  | 0x8 | 0x5);
      set_square(quiet_move.end_get(), token); // 0b000001111
    }
    else {
      //pretty_print();
      //std::cerr << "setting sqaure.." << std::endl;
      //move_piece(quiet_move.start_get(), quiet_move.end_get());
      //std::cerr << "token = " << std::hex << token << std::endl;
      set_square(quiet_move.start_get(), static_cast<char>(token >> 8));
      set_square(quiet_move.end_get(), static_cast<char>(token));
      //pretty_print();
    }
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
      bool color = color_get(pos) == plugin::Color::BLACK;
      std::string blue[2] = {"\x1B[48;5;111m", "\x1B[48;5;27m"};
      std::string orange[2] = {"\x1B[48;5;52m", "\x1B[48;5;214m"};
      std::string green[2] = {"\x1B[48;5;71m", "\x1B[48;5;29m"};
      std::string wtf[] = {"\x1B[48;5;46m", "\x1B[48;5;13m", "\x1B[48;5;9m"};
      if (color)
        std::cout << "\x1B[30m";
      std::cout << green[(i+j) % 2];
      /*if ((i + j) % 2 == 0)
        std::cout << "\x1B[48;5;27m"; // BLACK
        //std::cout << "\x1B[48;5;52m"; // BLACK
      else
        std::cout << "\x1B[48;5;111m"; // WHITE
        //std::cout << "\x1B[48;5;208m"; // WHITE
        //std::cout << "\x1B[31;48;5;95m";
        //std::cout << "\x1B[07m";*/
      std::cout << " ";
      if (piece == std::experimental::nullopt)
        std::cout << " ";
      else
      {
        /*if ((i + j) % 2)*/
          //color = !color;

        auxiliary::GetUnicodeChar(
            0x2654 + 6 +/*color * 6 +*/
            auxiliary::PieceTypeToInt(piece.value()), chars);
        std::cout << chars;
      }
      std::cout << " \x1B[00m";
    }
    std::cout << i + 1 << std::endl;
  }
  std::cout << "  a  b  c  d  e  f  g  h" << std::endl;
  /*for (int i = 0; i < 8; ++i)
    std::cout << "\x1B[A";
  std::cout << "bonjour";
  for (int i = 0; i < 8; ++i)
    std::cout << std::endl;

  std::cout << std::endl;*/
}

int sign(int n)
{
  return (n > 0) ? 1 : (n == 0) ? 0 : -1;
}

void ChessBoard::animate(const Move& move) const
{
  //pretty_print();
  //pretty_print();
  //std::cout << "Animate" << std::endl;
  if (move.move_type_get() == Move::Type::QUIET)
  {
    auto quiet_move = static_cast<const QuietMove&>(move);
    auto piecetype = quiet_move.piecetype_get();
    if (quiet_move.piecetype_get() == plugin::PieceType::KNIGHT) {
      auto tmp = ChessBoard(*this);
      tmp.apply_move(move);
      tmp.pretty_print();
      return;
    }
    auto start = quiet_move.start_get();
    auto end = quiet_move.end_get();
    int delta_file = ~end.file_get() - ~start.file_get();
    int delta_rank = ~end.rank_get() - ~start.rank_get();
    int dir_file = sign(delta_file);
    int dir_rank = sign(delta_rank);
    int file = ~start.file_get();
    int rank = ~start.rank_get();
    for (;
        not(file == ~end.file_get() + dir_file and rank == ~end.rank_get() + dir_rank);
        file += dir_file, rank += dir_rank)
    {
      auto tmp = ChessBoard(*this);
      tmp.set_square(quiet_move.start_get(), 0x7);
      auto pos = plugin::Position(static_cast<plugin::File>(file), static_cast<plugin::Rank>(rank));
      //std::cout << pos;
      tmp.set_square(pos, ((char)quiet_move.color_get() << 7) |(char)auxiliary::PieceTypeToInt(piecetype));
      tmp.pretty_print();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      if (not(file == ~end.file_get() and rank == ~end.rank_get()))
        for (int up = 0; up < 10; up++)
          std::cout << "\b\x1B[A";
    }
  }
  //std::cout << std::endl;
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
  /*auto opponent_color = !color;
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
    {
      plugin::Position pos(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));
      for (auto m: get_possible_actions(opponent_color, pos))
        if (m->move_type_get() == Move::Type::QUIET and static_cast<QuietMove&>(*m).end_get() == current_cell)
          return true;
    }*/

  for (int j = 0; j < 8; ++j)
    for (int i = 0; i < 8; ++i)
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
      plugin::Position pos(file, rank);
      auto pos_moves = get_possible_actions(playing_color, pos);
      moves.insert(moves.end(), pos_moves.begin(), pos_moves.end());
    }
  }
  return moves;
}


std::vector<std::shared_ptr<Move>> ChessBoard::get_possible_actions(plugin::Color playing_color, plugin::Position position) const
{
  std::vector<std::shared_ptr<Move>> moves;
  auto file = position.file_get();
  auto rank = position.rank_get();
  char dir = (playing_color == plugin::Color::WHITE) ? 1 : -1;
  auto color_piece = color_get(position);
  if (color_piece != playing_color)
    return moves;
  if (piecetype_get(position) == std::experimental::nullopt)
    return moves;
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
  return moves;
}

void ChessBoard::push_move(std::vector<std::shared_ptr<Move>>& moves, QuietMove move) const
{
  if (RuleChecker::is_move_valid(*this, move))
    moves.push_back(std::make_shared<QuietMove>(move));
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


