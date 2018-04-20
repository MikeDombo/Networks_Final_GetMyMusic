CC=g++
CFLAGS=-c -g -Wall --std=c++11
LDFLAGS=

# OS-dependent flags
UNAME := $(shell uname)
ifneq ($(UNAME), Linux)
CFLAGS+=-fsanitize=address
LDFLAGS+=-fsanitize=address
endif

EXECUTABLE=Project4Server Project4Client

all: testFiles Project4Client Project4Server

Project4Server: build/Project4Server.o build/Project4Common.o build/CCRC32.o build/HappyPathJSON.o
	$(CC) $(LDFLAGS) build/Project4Common.o build/Project4Server.o build/CCRC32.o build/HappyPathJSON.o -o Project4Server

Project4Client: build/Project4Client.o build/Project4Common.o build/CCRC32.o build/HappyPathJSON.o
	$(CC) $(LDFLAGS) build/Project4Common.o build/Project4Client.o build/CCRC32.o build/HappyPathJSON.o -o Project4Client

build/Project4Common.o: src/Project4Common.h src/Project4Common.cpp
	$(CC) $(CFLAGS) src/Project4Common.cpp -o build/Project4Common.o

build/Project4Client.o: src/Project4Client.cpp
	$(CC) $(CFLAGS) src/Project4Client.cpp -o build/Project4Client.o

build/Project4Server.o: src/Project4Server.cpp
	$(CC) $(CFLAGS) src/Project4Server.cpp -o build/Project4Server.o

build/CCRC32.o: src/lib/CCRC32.cpp src/lib/CCRC32.h
	$(CC) $(CFLAGS) src/lib/CCRC32.cpp -o build/CCRC32.o

client: testFiles Project4Client
	@./Project4Client -p 30600 -s 127.0.0.1 -d ./testClientDir

server: testFiles Project4Server
	@./Project4Server -p 30600 -d ./testServerDir

clean: testFiles
	rm -f *.o $(EXECUTABLE)
	rm -f build/*

build/HappyPathJSON.o: src/HappyPathJSON.cpp src/HappyPathJSON.h
	$(CC) $(CFLAGS) src/HappyPathJSON.cpp -o build/HappyPathJSON.o
	
testFiles:
	@mkdir -p testClientDir testServerDir
	@rm testClientDir/* testServerDir/* | true
	@cp testBackupDir/client/* testClientDir/
	@cp testBackupDir/server/* testServerDir/
