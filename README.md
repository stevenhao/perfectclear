# perfectclear

Let's get a perfect clear!

# Running locally

## Build & run the engine
- `make`
- `./server.o 4445`

## Run the web server
- `yarn global add coffeescript`
- `yarn`
- `coffee index.coffee 4445` (make sure `./server` is running in a separate process)

## Open the app
- Open `localhost:4444`

## Perf
- perf record -g ./enginetest.o 10
- perf report

## Book
`make book && ./book.o | tee book_100k.txt # or any other file name; edit book.cpp to control the size`

Have fun!
