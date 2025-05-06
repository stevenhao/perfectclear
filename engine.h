#ifndef ENGINE_H
#define ENGINE_H

#include <unordered_map>
#include <vector>
#include "tetris.h"

// Forward declarations
struct engineResult {
  std::vector<piece> moves;
  bool happy;
};

// Function declarations
void loadBook();
engineResult getBestMove(board b, std::vector<int> pieces);
std::vector<int> getPath(board b, piece p, piece inHand);

#endif // ENGINE_H
