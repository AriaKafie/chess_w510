
#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"

constexpr int INFINITE = 0x7fffffff;

namespace Search {
  // void init();
  Move best_move(uint64_t thinktime);
}

#endif
