#include "engine.cpp"
#include "steven.hpp"
#include <chrono>

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
using namespace std::chrono;

vector<int> getBag() {
  vector<int> res;
  for (int i = 0; i < 7; ++i) {
    res.push_back(i);
    swap(res[i], res[rng() % (i + 1)]);
  }
  return res;
}

vector<int> generate(int count) {
  // hold is random
  // rest is 7-bag
  vector<int> res;
  res.push_back(rng() % 7);
  vector<int> bag = getBag();
  bag.erase(bag.end() - (rng() % 7), bag.end());
  while (sz(res) < count) {
    res.push_back(bag.back());
    bag.pop_back();
    if (!sz(bag)) {
      bag = getBag();
    }
  }
  return res;
}

void go(int n) {
  cout << "running " << n << " trials\n";
  int nsuccess = 0;
  int ntot = 0;
  for (int i = 0; i < n; ++i) {
    cout << "TRIAL " << i << "\n";
    vector<int> queue = generate(11);
    board b;
    vector<int> p = queue;
    int found = -1;
    cout << "IN ";
    for (int i : queue) {
      cout << i << " ";
    }
    cout << endl;
    auto start = high_resolution_clock::now();

    for (int j = 0; j < 10; ++j) {
      vector<int> q = p;
      if (sz(q) > 8) q.resize(8);
      engineResult res = getBestMove(b, q);
      if (res.happy && found == -1) found = j;
      b.add(res.moves[0]);
      if (res.moves[0].pieceType != p[0]) {
        swap(p[0], p[1]);
      }
      p.erase(p.begin());
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "OUT " << found << endl;
    cout << "TIME " << duration.count() << " MS" << endl;
    cout << endl;

    ++ntot;
    if (found != -1) ++nsuccess;
    cout << "NSUCCESS " << nsuccess << " NTOT " << ntot << endl;
  }
}

int main(int argc, char **argv) {
  loadData();
  loadBook();

  if (argc >= 3) {
    setBeamSearchLimit(atoi(argv[2]));
  }

  if (argc >= 2) {
    go(atoi(argv[1]));
  } else {
    go(1);
  }
}
