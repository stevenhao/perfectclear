#include <cstdio>
#include <sstream>
#include <ctime>
#include <iostream>

#include "tetris.cpp"

using namespace std;

void precompute() {

}

piece getBestMove(board b, vector<int> pieces) {
  vector<piece> v = getMoves(b, pieces[0]);
  printf("Found %lu moves\n", v.size());
  piece bestMove = v[0];
  return bestMove;
}

int mmain() {
  precompute();
  loadPieceData();
  return 0;
}
