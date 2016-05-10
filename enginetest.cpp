#include "engine.cpp"

board b;
piece p;
int main() {
  loadData();
  precompute();
  disp(b);

  vector<piece> v = getMoves(b, 0);
  piece rnd = v.back();
  disp(b, rnd);
  cout << "getting path\n";
  vector<int> moves = getPath(b, rnd);
  p = piece(0);
  disp(b, p);
  for(int m: moves) {
    p = apply(m, p, b);
    disp(b, p);
  }
}
