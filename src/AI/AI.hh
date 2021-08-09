#pragma once

#include "plugin/color.hh"
#include "plugin/piece-type.hh"
#include "plugin/position.hh"
#include "chessboard.hh"
#include "player.hh"

#include <cmath>
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
   std::string play_next_move(const std::string &received_move) override;

private:
   float estimate_time(int nb_possible_moves, int max_depth = -1);
   int piece_numbers(const ChessBoard &board, plugin::PieceType type, plugin::Color color);

   int minimax(int depth, plugin::Color playing_color, int A, int B);

   int evaluate(const ChessBoard &board);

   int count_isolated(plugin::Color color);
   int board_bonus_position(const ChessBoard &board);
   int evaluation_function(const ChessBoard &board);
   int get_piece_bonus_position(plugin::Color color, plugin::PieceType piece, const plugin::Position &pos);

   const plugin::Color opponent_color_;
   std::shared_ptr<Move> best_move_;
   ChessBoard board_;

   std::vector<ChessBoard *> temporary_history_board_;
   std::vector<ChessBoard::board_t> permanent_history_board_;

   int max_depth_ = 3;
   unsigned int fixed_board_ = 0;
   double c_ = 5 / std::pow(20, 3);

   int king_zone_attack(plugin::Position king_pos, std::experimental::optional<plugin::PieceType> piece_type, int value_of_attack, int i, int j);
   int pawn_shield(const ChessBoard &board, plugin::Position king_pos);

   const std::array<std::array<eval_cell_t, 8>, 8> pawn_weight_board =
       {
           0, 0, 0, 0, 0, 0, 0, 0,
           50, 50, 50, 50, 50, 50, 50, 50,
           10, 10, 20, 30, 30, 20, 10, 10,
           5, 5, 10, 25, 25, 10, 5, 5,
           0, 0, 0, 20, 20, 0, 0, 0,
           5, -5, -10, 0, 0, -10, -5, 5,
           5, 10, 10, -20, -20, 10, 10, 5,
           0, 0, 0, 0, 0, 0, 0, 0};

   const std::array<std::array<eval_cell_t, 8>, 8> knight_weight_board =
       {
           -50, -40, -30, -30, -30, -30, -40, -50,
           -40, -20, 0, 0, 0, 0, -20, -40,
           -30, 0, 10, 15, 15, 10, 0, -30,
           -30, 5, 15, 20, 20, 15, 5, -30,
           -30, 0, 15, 20, 20, 15, 0, -30,
           -30, 5, 10, 15, 15, 10, 5, -30,
           -40, -20, 0, 5, 5, 0, -20, -40,
           -50, -40, -30, -30, -30, -30, -40, -50};

   const std::array<std::array<eval_cell_t, 8>, 8> bishop_weight_board =
       {
           -20, -10, -10, -10, -10, -10, -10, -20,
           -10, 0, 0, 0, 0, 0, 0, -10,
           -10, 0, 5, 10, 10, 5, 0, -10,
           -10, 5, 5, 10, 10, 5, 5, -10,
           -10, 0, 10, 10, 10, 10, 0, -10,
           -10, 10, 10, 10, 10, 10, 10, -10,
           -10, 5, 0, 0, 0, 0, 5, -10,
           -20, -10, -10, -10, -10, -10, -10, -20};

   const std::array<std::array<eval_cell_t, 8>, 8> rook_weight_board =
       {
           0, 0, 0, 0, 0, 0, 0, 0,
           5, 10, 10, 10, 10, 10, 10, 5,
           -5, 0, 0, 0, 0, 0, 0, -5,
           -5, 0, 0, 0, 0, 0, 0, -5,
           -5, 0, 0, 0, 0, 0, 0, -5,
           -5, 0, 0, 0, 0, 0, 0, -5,
           -5, 0, 0, 0, 0, 0, 0, -5,
           0, 0, 0, 5, 5, 0, 0, 0};

   const std::array<std::array<eval_cell_t, 8>, 8> queen_weight_board =
       {
           -20, -10, -10, -5, -5, -10, -10, -20,
           -10, 0, 0, 0, 0, 0, 0, -10,
           -10, 0, 5, 5, 5, 5, 0, -10,
           -5, 0, 5, 5, 5, 5, 0, -5,
           0, 0, 5, 5, 5, 5, 0, -10,
           -10, 5, 5, 5, 5, 5, 0, -10,
           -10, 0, 5, 0, 0, 0, 0, -10,
           -20, -10, -10, -5, -5, -10, -10, -20};

   const std::array<std::array<eval_cell_t, 8>, 8> king_middle_weight_board =
       {
           -30, -40, -40, -50, -50, -40, -40, -30,
           -30, -40, -40, -50, -50, -40, -40, -30,
           -30, -40, -40, -50, -50, -40, -40, -30,
           -30, -40, -40, -50, -50, -40, -40, -30,
           -20, -30, -30, -40, -40, -30, -30, -20,
           -10, -20, -20, -20, -20, -20, -20, -10,
           20, 20, 0, 0, 0, 0, 20, 20,
           20, 30, 10, 0, 0, 10, 30, 20};

   const std::array<std::array<eval_cell_t, 8>, 8> king_end_weight_board =
       {
           -50, -40, -30, -20, -20, -30, -40, -50,
           -30, -20, -10, 0, 0, -10, -20, -30,
           -30, -10, 20, 30, 30, 20, -10, -30,
           -30, -10, 30, 40, 40, 30, -10, -30,
           -30, -10, 30, 40, 40, 30, -10, -30,
           -30, -10, 20, 30, 30, 20, -10, -30,
           -30, -30, 0, 0, 0, 0, -30, -30,
           -50, -30, -30, -30, -30, -30, -30, -50};

   const std::array<std::array<eval_cell_t, 8>, 8> center_manhattan_distance =
       {
           6, 5, 4, 3, 3, 4, 5, 6,
           5, 4, 3, 2, 2, 3, 4, 5,
           4, 3, 2, 1, 1, 2, 3, 4,
           3, 2, 1, 0, 0, 1, 2, 3,
           3, 2, 1, 0, 0, 1, 2, 3,
           4, 3, 2, 1, 1, 2, 3, 4,
           5, 4, 3, 2, 2, 3, 4, 5,
           6, 5, 4, 3, 3, 4, 5, 6};

   const std::array<eval_cell_t, 7> attack_weight =
       {
           0, 50, 75, 88, 94, 97, 99 // King Safety wikiprog
       };
};
