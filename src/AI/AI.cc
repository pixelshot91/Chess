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


std::pair<plugin::Move,int> AI::minimax(int depth , plugin::Color playing_color)
{
    int bestValue = -9999;
    std::pair<plugin::Move,int> bestMove_value;
    std::pair<plugin::Move,int> move_value;

    for( i = 0; i < 8; i++ )
      for( j = 0; j < 8; j++ )
      {
        Position position = Position(static_cast<File> i,
            static_cast<Rank>j)i;
        if(board_.piecetype_get(position) != std::experimental::nullopt;
&& board_.color_get(position) == playing_color )

          std::vector<Move> moves = bord_.get_possible_actions(position);
            for(auto move : moves)
            {
              //Moving
              apply_move(move);
              move_value = minimax(depth-1 , !playing_color);

              //Savebest move
              if((move_value.first > bestMove_value.second )
                  == (playing_color == AI_color))
                bestMove_value = value;
              //Undo Move
        }
      }
    return bestMove_value;
}


