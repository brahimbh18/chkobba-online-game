#include "Server.h"

#include "../gameLogic/Player/Player.h"
#include "../gameLogic/Round/Round.h"
#include "../gameLogic/Move/Move.h"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <random>
#include <sstream>
#include <vector>

namespace gameServer {

// Generate a random 4-letter game code
std::string Server::generateCode() {
    static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static std::default_random_engine rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 25);
    std::string code;
    for (int i = 0; i < 4; ++i) {
        code += charset[dist(rng)];
    }
    return code;
}

// Relay turn-based messages between two player sockets
void Server::relayGame(int p1Sock, int p2Sock) {
    int current = 0;
    int sock[2] = {p1Sock, p2Sock};
    char buf[1024];

    while (true) {
        int active  = sock[current];
        int passive = sock[1 - current];

        // Prompt active player
        send(active, "TURN\n", 5, 0);

        // Receive their input
        int n = recv(active, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        buf[n] = '\0';

        // Forward to the other player
        std::string out = "MSG:" + std::string(buf);
        send(passive, out.c_str(), out.size(), 0);

        current = 1 - current;
    }

    close(p1Sock);
    close(p2Sock);
}

void Server::startGameSession(const std::string& code) {
    // Copy session info under lock
    GameSession sess;
    {
        std::lock_guard<std::mutex> lk(lobbyMutex);
        sess = lobbies[code];
    }

    // Create gameLogic players and round
    gameLogic::Player p1(sess.player1.name);
    gameLogic::Player p2(sess.player2.name);
    gameLogic::Round round(p1, p2);

    // Notify both clients
    auto sendAll = [&](const std::string& msg) {
        send(sess.player1.socket, msg.c_str(), msg.size(), 0);
        send(sess.player2.socket, msg.c_str(), msg.size(), 0);
    };
    sendAll("INFO:GAME_STARTED\n");

    char buf[1024];

    // Main turn loop
    while (!round.isRoundOver()) {
        // Send table state
        auto newTable = round.getTableCards();
        std::ostringstream ts;
        ts << "TABLE:";
        for (size_t i = 0; i < newTable.size(); ++i) {
            ts << "[" << i << "]" << newTable[i].toString();
            if (i + 1 < newTable.size()) ts << ",";
        }
        ts << "\n";
        std::string tableMsg = ts.str();
        sendAll(tableMsg);
        std::cout << "[SERVER] Sent table: " << tableMsg;

        // Send each player's hand
        std::string h1 = "HAND:" + p1.getHandString() + "\n";
        std::string h2 = "HAND:" + p2.getHandString() + "\n";
        send(sess.player1.socket, h1.c_str(), h1.size(), 0);
        send(sess.player2.socket, h2.c_str(), h2.size(), 0);
        std::cout << "[SERVER] Sent to p1: " << h1;
        std::cout << "[SERVER] Sent to p2: " << h2;

        // Prompt the active player
        gameLogic::Player& active = round.getCurrentPlayer();
        int activeSock = (active.getName() == p1.getName())
                         ? sess.player1.socket
                         : sess.player2.socket;
        send(activeSock, "TURN\n", 5, 0);

        // Receive a single MOVE command
        int n = recv(activeSock, buf, sizeof(buf)-1, 0);
        if (n <= 0) {
            std::cerr << "[SERVER] Disconnected or error on recv (n = " << n << ")\n";
            break;
        }
        buf[n] = '\0';

        std::string line(buf);
        std::cout << "[SERVER] Received from client: " << line << "\n";

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd != "MOVE") {
            send(activeSock,
                 "ERROR: Expected MOVE <cardIdx> [tableIdx...]\n",
                 44, 0);
            continue;
        }

        int cardIdx;
        iss >> cardIdx;

        std::vector<int> choices;
        int idx;
        while (iss >> idx) {
            choices.push_back(idx);
        }

        // Build and apply the Move
        gameLogic::Player& activeP = round.getCurrentPlayer();
        gameLogic::Move mv(
            activeP.getName(),
            cardIdx,
            activeP.getHand()[cardIdx],
            choices
        );
        std::string res = round.processMove(mv);
        std::cout << "[SERVER] Processed MOVE: " << line << "\n";

        // Send updated table
        auto updatedTable = round.getTableCards();
        std::ostringstream ts2;
        ts2 << "TABLE:";
        for (size_t i = 0; i < updatedTable.size(); ++i) {
            ts2 << "[" << i << "]" << updatedTable[i].toString();
            if (i + 1 < updatedTable.size()) ts2 << ",";
        }
        ts2 << "\n";
        std::string tableAfterMove = ts2.str();
        sendAll(tableAfterMove);
        std::cout << "[SERVER] Updated table: " << tableAfterMove;

        // Send updated hands
        std::string updatedH1 = "HAND:" + p1.getHandString() + "\n";
        std::string updatedH2 = "HAND:" + p2.getHandString() + "\n";
        send(sess.player1.socket, updatedH1.c_str(), updatedH1.size(), 0);
        send(sess.player2.socket, updatedH2.c_str(), updatedH2.size(), 0);
        std::cout << "[SERVER] Updated HAND p1: " << updatedH1;
        std::cout << "[SERVER] Updated HAND p2: " << updatedH2;
    }

    // Round over: finalize scores and send
    p1.calculateFinalScore();
    p2.calculateFinalScore();
    sendAll("SCORE:" + p1.getScore() + "\n");
    sendAll("SCORE:" + p2.getScore() + "\n");
}

// Handle a single client: CREATE or JOIN
void Server::handleClient(int clientSock) {
    char buf[1024];
    int n = recv(clientSock, buf, sizeof(buf)-1, 0);
    if (n <= 0) return;
    buf[n] = '\0';

    std::istringstream iss(buf);
    std::string cmd, name, code;
    iss >> cmd;

    if (cmd == "CREATE") {
        iss >> name;
        std::string gameCode = generateCode();
        {
            std::lock_guard<std::mutex> lk(lobbyMutex);
            lobbies[gameCode].player1 = {clientSock, name};
            lobbies[gameCode].ready   = false;
        }

        // --- CORRECTED: always use .size() so newline is included ---
        std::string response = "GAME_CREATED " + gameCode + "\n";
        send(clientSock, response.c_str(), response.size(), 0);

        // wait for JOIN
        while (true) {
            {
                std::lock_guard<std::mutex> lk(lobbyMutex);
                if (lobbies[gameCode].ready) break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        std::string joined = "INFO:PLAYER_JOINED\n";
        send(clientSock, joined.c_str(), joined.size(), 0);

        startGameSession(gameCode);

    } else if (cmd == "JOIN") {
        iss >> code >> name;
        bool ok = false;
        {
            std::lock_guard<std::mutex> lk(lobbyMutex);
            auto it = lobbies.find(code);
            if (it != lobbies.end() && !it->second.ready) {
                it->second.player2 = {clientSock, name};
                it->second.ready   = true;
                ok = true;
            }
        }
        if (ok) {
            std::string resp = "INFO:JOINED_GAME\n";
            send(clientSock, resp.c_str(), resp.size(), 0);
        } else {
            std::string invalid = "INFO:INVALID_CODE\n";
            send(clientSock, invalid.c_str(), invalid.size(), 0);
            close(clientSock);
        }
    }
}

// Start listening and accept clients
void Server::run(int port) {
    int srv = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(srv, (sockaddr*)&addr, sizeof(addr));
    listen(srv, 5);

    std::cout << "[Server] Listening on port " << port << "...\n";
    while (true) {
        int client = accept(srv, nullptr, nullptr);
        std::cout << "[Server] Client " << client << " connected\n";
        std::thread(&Server::handleClient, this, client).detach();
    }
}

} // namespace gameServer
