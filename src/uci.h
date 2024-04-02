
#ifndef UCI_H
#define UCI_H

#include "types.h"

#include <string>

constexpr auto STARTPOS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

inline std::string square_to_uci(Square sq) { return std::string(1, "hgfedcba"[sq % 8]) + std::string(1, "12345678"[sq / 8]); }

inline Square uci_to_square(const std::string& uci) { return 8 * (uci[1] - '1') + 'h' - uci[0]; }

inline Move uci_to_move(const std::string& uci) { return make_move(uci_to_square(uci.substr(0,2)), uci_to_square(uci.substr(2))); }

inline std::string move_to_uci(Move m) { return square_to_uci(from_sq(m)) + square_to_uci(to_sq(m)); }

namespace UCI { void loop(); }

namespace Perft {
  void bench(int depth);
  void go(int depth);
}

#endif
