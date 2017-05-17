#include "AI.hh"
#include "rule-checker.hh"
#include "plugin-auxiliary.hh"
#include "parser.hh"

AI::AI(plugin::Color color) 
  : Player(color) 
    , opponent_color_(!color)
    , best_move_(nullptr)
    , board_()
{
  std::cerr << "my color is " << color_ << " and my opponent color is " << opponent_color_ << std::endl;
}

std::string AI::play_next_move(const std::string& received_move)
{
  std::cout << "received = " << received_move << std::endl;
  if (received_move != "") {
    auto pos = received_move.find_last_of(' ');
    std::string move = received_move.substr(pos);
    auto opponent_move = Parser::parse_move(move, opponent_color_);
    std::cerr << "opponent played " << opponent_move << std::endl;
    board_.update(opponent_move);
  }

  best_move_ = nullptr;

  history_board_.push_back(&board_);
  board_.pretty_print();
  auto best_move_value = minimax(0, color_);
  history_board_.pop_back();
  std::cerr << "best move is : " << *best_move_ << std::endl << "its value is " << best_move_value << std::endl;
  board_.update(best_move_);
  std::string input = best_move_->to_lan();
  return input;
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

  for (auto i = 0; i < 8; i++)
  {
    for (auto j = 0; j < 8; j++)
    {
      auto pos = plugin::Position(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));
      auto piece_type = board.piecetype_get(pos);
      auto piece_color = board.color_get(pos);

      if (piece_type != std::experimental::nullopt)
      {
        if (piece_color == plugin::Color::BLACK)
          mobility += get_piece_bonus_position(piece_type.value(), i, j);
        else
          mobility += get_piece_bonus_position(piece_type.value(), 7 - i, 7 - j);
      }
    }
  }
  return mobility;
}


int AI::board_material(const ChessBoard& board)
{
  int queens = piece_numbers(board, plugin::PieceType::QUEEN, color_) - piece_numbers(board, plugin::PieceType::QUEEN, opponent_color_);

  int rooks = piece_numbers(board, plugin::PieceType::ROOK, color_) - piece_numbers(board, plugin::PieceType::ROOK, opponent_color_);

  int bishops = piece_numbers(board, plugin::PieceType::BISHOP, color_) - piece_numbers(board, plugin::PieceType::BISHOP, opponent_color_);

  int knights = piece_numbers(board, plugin::PieceType::KNIGHT, color_) - piece_numbers(board, plugin::PieceType::KNIGHT, opponent_color_);

  int pawns = piece_numbers(board, plugin::PieceType::PAWN, color_) - piece_numbers(board, plugin::PieceType::PAWN, opponent_color_);
  int score = 9 * queens + 5 * rooks + 3 * (bishops + knights) + pawns;
  /*if (score != 0) {
    board.pretty_print();
    std::cerr << "score = " << score << std::endl;
  }*/
  return score; // backward
}


// Coefficients aren't set yet
int AI::evaluate(const ChessBoard& board)
{
  //std::cerr << "Evaluation of " << std::endl;
  //board.pretty_print();
  int material = board_material(board);
  //int bonus_position = board_bonus_position(board);
  /*int king_trop = king_tropism(board);
  int doubled = count_doubled(board, color_) - count_doubled(board, opponent_color_);
  int isolated = count_isolated(board, color_) - count_isolated(board, opponent_color_);*/
  plugin::Position king_position = board_.get_king_position(color_);
  plugin::Position opponent_king_position = board_.get_king_position(opponent_color_);
  int check = RuleChecker::isCheck(board_, opponent_king_position) - RuleChecker::isCheck(board_, king_position);


  int score = 1000 * check + material;// + 0.05 * bonus_position;// + king_trop + 0.5 * (doubled + isolated);
  //std::cerr << "score is " << score << std::endl;
  return score;
}

int AI::minimax(int depth , plugin::Color playing_color)
{
  /*std::cerr << "depth = " << depth << std::endl;
  std::cerr << "playing color = " << playing_color << std::endl;*/
  if (depth >= max_depth_)
  {
    return evaluate(*history_board_[depth]);
  }

  int best_move_value = (playing_color == color_) ? -100000 : 100000;

  const ChessBoard& board = *(history_board_.at(depth));

  for (auto i = 0; i < 8; i++)
  {
    for (auto j = 0; j < 8; j++)
    {
      auto position = plugin::Position(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));
      if (board.piecetype_get(position) == std::experimental::nullopt or board.color_get(position) != playing_color)
        continue;

      std::vector<std::shared_ptr<Move>> moves = RuleChecker::possible_moves(board, position);

      for (auto move_ptr : moves)
      {
        Move& move = *move_ptr;
        //std::cerr << "move is " << move << std::endl;
        //Moving
        ChessBoard tmp = ChessBoard(board);

        tmp.apply_move(move);
        history_board_.push_back(&tmp);
        //tmp.pretty_print();
        auto move_value = minimax(depth + 1, !playing_color);

        //Save best move
        if (depth == 0)
          std::cerr << "move " << move << " scored " << move_value << std::endl;
        if ((move_value > best_move_value) == (playing_color == color_)) {
          best_move_value = move_value;
          if (depth == 0) {
            best_move_ = move_ptr;
            std::cerr << "best_move so far is " << *best_move_ << std::endl;
          }

        }
        history_board_.pop_back();
        //Undo Move
      }
    }
  }
  //julien est bete ohhhhhhhh! non mais on l'aime notre juju :D
  return best_move_value;
}


int AI::piece_numbers(const ChessBoard& board, plugin::PieceType type,
    plugin::Color color)
{
  int count = 0;
  for (auto i = 0; i < 8; i++)
  {
    for (auto j = 0; j < 8; j++)
    {
      auto piece_type = board.piecetype_get(plugin::Position(static_cast<plugin::File>(i),
            static_cast<plugin::Rank>(j)));
      auto piece_color = board.color_get(plugin::Position(static_cast<plugin::File>(i),
            static_cast<plugin::Rank>(j)));
      if (piece_type == type && piece_color == color)
        ++count;
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
int AI::king_tropism(const ChessBoard& board)
{
  int count = 0;
  auto king_pos = board_.get_king_position(color_);

  for (auto i = 0; i < 8; i++)
  {
    for (auto j = 0; j < 8; j++)
    {
      auto pos = plugin::Position(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));
      if (board.piecetype_get(pos) == std::experimental::nullopt)
        continue;
      auto piece_type = board.piecetype_get(pos).value();
      auto piece_color = board.color_get(pos);
      if (piece_color != opponent_color_)
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
