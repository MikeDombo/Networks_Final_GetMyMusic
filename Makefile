CC=g++
CFLAGS=-c -g -Wall --std=c++11
LDFLAGS=

# OS-dependent flags
UNAME := $(shell uname)
ifneq ($(UNAME), Linux)
CFLAGS+=-fsanitize=address
LDFLAGS+=-fsanitize=address
endif

EXECUTABLE=Project4Server Project4Client MessageTester JSONTest

all: testFiles $(EXECUTABLE)

################################################################################
# Executables
################################################################################

Project4Server: build/Project4Server.o build/Project4Common.o build/CRC32.o build/HappyPathJSON.o
	$(CC) $(LDFLAGS) build/Project4Common.o build/Project4Server.o build/CRC32.o build/HappyPathJSON.o -o Project4Server

Project4Client: build/Project4Client.o build/Project4Common.o build/CRC32.o build/HappyPathJSON.o
	$(CC) $(LDFLAGS) build/Project4Common.o build/Project4Client.o build/CRC32.o build/HappyPathJSON.o -o Project4Client

MessageTester: build/MessageTester.o build/Project4Common.o build/CRC32.o build/HappyPathJSON.o
	$(CC) $(LDFLAGS) build/MessageTester.o build/Project4Common.o build/CRC32.o build/HappyPathJSON.o -o MessageTester

JSONTest: build/JSONTest.o build/HappyPathJSON.o
	$(CC) $(LDFLAGS) build/JSONTest.o build/HappyPathJSON.o -o JSONTest


################################################################################
# Object Files
################################################################################

build/Project4Common.o: src/Project4Common.h src/Project4Common.cpp
	$(CC) $(CFLAGS) src/Project4Common.cpp -o build/Project4Common.o

build/Project4Client.o: src/Project4Client.cpp
	$(CC) $(CFLAGS) src/Project4Client.cpp -o build/Project4Client.o

build/Project4Server.o: src/Project4Server.cpp
	$(CC) $(CFLAGS) src/Project4Server.cpp -o build/Project4Server.o

build/Base64Tester.o: src/Base64Tester.cpp
	$(CC) $(CFLAGS) src/Base64Tester.cpp -o build/Project4Server.o

build/MessageTester.o: src/MessageTester.cpp
	$(CC) $(CFLAGS) src/MessageTester.cpp -o build/MessageTester.o

build/CRC32.o: src/lib/CRC32.cpp src/lib/CRC32.h
	$(CC) $(CFLAGS) src/lib/CRC32.cpp -o build/CRC32.o

build/HappyPathJSON.o: src/HappyPathJSON.cpp src/HappyPathJSON.h
	$(CC) $(CFLAGS) src/HappyPathJSON.cpp -o build/HappyPathJSON.o

build/JSONTest.o: src/JSONTest.cpp
	$(CC) $(CFLAGS) src/JSONTest.cpp -o build/JSONTest.o

################################################################################
# "Tasks" to run before or after making the executables
################################################################################

client: testFiles Project4Client
	@./Project4Client -p 30600 -s 127.0.0.1 -d ./testClientDir

server: testFiles Project4Server
	@./Project4Server -p 30600 -d ./testServerDir

tester: MessageTester
	@./MessageTester

clean: testFiles
	rm -f *.o $(EXECUTABLE)
	rm -f build/*

testFiles:
	@mkdir -p testClientDir testServerDir
	@rm testClientDir/* testServerDir/* | true
	@cp testBackupDir/client/* testClientDir/
	@cp testBackupDir/server/* testServerDir/
