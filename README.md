# perfectclear

Let's get a perfect clear!

# Running locally

## Build
- `make`

## Run the web server
- `yarn global add coffeescript`
- `yarn`
- `coffee index.coffee`

## Open the app
- Open `localhost:4444`

## Perf
- perf record -g ./enginetest.o 10
- perf report

## Book
`make book && ./book.o | tee book_100k.txt # or any other file name; edit book.cpp to control the size`

Have fun!
