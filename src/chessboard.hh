#pragma once

#include <vector>
#include <iostream>
#include <iomanip>
#include <experimental/optional>
#include "rule-checker.hh"
#include "plugin/position.hh"
#include "plugin/piece-type.hh"

class ChessBoard
{
  public:
    using cell_t = uint8_t;
    ChessBoard();
    cell_t get_square(plugin::Position position);
    cell_t get_opt(plugin::Position position, cell_t mask);

    std::experimental::optional<plugin::PieceType> piece_type_at(plugin::Position position);
    bool moved_at(plugin::Position position);
    bool color_at(plugin::Position position);
    bool castleflag_at(plugin::Position position);
    bool is_attacked(Color color, plugin::Position);

    static plugin::Position initial_king_position(plugin::Color c);
    static plugin::Position initial_rook_position(plugin::Color c, bool king_side);
    //bool moved(plugin::Position);
    //Color color(plugin::Position);
    void print_board();
  private:
    std::array<std::array<cell_t, 8>, 8> board_;
};

/*
;     Bit 7 -- Color of the piece
;     1 -- Black
;     0 -- White
;     Bit 6 -- Not used
;     Bit 5 -- Not used
;     Bit 4 --Castle flag for Kings only
;     Bit 3 -- Piece has moved flag
;     Bits 2-0 Piece type
;         0 -- King
;         1 -- Queen
;         2 -- Rook
;         3 -- Bishop
;         4 -- Knight
;         5 -- Pawn
;         6 -- Not used
;         7 -- Empty Square
*/
