
#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "types.h"

class MoveList {
public:
  MoveList();
  void put_first(Move m) {
    for (int i = 0; i < this->length(); i++) {
      if (moves[i] == m) {
        Move temp = moves[0];
        moves[0] = m;
        moves[i] = temp;
        return;
      }
    }
  }
  Move* begin()  { return moves; }
  Move* end()    { return last; }
  int   length() { return last - moves; }
  Move operator[](Move i) { return moves[i]; }
private:
  Move moves[MAX_PLY], *last;
};

#endif
