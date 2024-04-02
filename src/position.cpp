
#include "position.h"

#include <cstring>
#include <sstream>

Bitboard bitboards[B_KING + 1];
Piece board[SQUARE_NB];

void Position::set(const std::string& fen) {

  std::memset(board, NO_PIECE, 64 * sizeof(Piece));
  std::memset(bitboards, 0, 16 * sizeof(Bitboard));

  uint8_t token;
  Square sq = A8;
  size_t piece, idx;

  std::istringstream iss(fen);
  iss >> std::noskipws;

  while (iss >> token) {
    if (std::isspace(token))
      break;
    if (std::isdigit(token))
      sq -= token - '0';
    else if ((piece = piece_to_char.find(token)) != std::string::npos) {
      board[sq] = piece;
      bitboards[piece] ^= square_bb(sq);
      bitboards[color_of(piece)] ^= square_bb(sq);
      sq--;
    }
  }

  iss >> std::skipws;
  iss >> token;
  side_to_move = token == 'w' ? WHITE : BLACK;

  state_ptr->castling_rights = 0;
  while (iss >> token)
    if ((idx = std::string("qkQK").find(token)) != std::string::npos)
      state_ptr->castling_rights ^= 1 << idx;
}

std::string Position::fen() {
  std::stringstream fen;
  for (int rank = 7; rank >= 0; rank--) {
    for (int file = 7; file >= 0; file--) {
      Piece pc = piece_on(rank * 8 + file);
      if (pc)
        fen << piece_to_char[pc];
      else {
        int empty_squares = 0, f;
        for (f = file; f >= 0 && !piece_on(rank * 8 + f); f--)
          empty_squares++;
        fen << empty_squares;
        file = f + 1;
      }
    }
    if (rank)
      fen << "/";
  }
  fen << " " << "wb"[side_to_move] << " ";
  if (!state_ptr->castling_rights)
    fen << "-";
  else {
    if (kingside_rights <WHITE>()) fen << "K";
    if (queenside_rights<WHITE>()) fen << "Q";
    if (kingside_rights <BLACK>()) fen << "k";
    if (queenside_rights<BLACK>()) fen << "q";
  }                                fen << " - 0 1";
  return fen.str();
}

std::string Position::to_string() {
  std::stringstream ss;
  ss << "\n+---+---+---+---+---+---+---+---+\n";
  for (Square sq = A8; sq >= H1; sq--) {
    ss << "| " << "  PNBRQK  pnbrqk"[board[sq]] << " ";
    if (sq % 8 == 0)
      ss << "| " << (sq / 8 + 1) << "\n+---+---+---+---+---+---+---+---+\n";
  }
  ss << "  a   b   c   d   e   f   g   h\n\n" << fen() << "\n\n";
  return ss.str();
}

void Position::do_commit(Move m) {
  do_move(m);
  state_stack[0] = *state_ptr;
  state_ptr = state_stack;
}

template<MoveType Type>
void update_castling_rights() {

  Color them = Position::side_to_move, us = !them;

  uint8_t ClearRights = us == WHITE ? 0b0011 : 0b1100;

  Bitboard FKingStart = square_bb(us == WHITE ? E1 : E8);
  Bitboard EKingStart = square_bb(us == WHITE ? E8 : E1);
  Bitboard FRookStart = us == WHITE ? square_bb(A1, H1) : square_bb(A8, H8);

  Piece FKing     = make_piece(us  , KING);
  Piece FRook     = make_piece(us  , ROOK);
  Piece EnemyRook = make_piece(them, ROOK);
  
  if constexpr (Type == NORMAL)
    state_ptr->castling_rights &=
      castle_mask(bitboards[FKing] & FKingStart | bitboards[FRook] & FRookStart | bitboards[EnemyRook] | EKingStart);

  else if constexpr (Type == PROMOTION)
    state_ptr->castling_rights &= castle_mask(bitboards[EnemyRook] | FKingStart | FRookStart | EKingStart);

  else if constexpr (Type == SHORTCASTLE || Type == LONGCASTLE)
    state_ptr->castling_rights &= ClearRights;

}

void do_move(Move m) {

  Color us = Position::side_to_move, them = !us;

  Position::side_to_move = !Position::side_to_move;

  Piece Pawn  = make_piece(us, PAWN);
  Piece Rook  = make_piece(us, ROOK);
  Piece Queen = make_piece(us, QUEEN);
  Piece King  = make_piece(us, KING);

  Direction Up  = us == WHITE ? NORTH : SOUTH;
  Direction Up2 = Up * 2;

  Square from = from_sq(m);
  Square to   = to_sq(m);

  memcpy(state_ptr + 1, state_ptr, sizeof(StateInfo));
  state_ptr++;
  state_ptr->captured = piece_on(to);
  state_ptr->ep_sq = (from + Up) * !(to - from ^ Up2 | piece_type_on(from) ^ PAWN);

  Bitboard zero_to = ~square_bb(to);
  Bitboard from_to =  square_bb(from, to);

  switch (type_of(m)) {
  case NORMAL:
    bitboards[board[to]] &= zero_to;
    bitboards[them] &= zero_to;
    bitboards[board[from]] ^= from_to;
    bitboards[us] ^= from_to;
    board[to] = board[from];
    board[from] = NO_PIECE;
    update_castling_rights<NORMAL>();
    return;
  case PROMOTION:
    bitboards[board[to]] &= zero_to;
    bitboards[them] &= zero_to;
    bitboards[Pawn] ^= square_bb(from);
    bitboards[Queen] ^= ~zero_to;
    bitboards[us] ^= from_to;
    board[to] = Queen;
    board[from] = NO_PIECE;
    update_castling_rights<PROMOTION>();
    return;
  case SHORTCASTLE:
  {
    Square king_from = us == WHITE ? E1 : E8;
    Square king_to   = us == WHITE ? G1 : G8;
    Square rook_from = us == WHITE ? H1 : H8;
    Square rook_to   = us == WHITE ? F1 : F8;

    Bitboard king_from_to = square_bb(king_from, king_to);
    Bitboard rook_from_to = square_bb(rook_from, rook_to);

    bitboards[King] ^= king_from_to;
    bitboards[Rook] ^= rook_from_to;
    bitboards[us] ^= king_from_to ^ rook_from_to;
    board[king_from] = NO_PIECE;
    board[rook_from] = NO_PIECE;
    board[king_to] = King;
    board[rook_to] = Rook;
    update_castling_rights<SHORTCASTLE>();
  }
    return;
  case LONGCASTLE:
  {
    Square king_from = us == WHITE ? E1 : E8;
    Square king_to   = us == WHITE ? C1 : C8;
    Square rook_from = us == WHITE ? A1 : A8;
    Square rook_to   = us == WHITE ? D1 : D8;

    Bitboard king_from_to = square_bb(king_from, king_to);
    Bitboard rook_from_to = square_bb(rook_from, rook_to);

    bitboards[King] ^= king_from_to;
    bitboards[Rook] ^= rook_from_to;
    bitboards[us] ^= king_from_to ^ rook_from_to;
    board[king_from] = NO_PIECE;
    board[rook_from] = NO_PIECE;
    board[king_to] = King;
    board[rook_to] = Rook;
    update_castling_rights<LONGCASTLE>();
  }
  return;
  case ENPASSANT:
    Piece EPawn = make_piece(them, PAWN);
    Square capsq = to + (us == WHITE ? SOUTH : NORTH);
    bitboards[Pawn] ^= from_to;
    bitboards[EPawn] ^= square_bb(capsq);
    bitboards[us] ^= from_to;
    bitboards[them] ^= square_bb(capsq);
    board[from] = NO_PIECE;
    board[to] = Pawn;
    board[capsq] = NO_PIECE;
    return;
  }
}

void undo_move(Move m) {

  Position::side_to_move = !Position::side_to_move;
  
  Color us = Position::side_to_move, them = !us;
  
  Piece Pawn  = make_piece(us, PAWN);
  Piece Rook  = make_piece(us, ROOK);
  Piece Queen = make_piece(us, QUEEN);
  Piece King  = make_piece(us, KING);

  Piece captured = state_ptr->captured;
  state_ptr--;

  Square from = from_sq(m);
  Square to   = to_sq(m);

  Bitboard to_bb      = square_bb(to);
  Bitboard from_to    = square_bb(from, to);
  Bitboard capture_bb = to_bb * bool(captured);

  switch (type_of(m)) {
  case NORMAL:
    bitboards[board[to]] ^= from_to;
    bitboards[us] ^= from_to;
    bitboards[captured] ^= capture_bb;
    bitboards[them] ^= capture_bb;
    board[from] = board[to];
    board[to] = captured;
    return;
  case PROMOTION:
    bitboards[Queen] ^= to_bb;
    bitboards[Pawn] ^= square_bb(from);
    bitboards[us] ^= from_to;
    bitboards[captured] ^= capture_bb;
    bitboards[them] ^= capture_bb;
    board[to] = captured;
    board[from] = Pawn;
    return;
  case SHORTCASTLE:
  {
    Square king_from = us == WHITE ? E1 : E8;
    Square king_to   = us == WHITE ? G1 : G8;
    Square rook_from = us == WHITE ? H1 : H8;
    Square rook_to   = us == WHITE ? F1 : F8;

    Bitboard king_from_to = square_bb(king_from, king_to);
    Bitboard rook_from_to = square_bb(rook_from, rook_to);

    bitboards[King] ^= king_from_to;
    bitboards[Rook] ^= rook_from_to;
    bitboards[us] ^= king_from_to ^ rook_from_to;
    board[king_to] = NO_PIECE;
    board[rook_to] = NO_PIECE;
    board[king_from] = King;
    board[rook_from] = Rook;
  }
  return;
  case LONGCASTLE:
  {
    Square king_from = us == WHITE ? E1 : E8;
    Square king_to   = us == WHITE ? C1 : C8;
    Square rook_from = us == WHITE ? A1 : A8;
    Square rook_to   = us == WHITE ? D1 : D8;

    Bitboard king_from_to = square_bb(king_from, king_to);
    Bitboard rook_from_to = square_bb(rook_from, rook_to);

    bitboards[King] ^= king_from_to;
    bitboards[Rook] ^= rook_from_to;
    bitboards[us] ^= king_from_to ^ rook_from_to;
    board[king_to] = NO_PIECE;
    board[rook_to] = NO_PIECE;
    board[king_from] = King;
    board[rook_from] = Rook;
  }
  return;
  case ENPASSANT:
    Piece EPawn = make_piece(them, PAWN);
    Square capsq = to + (us == WHITE ? SOUTH : NORTH);

    bitboards[Pawn] ^= from_to;
    bitboards[us] ^= from_to;
    bitboards[EPawn] ^= square_bb(capsq);
    bitboards[them] ^= square_bb(capsq);
    board[to] = NO_PIECE;
    board[from] = Pawn;
    board[capsq] = EPawn;
    return;
  }
}

