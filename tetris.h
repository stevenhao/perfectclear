#ifndef TETRIS_H
#define TETRIS_H

#include <algorithm>
#include <bitset>
#include <vector>
using namespace std;

typedef pair<int, int> pii;
#define x first
#define y second

namespace std {
template <typename T, typename U>
struct hash<pair<T, U>> {
  std::size_t operator()(const pair<T, U> &k) const {
    return (hash<T>()(k.x) ^ hash<U>()(k.y));
  }
};
}  // namespace std

extern const pii LEFT, RIGHT, DOWN;

pii operator+(pii a, pii b);
pii rotate(pii a, int r);

extern const int MAXN;
extern const int W;
extern const int H;

typedef bitset<256> lll;
typedef long long ll;

inline int _get(lll a, int p);
inline void _set(lll &a, int p, int v);
inline int abspos(int x, int y);
inline int pos(int x, int y);
inline lll rowmsk(int y);
lll borderMask();
extern const lll border;

extern lll pieces[15][4];
extern pii centers[15];

struct piece {
  int pieceType;
  pii dxy;
  int dt;

  piece(int pType);
  lll blocks();
  piece();
  bool operator==(const piece &o) const;
  bool isNull();
};

namespace std {
template <>
struct hash<piece> {
  std::size_t operator()(const piece &k) const {
    return (hash<int>()((k.pieceType << 10) | (k.dxy.x << 6) | (k.dxy.y << 2) |
                        (k.dt)));
  }
};
}  // namespace std

extern vector<pii> Ikicks[5][5];
extern vector<pii> Skicks[5][5];

struct board {
  lll grid;
  inline int get(int x, int y);
  inline void set(int i, int j, int b);

  board(char k[MAXN][MAXN], char ch);
  board(lll g = 0);

  bool operator==(const board &o) const;
  bool fits(piece move);
  void checkclear();
  void add(piece move);
  ll toLL();
  void fromLL(ll val);
  const int count() const;
};

namespace std {
template <>
struct hash<board> {
  std::size_t operator()(const board &k) const {
    return hash<lll>()(k.grid);
  }
};
}  // namespace std

piece rot(piece p, int dt);
piece trans(piece p, const pii &dxy);
bool rest(piece &p, board &b);
inline vector<pii> getKicks(piece p, int rotType);

extern const int MOVES;
piece apply(int move, piece cur, board &b);

#endif // TETRIS_H
