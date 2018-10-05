#include <vector>
#include <bitset>
#include <algorithm>
using namespace std;

typedef pair<int, int> pii;
#define x first
#define y second

namespace std {
  template <typename T, typename U> struct hash<pair<T, U>>
  {
    std::size_t operator()(const pair<T, U>& k) const
    {
      // Compute individual hash values for two data members and combine them using XOR and bit shifting
      return (hash<T>()(k.x) ^ hash<U>()(k.y));
    }
  };
}

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

const int MAXN = 15;
const int W = 10;
const int H = 10;

typedef bitset<256> lll;
typedef long long ll;

/*
 * bit tricks
 * */
inline int _get(lll a, int p) {
  return a[p];
}
inline void _set(lll &a, int p, int v) {
  if (v) a |= lll(1) << p;
  else a &= ~(lll(1) << p);
}

/*
 * grid -> bit index
 */
inline int abspos(int x, int y) { // 13 x 9 grid
  return y * 13 + x;
}

inline int pos(int x, int y) {
  return abspos(x + 3, y + 3);
}

inline lll rowmsk(int y) {
  lll ret = (ll(1) << W) - 1;
  return ret << pos(0, y);
}

lll borderMask() {
  lll ret = ~lll(0);
  for(int y = 0; y < H; ++y) {
    ret ^= rowmsk(y);
  }
  return ret;
}
const lll border = borderMask();

/*
 * piece data (loaded in tetrisio.cpp)
 */

lll pieces[MAXN][4]; // when bounding square is absolute bottom left corner
pii centers[MAXN]; // bit offset of initial bounding square
// center[p] = pos(x, y), where x,y is lowerleft corner of bounding square

struct piece {
  int pieceType;
  pii dxy;
  int dt;

  piece(int pType) : pieceType(pType) {
    dxy = centers[pType];
    dxy.y += H - 6;
    dt = 0;
  }

  lll blocks() {
    return pieces[pieceType][dt] << pos(dxy.x, dxy.y);
  }

  piece() {}
  bool operator==(const piece &o) const {
    return pieceType == o.pieceType && dxy == o.dxy && dt == o.dt;
  }
};

namespace std {
  template <> struct hash<piece>
  {
    std::size_t operator()(const piece& k) const
    {
      // Compute individual hash values for two data members and combine them using XOR and bit shifting
      return (hash<int>()((k.pieceType << 10) | (k.dxy.x << 6) | (k.dxy.y << 2) | (k.dt)));
    }
  };
}

vector<pii> Ikicks[5][5]; // kicks for I piece
vector<pii> Skicks[5][5]; // kicks for other

struct board {
  lll grid;
  inline int get(int x, int y) {
    return _get(grid, pos(x, y));
  }
  inline void set(int i, int j, int b) {
    _set(grid, pos(i, j), b);
  }

  board (char k[MAXN][MAXN], char ch) {
    grid = 0;
    for(int i = 0; i < W; ++i) {
      for(int j = 0; j < H; ++j) {
        set(i, j, k[H - j - 1][i] != ch);
      }
    }
  }

  board(lll g = 0) : grid(g) {}

  bool operator==(const board &o) const {
    return grid == o.grid;
  }

  bool fits(piece move) {
    lll b = move.blocks();
    if ((b & (grid | border)) != 0) return false;
    /*
    for(int i = 0; i < 4; ++i) {
      if (!b) {
        return false; // at least 4 blocks
      }
      b &= b - 1;
    }
    return !b; // at most 4 blocks
    */
    return true;
  }

  void checkclear() {
    int clears = 0;
    lll ngrid = 0;
    int h = H - 1;
    lll rmsk = (ll(1) << W) - 1;
    for(int i = H - 1; i >= 0; --i) {
      lll row = (grid >> pos(0, i)) & rmsk;
      if (row != rmsk) {
        ngrid |= row << pos(0, h);
        --h;
      }
    }
    while (h >= 0) {
      ngrid |= rmsk << pos(0, h);
      --h;
    }
    grid = ngrid;
  }

  void add(piece move) {
    grid |= move.blocks();
    checkclear();
  }
};

namespace std {
  template <> struct hash<board>
  {
    std::size_t operator()(const board &k) const
    {
      // Compute individual hash values for two data members and combine them using XOR and bit shifting
      return hash<lll>()(k.grid);
    }
  };
}

/*
 * computing moves
 * */
piece rot(piece p, int dt) {
  p.dt = (p.dt + dt + 4) % 4;
  return p;
}

piece trans(piece p, pii dxy) {
  p.dxy = p.dxy + dxy;
  return p;
}

bool rest(piece &p, board &b) {
  return !b.fits(trans(p, DOWN));
}

inline vector<pii> getKicks(piece p, int rotType) {
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
      if (b.fits(nk)) {
        return nk;
      }
    }
  } else if (move == 2 || move == 3) {
    pii dxy = (move == 2 ? LEFT : RIGHT);
    piece n = trans(cur, dxy);
    if (b.fits(n)) return n;
  } else if (move == 4) {
    while (!rest(cur, b)) {
      cur = trans(cur, DOWN);
    }
    return cur;
  }
  return cur;
}

