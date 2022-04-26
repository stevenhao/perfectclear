#include "engine.cpp"
#include "steven.hpp"

bool reachable(board b, int turns) {
  int diff = turns * 4 - b.count();
  if (diff < 0 || diff % 10) return false;
  int height = 0;
  for (int j = 0; j < 4; ++j) {
    for (int i = 0; i < 10; ++i) {
      if (b.get(i, j)) height = j;
    }
  }
  if (height + diff / 10 >= 4) return false;
  if (diff == 0) {
    vector<int> par(40);
    for (int i = 0; i < 40; ++i) par[i] = i;
    auto find = yc([&](auto rec, int x) -> int {
      if (par[x] == x) return x;
      return par[x] = rec(par[x]);
    });
    auto join = [&](int a, int b) -> void {
      a = find(a);
      b = find(b);
      par[a] = b;
    };
    for (int i = 0; i < 10; ++i) {
      for (int j = 0; j < 4; ++j) {
        if (!b.get(i, j)) continue;
        if (j + 1 < 4 && b.get(i, j + 1)) join(i * 4 + j, i * 4 + j + 1);
        if (i + 1 < 10 && b.get(i + 1, j)) join(i * 4 + j, (i + 1) * 4 + j);
      }
    }
    vector<int> counts(40);
    for (int i = 0; i < 10; ++i) {
      for (int j = 0; j < 4; ++j) {
        if (!b.get(i, j)) continue;
        ++counts[find(4 * i + j)];
      }
    }
    for (int i = 0; i < 40; ++i) {
      if (counts[i] % 4) return false;
    }
  }
  return true;
}

board shuffleRows(board b, vector<int> rowPerm) {
  board res;
  int cnt = 0;
  for (int i = 0; i < 4; ++i) {
    res.grid |= rowmsk(i);
  }
  for (int i : rowPerm) {
    res.grid ^= rowmsk(i);
  }
  for (int i = 0; i < 4; ++i) {
    if ((b.grid & rowmsk(i)).count()) {
      res.grid |= (b.grid & rowmsk(i)) << (13 * (rowPerm[cnt++] - i));
    }
  }
  return res;
}
// precondition: b is the result of making
// `turns` moves on a blank board
vector<vector<board>> preImage(board b, int turns) {
  int rowsToAdd = (turns * 4 - b.count()) / 10;
  int rows = 4 - rowsToAdd;
  vector<unordered_set<board>> ret(7);
  for (int msk = 0; msk < 16; ++msk) {
    vector<int> rowPerm;
    for (int i = 0; i < 4; ++i) {
      if ((msk >> i) & 1) {
        rowPerm.push_back(i);
      }
    }
    if (sz(rowPerm) != rows) continue;
    board nb = shuffleRows(b, rowPerm);
    for (int pieceType = 0; pieceType < 7; ++pieceType) {
      piece p(pieceType);
      unordered_set<lll> bmsks;
      for (int dt = 0; dt < 4; ++dt) {
        for (int dy = 0; dy < 4; ++dy) {
          for (int dx = 0; dx < 10; ++dx) {
            piece q = p;
            q.dxy = {dx, dy};
            q.dt = dt;
            lll pmsk = q.blocks();
            if ((nb.grid & pmsk) == pmsk) {
              bmsks.insert(nb.grid ^ pmsk);
            }
          }
        }
      }
      for (lll bmsk : bmsks) {
        board cand(bmsk);
        if (!reachable(cand, turns - 1)) {
          continue;
        }
        vector<piece> moves = getMoves(cand, pieceType);
        for (auto move : moves) {
          lll moveMsk = move.blocks();
          if ((moveMsk & nb.grid) == moveMsk) {
            cand.checkclear();
            ret[pieceType].insert(cand);
            break;
          }
        }
      }
    }
  }
  vector<vector<board>> res(7);
  for (int i = 0; i < 7; ++i) {
    res[i].reserve(sz(ret[i]));
    for (auto b : ret[i]) {
      res[i].push_back(b);
    }
  }
  return res;
}

const int MAX_SIZE = 100000;

void go() {
  double fac = 1, fac2 = 1;
  unordered_map<board, unordered_map<int, int>> bs;
  bs[board()][0] = 1;
  for (int i = 10; i >= 5; --i) {
    fac *= 1. / (7 - (10 - i));
    fac2 *= 2. / (12 - i);
    pp(i, sz(bs));
    pp("Computing next layer...\n");
    unordered_map<board, unordered_map<int, int>> nbs;
    int cnt = 0;
    for (auto bp : bs) {
      pp(cnt++, sz(bs));
      vector<vector<board>> bPre = preImage(bp.fi, i);

      for (int j = 0; j < 7; ++j) {
        for (pii k : bp.se) {
          if (!((k.fi >> j) & 1)) {
            for (board b : bPre[j]) {
              if (b.toLL() == 1061130211LL) {
                // cout << "FOUND 1061130211: " << k.fi << " " << (1 << j) << "
                // "
                //      << k.se << "\n";
              }
              nbs[b][k.fi | (1 << j)] += k.se;
            }
          }
        }
      }
    }
    bs = nbs;
    vector<pair<board, unordered_map<int, int>>> vv(bs.begin(), bs.end());
    vector<pair<double, int>> v;
    for (int i = 0; i < sz(vv); ++i) {
      double cnt = 0;
      for (pii p : vv[i].se) {
        cnt += log2(p.se) + 1;
      }
      v.emplace_back(cnt, i);
    }
    sort(v.begin(), v.end());
    reverse(v.begin(), v.end());
    if (sz(bs) > MAX_SIZE) {
      pp("TOO MANY, %d\n", sz(bs));
      v.resize(MAX_SIZE / 2);
    }

    pp(i);
    pp(sz(v));
    for (pii pi : v) {
      pp(vv[pi.se].fi, pi.fi);
      // disp(vv[pi.se].fi);
      for (auto p : vv[pi.se].se) {
        cout << 11 - i << " " << vv[pi.se].fi.toLL() << " " << p.fi << " "
             << p.se * fac2 << "\n";
      }
    }

    bs.clear();
    for (auto p : v) {
      bs[vv[p.se].fi] = vv[p.se].se;
    }
  }
}

int main() {
  loadData();

  go();
}
