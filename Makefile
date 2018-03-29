CC=g++
CFLAGS=-c -g -Wall --std=c++11
LDFLAGS=
EXECUTABLE=Project4Server Project4Client OfflineTester

all: testFiles Project4Client Project4Server OfflineTester JSONTest

Project4Server: Project4Server.o Project4Common.o lib/CCRC32.o HappyPathJSON.o
	$(CC) $(LDFLAGS) build/Project4Common.o build/Project4Server.o build/CCRC32.o build/HappyPathJSON.o -o Project4Server

Project4Client: Project4Client.o Project4Common.o lib/CCRC32.o HappyPathJSON.o
	$(CC) $(LDFLAGS) build/Project4Common.o build/Project4Client.o build/CCRC32.o build/HappyPathJSON.o -o Project4Client

OfflineTester: OfflineTester.o Project4Common.o lib/CCRC32.o HappyPathJSON.o
	$(CC) $(LDFLAGS) build/OfflineTester.o build/Project4Common.o build/CCRC32.o build/HappyPathJSON.o -o OfflineTester

Project4Common.o: src/Project4Common.h src/Project4Common.cpp
	$(CC) $(CFLAGS) src/Project4Common.cpp -o build/Project4Common.o

Project4Client.o: src/Project4Client.cpp
	$(CC) $(CFLAGS) src/Project4Client.cpp -o build/Project4Client.o

Project4Server.o: src/Project4Server.cpp
	$(CC) $(CFLAGS) src/Project4Server.cpp -o build/Project4Server.o

OfflineTester.o: src/OfflineTester.cpp
	$(CC) $(CFLAGS) src/OfflineTester.cpp -o build/OfflineTester.o

lib/CCRC32.o: src/lib/CCRC32.cpp src/lib/CCRC32.h
	$(CC) $(CFLAGS) src/lib/CCRC32.cpp -o build/CCRC32.o

client: Project4Client
	@./Project4Client -p 30600 -s 127.0.0.1 -d ./testClientDir

server: Project4Server
	@./Project4Server -p 30600 -d ./testServerDir

JSONTest.o: src/JSONTest.cpp
	$(CC) $(CFLAGS) src/JSONTest.cpp -o build/JSONTest.o

JSONTest: JSONTest.o HappyPathJSON.o
	$(CC) $(LDFLAGS) build/JSONTest.o build/HappyPathJSON.o -o JSONTest

clean: testFiles
	rm -f *.o $(EXECUTABLE)
	rm -f build/*

HappyPathJSON.o: src/HappyPathJSON.cpp src/HappyPathJSON.h
	$(CC) $(CFLAGS) src/HappyPathJSON.cpp -o build/HappyPathJSON.o
	
testFiles:
	@rm testClientDir/* testServerDir/* | true
	@cp testBackupDir/client/* testClientDir/
	@cp testBackupDir/server/* testServerDir/
