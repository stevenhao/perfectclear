#include "wasm_engineutils.h"

using namespace std;

#define umap unordered_map
umap<piece, int> wasm_vis;
umap<piece, pair<piece, int>> wasm_trace;

// Use dynamic allocation instead of VLA
const int WASM_MAX_QUEUE_SIZE = 15 * 15 * 5;
piece* wasm_piece_queue = new piece[WASM_MAX_QUEUE_SIZE];

void wasm_cleanup() {
  delete[] wasm_piece_queue;
  wasm_piece_queue = nullptr;
}

umap<lll, bool> wasm_seen;
int wasm_ql, wasm_qr;

void wasm_psh(piece cur, piece prv, int move) {
  if (wasm_vis[cur]) return;
  wasm_vis[cur] = 1;
  wasm_trace[cur] = make_pair(prv, move);
  wasm_piece_queue[wasm_qr] = cur;
  ++wasm_qr;
}

const int WASM_MAX_MEMO_SIZE = 30000;
ll wasm_cacheHits = 0;
ll wasm_cacheMisses = 0;
int wasm_bfsCount = 0;
unordered_map<pair<lll, int>,
              pair<umap<piece, pair<piece, int>>, vector<piece>>>
    wasm_memo1, wasm_memo2;

vector<piece> wasm_bfs(board &b, piece initial) {
  pair<lll, int> key(b.grid, initial.pieceType);
  if (wasm_memo1.count(key)) {
    ++wasm_cacheHits;
    wasm_memo2[key] = wasm_memo1[key];
    wasm_trace = wasm_memo1[key].first;
    return wasm_memo1[key].second;
  }
  if (wasm_memo2.count(key)) {
    ++wasm_cacheHits;
    wasm_trace = wasm_memo2[key].first;
    return wasm_memo2[key].second;
  }
  ++wasm_cacheMisses;
  ++wasm_bfsCount;
  wasm_vis.clear();
  wasm_trace.clear();
  wasm_ql = wasm_qr = 0;
  wasm_psh(initial, piece(), -1);
  wasm_seen.clear();

  vector<piece> ret;
  while (wasm_ql < wasm_qr) {
    piece cur = wasm_piece_queue[wasm_ql];
    ++wasm_ql;
    if (rest(cur, b)) {
      board nb;
      nb.add(cur);
      if (wasm_seen[nb.grid]) continue;
      wasm_seen[nb.grid] = true;
      ret.push_back(cur);
    }
    for (int move = 0; move < MOVES; ++move) {
      piece nxt = apply(move, cur, b);
      wasm_psh(nxt, cur, move);
    }
  }
  if (wasm_memo2.size() >= WASM_MAX_MEMO_SIZE) {
    wasm_memo1 = wasm_memo2;
    wasm_memo2.clear();
  }
  wasm_memo2[key] = make_pair(wasm_trace, ret);
  return ret;
}

vector<piece> wasm_getMoves(board b, int pType) {
  piece initial = piece(pType);
  vector<piece> ret = wasm_bfs(b, initial);
  return ret;
}

vector<int> wasm_getPath(board b, piece p, piece inHand) {
  vector<int> ans;
  piece initial = piece(p.pieceType);
  wasm_bfs(b, initial);
  if (!wasm_trace.count(p)) {
    printf("ILLEGAL MOVE!\n");
    return ans;
  }
  while (!(p == initial)) {
    piece n = wasm_trace[p].x;
    int move = wasm_trace[p].y;
    ans.push_back(move);
    p = n;
  }
  if (p.pieceType != inHand.pieceType) {
    ans.push_back(5);
  }
  reverse(ans.begin(), ans.end());
  return ans;
}
