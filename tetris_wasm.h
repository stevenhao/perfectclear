#ifndef TETRIS_WASM_H
#define TETRIS_WASM_H

#include <algorithm>
#include <bitset>
#include <vector>
#include <string>
#include <emscripten/bind.h>

// Forward declarations
typedef long long ll;
typedef std::bitset<256> lll;
typedef std::pair<int, int> pii;

// Function declarations
std::string handleWasmRequest(std::string input);
void loadWasmData();

#endif // TETRIS_WASM_H
