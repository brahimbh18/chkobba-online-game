#ifndef GAME_H
#define GAME_H

#include "../Player/Player.h"
#include "../Round/Round.h"
#include <string>

namespace gameLogic {

class Game {
private:
    Player player1;
    Player player2;
    bool isGameOver;

public:
    Game(const std::string& p1Name, const std::string& p2Name);
    void run();
    void runAuto(); 
};

} // namespace gameLogic

#endif // GAME_H
