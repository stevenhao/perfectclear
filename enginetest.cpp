#include "engine.cpp"
void go() {
  board b;
  vector<int> p({0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3});
  gameState g = {b, p};
  can(g, true);
}

int main() {
  loadData();
  printf("beam search %d %d\n", pieces[0], pieces[1]);

  go();
}
