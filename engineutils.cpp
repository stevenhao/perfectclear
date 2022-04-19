#include "tetrisio.cpp"
#include <unordered_map>
#include <unordered_set>

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

const int MAX_MEMO_SIZE = 100000;
ll cacheHits = 0;
ll cacheMisses = 0;
int bfsCount = 0;
unordered_map<pair<lll, int>, pair<umap<piece, pair<piece, int> > , vector<piece>>> memo1, memo2;
vector<piece> bfs(board &b, piece initial) {
  pair<lll, int> key(b.grid, initial.pieceType);
  if (memo1.count(key)) {
    ++cacheHits;
    trace = memo1[key].first;
    return memo1[key].second;
  }
  if (memo2.count(key)) {
    ++cacheHits;
    trace = memo2[key].first;
    return memo2[key].second;
  }
  ++cacheMisses;
  ++bfsCount;
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
  if (memo2.size() >= MAX_MEMO_SIZE) {
    printf("CLEARING THE MEMO!!\n");
    memo1 = memo2;
    memo2.clear();
  }
  memo2[key] = make_pair(trace, ret);
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
  if (!trace.count(p)) {
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

