
#ifndef POSITION_H
#define POSITION_H

#define bb(pc) bitboard<pc>()

#include "bitboard.h"
#include "types.h"

#include <string>

inline std::string piece_to_char = "  PNBRQK  pnbrqk";

extern Bitboard bitboards[];
extern Piece board[];

inline StateInfo state_stack[MAX_PLY], *state_ptr = state_stack;

void do_move(Move m);
void undo_move(Move m);

inline Bitboard occupied_bb() { return bitboards[WHITE] | bitboards[BLACK]; }

namespace Position {

inline Color side_to_move;
  
void set(const std::string& fen);
std::string to_string();
std::string fen();
void do_commit(Move m);
  
template<Color Perspective>
uint8_t kingside_rights()
{
  return Perspective == WHITE
    ? state_ptr->castling_rights & 0b1000
    : state_ptr->castling_rights & 0b0010;
}
template<Color Perspective>
uint8_t queenside_rights()
{
  return Perspective == WHITE
    ? state_ptr->castling_rights & 0b0100
    : state_ptr->castling_rights & 0b0001;
}

inline Bitboard ep_bb() { return square_bb(state_ptr->ep_sq); }

inline bool white_to_move() { return side_to_move == WHITE; }
  
} // namespace Position

template<Piece P>
Bitboard bitboard() { return bitboards[P]; }

inline Piece piece_on(Square sq) { return board[sq]; }

inline PieceType piece_type_on(Square sq) { return type_of(board[sq]); }

#endif
