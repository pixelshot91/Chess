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
  std::cerr << "my color is " << color_ << " and my opponent color is " << opponent_color_ << std::endl;
}

void AI::set_scripted_moves(std::vector<std::shared_ptr<Move>> moves)
{
  scripted_moves_ = moves;
  std::cerr << "AI moves are : " << std::endl;
  for (auto m : scripted_moves_)
    std::cerr << *m << std::endl;
}

std::string AI::play_next_move(const std::string& received_move)
{
  //std::cout << "received = " << received_move << std::endl;
  if (received_move != "") {
    auto pos = received_move.find_last_of(' ');
    std::string move = received_move.substr(pos + 1);
    auto opponent_move = Parser::parse_uci(move, opponent_color_, board_);
    //std::cerr << "opponent played " << *opponent_move << std::endl;
    board_.apply_move(*opponent_move);
    permanent_history_board_.push_back(board_.board_get());
  }
  if (scripted_moves_.size() != 0)
  {
    std::cerr << "PLAY NEXT MOVE AI moves are : " << std::endl;
    /*for (auto m : scripted_moves_)
      std::cerr << *m << std::endl;*/

    auto move = scripted_moves_.front();
    std::cerr << "Playing scripted move " << *move << std::endl;
    scripted_moves_.erase(scripted_moves_.begin());
    if (scripted_moves_.size() != 0)
      scripted_moves_.erase(scripted_moves_.begin());
    board_.apply_move(*move);
    std::string input = move->to_an();
    return input;
  }
  else {
    best_move_ = nullptr;

    if (temporary_history_board_.size() != 0) {
      std::cerr << "temporary_history_board_ is not empty" << std::endl;
      exit(42);
    }

    temporary_history_board_.push_back(&board_);
    board_.pretty_print();

    /*std::vector<std::shared_ptr<Move>> moves = RuleChecker::possible_moves(board_, color_);
    size_t new_possible_nb = moves.size();
    std::cerr << "nb_possible_move = " << new_possible_nb << std::endl;
    max_depth_ = std::round(std::log2(3.5 / c_) / std::log2(new_possible_nb + 7));
    if (max_depth_ < 3)
      max_depth_ = 3;

    std::cerr << "c = " << c_ << std::endl;
    std::cerr << "Max depth = " << max_depth_ << std::endl;*/
    double time = 0;
    {
      scoped_timer timer(time);
      auto best_move_value = minimax(0, color_, -10000000, 10000000);
    }
    //c_ = (c_ + time / std::pow(new_possible_nb, max_depth_)) / 2;
    /*if (time < 1) {
      max_depth_++;
      std::cerr << "New max_depth is " << max_depth_ << std::endl;
      }
      else if (time > 5) {
      max_depth_--;
      std::cerr << "New max_depth is " << max_depth_ << std::endl;
      }*/
    std::cout << "Time is : " << time << "s" << std::endl;
    if (best_move_ == nullptr)
    {
      std::cerr << "I am doomed" << std::endl;
      std::vector<std::shared_ptr<Move>> moves = RuleChecker::possible_moves(board_, color_);
      best_move_ = moves[0];
    }
    temporary_history_board_.pop_back();
    //std::cerr << "best move is : " << *best_move_ << std::endl << "its value is " << best_move_value << std::endl;
    board_.apply_move(*best_move_);
    permanent_history_board_.push_back(board_.board_get());
    std::string input = best_move_->to_an();
    exit(43);
    return input;
  }
}

int AI::get_piece_bonus_position(plugin::PieceType piece, int i, int j)
{
  switch (piece)
  {
    case plugin::PieceType::PAWN:
      return pawn_weight_board[i][j];
    case plugin::PieceType::QUEEN:
      return queen_weight_board[i][j];
    case plugin::PieceType::ROOK:
      return rook_weight_board[i][j];
    case plugin::PieceType::BISHOP:
      return bishop_weight_board[i][j];
    case plugin::PieceType::KING:
      return king_middle_weight_board[i][j];
    case plugin::PieceType::KNIGHT:
      return knight_weight_board[i][j];
    default:
      return 0;
  }
}

int AI::board_bonus_position(const ChessBoard& board)
{
  //int material = 0;
  //int bonus = 0;
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
        if (piece_color == plugin::Color::BLACK)
          bonus_pos += get_piece_bonus_position(piece_type.value(), i, j);
        else
          bonus_pos += get_piece_bonus_position(piece_type.value(), 7 - i, 7 - j);
      }
    }
  }

  return 900 * queen + 500 * rook + 300 * bishop + 300 * knight + 100 * pawn + 0.5 * bonus_pos;
}


int AI::evaluation_function(const ChessBoard& board)
{
  //int bonus = 0;
  int king_tropism = 0;
  int bonus_pos = 0;
  int pawn = 0, queen = 0, rook = 0, bishop = 0, knight = 0;
  int op_pawn = 0, op_queen = 0, op_rook = 0, op_bishop = 0, op_knight = 0;



  auto king_pos = board.get_king_position(color_);
  auto op_king_pos = board.get_king_position(opponent_color_);
  int king_file = static_cast<int>(king_pos.file_get());
  int king_file_malus = 0;

  bool left_king_file_empty = true;
  bool right_king_file_empty = true;

  /*static int piece_attacking = 0;
    static int value_of_attack = 0;
    static int opponent_value_of_attack = 0;
    static int opponent_piece_attacking = 0;*/

  int double_count = 0;
  int op_double_count = 0;
  for (auto i = 0; i < 8; i++)
  {
    int nb_pawn_file = 0;
    int op_nb_pawn_file = 0;
    for (auto j = 0; j < 8; j++)
    {
      auto pos = plugin::Position(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));
      auto piece_type = board.piecetype_get(pos);
      auto piece_color = board.color_get(pos);

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
        if (j == king_file - 1)
          left_king_file_empty = false;
        else if (j == king_file + 1)
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
        if (piece_color == plugin::Color::BLACK)
          bonus_pos += get_piece_bonus_position(piece_type.value(), i, j);
        else
          bonus_pos += get_piece_bonus_position(piece_type.value(), 7 - i, 7 - j);
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
  /*if (rook > 1)
    material_bonus -= 15;
    if (op_rook > 1)
    material_bonus += 15;
    if (knight > 1)
    material_bonus -= 20;
    if (op_knight > 1)
    material_bonus += 20;*/

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

  return piece_material + material_bonus 
    + 0.5 * bonus_pos 
    + king_tropism 
    - 50 * (double_count - op_double_count + count_isolated(color_) - count_isolated(opponent_color_))
    - king_file_malus;
  //  + (opponent_attacking_king_zone - attacking_king_zone);

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


/*int AI::board_material(const ChessBoard& board)
  {
  int piece_nb[6] = {0};
  int piece_weight[6] = {1000, 9, 5, 3, 3, 1};
  int score = 0;

  for (auto i = 0; i < 8; i++)
  {
  for (auto j = 0; j < 8; j++)
  {
  auto pos = plugin::Position(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));

  }
  }*/


/*for (int i = 0; i < 6; ++i)
  {
  auto type = plugin::piecetype_array()[i];
  piece_nb[i] = piece_numbers(board, type, color_) - piece_numbers(board, type, opponent_color_);
  if (i == 0 and piece_nb[0])
  {
  board.pretty_print();
  std::cerr << "a king is missing" << std::endl;
  std::cerr << "White : " << piece_numbers(board, plugin::PieceType::KING, color_) << " Black : " << piece_numbers(board, plugin::PieceType::KING, opponent_color_) << std::endl;
  throw std::invalid_argument("A king is dead");
  }


//std::cerr << type << " = " << piece_nb[i] << std::endl;
score += 100 * piece_weight[i] * piece_nb[i];
}*/
//return score; // backward
//}


// Coefficients aren't set yet
int AI::evaluate(const ChessBoard& board)
{
  /*std::cerr << "Evaluation of " << std::endl;
    board.pretty_print();*/
  /*int king_trop = king_tropism(board, color_) - king_tropism(board, opponent_color_);
    int doubled = count_doubled(board, color_) - count_doubled(board, opponent_color_);
    int isolated = count_isolated(board, color_) - count_isolated(board,
    opponent_color_);*/
  /*plugin::Position king_position = board_.get_king_position(color_);
    plugin::Position opponent_king_position = board_.get_king_position(opponent_color_);
    int check = RuleChecker::isCheck(board_, opponent_king_position) - RuleChecker::isCheck(board_, king_position);*/

  //return evaluation_function(board) / 50;
  //  int score = /*1000 * check +*/ material_bonus_position + 3 * king_trop + 50 * (doubled + isolated);
  //int score = material;
  /*std::cerr << "score is " << score << " (material : " << material << ", position " << 0.5 * bonus_position
    << ", king_tropism " << 3 * king_trop << ")" << std::endl;*/
  int material_bonus_position = board_bonus_position(board);
  return material_bonus_position / 50;
}

int AI::minimax(int depth, plugin::Color playing_color, int A, int B)
{
  /*std::cerr << "depth = " << depth << std::endl;
    std::cerr << "playing color = " << playing_color << std::endl;*/
  const ChessBoard& board = *(temporary_history_board_[depth]);
  std::vector<std::shared_ptr<Move>> moves = /*board.get_possible_actions(playing_color);*/RuleChecker::possible_moves(board, playing_color);
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

  int best_move_value = -10000000;

  /*for (auto i = 0; i < 8; i++)
    {
    for (auto j = 0; j < 8; j++)
    {
    auto position = plugin::Position(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));
    if (board.piecetype_get(position) == std::experimental::nullopt or board.color_get(position) != playing_color)
    continue;*/

  for (auto move_ptr : moves)
  {
    Move& move = *move_ptr;
    //std::cerr << "move is " << move << std::endl;
    //Moving
    ChessBoard tmp = ChessBoard(board);

    tmp.apply_move(move);
    temporary_history_board_.push_back(&tmp);
    if (RuleChecker::three_fold_repetition(permanent_history_board_, temporary_history_board_)) {
      temporary_history_board_.pop_back();
      /*std::cerr << "threefold _repietition" << std::endl;
        tmp.pretty_print();
        std::cerr << "permanent history :" << permanent_history_board_.size() << std::endl;
        for (auto b : temporary_history_board_)
        b->pretty_print();
        exit(43);*/
      return 0;
    }
    int move_value;
    try {
      move_value = -minimax(depth + 1, !playing_color, -B, -A);
    }
    catch (std::invalid_argument e) {
      std::cerr << "Move is " << move << std::endl;
      board.pretty_print();
      throw e;
    }

    //Save best move
    if (depth == 0)
      std::cerr << "move " << move << " scored " << move_value << std::endl;
    if (move_value == best_move_value)
    {
      int rand = std::experimental::randint(1, 100);
      if (rand < 40) {
        best_move_value = move_value;
        if (depth == 0) {
          best_move_ = move_ptr;
          std::cerr << "best_move so far is " << *best_move_ << std::endl;
        }
      }
    }
    else if (move_value > best_move_value) {
      best_move_value = move_value;
      if (depth == 0) {
        best_move_ = move_ptr;
        std::cerr << "best_move so far is " << *best_move_ << std::endl;
      }

      if (A > move_value) {
        A = move_value;
        if (A >= B) {
          temporary_history_board_.pop_back();
          break;
        }
      }

    }
    temporary_history_board_.pop_back();
  }
  //julien est bete ohhhhhhhh! non mais on l'aime notre juju :D
  return best_move_value;
}


/*int AI::piece_numbers(const ChessBoard& board, plugin::PieceType type,
  plugin::Color color)
  {*/
/*std::cerr << "counting piece in " << std::endl;
  board.pretty_print();*/
/*  int count = 0;
    for (auto i = 0; i < 8; i++)
    {
    for (auto j = 0; j < 8; j++)
    {
    plugin::Position pos(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));
    if (board.piecetype_get(pos) == std::experimental::nullopt)
    continue;
    auto piece_type = board.piecetype_get(pos).value();
    auto piece_color = board.color_get(pos);
    if (piece_type == type && piece_color == color)
    count++;
    }
    }
    return count;
    }*/



int AI::count_doubled(const ChessBoard& board, plugin::Color color)
{
  auto count = 0;
  for (auto j = 0; j < 8; j++)
  {
    bool present = false;
    for (auto i = 0; i < 8; i++)
    { // vertical traversal - not horizontal like we usually do
      auto piece_type = board.piecetype_get(plugin::Position(static_cast<plugin::File>(i),
            static_cast<plugin::Rank>(j)));
      auto piece_color = board.color_get(plugin::Position(static_cast<plugin::File>(i),
            static_cast<plugin::Rank>(j)));

      if (!present && (piece_type == plugin::PieceType::PAWN) && (piece_color == color))
        present = true;
      else if (piece_type == plugin::PieceType::PAWN && piece_color == color)
        count ++;
    }
  }
  return count;
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

/*int AI::count_backward(const ChessBoard& board)
  {
  return 0;
  }*/


// Always searches opponent -> thus opponent color
int AI::king_tropism(const ChessBoard& board, plugin::Color color)
{
  int count = 0;
  auto king_pos = board_.get_king_position(color);

  for (auto i = 0; i < 8; i++)
  {
    for (auto j = 0; j < 8; j++)
    {
      auto pos = plugin::Position(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));
      if (board.piecetype_get(pos) == std::experimental::nullopt)
        continue;
      auto piece_type = board.piecetype_get(pos).value();
      auto piece_color = board.color_get(pos);
      if (piece_color == !color)
        continue;

      auto dist = auxiliary::distance(pos, king_pos);

      switch(piece_type)
      {
        case plugin::PieceType::QUEEN:
          dist *= 4;
          break;
        case plugin::PieceType::ROOK:
          dist *= 2;
          break;
        case plugin::PieceType::BISHOP:
          dist *= 2;
          break;
        default:
          break;
      }
      count += dist;
    }
  }
  return count;
}
