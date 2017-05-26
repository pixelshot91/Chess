#include "AI.hh"
#include "rule-checker.hh"
#include "plugin-auxiliary.hh"
#include "parser.hh"
#include <experimental/random>

AI::AI(plugin::Color color) 
  : Player(color) 
  , opponent_color_(!color)
  , best_move_(nullptr)
  , board_()
    , scripted_moves_()
{
  //std::cerr << "my color is " << color_ << " and my opponent color is " << opponent_color_ << std::endl;
}

void AI::set_scripted_moves(std::vector<std::shared_ptr<Move>> moves)
{
  scripted_moves_ = moves;
  for (auto m : scripted_moves_)
    std::cerr << *m << std::endl;
}

std::string AI::play_next_move(const std::string& received_move)
{
  if (received_move != "") {
    auto pos = received_move.find_last_of(' ');
    std::string move = received_move.substr(pos + 1);
    auto opponent_move = Parser::parse_uci(move, opponent_color_, board_);
    board_.apply_move(*opponent_move);
    permanent_history_board_.push_back(board_.board_get());
  }
  if (scripted_moves_.size() != 0)
  {
    auto move = scripted_moves_.front();
    scripted_moves_.erase(scripted_moves_.begin());
    if (scripted_moves_.size() != 0)
      scripted_moves_.erase(scripted_moves_.begin());
    board_.apply_move(*move);
    std::string input = move->to_an();
    return input;
  }
  else {
    best_move_ = nullptr;
    std::cerr << std::endl;

    temporary_history_board_.push_back(&board_);
    //board_.pretty_print();

    std::vector<std::shared_ptr<Move>> moves = RuleChecker::possible_moves(board_, color_);
    if (moves.size() == 1)
    {
      Move& only_move = *moves[0];
      std::cerr << "Only move possible is : " << only_move << std::endl;
      board_.apply_move(only_move);
      permanent_history_board_.push_back(board_.board_get());
      std::string input = only_move.to_an();
      return input;
    }
    size_t nb_possible_moves = moves.size();
    //max_depth_ = std::round(std::log2(3.5 / c_) / std::log2(new_possible_nb + 7));
    max_depth_ = std::round(std::log2(5 / (c_ * nb_possible_moves)) / std::log2(20) + 1);
    const int min_max_dept = 2;
    if (max_depth_ < min_max_dept)
      max_depth_ = min_max_dept;
    //max_depth_ = 1;
    std::cerr << nb_possible_moves << " moves possible, max_depth_ = " << max_depth_ << std::endl;
    //float estimated_time = c_ * nb_possible_moves * std::pow(20, max_depth_ - 1);
    std::cerr << "Estimated time: " << estimate_time(nb_possible_moves) << std::endl;
    std::cerr << "one depth less: " << estimate_time(nb_possible_moves, max_depth_ - 1) << std::endl
              << "one_depth_more: " << estimate_time(nb_possible_moves, max_depth_ + 1) << std::endl;
    int best_move_value;
    double time = 0;
    {
      scoped_timer timer(time);
      best_move_value = minimax(0, color_, -10000000, 10000000);
    }
    std::cerr << "Time : " << time << std::endl;
    c_ = time / (nb_possible_moves * std::pow(20, max_depth_ - 1));
    if (best_move_ == nullptr)
    {
      std::cerr << "I am doomed" << std::endl;
      best_move_ = moves[0];
    }
    temporary_history_board_.pop_back();
    std::cerr << "Best move is : " << *best_move_ << " (score: " << best_move_value << ")" << std::endl;
    board_.apply_move(*best_move_);
    permanent_history_board_.push_back(board_.board_get());
    std::string input = best_move_->to_an();
    std::cerr << std::endl;
    return input;
  }
}

float AI::estimate_time(int nb_possible_moves, int max_depth)
{
  if (max_depth < 0)
    max_depth = max_depth_;
  return c_ * nb_possible_moves * std::pow(20, max_depth - 1);
}

int AI::get_piece_bonus_position(plugin::Color color, plugin::PieceType piece, const plugin::Position& pos)
{
  int file = ~pos.file_get();
  int rank = ~pos.rank_get();
  if (color == plugin::Color::WHITE)
    rank = 7 - rank;
  switch (piece)
  {
    case plugin::PieceType::PAWN:
      return pawn_weight_board[rank][file];
    case plugin::PieceType::QUEEN:
      return queen_weight_board[rank][file];
    case plugin::PieceType::ROOK:
      return rook_weight_board[rank][file];
    case plugin::PieceType::BISHOP:
      return bishop_weight_board[rank][file];
    case plugin::PieceType::KING:
      return king_middle_weight_board[rank][file];
    case plugin::PieceType::KNIGHT:
      return knight_weight_board[rank][file];
    default:
      return 0;
  }
}

int AI::board_bonus_position(const ChessBoard& board)
{
  int bonus_pos = 0;
  int pawn = 0, queen = 0, rook = 0, bishop = 0, knight = 0;

  for (auto j = 0; j < 8; j++)
  {
    for (auto i = 0; i < 8; i++)
    {
      auto pos = plugin::Position(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));
      auto piece_type = board.piecetype_get(pos);
      auto piece_color = board.color_get(pos);
      int positive_if_ai_color = (piece_color == color_) ? 1 : -1;

      if (piece_type != std::experimental::nullopt)
      {
        switch(piece_type.value()) {
          case plugin::PieceType::PAWN:
            pawn += positive_if_ai_color;
            break;
          case plugin::PieceType::QUEEN:
            queen += positive_if_ai_color;
            break;
          case plugin::PieceType::ROOK:
            rook += positive_if_ai_color;
            break;
          case plugin::PieceType::BISHOP:
            bishop += positive_if_ai_color;
            break;
          case plugin::PieceType::KNIGHT:
            knight += positive_if_ai_color;
            break;
          default:
            break;
        }
        /*if (piece_color == plugin::Color::BLACK)
          bonus_pos += get_piece_bonus_position(piece_type.value(), i, j);
        else
          bonus_pos += get_piece_bonus_position(piece_type.value(), 7 - i, 7 - j);*/
      }
    }
  }

  return 900 * queen + 500 * rook + 300 * bishop + 300 * knight + 100 * pawn + 0.5 * bonus_pos;
}


int AI::evaluation_function(const ChessBoard& board)
{
  int king_tropism = 0;
  int bonus_pos = 0;
  int pawn = 0, queen = 0, rook = 0, bishop = 0, knight = 0;
  int op_pawn = 0, op_queen = 0, op_rook = 0, op_bishop = 0, op_knight = 0;



  auto king_pos = board.get_king_position(color_);
  auto op_king_pos = board.get_king_position(opponent_color_);
  auto king_file = king_pos.file_get();
  int king_file_malus = 0;

  bool left_king_file_empty = true;
  bool right_king_file_empty = true;

  int double_count = 0;
  int op_double_count = 0;
  
  for (auto i = 0; i < 8; i++)
  {
    int nb_pawn_file = 0;
    int op_nb_pawn_file = 0;
    for (auto j = 0; j < 8; j++)
    {
      auto file = static_cast<plugin::File>(i);
      auto rank = static_cast<plugin::Rank>(j);
      auto pos = plugin::Position(file, rank);
      auto piece_type = board.piecetype_get(pos);
      auto piece_color = board.color_get(pos);
      int me = (piece_color == color_) ? 1 : -1;

      /**************************************
       * 
       * Material Count and King Tropism Count
       *
       ***************************************/

      auto dist = auxiliary::distance(pos, king_pos);

      if (piece_type != std::experimental::nullopt)
      {

        /* Attacking Our King Zone */
        /*if (piece_color == opponent_color_)
          value_of_attack = king_zone_attack(king_pos, piece_type, value_of_attack, i, j);
          else
          opponent_value_of_attack = king_zone_attack(op_king_pos, piece_type, opponent_value_of_attack, i, j); */

        /* Calculating king file disadvantage */
        if (~i == ~king_file - 1)
          left_king_file_empty = false;
        else if (~i == ~king_file + 1)
          right_king_file_empty = false;

        if (piece_color == color_)
        {
          switch(piece_type.value()) {
            case plugin::PieceType::PAWN:
              pawn++;
              nb_pawn_file++;
              break;
            case plugin::PieceType::QUEEN:
              queen++;
              dist *= 2;
              break;
            case plugin::PieceType::ROOK:
              rook++;
              dist *= 0.5;
              break;
            case plugin::PieceType::BISHOP:
              bishop++;
              dist *= 0.5;
              break;
            case plugin::PieceType::KNIGHT:
              knight++;
              break;
            default:
              break;
          }
        }
        else 
        {
          switch(piece_type.value()) {
            case plugin::PieceType::PAWN:
              op_pawn++;
              op_nb_pawn_file++;
              break;
            case plugin::PieceType::QUEEN:
              op_queen++;
              break;
            case plugin::PieceType::ROOK:
              op_rook++;
              break;
            case plugin::PieceType::BISHOP:
              op_bishop++;
              break;
            case plugin::PieceType::KNIGHT:
              op_knight++;
              break;
            default:
              break;
          }
        }
        bonus_pos += get_piece_bonus_position(piece_color, piece_type.value(), plugin::Position(file, rank)) * me;
      }

      king_tropism += dist;
    }

    if (nb_pawn_file >= 2) // Double count
      double_count++;
    if (op_nb_pawn_file >= 2)
      op_double_count++;

  }

  /**************************************
   * 
   * Material Adjustement
   *
   ***************************************/
  int material_bonus = 0;
  if (bishop > 1)
    material_bonus += 50;
  if (op_bishop > 1)
    material_bonus -= 50;
  
  // lack of pawns
  if (!pawn)
    material_bonus -= 10;
  if (!op_pawn)
    material_bonus +=10;

  /**************************************
   * 
   * Pawn shield
   *
   ***************************************/

  material_bonus += pawn_shield(board, king_pos);
  material_bonus -= pawn_shield(board, op_king_pos);


  /*************************************
   *
   * Open files near king
   *
   *************************************/

  if (!left_king_file_empty && !right_king_file_empty)
    king_file_malus = 60;
  if (!left_king_file_empty || !right_king_file_empty)
    king_file_malus = 20;


  /******************/

  //int attacking_king_zone = value_of_attack * attack_weight[piece_attacking]/ 100;
  //int opponent_attacking_king_zone = opponent_value_of_attack * attack_weight[opponent_piece_attacking] / 100;
  //std::cerr << "there is " << queen << " queen" << std::endl;

  int piece_material = 900 * (queen - op_queen) + 500 * (rook - op_rook) + 300 * (bishop - op_bishop) + 300 * (knight - op_knight) + 100 * (pawn - op_pawn);
  int pawn_formation = double_count - op_double_count + count_isolated(color_) - count_isolated(opponent_color_);
  bonus_pos *= 0.2;
  king_tropism *= 0.2;
  int total = piece_material + material_bonus 
    + bonus_pos 
    + king_tropism 
    - 50 * pawn_formation
    - king_file_malus;
  //  + (opponent_attacking_king_zone - attacking_king_zone);
  /*std::cout << "material " << piece_material << " material bonus " << material_bonus << " king trop " << king_tropism << " position " << bonus_pos <<
    std::endl << " total " << total << std::endl;*/
  return total;

}


int AI::king_zone_attack(plugin::Position king_pos, std::experimental::optional<plugin::PieceType> piece_type, int value_of_attack, int i , int j)
{
  int piece_attacking = 0;
  int nb_squares_attacked = 0;

  if (abs(static_cast<int>(king_pos.file_get()) - i) <= 2 && abs(static_cast<int>(king_pos.rank_get()) - j) <= 2)
  {
    piece_attacking++;
    nb_squares_attacked = /* call to function */ 1;

    switch(piece_type.value())
    {
      case plugin::PieceType::QUEEN:
        value_of_attack += nb_squares_attacked * 80;
        break;
      case plugin::PieceType::ROOK:
        value_of_attack += nb_squares_attacked * 40;
        break;
      case plugin::PieceType::BISHOP:
        value_of_attack += nb_squares_attacked * 20;
        break;
      case plugin::PieceType::KNIGHT:
        value_of_attack += nb_squares_attacked * 20;
        break;
      default:
        break;
    }
  }
  return value_of_attack;
}


int AI::pawn_shield(const ChessBoard& board, plugin::Position king_pos)
{
  auto pawn_file = static_cast<int>(king_pos.file_get()) - 1;
  int pawn_shield_count = 0;

  for (auto i = 0; i <= 2; i++)
  {
    pawn_file += i;
    plugin::Position pawn_pos(static_cast<plugin::File>(pawn_file), static_cast<plugin::Rank>(static_cast<int>(king_pos.rank_get()) + 1));

    auto piece = board.piecetype_get(pawn_pos); 
    if (piece != std::experimental::nullopt && piece == plugin::PieceType::PAWN && board.color_get(pawn_pos) == color_)
      pawn_shield_count += 1;
  }

  if (pawn_shield_count > 1)
    return 50;
  return 0;
}


// Coefficients aren't set yet
int AI::evaluate(const ChessBoard& board)
{
  return evaluation_function(board);// / 50;
  /*int material_bonus_position = board_bonus_position(board);
  return material_bonus_position / 10;*/
}

int AI::minimax(int depth, plugin::Color playing_color, int A, int B)
{
  const ChessBoard& board = *(temporary_history_board_[depth]);
  std::vector<std::shared_ptr<Move>> moves = board.get_possible_actions(playing_color);//RuleChecker::possible_moves(board, playing_color);
  /*struct {
    bool operator()(std::shared_ptr<Move> m1, std::shared_ptr<Move> m2)
    {
      return *m1 < *m2;
    }
  } custom;
  std::sort(moves.begin(), moves.end(), custom);*/
  if (moves.size() == 0)
  {
    auto playing_king_position = board.get_king_position(playing_color);
    if (RuleChecker::isCheck(board, playing_king_position))
      return -100000 * (max_depth_ - depth + 1);
    else
      return 0;
  }

  if (depth >= max_depth_)
  {
    int ret = evaluate(board);
    if (depth % 2)
      return -ret;
    return ret;
  }

  int best_move_value = -1000000;


  for (auto move_ptr : moves)
  {
    Move& move = *move_ptr;
    //std::cerr << move << std::endl;
    /*if (tmp.board_get() != board.board_get())
    {
      board.pretty_print();
      tmp.pretty_print();
      throw std::invalid_argument("board mismatch");
    }*/
    ChessBoard tmp = ChessBoard(board);
    
    short token = tmp.apply_move(move);
    temporary_history_board_.push_back(&tmp);
    if (RuleChecker::three_fold_repetition(permanent_history_board_, temporary_history_board_)) {
      temporary_history_board_.pop_back();
        return 0;
    }
    int move_value = -minimax(depth + 1, !playing_color, -B, -A);

    //Save best move
    if (depth == 0)
      std::cerr << "move " << move << "(" << move.priority_ << ") scored " << move_value << std::endl;
    /*if (move_value == best_move_value)
    {
      int rand = std::experimental::randint(1, 100);
      if (rand < 40) {
        best_move_value = move_value;
        if (depth == 0) {
          best_move_ = move_ptr;
          std::cerr << "best_move so far is " << *best_move_ << " score: " << move_value << std::endl;
        }
      }
    }
    else*/ if (move_value > best_move_value) {
      best_move_value = move_value;
      if (depth == 0) {
        best_move_ = move_ptr;
        std::cerr << "best_move so far is " << *best_move_ << " score: " << move_value << std::endl;
      }

      if (move_value > A) {
        A = move_value;
        if (A >= B) {
          temporary_history_board_.pop_back();
          //std::cerr << "AB pruning" << std::endl;
          return best_move_value;
        }
      }

    }
    temporary_history_board_.pop_back();
    //tmp.undo_move(move, token);
  }
  //julien est bete ohhhhhhhh! non mais on l'aime notre juju :D
  return best_move_value;
}

int AI::count_isolated(plugin::Color color)
{
  int count = 0;
  bool present = false;
  int meet = 0;

  for (auto j = 0; j < 8; j++)
  {
    for (auto i = 0; i < 8; i++)
    { // vertical traversal - not horizontal like we usually do
      auto piece_type = board_.piecetype_get(plugin::Position(static_cast<plugin::File>(i),
            static_cast<plugin::Rank>(j)));
      auto piece_color = board_.color_get(plugin::Position(static_cast<plugin::File>(i),
            static_cast<plugin::Rank>(j)));

      if (piece_type == plugin::PieceType::PAWN && piece_color == color)
      {
        if (!present) //first time meeting a pawn
        {
          present = true;
        }
        meet++;
      }
    }

    if (present && !meet)
    {
      present = false;
      count++;
    }
    meet = 0;
  }
  return count;

}

