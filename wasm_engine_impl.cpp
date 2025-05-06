#include "engine.h"
#include <fstream>
#include <iostream>
#include <unordered_map>

using namespace std;

unordered_map<ll, unordered_map<int, int>> book;

void loadBook() {
  cerr << "Loading book from book_100k.txt...\n";
  ifstream fin("book_100k.txt");
  if (!fin.is_open()) {
    fin.open("book_compressed.txt");
    if (!fin.is_open()) {
      cerr << "Failed to open book file\n";
      return;
    }
  }
  
  int turn, msk, cnt;
  ll board;
  while (fin >> turn >> board >> msk >> cnt) {
    book[board][msk] = cnt;
  }
  cerr << "Loaded " << book.size() << " unique positions...\n";
}

engineResult getBestMove(board b, vector<int> pieces) {
  vector<piece> moves = wasm_getMoves(b, pieces[0]);
  
  if (moves.empty()) {
    return {{}, 0};
  }
  
  return {{moves[0]}, 0};
}

vector<int> getPath(board b, piece p, piece inHand) {
  return wasm_getPath(b, p, inHand);
}
