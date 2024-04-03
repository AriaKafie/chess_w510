
#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.h"

#include <cmath>
#include <string>

// inline Bitboard rook_magics[SQUARE_NB] =
// {
//   0x80006080400010ull,
//   0x80112000400080ull,
//   0x300104020008900ull,
//   0x100100100488420ull,
//   0x8080040080080002ull,
//   0x200089e00100514ull,
//   0x8200040088020019ull,
//   0x5100030002208042ull,
//   0x800082204000ull,
//   0x10401000442001ull,
//   0x806000809000ull,
//   0x85002010000901ull,
//   0x425004510080100ull,
//   0x202000200285044ull,
//   0x201000402000100ull,
//   0x22000082010154ull,
//   0x801208002400180ull,
//   0x120008030804000ull,
//   0x2005050020004210ull,
//   0x808008021000ull,
//   0xa000808004000800ull,
//   0x3010008040042ull,
//   0x62040012031008ull,
//   0x84060002804324ull,
//   0x10400080003080ull,
//   0x689000c2c0002010ull,
//   0x100200080801000ull,
//   0x10002100500901ull,
//   0x801000500121800ull,
//   0x8018200801341040ull,
//   0xa001200440841ull,
//   0x2c180820002630cull,
//   0x3824000800423ull,
//   0x40806101004000ull,
//   0xf00200081801008ull,
//   0x842044022001008ull,
//   0x10802400800800ull,
//   0x1220800200800400ull,
//   0x2002000816000401ull,
//   0x23140142000081ull,
//   0x202400490208002ull,
//   0x4020a000d0014001ull,
//   0x200200100110040ull,
//   0x70002300090030ull,
//   0x603008408010050ull,
//   0x1042008004008100ull,
//   0x51002440048ull,
//   0x4080408d0820003ull,
//   0x4240012080004880ull,
//   0x2006a080400080ull,
//   0x410014200900ull,
//   0x201204200900a00ull,
//   0x3102100800250100ull,
//   0x41802400020080ull,
//   0x2024811002080c00ull,
//   0x8680010c00408200ull,
//   0x112008100102242ull,
//   0x2203100400089ull,
//   0x8c081040800a00a2ull,
//   0x82120300100098dull,
//   0x1720020a4289002ull,
//   0x4051002208040001ull,
//   0x802000400810822ull,
//   0x408050454042082ull,
// };

// inline Bitboard bishop_magics[SQUARE_NB] =
// {
//   0x8830108101002204ull,
//   0x8830108101002204ull,
//   0x210b18081022a00ull,
//   0x82140c8000004ull,
//   0x8004030880801002ull,
//   0x2020220401001ull,
//   0x30c008808280800ull,
//   0x2004908080213ull,
//   0xa81022584600840cull,
//   0x81428202040300ull,
//   0x4200043404204052ull,
//   0xa2880610400880ull,
//   0x4100308820102402ull,
//   0x40228044000c0ull,
//   0xc0e1110080402ull,
//   0x800540a011000ull,
//   0x420008404040800ull,
//   0xc102041040100ull,
//   0x6004000800240011ull,
//   0x8818014c2200a000ull,
//   0x4000201211004ull,
//   0x1062010101008280ull,
//   0x2202110101100208ull,
//   0x6912236090841000ull,
//   0x878a004a00800ull,
//   0x1010200805810bull,
//   0x8000900008002022ull,
//   0x490040000440008ull,
//   0x12404004010040ull,
//   0x3000810002006628ull,
//   0x80401000c010100ull,
//   0x800a120080885100ull,
//   0x4004100404c004c0ull,
//   0x111109028080100ull,
//   0x8001410000a0680ull,
//   0x18840108040101ull,
//   0x8400080200600a0ull,
//   0x2010020281203000ull,
//   0x800c0115c1020800ull,
//   0x83900402103d0ull,
//   0x4908011008001024ull,
//   0x2c0e034402102004ull,
//   0x4014202470000809ull,
//   0x4202001144014800ull,
//   0x2044016011000200ull,
//   0x20008114402200ull,
//   0x2080505044410ull,
//   0x201010401108480ull,
//   0x4041040104422238ull,
//   0x4041040104422238ull,
//   0x50924601902100ull,
//   0x4000000b08482104ull,
//   0x840601020320022ull,
//   0x200410048000ull,
//   0x408080810941208ull,
//   0x4820044422906010ull,
//   0x2002104100402e0ull,
//   0x400a004044100826ull,
//   0x80821004200b000ull,
//   0x42000300242061aull,
//   0x82000002020410aull,
//   0x42032125010200ull,
//   0xca00401080100ull,
//   0x40700404828030ull,
// };

constexpr int index64[64] = {
   0,  1, 48,  2, 57, 49, 28,  3,
  61, 58, 50, 42, 38, 29, 17,  4,
  62, 55, 59, 36, 53, 51, 43, 22,
  45, 39, 33, 30, 24, 18, 12,  5,
  63, 47, 56, 27, 60, 41, 37, 16,
  54, 35, 52, 21, 44, 32, 23, 11,
  46, 26, 40, 15, 34, 20, 31, 10,
  25, 14, 19,  9, 13,  8,  7,  6
};

namespace Bitboards { void init(); }

std::string to_string(Bitboard bb);

inline int      rook_shift[SQUARE_NB];
inline int      bishop_shift[SQUARE_NB];
inline int      rook_base[SQUARE_NB];
inline int      bishop_base[SQUARE_NB];
inline Bitboard rookxray[102400];
inline Bitboard bishopxray[5248];
inline Bitboard rook_atk[102400];
inline Bitboard bishop_atk[5248];
inline Bitboard bishop_masks[SQUARE_NB];
inline Bitboard rook_masks[SQUARE_NB];
inline Bitboard bishop_magics[SQUARE_NB];
inline Bitboard rook_magics[SQUARE_NB];
inline Bitboard doublecheck[SQUARE_NB];
inline Bitboard knight_atk[SQUARE_NB];
inline Bitboard king_atk[SQUARE_NB];
inline Bitboard pawn_atk[COLOR_NB][SQUARE_NB];
inline Bitboard checkray[SQUARE_NB][SQUARE_NB];
inline Bitboard pinmask[SQUARE_NB][SQUARE_NB];
inline Bitboard main_diagonal[SQUARE_NB];
inline Bitboard anti_diagonal[SQUARE_NB];
inline Bitboard file[SQUARE_NB];
inline uint8_t castle_masks[1 << 6];

constexpr Bitboard ALL_SQUARES = 0xffffffffffffffffull;
constexpr Bitboard FILE_A = 0x8080808080808080ull;
constexpr Bitboard FILE_B = FILE_A >> 1;
constexpr Bitboard FILE_C = FILE_A >> 2;
constexpr Bitboard FILE_D = FILE_A >> 3;
constexpr Bitboard FILE_E = FILE_A >> 4;
constexpr Bitboard FILE_F = FILE_A >> 5;
constexpr Bitboard FILE_G = FILE_A >> 6;
constexpr Bitboard FILE_H = FILE_A >> 7;
constexpr Bitboard NOT_FILE_A = ~FILE_A;
constexpr Bitboard NOT_FILE_H = ~FILE_H;

constexpr Bitboard RANK_1 = 0xffull;
constexpr Bitboard RANK_2 = RANK_1 << 8;
constexpr Bitboard RANK_3 = RANK_1 << 16;
constexpr Bitboard RANK_4 = RANK_1 << 24;
constexpr Bitboard RANK_5 = RANK_1 << 32;
constexpr Bitboard RANK_6 = RANK_1 << 40;
constexpr Bitboard RANK_7 = RANK_1 << 48;
constexpr Bitboard RANK_8 = RANK_1 << 56;
constexpr Bitboard NOT_RANK_2 = ~RANK_2;
constexpr Bitboard NOT_RANK_7 = ~RANK_7;

inline void pop_lsb(Bitboard& b) {
  b &= b - 1;
}

inline int popcount(Bitboard bb) {
  int count = 0;
  for (;bb; pop_lsb(bb))
    count++;
  return count;
}

inline int lsb(Bitboard bb) {
  return index64[((bb & -bb) * 0x03f79d71b4cb0a89ull) >> 58];
}

template<Direction D>
constexpr Bitboard shift(Bitboard bb) {
  if constexpr (D == NORTH)      return  bb << 8;
  if constexpr (D == NORTH_EAST) return (bb & NOT_FILE_H) << 7;
  if constexpr (D == EAST)       return  bb >> 1;
  if constexpr (D == SOUTH_EAST) return (bb & NOT_FILE_H) >> 9;
  if constexpr (D == SOUTH)      return  bb >> 8;
  if constexpr (D == SOUTH_WEST) return (bb & NOT_FILE_A) >> 7;
  if constexpr (D == WEST)       return  bb << 1;
  if constexpr (D == NORTH_WEST) return (bb & NOT_FILE_A) << 9;
  if constexpr (D == NORTHNORTH) return  bb << 16;
  if constexpr (D == SOUTHSOUTH) return  bb >> 16;
}

inline Bitboard pin_mask(Square ksq, Square pinned) {
  return pinmask[ksq][pinned];
}

inline Bitboard main_diag(Square s) {
  return main_diagonal[s];
}

inline Bitboard anti_diag(Square s) {
  return anti_diagonal[s];
}

inline Bitboard file_bb(Square s) {
  return file[s];
}

inline Bitboard double_check(Square ksq) {
  return doublecheck[ksq];
}

inline Bitboard check_ray(Square ksq, Square checker) {
  return checkray[ksq][checker];
}

constexpr Bitboard square_bb(Square s) {
  return 1ull << s;
}

template<typename... squares>
inline constexpr Bitboard square_bb(Square sq, squares... sqs) {
  return square_bb(sq) | square_bb(sqs...);
}

inline Bitboard rank_of(Square s) {
  return RANK_1 << 8 * (s / 8);
}

inline Bitboard file_of(Square s) {
  return FILE_H << (s % 8);
}

inline Bitboard mask(Square s, Direction d) {
  switch (d) 
  {
    case NORTH_EAST: return mask(s, NORTH) & mask(s, EAST);
    case SOUTH_EAST: return mask(s, SOUTH) & mask(s, EAST);
    case SOUTH_WEST: return mask(s, SOUTH) & mask(s, WEST);
    case NORTH_WEST: return mask(s, NORTH) & mask(s, WEST);
  }
  if (d == NORTH || d == SOUTH) {
    Bitboard m = 0;
    while (is_ok(s += d))
      m |= rank_of(s);
    return m;
  }
  else {
    Bitboard r = rank_of(s);
    Bitboard m = 0;
    while (square_bb(s += d) & r)
      m |= FILE_H << (s % 8);
    return m;
  }
}

inline uint64_t more_than_one(Bitboard b) {
  return b == 0 ? 0 : b & b - 1;
}

inline Bitboard knight_attacks(Square sq) {
  return knight_atk[sq];
}

inline Bitboard bishop_attacks(Square sq, Bitboard occupied) {
  return bishop_atk[bishop_base[sq] + ((occupied & bishop_masks[sq]) * bishop_magics[sq] >> bishop_shift[sq])];
}

inline Bitboard bishop_xray(Square sq, Bitboard occupied) {
  return bishopxray[bishop_base[sq] + ((occupied & bishop_masks[sq]) * bishop_magics[sq] >> bishop_shift[sq])];
}

inline Bitboard rook_attacks(Square sq, Bitboard occupied) {
  return rook_atk[rook_base[sq] + ((occupied & rook_masks[sq]) * rook_magics[sq] >> rook_shift[sq])];
}

inline Bitboard rook_xray(Square sq, Bitboard occupied) {
  return rookxray[rook_base[sq] + ((occupied & rook_masks[sq]) * rook_magics[sq] >> rook_shift[sq])];
}

inline Bitboard queen_attacks(Square sq, Bitboard occupied) {
  return rook_attacks(sq, occupied) | bishop_attacks(sq, occupied);
}

inline Bitboard king_attacks(Square sq) {
  return king_atk[sq];
}

template<Color C>
constexpr Bitboard pawn_attacks(Square sq) {
  return pawn_atk[C][sq];
}

template<Color C>
constexpr Bitboard pawn_attacks(Bitboard pawns) {
  if constexpr (C == WHITE)
    return shift<NORTH_EAST>(pawns) | shift<NORTH_WEST>(pawns);
  else 
    return shift<SOUTH_WEST>(pawns) | shift<SOUTH_EAST>(pawns);
}

inline void toggle_square(Bitboard& b, Square s) {
  b ^= 1ull << s;
}

inline Bitboard generate_occupancy(Bitboard mask, int permutation) {
  int bitcount = popcount(mask);
  Bitboard occupancy = 0;
  for (int bitpos = 0; bitpos < bitcount; bitpos++) {
    if (permutation & (1 << bitpos))
      occupancy ^= 1ull << lsb(mask);
    pop_lsb(mask);
  }
  return occupancy;
}

inline uint8_t castle_mask(Bitboard occupied) {
  constexpr Bitboard mask = square_bb(A1, E1, H1, A8, E8, H8);
  return castle_masks[(occupied & mask) * 0xc2314a1708abc41ull >> 58];
}

inline int file_distance(Square a, Square b) {
  return std::abs((a % 8) - (b % 8));
}

inline int rank_distance(Square a, Square b) {
  return std::abs((a / 8) - (b / 8));
}

inline int square_distance(Square a, Square b) {
  return std::max(file_distance(a, b), rank_distance(a, b));
}

inline Bitboard safe_step(Square s, int step) {
  Square to = s + step;
  return (is_ok(to) && square_distance(s, to) <= 2) ? square_bb(to) : 0;
}

#endif
