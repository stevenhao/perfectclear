#include "engine.cpp"
#include "steven.hpp"

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

vector<int> getBag() {
  vector<int> res;
  for (int i = 0; i < 7; ++i) {
    res.push_back(i);
    swap(res[i], res[rng() % (i + 1)]);
  }
  return res;
}

vector<int> generate(int count) {
  // hold is random
  // rest is 7-bag
  vector<int> res;
  res.push_back(rng() % 7);
  vector<int> bag = getBag();
  bag.erase(bag.begin() + (rng() % 7), bag.end());
  while (sz(res) < count) {
    res.append(bag.back());
    bag.pop_back();
    if (!sz(bag)) {
      bag = getBag();
    }
  }
  return res;
}

void go(int n) {
  cout << "running " << n << " trials\n";
  for (int i = 0; i < n; ++i) {
    vector<int> queue = generate(11);
    board b;
    for (int j = 0; j < 10; ++j) {
      gameState g = {b, p};
      vector<gameState> v(1);
      v[0] = g;
      searchResult s = beamSearch(v, sz(g.queue), true);
      printf("bfscount is %d\n", bfsCount);
      printf("memo1 size is %d\n", int(memo1.size()));
      printf("memo2 size is %d\n", int(memo2.size()));
      printf("cache hits is %d\n", int(cacheHits));
      printf("cache misses is %d\n", int(cacheMisses));
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
