
#include "movegen.h"

#include "bitboard.h"
#include "position.h"

Move* make_moves(Move* list, Square from, Bitboard to) {
  for (;to; pop_lsb(to))
    *list++ = make_move(from, lsb(to));
  return list;
}

template<Direction D>
Move* make_pawn_moves(Move* list, Bitboard to_bb) {
  for (;to_bb; pop_lsb(to_bb)) {
    Square to = lsb(to_bb);
    *list++ = make_move(to - D, to);
  }
  return list;
}

template<Color Us>
Move* generate_moves(Move* list)
{
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
    return make_moves(list, ksq, king_attacks(ksq) & ~(seen_by_enemy | bb(Us)));
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

  list = make_pawn_moves<UpRight>(list, shift<UpRight>(pawns & (not_pinned | anti_diag(ksq))) & bb(Them) & checkmask);
  list = make_pawn_moves<UpLeft >(list, shift<UpLeft >(pawns & (not_pinned | main_diag(ksq))) & bb(Them) & checkmask);
  list = make_pawn_moves<Up     >(list, shift<Up     >(pawns & (not_pinned | file_bb  (ksq))) & empty    & checkmask);
  list = make_pawn_moves<Up2    >(list, shift<Up2    >(pawns & (not_pinned | file_bb  (ksq))) & e        & checkmask);

  if (Bitboard promotable = bb(FriendlyPawn) & Promote) {
    for (Bitboard b = shift<UpRight>(promotable & (not_pinned | anti_diag(ksq))) & bb(Them) & checkmask; b; pop_lsb(b)) {
      Square to = lsb(b);
      *list++ = make_move<PROMOTION>(to - UpRight, to);
    }
    for (Bitboard b = shift<UpLeft >(promotable & (not_pinned | main_diag(ksq))) & bb(Them) & checkmask; b; pop_lsb(b)) {
      Square to = lsb(b);
      *list++ = make_move<PROMOTION>(to - UpLeft, to);
    }
    for (Bitboard b = shift<Up>(promotable & not_pinned) & empty & checkmask; b; pop_lsb(b)) {
      Square to = lsb(b);
      *list++ = make_move<PROMOTION>(to - Up, to);
    }
  }

  if (Bitboard b = shift<UpRight>(bb(FriendlyPawn)) & Position::ep_bb() & EnemyEP) {
    Square to = lsb(b);
    *list++ = make_move<ENPASSANT>(to - UpRight, to);
    Bitboard o = occupied ^ (b | shift<-UpRight>(b) | shift<-Up>(b));
    list -= bool(bishop_attacks(ksq, o) & enemy_bishop_queen | rook_attacks(ksq, o) & enemy_rook_queen);
  }
  if (Bitboard b = shift<UpLeft >(bb(FriendlyPawn)) & Position::ep_bb() & EnemyEP) {
    Square to = lsb(b);
    *list++ = make_move<ENPASSANT>(to - UpLeft, to);
    Bitboard o = occupied ^ (b | shift<-UpLeft>(b) | shift<-Up>(b));
    list -= bool(bishop_attacks(ksq, o) & enemy_bishop_queen | rook_attacks(ksq, o) & enemy_rook_queen);
  }

  Bitboard friendly_rook_queen   = bb(FriendlyQueen) | bb(FriendlyRook);
  Bitboard friendly_bishop_queen = bb(FriendlyQueen) | bb(FriendlyBishop);

  Bitboard legal = checkmask &~ bb(Us);

  for (Bitboard b = bb(FriendlyKnight) & not_pinned; b; pop_lsb(b)) {
    Square from = lsb(b);
    list = make_moves(list, from, knight_attacks(from) & legal);
  }
  for (Bitboard b = friendly_bishop_queen & not_pinned; b; pop_lsb(b)) {
    Square from = lsb(b);
    list = make_moves(list, from, bishop_attacks(from, occupied) & legal);
  }
  for (Bitboard b = friendly_bishop_queen & pinned; b; pop_lsb(b)) {
    Square from = lsb(b);
    list = make_moves(list, from, bishop_attacks(from, occupied) & legal & pin_mask(ksq, from));
  }
  for (Bitboard b = friendly_rook_queen & not_pinned; b; pop_lsb(b)) {
    Square from = lsb(b);
    list = make_moves(list, from, rook_attacks(from, occupied) & legal);
  }
  for (Bitboard b = friendly_rook_queen & pinned; b; pop_lsb(b)) {
    Square from = lsb(b);
    list = make_moves(list, from, rook_attacks(from, occupied) & legal & pin_mask(ksq, from));
  }

  list = make_moves(list, ksq, king_attacks(ksq) & ~(seen_by_enemy | bb(Us)));

  if (~checkmask) return list;

  constexpr Bitboard KingBan  = Us == WHITE ? square_bb(F1, G1)     : square_bb(F8, G8);
  constexpr Bitboard QueenOcc = Us == WHITE ? square_bb(B1, C1, D1) : square_bb(B8, C8, D8);
  constexpr Bitboard QueenAtk = Us == WHITE ? square_bb(C1, D1)     : square_bb(C8, D8);
  constexpr Bitboard KingKey  = Us == WHITE ? 0b1000                : 0b0010;
  constexpr Bitboard QueenKey = Us == WHITE ? 0b0100                : 0b0001;
  constexpr Move     SCASTLE  = Us == WHITE ? W_SCASTLE             : B_SCASTLE;
  constexpr Move     LCASTLE  = Us == WHITE ? W_LCASTLE             : B_LCASTLE;
            
  *list = SCASTLE;
  list += !(((occupied | seen_by_enemy) & KingBan | Position::kingside_rights<Us>()) ^ KingKey);

  *list = LCASTLE;
  list += !((occupied & QueenOcc | seen_by_enemy & QueenAtk | Position::queenside_rights<Us>()) ^ QueenKey);

  return list;
}

MoveList::MoveList() :
  last(Position::white_to_move() ? generate_moves<WHITE>(moves) : generate_moves<BLACK>(moves)) {}

