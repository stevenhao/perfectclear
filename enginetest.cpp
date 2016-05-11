#include "engine.cpp"
void go() {
  board end;
  for(int i = 0; i < 4; ++i) {
    end.grid |= rowmsk(i);
  }

  umap<board, int> cur;
  cur[end] = 1;

  int depth = 10;
  for(int i = 0; i < depth; ++i) {
    umap<board, int> nxt;
    cerr << "start loop\n";
    for (auto bi: cur) {
      const board &b = bi.first;
      for(int j = 0; j < 7; ++j) {
        for (auto bi2: inEdges(b, j)) {
          nxt[bi2.first] = 1;
        }
      }
    }
    cerr << "end loop\n";
    cur = nxt;
    cerr << "i=" << i << ", cur.size = " << cur.size() << "\n";

    vector<board> v;
    for (auto bi: cur) {
      v.push_back(bi.first);
    }
    int lim = 5;
    for(int i = 0; i < lim && v.size(); ++i) {
      board b = v[rand() % v.size()];
      disp(b);
    }

  }
}

void go2() {
  int depth = 5;
  umap<board, int> cur;
  cur[board()] = 1;
  for(int i = 0; i < depth; ++i) {
    umap<board, int> nxt;
    for (auto bi: cur) {
      const board &b = bi.first;
      for(int j = 0; j < 7; ++j) {
        for (auto bi2: outEdges(b, j)) {
          nxt[bi2.first] = 1;
        }
      }
    }
    cur = nxt;
    cerr << "i=" << i << ", cur.size = " << cur.size() << "\n";


    vector<board> v;
    for (auto bi: cur) {
      v.push_back(bi.first);
    }
    int lim = 5;
    for(int i = 0; i < lim; ++i) {
      board b = v[rand() % v.size()];
      disp(b);
    }
  }
}

void go3() {
  board b;
  lll rmsk = (lll(1) << W) - 1;
  b.grid |= rmsk << pos(0, 0);
  b.grid |= lll(60) << pos(0, 1);
  disp(b);
  umap<board, int> pr = inEdges(b, 1);
  for (auto bi: pr) {
    disp(bi.first);
  }
}

int main() {
  loadData();
  precompute();
  go();
}
