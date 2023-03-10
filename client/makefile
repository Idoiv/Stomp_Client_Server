CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all: StompWCIClient

StompWCIClient: bin/ConnectionHandler.o bin/event.o bin/Game.o bin/user.o bin/StompProtocol.o bin/ServerThread.o bin/StompClient.o
	g++ -o bin/StompWCIClient bin/ConnectionHandler.o bin/event.o bin/Game.o bin/user.o bin/StompProtocol.o bin/ServerThread.o bin/StompClient.o  $(LDFLAGS)

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/event.o: src/event.cpp
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/Game.o: src/Game.cpp
	g++ $(CFLAGS) -o bin/Game.o src/Game.cpp

bin/user.o: src/user.cpp
	g++ $(CFLAGS) -o bin/user.o src/user.cpp
		
bin/StompProtocol.o: src/StompProtocol.cpp
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/ServerThread.o: src/ServerThread.cpp
	g++ $(CFLAGS) -o bin/ServerThread.o src/ServerThread.cpp

bin/StompClient.o: src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp	

.PHONY: clean
clean:
	rm -f bin/*
