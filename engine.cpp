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

int getHeight(gameState &gameState) {
  for (int i = H - 1; i >= 0; --i) {
    if ((gameState.b.grid & rowmsk(i)) != 0) {
      return i;
    }
  }
  return -1;

}

bool badHeight(gameState &gameState) {
  for (int i = 4; i < H; ++i) {
    if ((gameState.b.grid & rowmsk(i)) != 0) {
      return true;
    }
  }
  return false;
}

bool badMod4(gameState &gameState) {
  board &b = gameState.b;
  int cur = 0;
  for(int i = 0; i < W; ++i) {
    int cnt = 0;
    for(int j = 0; j < 4; ++j) {
      if (b.get(i, j)) {
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
  board &b = gameState.b;
  for(int i = 0; i < W; ++i) {
    int cnt = 0;
    int cur;
    for(int j = 0; j < 4; ++j) {
      if (b.get(i, j)) {
        ++cnt;
      } else {
        cur = j;
      }
    }
    if (cnt == 3) {
      if ((i + 1 == W || b.get(i + 1, cur)) &&
          (i == 0 || b.get(i - 1, cur))) {
        return true;
      }
    }
  }

  return false;
}

int countHoles(gameState &gameState) {
  board &b = gameState.b;
  int holes = 0;
  for(int i = 0; i < W; ++i) {
    for(int j = 0; j < 4; ++j) {
      if (!b.get(i, j) && b.get(i, j + 1)) {
        if ((i + 1 == W || b.get(i + 1, j)) &&
            (i == 0 || b.get(i - 1, j))) {
          ++holes;
        }
      }
    }
  }

  return holes;
}

int countRowDependencies(gameState &gameState) {
  board &b = gameState.b;
  int holes = 0;
  for(int i = 0; i < W; ++i) {
    for(int j = 0; j < 4; ++j) {
      if (!b.get(i, j) && b.get(i, j + 1)) {
        if ((i + 1 == W || b.get(i + 1, j)) &&
            (i == 0 || b.get(i - 1, j))) {
          ++holes;
        }
      }
    }
  }

  return holes;
}

int countNeeded(gameState &gameState) {
  board &b = gameState.b;
  int cnt = 0;
  int height = 0;
  for(int i = 0; i < W; ++i) {
    for(int j = 0; j < 4; ++j) {
      if (b.get(i, j)) {
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
    return -10 - getHeight(gameState);
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


pair<piece, double> getMostPopular(const vector<gameState> &gameStates) {
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
  double ratio =  counts[m] / max(1., total);
  printf("mostPopular: %lf, %lf\n",  counts[m], ratio);
  return make_pair(m, ratio);
}

/*
global cache hit rate 0.449898
hardQueries: 618, cacheMisses: 1083343
wins: 92, totalQueries: 975; ratio: 0.094359
*/

int earlyExitPopular = -1;
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
  if (depth == 0 || (depth <= earlyExitPopular && getMostPopular(cur).second > 0.3)) {
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
  printf("nxt gamestates: %d\n", int(nxt.size()));
  for(int i = 0; i < 4; ++i) {
    checkpoints[i] = 0;
  }
  vector<pii> scores(sz(nxt));
  for (int i = 0; i < sz(nxt); ++i) {
    scores[i] = pii(-getScore(nxt[i]), i);
    printf("%d: %d\n", i, scores[i].first);
  }
  sort(scores.begin(), scores.end());
  printf("Score: %d\n", scores[0].first);
  vector<gameState> filtered;
  for(int i = 0; i < beamSearchLimit; ++i) {
    if (i < sz(scores) && (scores[i].first <= 0 || i < 100)) {
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

board lastB;
vector<int> lastP;
vector<piece> lastT;

ll wins = 0;
ll totalQueries = 0;
ll hardQueries = 0;
engineResult getBestMove(board b, vector<int> pieces) {
  ++totalQueries;
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
  ++hardQueries;
  // beam search
  for(int i = 0; i < 4; ++i) {
    checkpoints[i] = 0;
  }

  vector<gameState> inp(1);
  inp[0] = {b, pieces};
  int tmpBeamSearchLimit = beamSearchLimit;
  /* global cache hit rate 0.416820
  hardQueries: 65, cacheMisses: 105538
  wins: 12, totalQueries: 111; ratio: 0.108108 */
  beamSearchLimit = 150; // ratio of cacheMisses:hardQueries = 2000 with this optimization
  // ratio = 1576 with additional early exit
  // ratio of cacheMisses:hardQueries = 3200 without
  earlyExitPopular = -1;

  searchResult res = beamSearch(inp, sz(pieces), true);
  beamSearchLimit = tmpBeamSearchLimit;
  printf("global cache hit rate %lf\n", 1. * cacheHits / (cacheHits + cacheMisses));
  printf("hardQueries: %lld, cacheMisses: %lld\n", hardQueries, cacheMisses);
  printf("wins: %lld, totalQueries: %lld; ratio: %lf\n", wins, totalQueries, 1. * wins / totalQueries);
  if (!res.failed) {
    printf("IT IS POSSIBLE\n");
    ++wins;
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
    earlyExitPopular = 2;
    res = beamSearch(inp, min(5, sz(pieces)), true);
    piece p = getMostPopular(res.gameStates).first;
    if (!p.isNull()) {
      return {p, 0};
    } else {
      printf("piece %d is null :(\n", p.pieceType);
      vector<piece> v = getMoves(b, pieces[0]);
      printf("Found %lu moves\n", v.size());
      piece bestMove = v.back();
      return {bestMove, -1};
    }
  }
}
