#include <bits/stdc++.h>
using namespace std;

template <typename A, typename B>
ostream &operator<<(ostream &os, const pair<A, B> &p);
template <typename A>
ostream &operator<<(ostream &os, const set<A> &m) {
  os << "{";
  string sep = "";
  for (auto e : m) os << sep << e, sep = ", ";
  return os << "}";
}
template <typename A>
ostream &operator<<(ostream &os, const multiset<A> &m) {
  os << "{";
  string sep = "";
  for (auto e : m) os << sep << e, sep = ", ";
  return os << "}";
}
template <typename A, typename B>
ostream &operator<<(ostream &os, const map<A, B> &m) {
  os << "{";
  string sep = "";
  for (auto e : m) os << sep << e.first << ": " << e.second, sep = ", ";
  return os << "}";
}
template <typename T>
ostream &operator<<(ostream &os, const vector<T> &v) {
  os << "[";
  string sep = "";
  for (auto e : v) os << sep << e, sep = ", ";
  return os << "]";
}
template <typename T, size_t L>
ostream &operator<<(ostream &os, const array<T, L> &v) {
  os << "[";
  string sep = "";
  for (int i = 0; i < L; ++i) os << sep << v[i], sep = ", ";
  return os << "]";
}
template <typename A, typename B>
ostream &operator<<(ostream &os, const pair<A, B> &p) {
  os << '(' << p.first << ", " << p.second << ')';
  return os;
}
struct node;
ostream &operator<<(ostream &os, const node &node);
void dbg_out() { cerr << endl; }
template <typename Head, typename... Tail>
void dbg_out(Head H, Tail... T) {
  cerr << ' ' << H;
  dbg_out(T...);
}
#ifdef STEVEN
#define pp(...) cerr << "\t[" << #__VA_ARGS__ << "]:\t", dbg_out(__VA_ARGS__)
#define ppp(x) cerr << x << "\n"
#else
#define pp(...)
#define ppp(...)
#endif
template <class Fun>
class y_combinator_result {
  Fun fun_;

 public:
  template <class T>
  explicit y_combinator_result(T &&fun) : fun_(std::forward<T>(fun)) {}
  template <class... Args>
  decltype(auto) operator()(Args &&...args) {
    return fun_(std::ref(*this), std::forward<Args>(args)...);
  }
};
template <class Fun>
decltype(auto) y_combinator(Fun &&fun) {
  return y_combinator_result<std::decay_t<Fun>>(std::forward<Fun>(fun));
}
#define yc y_combinator
typedef long long ll;
typedef pair<int, int> pii;
typedef pair<ll, ll> pll;
typedef long double ld;
#define fi first
#define se second
#define sz(x) (int((x).size()))
#define data _data
