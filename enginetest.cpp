#include "engine.cpp"
void go() {
  board b;
  vector<int> p({0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3});
  gameState g = {b, p};
  vector<gameState> v(1);
  v[0] = g;
  beamSearch(v, sz(g.queue));

}

int main() {
  loadData();

  go();
}
