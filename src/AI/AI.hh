#pragma once

#include "../history.hh"
#include "../piece/piece.hh"
#include "../plugin/color.hh"
#include "../plugin/listener.hh"
#include "../plugin/piece-type.hh"
#include "../plugin/position.hh"
#include "../chessboard.hh"
#include "eval_boards.hh"


#include <experimental/optional>
#include <iomanip>
#include <iostream>
#include <vector>
#include <utility>

class Ai// : public Player
{
  public:
    Ai(plugin::Color ai_color);

  public:
    int piece_numbers(const ChessBoard& board, plugin::PieceType type, plugin::Color color);

    std::pair<Move, int> minimax(const ChessBoard& board, int depth, plugin::Color playing_color);

    int evaluate(const ChessBoard& board);

    int count_isolated(const ChessBoard& board, plugin::Color color);
    int count_doubled(const ChessBoard& board, plugin::Color color);
    int count_backward(const ChessBoard& board, plugin::Color color);
    int king_tropism(const ChessBoard& board);
    int board_material(const ChessBoard& board)
    int board_bonus_position(const ChessBoard& board)
    int get_piece_bonus_position(plugin::PieceType piece, int i, int j)

  private :
    std::string play_next_move(const std::string received_move) override;
    ChessBoard board_;
    EvalBoard evalboard_;
    const plugin::Color ai_color_;
    const plugin::Color opponent_color_;
    static std::vector<ChessBoard*> history_board_;
};
