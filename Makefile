all: server enginetest

OS_NAME=$(shell uname -s)
ifeq (${OS_NAME},Darwin)
	RTLIB=
	CXXFLAGS = -framework CoreFoundation -framework CoreServices -std=gnu++0x -stdlib=libc++ -g -O0 -O2 -I$(LIBUV_PATH)/include -I$(HTTP_PARSER_PATH) -I. -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

else
	RTLIB=-lrt
	CXXFLAGS = -std=gnu++0x -g -O0 -I$(LIBUV_PATH)/include -I$(HTTP_PARSER_PATH) -I. -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
endif

server: server.cpp
	$(CXX) $(CXXFLAGS) -o server.o server.cpp

enginetest: enginetest.cpp
	$(CXX) $(CXXFLAGS) -o enginetest.o enginetest.cpp

tetristest: tetristest.cpp
	$(CXX) $(CXXFLAGS) -o tetristest.o tetristest.cpp

clean:
	rm *.o
