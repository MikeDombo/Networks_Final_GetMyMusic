CC=g++
CFLAGS=-c -g -Wall --std=c++11 -I lib
LDFLAGS=
EXECUTABLE=Project4Server Project4Client

all: Project4Client Project4Server
	
Project4Server: Project4Server.o Project4Common.o
	$(CC) $(LDFLAGS) Project4Common.o Project4Server.o -o Project4Server

Project4Client: Project4Client.o Project4Common.o
	$(CC) $(LDFLAGS) Project4Common.o Project4Client.o -o Project4Client

Project4Common.o: Project4Common.h Project4Common.cpp
	$(CC) $(CFLAGS) Project4Common.cpp -o Project4Common.o

Project4Client.o: Project4Client.cpp
	$(CC) $(CFLAGS) Project4Client.cpp -o Project4Client.o

Project4Server.o: Project4Server.cpp
	$(CC) $(CFLAGS) Project4Server.cpp -o Project4Server.o

client: Project4Client
	@./Project4Client 

server: Project4Server
	@./Project4Server 

clean:
	rm -f *.o $(EXECUTABLE)