/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "json.hpp"
#include "engine.cpp"

using json = nlohmann::json;
using namespace std;


void error(const char *msg)
{
  perror(msg);
  exit(1);
}

int toInt(string s) {
  int i;
  stringstream ss;
  ss << s;
  ss >> i;
  return i;
}

board readBoard(json j) {
  if (j["W"].is_string() && j["H"].is_string() && j["data"].is_string()) {
    int _W = toInt(j["W"].get<string>());
    int _H = toInt(j["H"].get<string>());
    string s = j["data"].get<string>();
    if (_W != W || _H != H) throw "gg";
    char buf[MAXN][MAXN];

    int cnt = 0;
    for(int i = 0; i < H; ++i) {
      for(int j = 0; j < W; ++j) {
        buf[i][j] = s[cnt];
        ++cnt;
      }
    }
    return board(buf, '0');
  } else {
    throw "cannot parse board";
  }
}

vector<int> readPieces(json j) {
  vector<string> v = j.get<vector<string>>();
  vector<int> ret;
  stringstream ss;
  for (auto &s: v) {
    ret.push_back(getPieceIndex(s));
  }
  return ret;
}

string handleRequest(string input) {
  if (!input.size()) return "";
  try {
    auto j = json::parse(input);
    cerr << j << "\n";
    if (j.is_object() && j["board"].is_object() && j["pieces"].is_array() && j["reqid"].is_number()) {
      int reqid = j["reqid"].get<int>();
      board b = readBoard(j["board"]);
      vector<int> pieces = readPieces(j["pieces"]);
      piece initial = piece(pieces[0]);
      engineResult result = getBestMove(b, pieces);
      vector<int> path = getPath(b, result.move, initial);
      piece p = result.move;
      disp(b, p);
      vector<string> pathStrings;
      p = piece(p.pieceType);
      pathStrings.push_back(toString(p));
      for(auto i: path) {
        p = apply(i, p, b);
        pathStrings.push_back(toString(p));
      }
      b.add(p);

      int happy = result.happy;
      json ret;
      ret["path"] = path;
      ret["happy"] = happy;
      ret["pathStrings"] = pathStrings;
      ret["board"] = toString(b);
      json msg;
      msg["body"] = ret;
      msg["reqid"] = reqid;
      return msg.dump();
    }
  } catch (int e) {
  }

  return "ok";
}

int main(int argc, char *argv[])
{
  loadData();
  /*  stringstream ss;
      ss << cin.rdbuf();
      string ans = handleRequest(ss.str());
      cout << ans << "\n";*/

  int sockfd, newsockfd, portno;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  if (argc >= 3) {
    setBeamSearchLimit(atoi(argv[2]));
  }
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (::bind(sockfd, (struct sockaddr *) &serv_addr,
        sizeof(serv_addr)) < 0) 
    error("ERROR on binding");
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  while (true) {
    newsockfd = accept(sockfd, 
        (struct sockaddr *) &cli_addr, 
        &clilen);
    if (newsockfd < 0) 
      error("ERROR on accept");
    while (true) {
      bzero(buffer,256);
      n = read(newsockfd,buffer,255);
      if (n < 0) { cerr << "ERROR reading from socket\n"; break; }
      string res = handleRequest(string(buffer));
      if (res.size()) {
        n = write(newsockfd, res.c_str(), res.size());
      } else {
        break;
      }
      if (n < 0) { cerr << "ERROR writing to socket\n"; break; }
    }
    close(newsockfd);
  }
  close(sockfd);
  return 0; 
}
