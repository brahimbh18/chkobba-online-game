#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <unordered_map>
#include <mutex>

namespace gameServer {

struct PlayerConn {
    int socket;
    std::string name;
};

struct GameSession {
    PlayerConn player1;
    PlayerConn player2;
    bool ready = false;
};

class Server {
public:
    void run(int port);

private:
    std::unordered_map<std::string, GameSession> lobbies;
    std::mutex lobbyMutex;

    void handleClient(int clientSocket);
    std::string generateCode();
    void startGameSession(const std::string& code);
    void relayGame(int p1Socket, int p2Socket);
};

} // namespace gameServer

#endif // SERVER_H
