#include "tetrisio.cpp"
using namespace std;

typedef long long ll;

board b;
void rot(piece &p, int dir=0) {
  p = apply(dir, p, b);
}
void mov(piece &p, int dir=0) {
  p = apply(2 + dir, p, b);
}
void disp(piece &p) {
  cout << p.pieceType 
    << " " << p.dxy.x 
    << " " << p.dxy.y 
    << " "<< p.dt << "\n";
}

int main() {
  loadData();
  disp(b);
  piece p(0);
    disp(b, p);
  while (b.fits(p)) {
    disp(b, p);
    cerr << "dxy:" << p.dxy.x << "," << p.dxy.y << "\n";
    p = trans(p, DOWN);
  }


  p = piece(0);
  disp(b, p);
  mov(p, 2);
  disp(b, p);
}
