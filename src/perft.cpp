
#include "movegen.h"
#include "position.h"
#include "uci.h"

#include <chrono>
#include <iomanip>
#include <iostream>

uint64_t nodes;

template<Color Us>
int bulk()
{
  int moves = 0;
  
  constexpr Color Them           = !Us;
  constexpr Piece FriendlyPawn   = make_piece(Us, PAWN);
  constexpr Piece EnemyPawn      = make_piece(Them, PAWN);
  constexpr Piece FriendlyKnight = make_piece(Us, KNIGHT);
  constexpr Piece EnemyKnight    = make_piece(Them, KNIGHT);
  constexpr Piece FriendlyBishop = make_piece(Us, BISHOP);
  constexpr Piece EnemyBishop    = make_piece(Them, BISHOP);
  constexpr Piece FriendlyRook   = make_piece(Us, ROOK);
  constexpr Piece EnemyRook      = make_piece(Them, ROOK);
  constexpr Piece FriendlyQueen  = make_piece(Us, QUEEN);
  constexpr Piece EnemyQueen     = make_piece(Them, QUEEN);
  constexpr Piece FriendlyKing   = make_piece(Us, KING);
  constexpr Piece EnemyKing      = make_piece(Them, KING);

  Bitboard enemy_rook_queen   = bb(EnemyQueen) | bb(EnemyRook);
  Bitboard enemy_bishop_queen = bb(EnemyQueen) | bb(EnemyBishop);
  Square   ksq                = lsb(bb(FriendlyKing));
  Bitboard occupied           = occupied_bb() ^ square_bb(ksq);

  Bitboard seen_by_enemy = pawn_attacks<Them>(bb(EnemyPawn)) | king_attacks(lsb(bb(EnemyKing)));
  for (Bitboard b = bb(EnemyKnight); b; pop_lsb(b))
    seen_by_enemy |= knight_attacks(lsb(b));
  for (Bitboard b = enemy_bishop_queen; b; pop_lsb(b))
    seen_by_enemy |= bishop_attacks(lsb(b), occupied);
  for (Bitboard b = enemy_rook_queen; b; pop_lsb(b))
    seen_by_enemy |= rook_attacks(lsb(b), occupied);

  toggle_square(occupied, ksq);

  Bitboard checkmask = knight_attacks(ksq) & bb(EnemyKnight) | pawn_attacks<Us>(ksq) & bb(EnemyPawn);
  for (Bitboard checkers = bishop_attacks(ksq, occupied) & enemy_bishop_queen | rook_attacks(ksq, occupied) & enemy_rook_queen; checkers; pop_lsb(checkers))
    checkmask |= check_ray(ksq, lsb(checkers));
  if (more_than_one(checkmask & double_check(ksq)))
    return popcount(king_attacks(ksq) & ~(seen_by_enemy | bb(Us)));
  if (checkmask == 0)
    checkmask = ALL_SQUARES;

  Bitboard pinned = 0;
  for (Bitboard pinners = bishop_xray(ksq, occupied) & enemy_bishop_queen | rook_xray(ksq, occupied) & enemy_rook_queen; pinners; pop_lsb(pinners))
    pinned |= check_ray(ksq, lsb(pinners));

  constexpr Direction Up        = Us == WHITE ? NORTH      : SOUTH;
  constexpr Direction Up2       = Us == WHITE ? NORTHNORTH : SOUTHSOUTH;
  constexpr Direction UpRight   = Us == WHITE ? NORTH_EAST : SOUTH_WEST;
  constexpr Direction UpLeft    = Us == WHITE ? NORTH_WEST : SOUTH_EAST;
  constexpr Bitboard  FriendEP  = Us == WHITE ? RANK_3     : RANK_6;
  constexpr Bitboard  EnemyEP   = Us == WHITE ? RANK_6     : RANK_3;
  constexpr Bitboard  Start     = Us == WHITE ? RANK_2     : RANK_7;
  constexpr Bitboard  Promote   = Us == WHITE ? RANK_7     : RANK_2;
  constexpr Bitboard  NoPromote = ~Promote;

  Bitboard empty      = ~occupied;
  Bitboard not_pinned = ~pinned;
  Bitboard pawns      = bb(FriendlyPawn) & NoPromote;
  Bitboard e          = shift<Up>(FriendEP & empty) & empty;

  moves += popcount(shift<UpRight>(pawns & (not_pinned | anti_diag(ksq))) & bb(Them) & checkmask);
  moves += popcount(shift<UpLeft >(pawns & (not_pinned | main_diag(ksq))) & bb(Them) & checkmask);
  moves += popcount(shift<Up     >(pawns & (not_pinned | file_bb  (ksq))) & empty    & checkmask);
  moves += popcount(shift<Up2    >(pawns & (not_pinned | file_bb  (ksq))) & e        & checkmask);

  if (Bitboard promotable = bb(FriendlyPawn) & Promote) {
    moves += popcount(shift<UpRight>(promotable & (not_pinned | anti_diag(ksq))) & bb(Them) & checkmask);
    moves += popcount(shift<UpLeft >(promotable & (not_pinned | main_diag(ksq))) & bb(Them) & checkmask);
    moves += popcount(shift<Up>(promotable & not_pinned) & empty & checkmask);
  }

  if (Bitboard b = shift<UpRight>(bb(FriendlyPawn)) & Position::ep_bb() & EnemyEP) {
    moves++;
    Bitboard o = occupied ^ (b | shift<-UpRight>(b) | shift<-Up>(b));
    moves -= bool(bishop_attacks(ksq, o) & enemy_bishop_queen | rook_attacks(ksq, o) & enemy_rook_queen);
  }
  if (Bitboard b = shift<UpLeft >(bb(FriendlyPawn)) & Position::ep_bb() & EnemyEP) {
    moves++;
    Bitboard o = occupied ^ (b | shift<-UpLeft>(b) | shift<-Up>(b));
    moves -= bool(bishop_attacks(ksq, o) & enemy_bishop_queen | rook_attacks(ksq, o) & enemy_rook_queen);
  }

  Bitboard friendly_rook_queen   = bb(FriendlyQueen) | bb(FriendlyRook);
  Bitboard friendly_bishop_queen = bb(FriendlyQueen) | bb(FriendlyBishop);

  Bitboard legal = checkmask &~ bb(Us);

  for (Bitboard b = bb(FriendlyKnight) & not_pinned; b; pop_lsb(b)) {
    Square from = lsb(b);
    moves += popcount(knight_attacks(from) & legal);
  }
  for (Bitboard b = friendly_bishop_queen & not_pinned; b; pop_lsb(b)) {
    Square from = lsb(b);
    moves += popcount(bishop_attacks(from, occupied) & legal);
  }
  for (Bitboard b = friendly_bishop_queen & pinned; b; pop_lsb(b)) {
    Square from = lsb(b);
    moves += popcount(bishop_attacks(from, occupied) & legal & pin_mask(ksq, from));
  }
  for (Bitboard b = friendly_rook_queen & not_pinned; b; pop_lsb(b)) {
    Square from = lsb(b);
    moves += popcount(rook_attacks(from, occupied) & legal);
  }
  for (Bitboard b = friendly_rook_queen & pinned; b; pop_lsb(b)) {
    Square from = lsb(b);
    moves += popcount(rook_attacks(from, occupied) & legal & pin_mask(ksq, from));
  }

  moves += popcount(king_attacks(ksq) & ~(seen_by_enemy | bb(Us)));

  if (~checkmask) return moves;

  constexpr Bitboard KingBan  = Us == WHITE ? square_bb(F1, G1)     : square_bb(F8, G8);
  constexpr Bitboard QueenOcc = Us == WHITE ? square_bb(B1, C1, D1) : square_bb(B8, C8, D8);
  constexpr Bitboard QueenAtk = Us == WHITE ? square_bb(C1, D1)     : square_bb(C8, D8);
  constexpr Bitboard KingKey  = Us == WHITE ? 0b1000                : 0b0010;
  constexpr Bitboard QueenKey = Us == WHITE ? 0b0100                : 0b0001;
  constexpr Move     SCASTLE  = Us == WHITE ? W_SCASTLE             : B_SCASTLE;
  constexpr Move     LCASTLE  = Us == WHITE ? W_LCASTLE             : B_LCASTLE;
            
  moves += !(((occupied | seen_by_enemy) & KingBan | Position::kingside_rights<Us>()) ^ KingKey);

  moves += !((occupied & QueenOcc | seen_by_enemy & QueenAtk | Position::queenside_rights<Us>()) ^ QueenKey);

  return moves;
}

int bulk() {
  return Position::white_to_move() ? bulk<WHITE>() : bulk<BLACK>();
}

void expand(int depth) {
  
  if (depth == 0) {
    nodes++;
    return;
  }

  if (depth == 1) {
    nodes += bulk();
    return;
  }
  
  MoveList moves;
  
  // if (depth == 1) {
  //   nodes += moves.length();
  //   return;
  // }
  
  for (Move m : moves) {
    do_move(m);
    expand(depth - 1);
    undo_move(m);
  }
}

void Perft::go(int depth) {
  uint64_t total_nodes = 0;
  MoveList moves;
  for (Move m : moves) {
    nodes = 0;
    std::cout << move_to_uci(m) << ": ";
    do_move(m);
    expand(depth - 1);
    undo_move(m);
    std::cout << nodes << "\n";
    total_nodes += nodes;
  }
  std::cout << "\nnodes searched: " << total_nodes << "\n\n";
}

void Perft::bench(int depth) {
  std::cout << "depth  nodes       ms      nodes\\second\n" << std::left;
  MoveList moves;
  for (int d = 1; d <= depth; d++) {
    nodes = 0;
    auto start = curr_time_millis();
    for (Move m : moves) {
      do_move(m);
      expand(d - 1);
      undo_move(m);
    }
    auto delta = curr_time_millis() - start;
    std::cout << std::setw(7) << d << std::setw(12) << nodes << std::setw(8) << delta << std::max(0, int(double(nodes) / (double(delta) / 1000.0))) << "\n";
  }
  std::cout << "\n";
}
