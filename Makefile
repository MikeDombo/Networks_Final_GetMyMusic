CC=g++
CFLAGS=-c -g -Wall --std=c++11
LDFLAGS=
EXECUTABLE=Project4Server Project4Client

all: Project4Client Project4Server

Project4Server: Project4Server.o Project4Common.o lib/CCRC32.o
	$(CC) $(LDFLAGS) Project4Common.o Project4Server.o lib/CCRC32.o -o Project4Server

Project4Client: Project4Client.o Project4Common.o lib/CCRC32.o
	$(CC) $(LDFLAGS) Project4Common.o Project4Client.o lib/CCRC32.o -o Project4Client

Project4Common.o: Project4Common.h Project4Common.cpp
	$(CC) $(CFLAGS) Project4Common.cpp -o Project4Common.o

Project4Client.o: Project4Client.cpp
	$(CC) $(CFLAGS) Project4Client.cpp -o Project4Client.o

Project4Server.o: Project4Server.cpp
	$(CC) $(CFLAGS) Project4Server.cpp -o Project4Server.o

lib/CCRC32.o: lib/CCRC32.cpp lib/CCRC32.h
	$(CC) $(CFLAGS) lib/CCRC32.cpp -o lib/CCRC32.o

client: Project4Client
	@./Project4Client -p 30600 -s 127.0.0.1

server: Project4Server
	@./Project4Server -p 30600

clean:
	rm -f *.o $(EXECUTABLE)