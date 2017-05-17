#include "AI.hh"
#include "../plugin-auxiliary.hh"

AI::AI(plugin::Color ai_color) 
      : board_()
      , ai_color_(ai_color)
      , opponent_color_(static_cast<plugin::Color>(!static_cast<bool>(ai_color_)))
      , evalboard_()
{
}

std::string AI::play_next_move(const std::string& received_move)
{
  std::cout << received_move << std::endl;

  /*
  auto next_move = minimax(board_, 5, ai_color_);
  std::string input = auxiliary::to_lan(std::get<Move>(next_move));
  input;
  */
  return "toto";
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

      if (piece_type == std::experimental::nullopt)
      {
        if (piece_color == plugin::Color::WHITE)
          mobility += get_piece_bonus_position(piece_type, i, j);
        else
          mobility += get_piece_bonus_position(piece_type, 7 - i, 7 - j);
      }
    }
  }
  return mobility;
}


int AI::board_material(const ChessBoard& board)
{
  int queens = 9 * (piece_numbers(board, plugin::PieceType::QUEEN, ai_color_) - piece_numbers(board, plugin::PieceType::QUEEN, opponent_color_));

  int rooks = 5 * (piece_numbers(board, plugin::PieceType::ROOK, ai_color_) - piece_numbers(board, plugin::PieceType::ROOK, opponent_color_));

  int bishops = piece_numbers(board, plugin::PieceType::BISHOP, ai_color_) - piece_numbers(board, plugin::PieceType::BISHOP, opponent_color_);

  int knights = piece_numbers(board, plugin::PieceType::KNIGHT, ai_color_) - piece_numbers(board, plugin::PieceType::KNIGHT, opponent_color_);

  int pawns = piece_numbers(board, plugin::PieceType::PAWN, ai_color_) - piece_numbers(board, plugin::PieceType::PAWN, opponent_color_);

  return queens + rooks + 3 *(bishops - knights) + pawns + 0.5 * (doubled + isolated); // backward
}


// Coefficients aren't set yet
int AI::evaluate(const ChessBoard& board)
{
  int bonus_position = board_bonus_position(board);
  int material = board_material(board);
  int king_trop = king_tropism(board);
  int doubled = count_doubled(board, ai_color_) - count_doubled(board, opponent_color_);
  int isolated = count_isolated(board, ai_color_) - count_isolated(board, opponent_color_);

  return bonus_position + material + king_trop + doubled + isolated;
}

/*
std::pair<Move,int> AI::minimax(const ChessBoard& board, int depth ,
plugin::Color playing_color)
{
    if (!depth)
    {
      auto eval  = evaluate(board);
    }

    int bestValue = -9999;
    std::pair<Move,int> bestMove_value;
    std::pair<Move,int> move_value;

    history_board_.push_back(&board);

    for (auto i = 0; i < 8; i++)
      for (auto j = 0; j < 8; j++)
      {
        Position position = Position(static_cast<File> i,
            static_cast<Rank>j)i;
        if (board_.piecetype_get(position) != std::experimental::nullopt;
&& board_.color_get(position) == playing_color)

          std::vector<Move> moves = bord_.get_possible_actions(position);
            for (auto move : moves)
            {
              //Moving
              
              apply_move(move);
              move_value = minimax(depth-1 , !playing_color);

              //Save best move
              if ((move_value.first > bestMove_value.second)
                  == (playing_color == AI_color))
                bestMove_value = value;
              //Undo Move
        }
      }
    //julien est bete ohhhhhhhh! non mais on l'aime notre juju :D
    history_board_.pop_back();
    return bestMove_value;
}
*/


int AI::piece_numbers(const ChessBoard& board, plugin::PieceType type,
plugin::Color color)
{
  int count;
  for (auto i = 0; i < 8; i++)
  {
    for (auto j = 0; j < 8; j++)
    {
      auto piece_type = board.piecetype_get(plugin::Position(static_cast<plugin::File>(i),
                                                        static_cast<plugin::Rank>(j)));
      auto piece_color = board.color_get(plugin::Position(static_cast<plugin::File>(i),
                                                        static_cast<plugin::Rank>(j)));
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

int AI::count_backward(const ChessBoard& board)
{
  return 0;
}


// Always searches opponent -> thus opponent color
int AI::king_tropism(const ChessBoard& board)
{
  int count = 0;
  auto king_pos = board_.get_king_position(ai_color_);

  for (auto i = 0; i < 8; i++)
  {
    for (auto j = 0; j < 8; j++)
    {
      auto pos = plugin::Position(static_cast<plugin::File>(i), static_cast<plugin::Rank>(j));
      auto piece_color = board.color_get(pos);
      auto piece_type = board.piecetype_get(pos);

      if (piece_color == opponent_color_)
      {
        switch(piece_type)
        {
          case plugin::PieceType::QUEEN:
            // Manhattan distance
            count += 4 * (abs(j - static_cast<int>(king_pos.file_get()) + abs(i - static_cast<int>(king_pos.rank_get()))));
            break;

          case plugin::PieceType::ROOK:
            // Manhattan distance
            count += 2 * (abs(j - static_cast<int>(king_pos.file_get()) + abs(i - static_cast<int>(king_pos.rank_get())));
            break;

          case plugin::PieceType::BISHOP:
            // Manhattan distance
            count += 2 * (abs(j - static_cast<int>(king_pos.file_get())) + abs(i - static_cast<int>(king_pos.rank_get())));
            break;

          case plugin::PieceType::KNIGHT:
            // Manhattan distance
            count += (abs(j - static_cast<int>(king_pos.file_get())) + abs(i - static_cast<int>(king_pos.rank_get())));
            break;

          default:
            break;
        }
      }
    }
  }
  return count;
}
