#include "Server/Server.h"

int main() {
    gameServer::Server server;
    server.run(4000);
    return 0;
}
