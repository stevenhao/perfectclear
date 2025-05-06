#include <emscripten.h>
#include <cstdlib>
#include <string>
#include "json.hpp"
#include "engine.cpp"

using json = nlohmann::json;
using namespace std;

char* allocateString(const std::string& str) {
    char* result = (char*)malloc(str.length() + 1);
    strcpy(result, str.c_str());
    return result;
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
