all: server enginetest book vis

OS_NAME=$(shell uname -s)
CXX=g++-9
ifeq (${OS_NAME},Darwin)
	RTLIB=
  CXXFLAGS = -DSTEVEN -std=c++17 -framework CoreFoundation -framework CoreServices -g -O0 -O2 -I$(LIBUV_PATH)/include -I$(HTTP_PARSER_PATH) -I. -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

else
	RTLIB=-lrt
  CXXFLAGS = -DSTEVEN -std=c++17 -g -O0 -O2 -I$(LIBUV_PATH)/include -I$(HTTP_PARSER_PATH) -I. -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
endif

server: server.cpp
	$(CXX) $(CXXFLAGS) -o server.o server.cpp

enginetest: enginetest.cpp
	$(CXX) $(CXXFLAGS) -o enginetest.o enginetest.cpp

book: book.cpp
	$(CXX) $(CXXFLAGS) -o book.o book.cpp

vis: vis.cpp
	$(CXX) $(CXXFLAGS) -o vis.o vis.cpp

tetristest: tetristest.cpp
	$(CXX) $(CXXFLAGS) -o tetristest.o tetristest.cpp

clean:
	rm *.o
