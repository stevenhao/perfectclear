#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

#include "tetris.cpp"

string pieceNames = "IJLOSTZ";
int getPieceIndex(string s) {
  for (int i = 0; i < 7; ++i) {
    if (pieceNames[i] == s[0]) return i;
  }
  return -1;
}

void loadPieces() {
  ifstream fin("pieces", ifstream::in);
  int buf[MAXN][MAXN], nbuf[MAXN][MAXN];
  for (int i = 0; i < 7; ++i) {
    int size;
    fin >> size >> size;
    for (int y = 0; y < size; ++y) {
      string s;
      fin >> s;
      for (int x = 0; x < size; ++x) {
        buf[x][size - 1 - y] = s[x] == '+';
      }
    }

    for (int t = 0; t < 4; ++t) {
      lll &msk = pieces[i][t];
      for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
          if (buf[x][y]) {
          }
          _set(msk, abspos(x, y), buf[x][y]);
        }
      }

      for (int x = 0; x < size; ++x) {    // rotate buf
        for (int y = 0; y < size; ++y) {  // (x, y) -> (y, size-x-1)
          nbuf[y][size - x - 1] = buf[x][y];
        }
      }
      memcpy(buf, nbuf, sizeof(buf));
    }
  }
}

void loadCenters() {
  ifstream fin("centers", ifstream::in);
  for (int i = 0; i < 7; ++i) {
    fin >> centers[i].x >> centers[i].y;
  }
}

void loadKicks() {
  ifstream fin("kicks", ifstream::in);
  for (int i = 0; i < 2; ++i) {
    for (int k = 0; k < 2; ++k) {
      for (int j = 0; j < 4; ++j) {
        int dt = j;
        vector<pii> &v = ((i == 1) ? Ikicks : Skicks)[dt][k];
        for (int t = 0; t < 5; ++t) {
          int x, y;
          fin >> x >> y;
          v.emplace_back(x, y);
        }
      }
    }
  }
}

void loadData() {
  loadPieces();
  loadCenters();
  loadKicks();
  cerr << "loaded data\n";
}

void write(lll msk, char buf[MAXN][MAXN], char ch) {
  for (int i = 0; i < W; ++i) {
    for (int j = 0; j < H; ++j) {
      if (_get(msk, pos(i, j))) {
        buf[H - 1 - j][i] = ch;
      }
    }
  }
}

void clear(char buf[MAXN][MAXN]) { write(~lll(0), buf, '.'); }

template <class T>
void print(ostream &out, vector<T> v) {
  for (int i = 0; i < v.size(); ++i) {
    cout << v[i] << " ";
  }
  cout << "\n";
}

void print(ostream &out, char buf[MAXN][MAXN]) {
  for (int i = H - 6; i < H; ++i) {
    buf[i][W] = '\0';
    cout << buf[i] << "\n";
  }
}

void disp(board b, piece move) {
  cout << "Board with piece:\n";
  char buf[MAXN][MAXN];
  clear(buf);
  write(border, buf, 'O');
  write(b.grid, buf, '*');
  write(move.blocks(), buf, 'O');
  print(cout, buf);

  //  vector<int> moves = getPath(b, move);
  //  cout << "Moves:\n";
  //  print(moves);
}

void disp(board b) {
  cout << "Board:\n";
  char buf[MAXN][MAXN];
  clear(buf);
  write(border, buf, 'O');
  write(b.grid, buf, '*');
  print(cout, buf);
}

string toString(char buf[MAXN][MAXN]) {
  string s;
  for (int i = 0; i < H; ++i) {
    buf[i][W] = '\0';
    s += buf[i];
    s += '\n';
  }
  return s;
}

string toString(board b) {
  char buf[MAXN][MAXN];
  clear(buf);
  write(border, buf, 'O');
  write(b.grid, buf, '*');
  return toString(buf);
}

string toString(piece p) {
  char buf[MAXN][MAXN];
  clear(buf);
  write(border, buf, 'O');
  write(p.blocks(), buf, '+');
  return toString(buf);
}

char _buf[100];
string repr(piece p) {
  sprintf(_buf, "%d dt=%d dx=%d dy=%d", p.pieceType, p.dt, p.dxy.x, p.dxy.y);
  return string(_buf);
}

ostream &operator<<(ostream &os, const board &b) {
  return os << "((" << b.count() << "))";
}
