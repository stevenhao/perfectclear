#include "tetris_wasm.h"
#include "wasm_engineutils.h"
#include "tetrisio.h"
#include "engine.h"
#include <string>
#include <vector>
#include <emscripten/bind.h>

// Include implementation files
#include "wasm_engineutils.cpp"
#include "wasm_engine_impl.cpp"

using namespace emscripten;

// Parse JSON input
std::vector<int> parsePieces(const std::string& input) {
  std::vector<int> pieces;
  size_t pos = input.find("\"pieces\":");
  if (pos != std::string::npos) {
    size_t start = input.find("[", pos);
    size_t end = input.find("]", start);
    if (start != std::string::npos && end != std::string::npos) {
      std::string piecesStr = input.substr(start + 1, end - start - 1);
      size_t commaPos = 0;
      size_t prevPos = 0;
      while ((commaPos = piecesStr.find(",", prevPos)) != std::string::npos) {
        std::string numStr = piecesStr.substr(prevPos, commaPos - prevPos);
        pieces.push_back(std::stoi(numStr));
        prevPos = commaPos + 1;
      }
      std::string lastNum = piecesStr.substr(prevPos);
      if (!lastNum.empty()) {
        pieces.push_back(std::stoi(lastNum));
      }
    }
  }
  return pieces;
}

// Parse board from JSON
ll parseBoard(const std::string& input) {
  ll boardValue = 0;
  size_t pos = input.find("\"board\":");
  if (pos != std::string::npos) {
    size_t valueStart = input.find_first_not_of(" \t\n:", pos + 7);
    size_t valueEnd = input.find_first_of(",}", valueStart);
    if (valueStart != std::string::npos && valueEnd != std::string::npos) {
      std::string boardStr = input.substr(valueStart, valueEnd - valueStart);
      boardValue = std::stoll(boardStr);
    }
  }
  return boardValue;
}

// Handle WASM request
std::string handleWasmRequest(std::string input) {
  if (input.empty()) return "{}";
  
  try {
    // Parse input
    board b;
    ll boardValue = parseBoard(input);
    if (boardValue != 0) {
      b.fromLL(boardValue);
    }
    
    std::vector<int> pieces = parsePieces(input);
    if (!pieces.empty()) {
      piece initial = piece(pieces[0]);
      
      // Get best move
      engineResult result = getBestMove(b, pieces);
      if (result.moves.empty()) {
        return "{\"error\":\"No valid moves found\"}";
      }
      
      // Get path to best move
      std::vector<int> path = getPath(b, result.moves[0], initial);
      
      // Create response
      std::string response = "{\"path\":[";
      for (size_t i = 0; i < path.size(); ++i) {
        response += std::to_string(path[i]);
        if (i < path.size() - 1) {
          response += ",";
        }
      }
      response += "],\"happy\":" + std::string(result.happy ? "true" : "false") + "}";
      return response;
    }
  } catch (std::exception& e) {
    return "{\"error\":\"" + std::string(e.what()) + "\"}";
  }
  
  return "{}";
}

// Load data files
bool dataLoaded = false;
void loadWasmData() {
  if (!dataLoaded) {
    loadData();
    loadBook();
    dataLoaded = true;
  }
}

void unloadWasmData() {
  if (dataLoaded) {
    wasm_cleanup();
    dataLoaded = false;
  }
}

// Bind functions to JavaScript
EMSCRIPTEN_BINDINGS(perfectclear_module) {
  emscripten::function("handleWasmRequest", &handleWasmRequest);
  emscripten::function("loadWasmData", &loadWasmData);
  emscripten::function("unloadWasmData", &unloadWasmData);
}
