/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "engine.cpp"

using namespace std;


void error(const char *msg)
{
  perror(msg);
  exit(1);
}

board readBoard(stringstream &ss) {
  int buf[MAXN][MAXN];
  int _W, _H;
  ss >> _W >> _H;
  if (_W != W || _H != H) throw "gg";
  for(int i = 0; i < H; ++i) {
    string tmp;
    ss >> tmp;
    for(int j = 0; j < W; ++j) {
      buf[i][j] = tmp[j] != '.';
    }
  }
  return board(buf);
}

vector<int> readPieces(stringstream &ss) {
  int N;
  ss >> N;
  vector<int> v(N);
  for(int i = 0; i < N; ++i) {
    ss >> v[i];
  }
  return v;
}

string handleRequest(string input) {
  stringstream ss(input);
  board b = readBoard(ss);
  vector<int> pieces = readPieces(ss);
  piece bestMove = getBestMove(b, pieces);
  return disp(b, bestMove);
}

int main(int argc, char *argv[])
{
  precompute();
  loadPieceData();
  stringstream ss;
  ss << cin.rdbuf();
  string ans = handleRequest(ss.str());
  cout << ans << "\n";

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
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
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
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");
    close(newsockfd);
  }
  close(sockfd);
  return 0; 
}
