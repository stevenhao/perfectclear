# perfectclear

[![Ask https://DeepWiki.com](https://devin.ai/assets/askdeepwiki.png)](https://deepwiki.com/stevenhao/perfectclear)

Let's get a perfect clear!

## Introduction

perfectclear is a web-based game focused on achieving "perfect clears" in a tetris-like environment. The application consists of a backend server written in C++ and a frontend client implemented with CoffeeScript.

## Running locally

### Build
- `make`

### Run the web server
- `yarn global add coffeescript`
- `yarn`
- `coffee index.coffee 4445`

### Run the backend
- `./server.o 4445`

### Open the app
- Open `localhost:4444`

## Performance testing
- `perf record -g ./enginetest.o 10`
- `perf report`

## Book
- `make book && ./book.o | tee book_100k.txt # or any other file name; edit book.cpp to control the size`

Have fun!
