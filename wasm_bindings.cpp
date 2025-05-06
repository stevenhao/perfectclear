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
  try {
    int _W, _H;
    if (j["W"].is_number()) {
      _W = j["W"].get<int>();
    } else if (j["W"].is_string()) {
      _W = toInt(j["W"].get<string>());
    } else {
      throw runtime_error("W is neither a number nor a string");
    }
    
    if (j["H"].is_number()) {
      _H = j["H"].get<int>();
    } else if (j["H"].is_string()) {
      _H = toInt(j["H"].get<string>());
    } else {
      throw runtime_error("H is neither a number nor a string");
    }
    
    if (!j["data"].is_string()) {
      throw runtime_error("data is not a string");
    }
    
    string s = j["data"].get<string>();
    if (_W != W || _H != H) {
      throw runtime_error("Board dimensions mismatch: expected " + to_string(W) + "x" + to_string(H) + 
                          ", got " + to_string(_W) + "x" + to_string(_H));
    }
    
    if (s.length() != W * H) {
      throw runtime_error("Data length mismatch: expected " + to_string(W * H) + 
                          ", got " + to_string(s.length()));
    }
    
    char buf[MAXN][MAXN];
    int cnt = 0;
    for (int i = 0; i < H; ++i) {
      for (int j = 0; j < W; ++j) {
        if (cnt < s.length()) {
          buf[i][j] = s[cnt];
          ++cnt;
        } else {
          buf[i][j] = '0'; // Default to empty if data is too short
        }
      }
    }
    return board(buf, '0');
  } catch (const exception& e) {
    throw runtime_error(string("Error in readBoard: ") + e.what() + 
                        ", JSON: " + j.dump());
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
            json j_board, j_pieces;
            try {
                j_board = json::parse(boardJson);
            } catch (const exception& e) {
                json error;
                error["error"] = string("Failed to parse board JSON: ") + e.what();
                return allocateString(error.dump());
            }
            
            try {
                j_pieces = json::parse(piecesJson);
            } catch (const exception& e) {
                json error;
                error["error"] = string("Failed to parse pieces JSON: ") + e.what();
                return allocateString(error.dump());
            }
            
            board b;
            vector<int> pieces;
            
            try {
                b = readBoard(j_board);
            } catch (const exception& e) {
                json error;
                error["error"] = string("Failed to read board: ") + e.what();
                error["board_json"] = boardJson;
                return allocateString(error.dump());
            }
            
            try {
                pieces = readPieces(j_pieces);
                if (pieces.empty()) {
                    json error;
                    error["error"] = "Pieces array is empty";
                    error["pieces_json"] = piecesJson;
                    return allocateString(error.dump());
                }
            } catch (const exception& e) {
                json error;
                error["error"] = string("Failed to read pieces: ") + e.what();
                error["pieces_json"] = piecesJson;
                return allocateString(error.dump());
            }
            
            setBeamSearchLimit(searchBreadth);
            
            engineResult result;
            try {
                json debug;
                debug["board"] = toString(b);
                debug["pieces"] = pieces;
                
                result = getBestMove(b, pieces);
                
                if (result.moves.empty()) {
                    json error;
                    error["error"] = "No valid moves found";
                    error["debug"] = debug;
                    return allocateString(error.dump());
                }
            } catch (const exception& e) {
                json error;
                error["error"] = string("Failed to get best move: ") + e.what();
                error["board_str"] = toString(b);
                error["pieces_count"] = pieces.size();
                if (!pieces.empty()) {
                    error["first_piece"] = pieces[0];
                }
                return allocateString(error.dump());
            }
            
            piece initial = piece(pieces[0]);
            
            vector<int> path;
            try {
                path = getPath(b, result.moves[0], initial);
            } catch (const exception& e) {
                json error;
                error["error"] = string("Failed to get path: ") + e.what();
                return allocateString(error.dump());
            }
            
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
            error["error"] = string("Unexpected error: ") + e.what();
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
