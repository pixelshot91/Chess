#pragma once

#include <vector>
#include <iostream>
#include <iomanip>
#include <experimental/optional>
#include "plugin/position.hh"
#include "plugin/piece-type.hh"
#include "plugin/color.hh"
#include "history.hh"
#include "piece/piece.hh"
#include "plugin/listener.hh"

class ChessBoard
{
  public:
    using cell_t = uint8_t;
    ChessBoard(std::vector<plugin::Listener*>);

    int update(Move& move);
    void move_piece(plugin::Position start, plugin::Position end);
    ChessBoard apply_move(ChessBoard board, Move& move);

    void set_square(plugin::Position position, cell_t value);
    cell_t get_square(plugin::Position position) const;
    cell_t get_opt(plugin::Position position, cell_t mask) const;

    std::experimental::optional<plugin::PieceType> piecetype_get(plugin::Position position) const;
    bool has_moved(plugin::Position position) const;
    plugin::Color color_get(plugin::Position position) const;
    bool castleflag_get(plugin::Position position) const;
    bool is_attacked(plugin::Color color, plugin::Position) const;

    std::vector<Piece*> get_piece(plugin::Color color);

    History history_get() const;

    static plugin::Position initial_king_position(plugin::Color c);
    static plugin::Position initial_rook_position(plugin::Color c, bool king_side);
    static plugin::Position castling_king_end_position(plugin::Color color, bool king_side);
    static plugin::Position castling_rook_end_position(plugin::Color color, bool king_side);
    //bool moved(plugin::Position);
    //Color color(plugin::Position);
    void print() const;

    plugin::Position get_king_position(plugin::Color color);

  private:
    std::array<std::array<cell_t, 8>, 8> board_=
    {
      0x82,0x84,0x83,0x81,0x80,0x83,0x84,0x82,
      0x85,0x85,0x85,0x85,0x85,0x85,0x85,0x85,
      0x87,0x87,0x87,0x87,0x87,0x87,0x87,0x87,
      0x87,0x87,0x87,0x87,0x87,0x87,0x87,0x87,
      0x87,0x87,0x87,0x87,0x87,0x87,0x87,0x87,
      0x87,0x87,0x87,0x87,0x87,0x87,0x87,0x87,
      0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,
      0x02,0x04,0x03,0x01,0x00,0x03,0x04,0x02
    };
    History history_;
    std::vector<plugin::Listener*> listeners_;
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
