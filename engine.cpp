#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "engineutils.cpp"

#define sz(x) (int((x).size()))
using namespace std;

struct gameState {
  board b;
  vector<int> queue;
  vector<piece> trace;
  bool isCleared() {
    return b.grid == 0;
  }
};

struct searchResult {
  vector<gameState> gameStates;
  bool failed;
};

bool badHeight(gameState &gameState) {
  for (int i = 4; i < H; ++i) {
    if ((gameState.b.grid & rowmsk(i)) != 0) {
      return true;
    }
  }
  return false;
}

bool badMod4(gameState &gameState) {
  board &board = gameState.b;
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
      if (cur % 4 != 0) return true;
    }
  }

  return false;
}

bool badHoles(gameState &gameState) {
  board &board = gameState.b;
  for(int i = 0; i < W; ++i) {
    int cnt = 0;
    int cur;
    for(int j = 0; j < 4; ++j) {
      if (board.get(i, j)) {
        ++cnt;
      } else {
        cur = j;
      }
    }
    if (cnt == 3) {
      if ((i + 1 == W || board.get(i + 1, cur)) &&
          (i == 0 || board.get(i - 1, cur))) {
        return true;
      }
    }
  }

  return false;
}

int countHoles(gameState &gameState) {
  board &board = gameState.b;
  int holes = 0;
  for(int i = 0; i < W; ++i) {
    for(int j = 0; j < 4; ++j) {
      if (!board.get(i, j) && board.get(i, j + 1)) {
        if ((i + 1 == W || board.get(i + 1, j)) &&
            (i == 0 || board.get(i - 1, j))) {
          ++holes;
        }
      }
    }
  }

  return holes;
}

int countRowDependencies(gameState &gameState) {
  board &board = gameState.b;
  int holes = 0;
  for(int i = 0; i < W; ++i) {
    for(int j = 0; j < 4; ++j) {
      if (!board.get(i, j) && board.get(i, j + 1)) {
        if ((i + 1 == W || board.get(i + 1, j)) &&
            (i == 0 || board.get(i - 1, j))) {
          ++holes;
        }
      }
    }
  }

  return holes;
}

int countNeeded(gameState &gameState) {
  board &board = gameState.b;
  int cnt = 0;
  int height = 0;
  for(int i = 0; i < W; ++i) {
    for(int j = 0; j < 4; ++j) {
      if (board.get(i, j)) {
        cnt++;
        if (j + 1 > height) {
          height = j + 1;
        }

      }
    }
  }

  int v = height * 10 - cnt;
  if (v % 4 != 0) v += 10;
  return v / 4;
}

int checkpoints[10];

int getScore(gameState gameState) {
  checkpoints[0]++;
  // check for immediate fails
  if (badHeight(gameState)) {
    return -3;
  }
  checkpoints[1]++;
  if (badMod4(gameState)) {
    return -2;
  }
  checkpoints[2]++;
  if (badHoles(gameState)) {
    return -1;
  }
  checkpoints[3]++;

  int holes = countHoles(gameState);
  int rowDependencies = countRowDependencies(gameState);
  int needed = countNeeded(gameState);

  // return a heuristic?
  return 1000000 - 1000 * needed - 40 * holes - 10 * rowDependencies + (rand() % 100);
}

vector<gameState> getNextGameStates(const gameState &g) {
  vector<gameState> result;
  vector<int> queue = g.queue;
  vector<piece> trace = g.trace;
  board bb = g.b;
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

const int DEFAULT_SEARCH_BREADTH = 500;
int beamSearchLimit = 500;
void setBeamSearchLimit(int limit) {
  beamSearchLimit = limit;
  printf("Beam search limit is now %d\n", limit);
}

searchResult beamSearch(const vector<gameState> cur, int depth, bool first=false) {
  if (!first) {
    for (auto g: cur) {
      if (g.isCleared()) {
        printf("IT IS CLEARED!");
        searchResult s;
        s.gameStates.push_back(g);
        s.failed = false;
        return s;
      }
    }
  }
  if (depth == 0) {
    searchResult s;
    s.gameStates = cur;
    s.failed = true;
    return s;
  }
  vector<gameState> nxt;
  for (auto g: cur) {
    vector<gameState> lst = getNextGameStates(g);
    nxt.insert(nxt.end(), lst.begin(), lst.end());
  }
  for(int i = 0; i < 4; ++i) {
    checkpoints[i] = 0;
  }
  vector<pii> scores(sz(nxt));
  for (int i = 0; i < sz(nxt); ++i) {
    scores[i] = pii(-getScore(nxt[i]), i);
  }
  sort(scores.begin(), scores.end());

  vector<gameState> filtered;
  for(int i = 0; i < beamSearchLimit; ++i) {
    if (i < sz(scores)) {
      int idx = scores[i].second;
      filtered.push_back(nxt[idx]);
    }
  }
  return beamSearch(filtered, depth - 1);
}

struct engineResult {
  piece move;
  int happy;
};

bool eq(vector<int> q1, vector<int> q2) {
  if (sz(q1) < 2 || sz(q2) < 2) return false;
  if (q1[0] == q2[0] && q1[1] == q2[1]) {
    return true;
  }
  if (q1[0] == q2[1] && q1[1] == q2[0]) return true;
  return false;
}

piece getMostPopular(vector<gameState> &gameStates) {
  piece m;
  umap<piece, double> counts;
  double total = 0;
  double f = 1;
  for (auto &g: gameStates) {
    if (!sz(g.trace)) continue;
    piece p = g.trace[0];
    counts[p] += f;
    total += f;
    f *= 0.9;
    if (counts[p] > counts[m]) {
      m = p;
    }
  }
  if (total) {
    printf("mostPopular: %lf, %lf\n",  counts[m], counts[m] / (total));
  }
  return m;
}

board lastB;
vector<int> lastP;
vector<piece> lastT;

engineResult getBestMove(board b, vector<int> pieces) {
  if (b.grid == lastB.grid && sz(lastP) && sz(pieces) && eq(pieces, lastP) && sz(lastT)) {
    printf("short circuiting\n");
    piece p = lastT[0];
    lastT.erase(lastT.begin());
    lastB.add(p);
    if (p.pieceType == lastP[0]) {
      lastP.erase(lastP.begin());
    } else if (sz(lastP) > 1) {
      lastP.erase(lastP.begin() + 1);
    }
    return {p, 1};
  }
  // beam search
  for(int i = 0; i < 4; ++i) {
    checkpoints[i] = 0;
  }

  vector<gameState> inp(1);
  inp[0] = {b, pieces};
  searchResult res = beamSearch(inp, sz(pieces), true);
  if (!res.failed) {
    printf("IT IS POSSIBLE\n");
    gameState g = res.gameStates[0];
    lastB = b;
    lastP = pieces;
    lastT = g.trace;
    piece p = lastT[0];
    lastB.add(p);
    if (p.pieceType == pieces[0]) {
      lastP.erase(lastP.begin());
    } else {
      lastP.erase(lastP.begin() + 1);
    }
    lastT.erase(lastT.begin());
    return {p, 1};
  } else {
    printf("BAD\n");
    piece p = getMostPopular(res.gameStates);
    if (!p.isNull()) {
      return {p, 0};
    } else {
      printf("piece %d is null :(\n", p.pieceType);
    }
  }
  vector<piece> v = getMoves(b, pieces[0]);
  printf("Found %lu moves\n", v.size());
  piece bestMove = v.back();
  return {bestMove, -1};
}
