#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "engineutils.cpp"

using namespace std;
umap<pair<board, int>, umap<board, int>> memo;
umap<pair<board, int>, int> memod;

umap<board, int> outEdges(const board &cur, int pType) {
  auto pp = make_pair(cur, pType);
  umap<board, int> &ret = memo[pp];
  if (!memod[pp]) {
    memod[pp] = 1;
    vector<piece> moves = getMoves(cur, pType);
    for (piece p: moves) {
      board nb = cur;
      nb.add(p);
      ret[nb] = 1;
    }
  }
  return ret;
}

#define str(a, b, c, d) ret.push_back(\
    (a << pos(0,0)) | (b << pos(0,1)) | (c << pos(0,2)) | (d << pos(0,3))\
    )
#define rw(a) ((grid >> pos(0,a)) & rmsk)
vector<lll> swapRows(lll grid) {
  lll rmsk = (ll(1) << W) - 1;
  lll a = rw(0), b = rw(1), c = rw(2), d = rw(3);

  vector<lll> ret;
  if (d == rmsk) {
    str(a,b,c,d);
  } else if (c == rmsk) {
    str(a, b, c, d);
    str(a, b, d, c);
    str(a, d, b, c);
    str(d, a, b, c);
  } else if (b == rmsk) {
    str(a, b, c, d);
    str(c, a, b, d);
    str(c, d, a, b);
    str(a, c, b, d);
    str(a, c, d, b);
    str(c, a, d, b);
  } else if (a == rmsk) {
    str(b, a, c, d);
    str(b, c, a, d);
    str(b, c, d, a);
    str(a, b, c, d);
  } else {
    str(a, b, c, d);
  }
  return ret;
}

umap<board, int> inEdges(const board &cur, int pType) {
  umap<board, int> ret;
  piece p(pType);
  for(lll grid: swapRows(cur.grid)) {
    for(int t = 0; t < 4; ++t) {
      p.dt = t;
      for(int i = -2; i < W; ++i) {
        p.dxy.x = i;
        for(int j = -2; j < H; ++j) {
          p.dxy.y = j;
          lll pp = p.blocks();
          if ((pp & border) == 0) {
            if ((pp & grid) == pp) {
              board b;
              b.grid = grid & ~pp;
              if (outEdges(b, pType)[cur]) {
                ret[b] = 1;
              }
            }
          }
        }
      }
    }
  }
  return ret;
}

void precompute() {

}


piece getBestMove(board b, vector<int> pieces) {
  vector<piece> v = getMoves(b, pieces[0]);
  printf("Found %lu moves\n", v.size());
  piece bestMove = v.back();
  return bestMove;
}
