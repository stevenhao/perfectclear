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
  piece p(1);
  mov(p, 1);
  mov(p, 2);
  rot(p);
  rot(p);
  disp(b, p);
  b.add(p);
  piece q(2);
  rot(q);
  rot(q);
  mov(q, 0);
  mov(q, 0);
  mov(q, 0);
  mov(q, 2);
  disp(b, q);
  b.add(q);

  piece r(3);
  mov(r, 1);
  mov(r, 1);
  piece z = r;
  mov(z, 2);
  disp(b, z);
  b.add(z);
  mov(r, 1);
  mov(r, 1);
  mov(r, 2);
  disp(b, r);
  b.add(r);
  disp(b);
}
