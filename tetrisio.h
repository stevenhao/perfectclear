#ifndef TETRISIO_H
#define TETRISIO_H

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include "tetris.h"

extern std::string pieceNames;
int getPieceIndex(std::string s);
void loadPieces();
void loadCenters();
void loadKicks();
void loadData();
void write(lll msk, char buf[MAXN][MAXN], char ch);
void clear(char buf[MAXN][MAXN]);

template <class T>
void print(std::ostream &out, std::vector<T> v);
void print(std::ostream &out, char buf[MAXN][MAXN]);
void disp(board b, piece move);
void disp(board b);
std::string toString(char buf[MAXN][MAXN]);
std::string toString(board b);
std::string toString(piece p);
std::string repr(piece p);

#endif // TETRISIO_H
