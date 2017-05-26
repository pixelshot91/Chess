#pragma once

#include "quiet-move.hh"
#include "plugin/color.hh"
#include "plugin/listener.hh"
#include "plugin/piece-type.hh"
#include "plugin/position.hh"
#include <experimental/optional>
#include <iomanip>
#include <iostream>
#include <vector>
#include <memory>

class ChessBoard
{
public:
  using cell_t = uint8_t;
  using board_t = std::array<std::array<cell_t, 8>, 8>;
  ChessBoard(std::vector<plugin::Listener*>);
  ChessBoard(const ChessBoard&);
  ChessBoard();

  bool three_fold_repetition();

  int update(std::shared_ptr<Move> move);
  void move_piece(plugin::Position start, plugin::Position end);
  short apply_move(const Move& move);
  void undo_move(const Move& move, short token);

  void set_square(plugin::Position position, cell_t value);
  cell_t get_square(plugin::Position position) const;
  inline cell_t get_opt(plugin::Position position, cell_t mask) const;
  
  const board_t& board_get() const;

  inline std::experimental::optional<plugin::PieceType>
  piecetype_get(plugin::Position position) const; /* {
    cell_t type_b = get_opt(position, 0b00000111);
    if (type_b != 0b00000111)
      return plugin::piecetype_array()[type_b];
    return std::experimental::nullopt;
  }*/
  bool has_moved(plugin::Position position) const;
  plugin::Color color_get(plugin::Position position) const;
  bool castleflag_get(plugin::Position position) const;
  bool is_attacked(plugin::Color color, plugin::Position) const;

  std::vector<std::shared_ptr<Move>> get_possible_actions(plugin::Color color) const;
  std::vector<std::shared_ptr<Move>> get_possible_actions(plugin::Color playing_color, plugin::Position pos) const;
  void push_move(std::vector<std::shared_ptr<Move>>& moves, QuietMove move) const;

  const std::shared_ptr<Move> last_move_get() const {
    return last_move_;
  }
  static plugin::Position initial_king_position(plugin::Color c);
  static plugin::Position initial_rook_position(plugin::Color c,
                                                bool king_side);
  static plugin::Position castling_king_end_position(plugin::Color color,
                                                     bool king_side);
  static plugin::Position castling_rook_end_position(plugin::Color color,
                                                     bool king_side);
  void print() const;
  void pretty_print() const;
  void animate(const Move& m) const;

  inline plugin::Position get_king_position(plugin::Color color) const;

private:
  board_t board_ = {
    0x82, 0x84, 0x83, 0x81, 0x80, 0x83, 0x84, 0x82, 0x85, 0x85, 0x85,
    0x85, 0x85, 0x85, 0x85, 0x85, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
    0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
    0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
    0x87, 0x87, 0x87, 0x87, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x02, 0x04, 0x03, 0x01, 0x00, 0x03, 0x04, 0x02};
  std::shared_ptr<Move> last_move_;
  std::vector<plugin::Listener*> listeners_;
  std::vector<board_t> previous_states_;
  unsigned char inactive_turn = 0;
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
inline ChessBoard::cell_t ChessBoard::get_opt(plugin::Position position,
    cell_t mask) const
{
  return get_square(position) & mask;
}

inline std::experimental::optional<plugin::PieceType>
ChessBoard::piecetype_get(plugin::Position position) const
{
  cell_t type_b = get_opt(position, 0b00000111);
  if (type_b != 0b00000111)
    return plugin::piecetype_array()[type_b];
  return std::experimental::nullopt;
}

inline plugin::Position ChessBoard::get_king_position(plugin::Color color) const
{
  for (auto i = 0; i < 8; i++)
  {
    for (auto j = 0; j < 8; j++)
    {
      auto p = plugin::Position(static_cast<plugin::File>(j),
          static_cast<plugin::Rank>(i));
      if (piecetype_get(p) != std::experimental::nullopt and piecetype_get(p).value() ==  plugin::PieceType::KING and color_get(p) == color) {
        return p;
      }
    }
  }
  throw std::invalid_argument("There is no king !");
}
