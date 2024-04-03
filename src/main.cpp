
#include "bitboard.h"
#include "uci.h"

int main() {
  
  Bitboards::init();
  UCI::loop();
  
  return 0;
}
