#include <emscripten.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include "json.hpp"

#define queue pieceQueue
#include "engine.cpp"
#undef queue

using json = nlohmann::json;
using namespace std;

char* allocateString(const std::string& str) {
    char* result = (char*)malloc(str.length() + 1);
    strcpy(result, str.c_str());
    return result;
}

int toInt(string s) {
  int i;
  stringstream ss;
  ss << s;
  ss >> i;
  return i;
}

board readBoard(json j) {
  if (j["W"].is_string() && j["H"].is_string() && j["data"].is_string()) {
    int _W = toInt(j["W"].get<string>());
    int _H = toInt(j["H"].get<string>());
    string s = j["data"].get<string>();
    if (_W != W || _H != H) throw runtime_error("Board dimensions mismatch");
    char buf[MAXN][MAXN];

    int cnt = 0;
    for (int i = 0; i < H; ++i) {
      for (int j = 0; j < W; ++j) {
        buf[i][j] = s[cnt];
        ++cnt;
      }
    }
    return board(buf, '0');
  } else {
    throw runtime_error("Cannot parse board");
  }
}

vector<int> readPieces(json j) {
  vector<string> v = j.get<vector<string>>();
  vector<int> ret;
  for (auto &s : v) {
    ret.push_back(getPieceIndex(s));
  }
  return ret;
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    char* wasm_get_best_move(const char* boardJson, const char* piecesJson, int searchBreadth) {
        try {
            auto j_board = json::parse(boardJson);
            auto j_pieces = json::parse(piecesJson);
            
            board b = readBoard(j_board);
            vector<int> pieces = readPieces(j_pieces);
            
            setBeamSearchLimit(searchBreadth);
            
            engineResult result = getBestMove(b, pieces);
            
            piece initial = piece(pieces[0]);
            
            vector<int> path = getPath(b, result.moves[0], initial);
            
            vector<string> pathStrings;
            pathStrings.push_back(toString(initial));
            piece p = piece(result.moves[0].pieceType);
            for (auto i : path) {
                p = apply(i, p, b);
                pathStrings.push_back(toString(p));
            }
            b.add(p);
            
            json j_result;
            j_result["path"] = path;
            j_result["happy"] = result.happy;
            j_result["pathStrings"] = pathStrings;
            j_result["board"] = toString(b);
            
            return allocateString(j_result.dump());
        } catch (const exception& e) {
            json error;
            error["error"] = e.what();
            return allocateString(error.dump());
        }
    }
    
    EMSCRIPTEN_KEEPALIVE
    void wasm_init_engine() {
        loadData();
        loadBook();
    }
    
    EMSCRIPTEN_KEEPALIVE
    void wasm_free_string(char* ptr) {
        free(ptr);
    }
}
