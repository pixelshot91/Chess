#pragma once

#include "plugin/color.hh"
#include "plugin/piece-type.hh"
#include "plugin/position.hh"
#include "chessboard.hh"
#include "player.hh"


#include <experimental/optional>
#include <iomanip>
#include <memory>
#include <iostream>
#include <vector>
#include <utility>

class AI : public Player
{
  public:
    using eval_cell_t = int;
    AI(plugin::Color ai_color);
    std::string play_next_move(const std::string& received_move) override;

  private :
    int piece_numbers(const ChessBoard& board, plugin::PieceType type, plugin::Color color);

    int minimax(int depth, plugin::Color playing_color,int A,int B);

    int evaluate(const ChessBoard& board);

    int count_isolated(const ChessBoard& board, plugin::Color color);
    int count_doubled(const ChessBoard& board, plugin::Color color);
    //int count_backward(const ChessBoard& board, plugin::Color color);
    int king_tropism(const ChessBoard& board, plugin::Color color);
    int board_material(const ChessBoard& board);
    int board_bonus_position(const ChessBoard& board);
    int get_piece_bonus_position(plugin::PieceType piece, int i, int j);

    const plugin::Color opponent_color_;
    std::shared_ptr<Move> best_move_;
    ChessBoard board_;

    std::vector<ChessBoard*> history_board_;
    int max_depth_ = 3;

    const std::array<std::array<eval_cell_t, 8>, 8> pawn_weight_board =
    {
       0, 0, 0, 0, 0, 0, 0, 0,
       50, 50, 50, 50, 50, 50, 50, 50,
       10, 10, 20, 30, 30, 20, 10, 10,
       5, 5, 10, 25, 25, 10, 5, 5,
       0, 0, 0, 20, 20, 0, 0, 0,
       5, -5, -10, 0, 0, -10, -5, 5,
       5, 10, 10, -20, -20, 10, 10, 5,
       0, 0, 0, 0, 0, 0, 0, 0
    };

    const std::array<std::array<eval_cell_t, 8>, 8> knight_weight_board =
    {
       -50, -40, -30, -30, -30, -30, -40, -50,
       -40, -20, 0, 0, 0, 0, -20, -40,
       -30, 0, 10, 15, 15, 10, 0, -30,
       -30, 5, 15, 20, 20, 15, 5, -30,
       -30, 0, 15, 20, 20, 15, 0, -30,
       -30, 5, 10, 15, 15, 10, 5, -30,
       -40, -20, 0, 5, 5, 0, -20, -40,
       -50, -40, -30, -30, -30, -30, -40, -50
    };


    const std::array<std::array<eval_cell_t, 8>, 8> bishop_weight_board =
    {
       -20, -10, -10, -10, -10, -10, -10, -20,
       -10, 0, 0, 0, 0, 0, 0, -10,
       -10, 0, 5, 10, 10, 5, 0, -10,
       -10, 5, 5, 10, 10, 5, 5, -10,
       -10, 0, 10, 10, 10, 10, 0, -10,
       -10, 10, 10, 10, 10, 10, 10, -10,
       -10, 5, 0, 0, 0, 0, 5, -10,
       -20, -10, -10, -10, -10, -10, -10, -20
    };


    const std::array<std::array<eval_cell_t, 8>, 8> rook_weight_board =
    {
       0, 0, 0, 0, 0, 0, 0, 0,
       5, 10, 10, 10, 10, 10, 10, 5,
       -5, 0, 0, 0, 0, 0, 0, -5,
       -5, 0, 0, 0, 0, 0, 0, -5,
       -5, 0, 0, 0, 0, 0, 0, -5,
       -5, 0, 0, 0, 0, 0, 0, -5,
       -5, 0, 0, 0, 0, 0, 0, -5,
       0, 0, 0, 5, 5, 0, 0, 0
    };

    const std::array<std::array<eval_cell_t, 8>, 8> queen_weight_board =
    {
       -20, -10, -10, -5, -5, -10, -10, -20,
       -10, 0, 0, 0, 0, 0, 0, -10,
       -10, 0, 5, 5, 5, 5, 0, -10,
       -5, 0, 5, 5, 5, 5, 0, -5,
       0, 0, 5, 5, 5, 5, 0, -10,
       -10, 5, 5, 5, 5, 5, 0, -10,
       -10, 0, 5, 0, 0, 0, 0, -10,
       -20, -10, -10, -5, -5, -10, -10, -20
    };

    const std::array<std::array<eval_cell_t, 8>, 8> king_middle_weight_board =
    {
       -30, -40, -40, -50, -50, -40, -40, -30,
       -30, -40, -40, -50, -50, -40, -40, -30,
       -30, -40, -40, -50, -50, -40, -40, -30,
       -30, -40, -40, -50, -50, -40, -40, -30,
       -20, -30, -30, -40, -40, -30, -30, -20,
       -10, -20, -20, -20, -20, -20, -20, -10,
       20, 20, 0, 0, 0, 0, 20, 20,
       20, 30, 10, 0, 0, 10, 30, 20
    };

    const std::array<std::array<eval_cell_t, 8>, 8> king_end_weight_board =
    {
       -50, -40, -30, -20, -20, -30, -40, -50,
       -30, -20, -10, 0, 0, -10, -20, -30,
       -30, -10, 20, 30, 30, 20, -10, -30,
       -30, -10, 30, 40, 40, 30, -10, -30,
       -30, -10, 30, 40, 40, 30, -10, -30,
       -30, -10, 20, 30, 30, 20, -10, -30,
       -30, -30, 0, 0, 0, 0, -30, -30,
       -50, -30, -30, -30, -30, -30, -30, -50
    };

};
