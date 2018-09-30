#include "tetrisio.cpp"
#include <unordered_map>

using namespace std;

#define umap unordered_map
umap<piece, int> vis;
umap<piece, pair<piece, int> > trace;
piece queue[MAXN*MAXN*5];

int ql, qr;
void psh(piece cur, piece prv, int move) {
  if (vis[cur]) return;
  vis[cur] = 1;
  trace[cur] = make_pair(prv, move);
  queue[qr] = cur;
  ++qr;
}

vector<piece> bfs(board &b, piece initial) {
  vis.clear();
  trace.clear();
  ql = qr = 0;
  psh(initial, piece(), -1);

  vector<piece> ret;
  while (ql < qr) {
    piece cur = queue[ql]; ++ql;
    if (rest(cur, b)) {
      ret.push_back(cur);
    }
    for(int move = 0; move < MOVES; ++move) {
      piece nxt = apply(move, cur, b);
      psh(nxt, cur, move);
    }
  }
  return ret;
}

vector<piece> getMoves(board b, int pType) {
  piece initial = piece(pType);
  vector<piece> ret = bfs(b, initial);
  return ret;
}

vector<int> getPath(board b, piece p) { // returns info on most recent getMoves
  vector<int> ans;
  piece initial = piece(p.pieceType);
  while (!(p == initial)) {
    piece n = trace[p].x;
    int move = trace[p].y;
    ans.push_back(move);
    p = n;
  }
  reverse(ans.begin(), ans.end());
  return ans;
}

