#include "engine.cpp"
void go() {
  board b;
  vector<int> p({0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3});
  gameState g = {b, p};
  vector<gameState> v(1);
  v[0] = g;
  beamSearch(v, sz(g.queue), true);
  printf("bfscount is %d\n", bfsCount);
  printf("memo1 size is %d\n", int(memo1.size()));
  printf("memo2 size is %d\n", int(memo2.size()));
  printf("cache hits is %d\n", int(cacheHits));
  printf("cache misses is %d\n", int(cacheMisses));
}

int main() {
  loadData();

  go();
}
