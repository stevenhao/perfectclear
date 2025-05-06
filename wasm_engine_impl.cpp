#include "engine.h"
#include <fstream>
#include <iostream>
#include <unordered_map>

using namespace std;

unordered_map<ll, unordered_map<int, int>> book;

void loadBook() {
  cerr << "Loading book from book_100k.txt...\n";
  
  ifstream fin("book_compressed.txt");
  if (!fin.is_open()) {
    fin.open("book_100k.txt");
    if (!fin.is_open()) {
      cerr << "Failed to open book file\n";
      return;
    }
    cerr << "Using uncompressed book file\n";
  } else {
    cerr << "Using compressed book file\n";
  }
  
  int turn, msk, cnt;
  ll board;
  int entries = 0;
  while (fin >> turn >> board >> msk >> cnt) {
    book[board][msk] = cnt;
    entries++;
  }
  cerr << "Loaded " << book.size() << " unique positions with " << entries << " entries...\n";
}

engineResult getBestMove(board b, vector<int> pieces) {
  vector<piece> moves = wasm_getMoves(b, pieces[0]);
  
  if (moves.empty()) {
    return {{}, 0};
  }
  
  piece bestMove = moves[0];
  double bestScore = -1e9;
  bool foundClear = false;
  
  for (piece move : moves) {
    board testBoard = b;
    testBoard.add(move);
    
    if (testBoard.count() == 0) {
      bestMove = move;
      foundClear = true;
      break;
    }
    
    double score = 0;
    ll key = testBoard.toLL();
    if (book.count(key)) {
      score = book[key].size();
    }
    
    if (score > bestScore) {
      bestScore = score;
      bestMove = move;
    }
  }
  
  return {{bestMove}, foundClear ? 1 : 0};
}

vector<int> getPath(board b, piece p, piece inHand) {
  return wasm_getPath(b, p, inHand);
}
