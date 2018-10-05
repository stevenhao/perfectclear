#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "engineutils.cpp"

#define sz(x) (int((x).size()))
using namespace std;

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

bool badHeight(gameState &gameState) {
  for (int i = 4; i < H; ++i) {
    if ((gameState.board.grid & rowmsk(i)) != 0) {
      return true;
    }
  }
  return false;
}

bool badMod4(gameState &gameState) {
  board &board = gameState.board;
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
  board &board = gameState.board;
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
  board &board = gameState.board;
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
  board &board = gameState.board;
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
  board &board = gameState.board;
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
    return -1;
  }
  checkpoints[1]++;
  if (badMod4(gameState)) {
    return -1;
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

const int BEAM_SEARCH_LIMIT = 500;

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
    gameState g = noGame;
    if (sz(cur) > 0) {
      g = cur[0];
    }
    g.failed = true;
    return g;
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
  for(int i = 0; i < 4; ++i) {
    checkpoints[i] = 0;
  }
  vector<pii> scores(sz(nxt));
  for (int i = 0; i < sz(nxt); ++i) {
    scores[i] = pii(-getScore(nxt[i]), i);
  }
  sort(scores.begin(), scores.end());

  printf("Checkpoints:\n");
  for(int i = 0; i < 4; ++i) {
    printf("%d\t", checkpoints[i]);
  }
  printf("\n");

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
  return beamSearch(v, sz(g.queue), skip);
}

struct engineResult {
  piece move;
  int happy;
};

engineResult getBestMove(board b, vector<int> pieces) {
  // beam search
  for(int i = 0; i < 4; ++i) {
    checkpoints[i] = 0;
  }
  printf("beam search %d %d\n", pieces[0], pieces[1]);
  for(int i = 0; i < 4; ++i) {
    printf("%d\t", checkpoints[i]);
  }
  printf("\n");
  gameState g = can({b, pieces}, true);
  if (!g.failed) {
    printf("IT IS POSSIBLE\n");
    return {g.trace[0], 1};
  } else {
    printf("BAD\n");
    if (sz(g.trace)) {
      return {g.trace[0], 0};
    }
  }
  vector<piece> v = getMoves(b, pieces[0]);
  printf("Found %lu moves\n", v.size());
  piece bestMove = v.back();
  return {bestMove, -1};
}
