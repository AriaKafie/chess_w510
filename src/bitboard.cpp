
#include "bitboard.h"

#include <cstring>
#include <chrono>
#include <initializer_list>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

bool has_duplicates(const std::vector<int>& keys);

Bitboard slider_attacks(PieceType pt, Square sq, Bitboard occupied) {

  Direction rook_dir[4] = { NORTH, EAST, SOUTH, WEST };
  Direction bishop_dir[4] = { NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST };
  
  Bitboard atk = 0;
  for (Direction d : (pt == ROOK) ? rook_dir : bishop_dir) {
    Square s = sq;
    while (safe_step(s, d) && !(square_bb(s) & occupied))
      atk |= square_bb(s += d);
  }
  return atk;
}

std::string to_string(Bitboard bb) {
  std::stringstream ss;
  ss << "\n+---+---+---+---+---+---+---+---+\n";
  for (Square sq = A8; sq >= H1; sq--) {
    ss << "| " << (square_bb(sq) & bb ? "@ " : "  ");
    if (sq % 8 == 0)
      ss << "|" << "\n+---+---+---+---+---+---+---+---+\n";
  }
  ss << "\n";
  return ss.str();
}

uint64_t generate_magic(uint64_t mask)
{
    static int magics_generated = 0;
    
    std::cout << "\rloading " << (100 * magics_generated++ / 127) << "%";
    std::cout.flush();
    
    int permutations = 1 << popcount(mask);

    uint64_t occupied[4096];
    bool visited[4096], failed;

    for (int p = 0; p < permutations; occupied[p] = generate_occupancy(mask, p), p++);

    std::mt19937_64 rng(0);
    uint64_t magic;

    do
    {
        magic = rng() & rng() & rng();
        memset(visited, false, permutations);

        for (int p = 0, key = 0; p < permutations; key = occupied[++p] * magic >> 64 - popcount(mask))
        {
            if (failed = visited[key]; failed)
                break;

            visited[key] = true;
        }

    } while (failed);

    return magic;
}

void init_magics(PieceType pt, Bitboard* attacks, Bitboard* xray) {

  Bitboard *masks  = pt == BISHOP ? bishop_masks  : rook_masks;
  uint64_t *magics = pt == BISHOP ? bishop_magics : rook_magics;
  int      *shift  = pt == BISHOP ? bishop_shift  : rook_shift;
  int      *base   = pt == BISHOP ? bishop_base   : rook_base;

  int permutations = 0;
  
  for (Square sq = H1; sq <= A8; sq++) {
    base[sq] = permutations;
    Bitboard edges = (RANK_1 | RANK_8) &~ rank_of(sq) | (FILE_A | FILE_H) &~ file_of(sq);
    Bitboard mask = slider_attacks(pt, sq, 0) &~ edges;
    permutations += 1 << popcount(mask);
    masks[sq] = mask;
    shift[sq] = 64 - popcount(mask);
    uint64_t magic = generate_magic(mask);
    magics[sq] = magic;
    for (int i = 0; i < 1 << popcount(mask); i++) {
      Bitboard occ = generate_occupancy(mask, i);
      int idx = occ * magic >> shift[sq];
      attacks[base[sq] + idx] = slider_attacks(pt, sq, occ);
      xray[base[sq] + idx] = slider_attacks(pt, sq, occ ^ attacks[base[sq] + idx] & occ);
    }
  }
}

void Bitboards::init() {
  
  init_magics(BISHOP, bishop_atk, bishopxray);
  init_magics(ROOK  , rook_atk  , rookxray  );

#define mdiag(s) (square_bb(s) | bishop_attacks(s, 0) & (mask(s, NORTH_WEST) | mask(s, SOUTH_EAST)))
#define adiag(s) (square_bb(s) | bishop_attacks(s, 0) & (mask(s, NORTH_EAST) | mask(s, SOUTH_WEST)))

#define md(a, b) (rank_distance(a, b) + file_distance(a, b))

  for (Square s1 = H1; s1 <= A8; s1++) {

    main_diagonal[s1] = mdiag(s1);
    anti_diagonal[s1] = adiag(s1);

    file[s1] = file_of(s1);

    for (Square s2 = H1; s2 <= A8; s2++) {
      pinmask[s1][s2] =
        mdiag  (s1) & mdiag  (s2) | adiag  (s1) & adiag  (s2)
      | rank_of(s1) & rank_of(s2) | file_of(s1) & file_of(s2);
    }

    for (Square ksq = H1; ksq <= A8; ksq++) {
      for (Direction d : { NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST }) {
        Bitboard bishop_ray = bishop_attacks(ksq, square_bb(s1)) & mask(ksq, d);
        if (bishop_ray & square_bb(s1))
          checkray[ksq][s1] = bishop_ray;
      }
      for (Direction d : { NORTH, EAST, SOUTH, WEST }) {
        Bitboard rook_ray = rook_attacks(ksq, square_bb(s1)) & mask(ksq, d);
        if (rook_ray & square_bb(s1))
          checkray[ksq][s1] = rook_ray;
      }
    }

    for (Direction d : { NORTH, NORTH_EAST, EAST, SOUTH_EAST,
                         SOUTH, SOUTH_WEST, WEST, NORTH_WEST })
      king_atk[s1] |= safe_step(s1, d);

    for (Direction d : { NORTHNORTH+EAST, NORTH_EAST+EAST, SOUTH_EAST+EAST, SOUTHSOUTH+EAST,
                         SOUTHSOUTH+WEST, SOUTH_WEST+WEST, NORTH_WEST+WEST, NORTHNORTH+WEST })
      knight_atk[s1] |= safe_step(s1, d);

    doublecheck[s1] = king_atk[s1] | knight_atk[s1];

    pawn_atk[WHITE][s1] = pawn_attacks<WHITE>(square_bb(s1));
    pawn_atk[BLACK][s1] = pawn_attacks<BLACK>(square_bb(s1));
  }

#undef fdiag
#undef bdiag
#undef md

  uint8_t clearK = 0b0111;
  uint8_t clearQ = 0b1011;
  uint8_t cleark = 0b1101;
  uint8_t clearq = 0b1110;
  
  for (int i = 0; i < 1 << 5; i++) {

    Bitboard w_occ = generate_occupancy(square_bb(A1, E1, H1, A8, H8), i);
    Bitboard b_occ = generate_occupancy(square_bb(A8, E8, H8, A1, H1), i);
    
    uint8_t w_rights = 0b1111;
    uint8_t b_rights = 0b1111;

    if ((w_occ & square_bb(A1)) == 0) w_rights &= clearQ;
    if ((w_occ & square_bb(E1)) == 0) w_rights &= clearK & clearQ;
    if ((w_occ & square_bb(H1)) == 0) w_rights &= clearK;
    if ((w_occ & square_bb(A8)) != 0) w_rights &= clearq;
    if ((w_occ & square_bb(H8)) != 0) w_rights &= cleark;

    if ((b_occ & square_bb(A8)) == 0) b_rights &= clearq;
    if ((b_occ & square_bb(E8)) == 0) b_rights &= cleark & clearq;
    if ((b_occ & square_bb(H8)) == 0) b_rights &= cleark;
    if ((b_occ & square_bb(A1)) != 0) b_rights &= clearQ;
    if ((b_occ & square_bb(H1)) != 0) b_rights &= clearK;

    castle_masks[WHITE][w_occ * 0x4860104020003061ull >> 59] = w_rights;
    castle_masks[BLACK][b_occ * 0x1080000400400c21ull >> 59] = b_rights;
  }
}

bool has_duplicates(const std::vector<int>& keys) {
  for (int i = 0; i < keys.size(); i++)
    for (int j = i + 1; j < keys.size(); j++)
      if (keys[i] == keys[j])
        return true;
  return false;
}
