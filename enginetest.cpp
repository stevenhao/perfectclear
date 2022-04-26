#include "engine.cpp"
#include "steven.hpp"

void go(int n) {
  board b;
  cout << "running " << n << " trials\n";
  for (int i = 0; i < n; ++i) {
    vector<int> p({0, 1, 3, 4, 5, 6});
    if (i != 0) {
      for (int j = 0; j < sz(p); ++j) {
        swap(p[j], p[rand() % (j + 1)]);
      }
    }
    gameState g = {b, p};
    vector<gameState> v(1);
    v[0] = g;
    searchResult s = beamSearch(v, sz(g.queue), true);
    printf("bfscount is %d\n", bfsCount);
    printf("memo1 size is %d\n", int(memo1.size()));
    printf("memo2 size is %d\n", int(memo2.size()));
    printf("cache hits is %d\n", int(cacheHits));
    printf("cache misses is %d\n", int(cacheMisses));

    if (s.failed) {
      cout << "FAILED!" << endl;
      disp(s.gameStates[0].b);
      pp(s.gameStates[0].b.toLL());
      pp(s.gameStates[0].queue);
      pp(getBookScore(s.gameStates[0]));
      int score = getScore({s.gameStates[0].b, {}, {}, 0});
      pp(score);
    } else {
      cout << "SUCCESS!" << endl;
      for (piece p : s.gameStates[0].trace) {
        disp(g.b, p);
        g.b.add(p);
        if (p.pieceType == g.queue[1]) {
          swap(g.queue[0], g.queue[1]);
        }
        g.queue.erase(g.queue.begin());
        int score = getScore({g.b, {}, {}, 0});
        pp(g.b.toLL());
        pp(g.queue);
        pp(getBookScore(g));
        pp(score);
      }
    }
  }
}

int main(int argc, char **argv) {
  loadData();
  loadBook();

  if (argc >= 2) {
    go(atoi(argv[1]));
  } else {
    go(1);
  }
}
