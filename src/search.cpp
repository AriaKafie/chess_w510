
#include "search.h"

#include "evaluation.h"
#include "movegen.h"
#include "position.h"

#include <iostream>

constexpr int reduction[MAX_PLY] = {
  0,0,0,0,0,0,0,0,0,0,
  2,2,2,2,2,2,2,2,2,2,
  4,4,4,4,4,4,4,4,4,4,
  8,8,8,8,8,8,8,8,8,8,
};

int search(int alpha, int beta, int depth, int ply_from_root) {
  
  if (depth <= 0)
    return static_evaluation();

  MoveList moves;

  if (moves.length() == 0)
    return -INFINITE + ply_from_root;
  
  int best_eval = -INFINITE;
  
  for (Move move : moves) {
    do_move(move);
    int eval = -search(-beta, -alpha, depth - 1, ply_from_root + 1);
    undo_move(move);
    alpha = std::max(alpha, eval);
    if (alpha >= beta)
      return alpha;
    if (eval > best_eval)
      best_eval = eval;
  }

  return best_eval; 
}

Move Search::best_move(uint64_t thinktime) {
  
  MoveList moves;
  Move best_move = NULLMOVE;
  auto start_time = curr_time_millis();
  
  for (int depth = 1, search_cancelled = false; depth < MAX_PLY && !search_cancelled; depth++) {

    std::cout << "searching depth " << depth << "\n";

    moves.put_first(best_move);
    
    int alpha = -INFINITE;
    
    for (int i = 0; i < moves.length(); i++) {
      
      if (curr_time_millis() - start_time > thinktime) {
        search_cancelled = true;
        break;
      }
      
      do_move(moves[i]);
      int eval = -search(-INFINITE, -alpha, depth - 1 - reduction[i], 0);
      if (eval > alpha && reduction[i])
	eval = -search(-INFINITE, -alpha, depth - 1, 0);
      undo_move(moves[i]);
      
      if (eval > alpha) {
	alpha = eval;
	best_move = moves[i];
      }
      
    }
  }
  return best_move;
}
