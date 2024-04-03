
#include "evaluation.h"

#include "bitboard.h"
#include "position.h"

int static_evaluation() {
  
  int score =
    100 * (popcount(bb(W_PAWN))   - popcount(bb(B_PAWN)))   +
    300 * (popcount(bb(W_KNIGHT)) - popcount(bb(B_KNIGHT))) +
    300 * (popcount(bb(W_BISHOP)) - popcount(bb(B_BISHOP))) +
    500 * (popcount(bb(W_ROOK))   - popcount(bb(B_ROOK)))   +
    900 * (popcount(bb(W_QUEEN))  - popcount(bb(B_QUEEN)));

  for (PieceType pt : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING})
  {
    for (Bitboard b = bitboards[make_piece(pt, WHITE)]; b; pop_lsb(b))
      score += square_score<WHITE>(pt, lsb(b));
    for (Bitboard b = bitboards[make_piece(pt, BLACK)]; b; pop_lsb(b))
      score -= square_score<BLACK>(pt, lsb(b));
  }
  
  return Position::white_to_move() ? score : -score;
}
