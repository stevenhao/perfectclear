#ifndef WASM_ENGINEUTILS_H
#define WASM_ENGINEUTILS_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "tetris.h"

// Forward declarations
std::vector<int> wasm_getPath(board b, piece p, piece inHand);
std::vector<piece> wasm_getMoves(board b, int pType);

#endif // WASM_ENGINEUTILS_H
