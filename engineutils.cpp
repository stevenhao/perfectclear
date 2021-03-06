#include "tetrisio.cpp"
#include <unordered_map>

using namespace std;

#define umap unordered_map
umap<piece, int> vis;
umap<piece, pair<piece, int> > trace;
piece queue[MAXN*MAXN*5];
umap<lll, bool> seen;
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
  seen.clear();

  vector<piece> ret;
  while (ql < qr) {
    piece cur = queue[ql]; ++ql;
    if (rest(cur, b)) {
      board nb;
      nb.add(cur);
      if (seen[nb.grid]) continue;
      seen[nb.grid] = true;
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

vector<int> getPath(board b, piece p, piece inHand) { // returns info on most recent getMoves
  vector<int> ans;
  piece initial = piece(p.pieceType);
  bfs(b, initial);
  if (!vis[p]) {
    printf("ILLEGAL MOVE!\n");
    return ans;
  }
  while (!(p == initial)) {
    piece n = trace[p].x;
    int move = trace[p].y;
    ans.push_back(move);
    p = n;
  }
  if (p.pieceType != inHand.pieceType) {
    // first swap
    ans.push_back(5);
  }
  reverse(ans.begin(), ans.end());
  return ans;
}

