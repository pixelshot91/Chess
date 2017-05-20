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
    std::cerr << "opponent played " << *opponent_move << std::endl;
    board_.update(opponent_move);
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
    board_.update(move);
    std::string input = move->to_an();
    return input;
  }
  else {
    best_move_ = nullptr;

    history_board_.push_back(&board_);
    board_.pretty_print();
    auto best_move_value = minimax(0, color_, -10000000, 10000000);
    if (best_move_ == nullptr)
    {
      std::cerr << "I am doomed" << std::endl;
      std::vector<std::shared_ptr<Move>> moves = RuleChecker::possible_moves(board_, color_);
      best_move_ = moves[0];
    }
    history_board_.pop_back();
    std::cerr << "best move is : " << *best_move_ << std::endl << "its value is " << best_move_value << std::endl;
    board_.update(best_move_);
    std::string input = best_move_->to_an();
    //exit(43);
    return input;
  }
}

/*
   int GameControl::Evaluate(ChessBoard _B)
   {
   int material=0,bonus=0,mobility=0;
   for(int i=0;i < 8; i++)
   for(int j = 0; j < 8; j++)
   {
   if(_B.Board[i][j]!=EMPTY)
   {
   if(_B.Board[i][j]->pieceColor==WHITE){
   material+=-_B.Board[i][j]->Weight;
   bonus+=-_B.Board[i][j]->bonusPosition[i][j];
   mobility+=-_B.Board[i][j]->getPossibleMovesList(i,j,B).size();
   }
   else {
   material+=_B.Board[i][j]->Weight;
   bonus+=_B.Board[i][j]->bonusPosition[i][j];
   mobility+=_B.Board[i][j]->getPossibleMovesList(i,j,B).size();
   }
   }
   }
   return material+bonus/10+mobility/20;
   }
   */

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
  int mobility = 0;
  int pawn = 0, queen = 0, rook = 0, bishop = 0, knight = 0;

  for (auto i = 0; i < 8; i++)
  {
    for (auto j = 0; j < 8; j++)
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
          mobility += get_piece_bonus_position(piece_type.value(), i, j);
        else
          mobility += get_piece_bonus_position(piece_type.value(), 7 - i, 7 - j);
      }
    }
  }

  return 900 * queen + 500 * rook + 300 * bishop + 300 * knight + 100 * pawn + 0.5 * mobility;
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
  //int material = board_material(board);
  int material_bonus_position = board_bonus_position(board);
  int king_trop = king_tropism(board, color_) - king_tropism(board, opponent_color_);
  int doubled = count_doubled(board, color_) - count_doubled(board, opponent_color_);
  int isolated = count_isolated(board, color_) - count_isolated(board, opponent_color_);
  /*plugin::Position king_position = board_.get_king_position(color_);
  plugin::Position opponent_king_position = board_.get_king_position(opponent_color_);
  int check = RuleChecker::isCheck(board_, opponent_king_position) - RuleChecker::isCheck(board_, king_position);*/


  int score = /*1000 * check +*/ material_bonus_position + 3 * king_trop + 50 * (doubled + isolated);
  //int score = material;
  /*std::cerr << "score is " << score << " (material : " << material << ", position " << 0.5 * bonus_position
    << ", king_tropism " << 3 * king_trop << ")" << std::endl;*/
  return score / 50;
}

int AI::minimax(int depth , plugin::Color playing_color, int A, int B)
{
  /*std::cerr << "depth = " << depth << std::endl;
  std::cerr << "playing color = " << playing_color << std::endl;*/
  const ChessBoard& board = *(history_board_[fixed_board_ + depth]);
  std::vector<std::shared_ptr<Move>> moves = RuleChecker::possible_moves(board, playing_color);
  if (moves.size() == 0)
  {
    auto playing_king_position = board.get_king_position(playing_color);
    if (RuleChecker::isCheck(board, playing_king_position))
      return -100000 * (max_depth_ - depth + 1);
    else
      return 0;
  }
  /*if (RuleChecker::three_fold_repetition(history_board_))
    return 0;*/

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
    int move_value;
    /*bool in_check = RuleChecker::isCheck(tmp, opponent_king_position);
    if (in_check) {
      if (RuleChecker::no_possible_move(tmp, !playing_color))
        move_value = 1000 * ((playing_color == color_) ? 1 : -1);
      else
        move_value = 100 * ((playing_color == color_) ? 1 : -1);
    }
    else {*/
    history_board_.push_back(&tmp);
      //tmp.pretty_print();
    try {
      move_value = -minimax(depth + 1, !playing_color, -A, -B);

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

      if (A < move_value) {
        A = move_value;
        if (A >= B) {
          history_board_.pop_back();
          //std::cerr << "Alpha beta pruning" << std::endl;
          break;
        }
      }

    }
    //if (!in_check)
      history_board_.pop_back();
    //Undo Move
  }
  /* }
  }*/
  //julien est bete ohhhhhhhh! non mais on l'aime notre juju :D
  return best_move_value;
}


int AI::piece_numbers(const ChessBoard& board, plugin::PieceType type,
    plugin::Color color)
{
  /*std::cerr << "counting piece in " << std::endl;
  board.pretty_print();*/
  int count = 0;
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
}



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

int AI::count_isolated(const ChessBoard& board, plugin::Color color)
{
  int count = 0;
  bool present = false;
  int meet = 0;

  for (auto j = 0; j < 8; j++)
  {
    for (auto i = 0; i < 8; i++)
    { // vertical traversal - not horizontal like we usually do
      auto piece_type = board.piecetype_get(plugin::Position(static_cast<plugin::File>(i),
            static_cast<plugin::Rank>(j)));
      auto piece_color = board.color_get(plugin::Position(static_cast<plugin::File>(i),
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
