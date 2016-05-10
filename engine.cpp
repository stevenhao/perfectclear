#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "engineutils.cpp"

using namespace std;
typedef long long ll;

int buf[MAXN][MAXN];

void precompute() {
}

piece getBestMove(board b, vector<int> pieces) {
  vector<piece> v = getMoves(b, pieces[0]);
  printf("Found %lu moves\n", v.size());
  piece bestMove = v.back();
  return bestMove;
}
