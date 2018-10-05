#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "engineutils.cpp"

#define sz(x) (int((x).size()))
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
        for(int j = -3; j < H; ++j) {
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

struct gameState {
  board board;
  vector<int> queue;
  vector<piece> trace;
  bool failed;
  bool isCleared() {
    return board.grid == 0;
  }
};

gameState noGame({ board(), vector<int>(), vector<piece>() ,true });

int getScore(gameState gameState) {
  board board = gameState.board;
  lll grid = board.grid;
  for (int i = 4; i < H; ++i) {
    if ((grid & rowmsk(i)) != 0) {
      return 0;
    }
  }
  int cur = 0;
  for(int i = 0; i < W; ++i) {
    int cnt = 0;
    for(int j = 0; j < 4; ++j) {
      if (board.get(i, j)) {
        ++cnt;
      } else {
        ++cur;
      }
    }
    if (cnt == 4) {
      if (cur % 4 != 0) return 0;
    }
  }
  // score by... height?
  return rand();
}

vector<gameState> getNextGameStates(const gameState &g) {
  vector<gameState> result;
  vector<int> queue = g.queue;
  vector<piece> trace = g.trace;
  board bb = g.board;
  for (int i = 0; i < 2; ++i) {
    if (i >= sz(g.queue)) continue;
    vector<piece> moves = getMoves(bb, queue[i]);
    for (auto move: moves) {
      board nb = bb;
      nb.add(move);
      vector<int> nq = queue;
      nq.erase(nq.begin() + i);
      vector<piece> tr = trace;
      tr.push_back(move);
      result.push_back({ nb, nq, tr });
    }
  }
  return result;
}

const int BEAM_SEARCH_LIMIT = 5000;

gameState beamSearch(const vector<gameState> cur, int depth, bool first=false) {
  printf("BEAMSEARCH %d %d\n", sz(cur), depth);
  if (!first) {
    for (auto g: cur) {
      if (g.isCleared()) {
        printf("IT IS CLEARED!");
        return g;
      }
    }
  }
  if (depth == 0) {
    return noGame;
  }
  for(int i = 0; i < 2; ++i) {
    if (i < sz(cur)) {
      disp(cur[i].board);
    }
  }
  vector<gameState> nxt;
  for (auto g: cur) {
    vector<gameState> lst = getNextGameStates(g);
    nxt.insert(nxt.end(), lst.begin(), lst.end());
  }
  vector<pii> scores(sz(nxt));
  for (int i = 0; i < sz(nxt); ++i) {
    scores[i] = pii(-getScore(nxt[i]), i);
  }
  sort(scores.begin(), scores.end());

  vector<gameState> filtered;
  for(int i = 0; i < BEAM_SEARCH_LIMIT; ++i) {
    if (i < sz(scores)) {
      if (i < 10) printf("score=%d\t", scores[i].first);
      int idx = scores[i].second;
      filtered.push_back(nxt[idx]);
    }
  }
  return beamSearch(filtered, depth - 1);
}

gameState can(const gameState &g, bool skip=false) {
  vector<gameState> v(1);
  v[0] = g;
  return beamSearch(v, 7, skip);
}

piece getBestMove(board b, vector<int> pieces) {
  // beam search
  printf("beam search %d %d\n", pieces[0], pieces[1]);
  gameState g = can({b, pieces}, true);
  if (!g.failed) {
    printf("IT IS POSSIBLE\n");
    return g.trace[0];
  } else {
    printf("BAD\n");
  }
  vector<piece> v = getMoves(b, pieces[0]);
  printf("Found %lu moves\n", v.size());
  piece bestMove = v.back();
  return bestMove;
}
