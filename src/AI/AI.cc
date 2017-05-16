#include "AI.hh"


Ai::Ai(plugin::Color ai_color) 
      : board_(), ai_color_(ai_color), opponent_color_(static_cast<plugin::Color>(!static_cast<bool>(ai_color_)))
{
}

std::string AI::play_next_move(std::string received_move)
{
  received_move = received_move;
  std::string input;
  input;
  //AI
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

int Ai::evaluate(const ChessBoard& board)
{
  int queens = 9 * (piece_numbers(board, plugin::PieceType::QUEEN, ai_color_) - piece_numbers(board, plugin::PieceType::QUEEN, opponent_color_));

  int rooks = 5 * (piece_numbers(board, plugin::PieceType::ROOK, ai_color_) - piece_numbers(board, plugin::PieceType::ROOK, opponent_color_));

  int bishops = piece_numbers(board, plugin::PieceType::BISHOP, ai_color_) - piece_numbers(board, plugin::PieceType::BISHOP, opponent_color_);

  int knights = piece_numbers(board, plugin::PieceType::KNIGHT, ai_color_) - piece_numbers(board, plugin::PieceType::KNIGHT, opponent_color_);

  int pawns = piece_numbers(board, plugin::PieceType::PAWN, ai_color_) - piece_numbers(board, plugin::PieceType::PAWN, opponent_color_);


  int doubled = count_doubled(board, plugin::Color::WHITE) - count_doubled(board, plugin::Color::BLACK);

  int isolated = count_isolated(board, plugin::Color::WHITE) - count_isolated(board, plugin::Color::BLACK);

  return queens + rooks + 3 *(bishops - knights) + pawns + 0.5 * (doubled + isolated); // backward
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

int Ai::piece_numbers(const ChessBoard& board, plugin::PieceType type,
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



int Ai::count_doubled(const ChessBoard& board, plugin::Color color)
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

int Ai::count_isolated(const ChessBoard& board, plugin::Color color)
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
