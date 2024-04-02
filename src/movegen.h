
#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "types.h"

class MoveList {
public:
  MoveList();
  Move* begin()  { return moves; }
  Move* end()    { return last; }
  int   length() { return last - moves; }
  Move operator[](Move& i) {
    return moves[i];
  }
private:
  Move moves[MAX_PLY], *last;
};

#endif
