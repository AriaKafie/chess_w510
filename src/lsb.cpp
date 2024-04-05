
#include <cstdint>
#include <iostream>
#include <random>

bool has_duplicates(int* keys) {
  for (int i = 0; i < 64; i++)
    for (int j = i + 1; j < 64; j++)
      if (keys[i] == keys[j])
        return true;
  return false;
}

int main() {

  std::mt19937_64 rng(0);
  
  uint64_t magic = rng();

  int keys[64];
  std::cout << "working\n";
  for (int i = 0; i < 64; i++) {
    keys[i] = (1ull << i) * magic >> 58;
    for (int j = 0; j < i; j++) {
      if (keys[i] == keys[j]) {
        i = 0;
        magic = rng() & rng();
        break;
      }
    }
  }
  
  std::cout << "0x" << std::hex << magic << "ull\n";
}
