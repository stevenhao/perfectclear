#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

typedef pair<int, int> pii;

const pii LEFT(-1, 0), RIGHT(1, 0), DOWN(0, -1);

pii operator+(pii a, pii b) {
  return pii(a.first + b.first, a.second + b.second);
}

pii rotate(pii a, int r) {
  if (r == 0) return a;
  else if (r == 1) return pii(a.second, -a.first);
  else if (r == 2) return pii(-a.first, -a.second);
  else return pii(-a.second, a.first);
}

/* computes moves and stuff
*/

const int MAXN = 11;
const int W = 10;
const int H = 6;

struct board {
  int grid[MAXN][MAXN];
  board (int k[MAXN][MAXN]) {
    for(int i = 0; i < W; ++i) {
      for(int j = 0; j < H; ++j) {
        grid[i][j] = k[H - j - 1][i];
      }
    }
  }
};

string pieceNames = "IJLOSTZ";
int getPieceIndex(string s) {
  for(int i = 0; i < 7; ++i) {
    if (pieceNames[i] == s[0]) return i + 1;
  }
  return -1;
}

pii pieceLocations[MAXN][4];
pii centers[MAXN];

struct piece {
  int pieceType;
  pii dxy;
  int dt;
  piece() {}
  piece(int pType) {
    pieceType = pType;
    dxy = pii();
    dt = 0;
  }

  pii block(int i) {
    pii cx = centers[pieceType];
    pii ret = cx + rotate(pieceLocations[pieceType][i], dt);
    return pii(ret.first / 2, ret.second / 2) + dxy;
  }

  int hash() {
    int dx = dxy.first, dy = dxy.second;
    return ((dx + 5) * 10 + (dy + 5)) * 4 + dt;
  }
};

vector<pii> Ikicks[5][5]; // kicks for I piece
vector<pii> Skicks[5][5]; // kicks for other

piece rot(piece p, int dt) {
  p.dt = (p.dt + dt + 4) % 4;
  return p;
}

piece trans(piece p, pii dxy) {
  p.dxy = p.dxy + dxy;
  return p;
}

bool fits(piece p, board &b) {
  for(int i = 0; i < 4; ++i) {
    pii xy = p.block(i); int x = xy.first, y = xy.second;
    if (x < 0 || y < 0 || x >= W || y >= H) return false;
    if (b.grid[x][y]) {
      return false;
    }
  }
  return true;
}

bool rest(piece &p, board &b) {
  return !fits(trans(p, DOWN), b);
}

vector<pii> getKicks(piece p, int rotType) {
  return (p.pieceType == 0 ? Ikicks : Skicks)[p.dt][rotType];
}

const int MOVES = 5;
piece apply(int move, piece cur, board &b) { // [ROT1, ROT2, LEFT, RIGHT, DROP]
  if (move == 0 || move == 1) {
    int dt = (move == 0) ? 1 : -1;
    vector<pii> k = getKicks(cur, move);
    piece n = rot(cur, dt);
    for(int i = 0; i < k.size(); ++i) {
      piece nk = trans(n, k[i]);
      if (fits(nk, b)) {
        return nk;
      }
    }
  } else if (move == 2 || move == 3) {
    pii dxy = (move == 2 ? LEFT : RIGHT);
    piece n = trans(cur, dxy);
    if (fits(n, b)) return n;
  } else if (move == 4) {
    while (!rest(cur, b)) {
      cur = trans(cur, DOWN);
    }
    return cur;
  }
  return cur;
}

bool vis[MAXN * MAXN * 5];
piece trace[MAXN * MAXN * 5];
piece queue[MAXN * MAXN * 5];

int ql, qr;
void psh(piece cur, piece prv) {
  int h = cur.hash();
  if (vis[h]) return;
  vis[h] = true;
  trace[h] = prv;
  queue[qr] = cur;
  ++qr;
}

vector<piece> bfs(board &b, piece initial) {
  memset(vis, false, sizeof(vis));
  memset(trace, false, sizeof(trace));
  memset(queue, false, sizeof(queue));
  ql = qr = 0;
  psh(initial, piece());

  vector<piece> ret;
  while (ql < qr) {
    piece cur = queue[ql]; ++ql;
    if (rest(cur, b)) {
      ret.push_back(cur);
    }
    for(int move = 0; move < MOVES; ++move) {
      piece nxt = apply(move, cur, b);
      psh(nxt, cur);
    }
  }
  return ret;
}

vector<piece> getMoves(board b, int pType) {
  piece initial = piece(pType);
  vector<piece> ret = bfs(b, initial);
  return ret;
}

vector<int> getPath(board b, int pType, piece p) { // returns info on most recent getMoves
  vector<int> ans ;
  piece initial = piece(pType);
  while (p.hash() != initial.hash()) {
    piece n = trace[p.hash()];
    int move = 0;
    for(int i = 0; i < MOVES; ++i) {
      if (apply(i, n, b).hash() == p.hash()) {
        move = i;
        break;
      }
    }
    ans.push_back(move);
    p = n;
  }
  reverse(ans.begin(), ans.end());
  return ans;
}

void loadPieceData() {
  FILE *fin = fopen("pieces", "r");
  for(int i = 1; i <= 7; ++i) {
    for(int j = 0; j < 4; ++j) {
      double x, y;
      fscanf(fin, "%lf %lf", &x, &y);
      pieceLocations[i][j] = pii(x*2, y*2);
    }
  }
  fin = fopen("centers", "r");
  for(int i = 1; i <= 7; ++i) {
    double x, y;
    fscanf(fin, "%lf %lf", &x, &y);
    centers[i] = pii(x*2,y*2);
  }

  fin = fopen("kicks", "r");
  for(int i = 0; i < 2; ++i) {
    for(int j = 0; j < 4; ++j) {
      for(int k = 0; k < 2; ++k) {
        int dt = (j + k) % 4;
        vector<pii> &v = ((i == 1) ? Ikicks : Skicks)[dt][k];
        for(int t = 0; t < 5; ++t) {
          int x, y;
          fscanf(fin, "%d %d", &x, &y);
          pii p(x, y);

          v.push_back(p);
        }
      }
    }
  }
}

void write(board b, char buf[MAXN][MAXN]) {
  for(int i = 0; i < H; ++i) {
    for(int j = 0; j < W; ++j) {
      buf[i][j] = b.grid[j][H - i - 1] ? '*' : '.';
    }
  }
}

void write(piece p, char buf[MAXN][MAXN], char ch='+') {
  for(int j = 0; j < 4; ++j) {
    pii xy = p.block(j);
    buf[H - xy.second - 1][xy.first] = ch;
  }
}

string print(char buf[MAXN][MAXN]) {
  stringstream ss;
  for(int i = 0; i < H; ++i) {
    buf[i][W] = '\0';
    ss << buf[i] << "\n";
  }
  return ss.str();
}

string disp(board b, piece move) {
  char buf[MAXN][MAXN];
  stringstream ss;
  write(b, buf);
  write(move, buf);
  ss << print(buf);
  vector<int> moves = getPath(b, move.pieceType, move);
  for(int i = 0; i < moves.size(); ++i) {
    ss << moves[i] << " ";
  }
  ss << "\n";
  return ss.str();
}

string disp(board b) {
  char buf[MAXN][MAXN];
  stringstream ss;
  write(b, buf);
  return print(buf);
}


