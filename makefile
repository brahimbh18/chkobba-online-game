CXX = g++
CXXFLAGS = -std=c++17 -I.

GAMELOGIC_SRCS = \
  gameLogic/Player/Player.cpp \
  gameLogic/Round/Round.cpp \
  gameLogic/Move/Move.cpp \
  gameLogic/Card/Card.cpp \
  gameLogic/Deck/Deck.cpp \
  gameLogic/Table/Table.cpp \
  gameLogic/Score/Score.cpp

SERVER_SRCS = Server/Server.cpp
CLIENT_SRCS = Client/test_client.cpp

all: server

server: main.cpp $(SERVER_SRCS) $(GAMELOGIC_SRCS)
	$(CXX) $(CXXFLAGS) main.cpp $(SERVER_SRCS) $(GAMELOGIC_SRCS) -o server



clean:
	rm -f server client
