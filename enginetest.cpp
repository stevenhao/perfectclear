#include "engine.cpp"
void go() {
  board b;
  vector<int> p({0, 1, 2, 3, 4, 5});
  gameState g = {b, p};
  can(g, true);
}

int main() {
  loadData();
  go();
}
