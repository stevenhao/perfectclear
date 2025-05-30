#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <random>
#include "engineutils.cpp"

#define sz(x) (int((x).size()))
using namespace std;
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

struct gameState {
  board b;
  vector<int> queue;
  vector<piece> trace;
  int bag;  // what's still in the bag?
  int index;
  int score;
  bool isCleared() { return b.grid == 0; }
};

struct searchResult {
  vector<gameState> gameStates;
  bool failed;
};

int getHeight(board &b) {
  for (int i = H - 1; i >= 0; --i) {
    if ((b.grid & rowmsk(i)) != 0) {
      return i;
    }
  }
  return -1;
}

bool badHeight(board &b) {
  for (int i = 4; i < H; ++i) {
    if ((b.grid & rowmsk(i)) != 0) {
      return true;
    }
  }
  return false;
}

bool badMod4(board &b) {
  int cur = 0;
  int cnt = 0;
  int height = 0;
  for (int i = 0; i < W; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (b.get(i, j)) {
        cnt++;
        if (j + 1 > height) {
          height = j + 1;
        }
      }
    }
  }

  int v = height * 10 - cnt;
  if (v % 4 != 0) v += 10, height += 1;
  if (height <= 2) height += 2;
  for (int i = 0; i < W; ++i) {
    cnt = 0;
    for (int j = 0; j < height; ++j) {
      if (b.get(i, j)) {
        ++cnt;
      } else {
        ++cur;
      }
    }
    if (cnt == height) {
      if (cur % 4 != 0) return true;
    }
  }

  return false;
}

bool badHoles(board &b) {
  for (int i = 0; i < W; ++i) {
    int cnt = 0;
    int cur;
    for (int j = 0; j < 4; ++j) {
      if (b.get(i, j)) {
        ++cnt;
      } else {
        cur = j;
      }
    }
    if (cnt == 3) {
      if ((i + 1 == W || b.get(i + 1, cur)) && (i == 0 || b.get(i - 1, cur))) {
        return true;
      }
    }
  }

  return false;
}

int countHoles(board &b) {
  int holes = 0;
  for (int i = 0; i < W; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (!b.get(i, j) && b.get(i, j + 1)) {
        if ((i + 1 == W || b.get(i + 1, j)) && (i == 0 || b.get(i - 1, j))) {
          ++holes;
        }
      }
    }
  }

  return holes;
}

int countHoles2(board &b) {
  int holes = 0;
  for (int i = 0; i < W; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (!b.get(i, j) && b.get(i, j + 1)) {
        if ((i + 1 == W || b.get(i + 1, j)) || (i == 0 || b.get(i - 1, j))) {
          ++holes;
        }
      }
    }
  }

  return holes;
}

int countRowDependencies(board &b) {
  int holes = 0;
  for (int i = 0; i < W; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (!b.get(i, j) && b.get(i, j + 1)) {
        if ((i + 1 == W || b.get(i + 1, j)) && (i == 0 || b.get(i - 1, j))) {
          ++holes;
        }
      }
    }
  }

  return holes;
}

int countNeeded(board &b) {
  int cnt = 0;
  int height = 0;
  for (int i = 0; i < W; ++i) {
    for (int j = 0; j < 4; ++j) {
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

char BOOK_PATH[] = "book_100k.txt";
vector<unordered_map<ll, unordered_map<int, double>>> book(10);
void loadBook() {
  printf("Loading book from %s...\n", BOOK_PATH);
  ll key;
  double score;
  int pmsk;
  int lines = 0, positions = 0;
  int needed;
  ifstream fin(BOOK_PATH);
  while (fin >> needed >> key >> pmsk >> score) {
    if (!book[needed].count(key)) ++positions;
    book[needed][key][pmsk] = score;
    ++lines;
  }
  printf("Loaded %d lines and %d unique positions...\n", lines, positions);
}

double getBookScore(gameState &g, bool verbose = false) {
  board &b = g.b;
  vector<int> &queue = g.queue;
  if (verbose) {
    disp(b);
    cout << "PIECES: ";
    for (int i : queue) {
      cout << i << " ";
    }
    cout << "\n";
  }
  ll bookKey = b.toLL();
  double ans = 0;
  int pmsk = 0;
  if (sz(queue)) pmsk = 1 << queue[0];
  int freedom = 0;
  for (int needed = 1; needed < sz(book); ++needed) {
    if (verbose) {
      cout << needed << " " << needed << " Book key " << bookKey << " msk "
           << pmsk << "\n";
    }
    if (needed < sz(queue))
      pmsk |= 1 << queue[needed];
    else {
      freedom += 1;
    }
    if (book[needed].count(bookKey) == 0) {
      continue;
    }
    double res = 1;
    for (pii p : book[needed][bookKey]) {
      int missing = __builtin_popcount(p.first & ~pmsk);
      if (missing > freedom || (p.first & ~(pmsk | ~g.bag))) continue;
      res *= max(0., 1 - p.second * pow(0.3, missing));
    }
    ans += (1 - res) * (needed > 1 ? .7 : 1);
  }

  return ans;  // likelihood of winning from current position given pmsk
}

int getScore(gameState gameState) {
  board b = gameState.b;
  if (b.grid == 0) return 1000000 * 10;
  // check for immediate fails
  if (badHeight(b)) {
    return -10 - getHeight(b);
  }
  if (badMod4(b)) {
    return -2;
  }
  if (badHoles(b)) {
    return -1;
  }

  int needed = countNeeded(b);
  int holes = countHoles(b);
  int holes2 = countHoles2(b);
  int rowDependencies = countRowDependencies(b);

  // return a heuristic?
  return 1 * 1000000 + getBookScore(gameState) * 500 - 30 * needed -
         40 * holes - 10 * holes2 - 10 * rowDependencies + (rng() % 10);
}

vector<gameState> getNextGameStates(const gameState &g) {
  vector<gameState> result;
  vector<int> queue = g.queue;
  vector<piece> trace = g.trace;
  board bb = g.b;
  for (int i = 0; i < 2; ++i) {
    if (i >= sz(g.queue)) continue;
    vector<piece> moves = getMoves(bb, queue[i]);
    for (auto move : moves) {
      board nb = bb;
      nb.add(move);
      vector<int> nq = queue;
      nq.erase(nq.begin() + i);
      vector<piece> tr = trace;
      tr.push_back(move);
      result.push_back({nb, nq, tr, g.index});
    }
  }
  return result;
}

const int DEFAULT_SEARCH_BREADTH = 500;
int beamSearchLimit = 1000;
void setBeamSearchLimit(int limit) {
  beamSearchLimit = limit;
  printf("Beam search limit is now %d\n", limit);
}

vector<piece> getMostPopular(const vector<gameState> &gameStates, int num) {
  piece m;
  umap<piece, double> counts;
  double total = 0;
  double f = 1;
  for (auto &g : gameStates) {
    if (!sz(g.trace)) continue;
    piece p = g.trace[0];
    counts[p] += f;
    total += g.score;
  }
  vector<piece> pieces;
  umap<piece, int> idx;
  for (auto p : counts) {
    idx[p.first] = sz(pieces);
    pieces.push_back(p.first);
  }
  vector<pair<double, int>> sorted;
  for (auto p : counts) {
    sorted.push_back({p.second, idx[p.first]});
  }
  sort(sorted.begin(), sorted.end());
  reverse(sorted.begin(), sorted.end());
  sorted.resize(num);
  vector<piece> res;
  for (auto p : sorted) {
    res.push_back(pieces[p.second]);
  }
  return res;
}

/*
global cache hit rate 0.449898
hardQueries: 618, cacheMisses: 1083343
wins: 92, totalQueries: 975; ratio: 0.094359
*/

int earlyExitPopular = -1;
searchResult beamSearch(const vector<gameState> cur, int depth,
                        bool first = false) {
  if (!first) {
    for (auto g : cur) {
      if (g.isCleared()) {
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
  unordered_set<pair<lll, int>> seen;
  for (auto g : cur) {
    vector<gameState> lst = getNextGameStates(g);
    nxt.insert(nxt.end(), lst.begin(), lst.end());
  }
  vector<gameState> nnxt = nxt;
  nxt.clear();
  for (gameState &g : nnxt) {
    pair<lll, int> p(g.b.grid, g.queue.size() ? g.queue[0] : -1);
    if (seen.count(p)) continue;
    seen.insert(p);
    nxt.push_back(g);
  }
  seen.clear();
  vector<pii> scores(sz(nxt));
  for (int i = 0; i < sz(nxt); ++i) {
    scores[i] = pii(-getScore(nxt[i]), i);
    nxt[i].score = scores[i].first;
  }
  sort(scores.begin(), scores.end());
  vector<gameState> filtered;
  for (int i = 0; i < beamSearchLimit; ++i) {
    if (i < sz(scores) && (scores[i].first <= 0 || i < 100)) {
      int idx = scores[i].second;
      nxt[idx].index = max(nxt[idx].index, i);
      filtered.push_back(nxt[idx]);
    }
  }
  searchResult nextRes = beamSearch(filtered, depth - 1);
  if (!nextRes.failed) return nextRes;
  if (getBookScore(filtered[0]) >= 1) {
    searchResult s;
    s.gameStates = {filtered[0]};
    s.failed = true;
    return s;
  }
  return nextRes;
}

struct engineResult {
  vector<piece> moves;
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

engineResult _getBestMove(board b, vector<int> pieces) {
  vector<gameState> inp = {{b, pieces}};
  searchResult res = beamSearch(inp, sz(pieces), true);
  if (!res.failed) {
    gameState g = res.gameStates[0];
    piece p = g.trace[0];
    return {g.trace, 1};
  } else {
    vector<piece> shortlist = getMostPopular(res.gameStates, 3);
    for (piece p : shortlist) {
      searchResult res = beamSearch(inp, sz(pieces), true);
      if (!res.failed) {
        return {res.gameStates[0].trace, 1};
      }
    }
    return {{shortlist[0]}, 0};
  }
}

ll wins = 0;
ll totalQueries = 0;
ll hardQueries = 0;

engineResult getBestMove(board b, vector<int> pieces) {
  ++totalQueries;
  if (b.grid == lastB.grid && sz(lastP) && sz(pieces) && eq(pieces, lastP) &&
      sz(lastT)) {
    vector<piece> moves = lastT;
    piece p = lastT[0];
    lastB.add(p);
    lastT.erase(lastT.begin());
    if (p.pieceType == lastP[0]) {
      lastP.erase(lastP.begin());
    } else if (sz(lastP) > 1) {
      lastP.erase(lastP.begin() + 1);
    }
    if (lastB.count() == 0) {
      ++wins;
    }
    return {moves, 1};
  }
  ++hardQueries;

  engineResult res = _getBestMove(b, pieces);
  piece p = res.moves[0];
  lastB = b;
  lastP = pieces;
  lastT = res.moves;
  lastB.add(p);
  if (p.pieceType == pieces[0]) {
    lastP.erase(lastP.begin());
  } else {
    lastP.erase(lastP.begin() + 1);
  }
  lastT.erase(lastT.begin());
  if (lastB.count() == 0) {
    ++wins;
  }
  return res;
}
