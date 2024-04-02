
#include "movegen.h"
#include "position.h"
#include "uci.h"

#include <chrono>
#include <iomanip>
#include <iostream>

uint64_t nodes;

void expand(int depth) {
  
  if (depth == 0) {
    nodes++;
    return;
  }
  
  MoveList moves;
  
  if (depth == 1) {
    nodes += moves.length();
    return;
  }
  
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
