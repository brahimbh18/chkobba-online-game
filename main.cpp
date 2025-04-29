#include "gameLogic/Game/Game.h"
#include <iostream>
#include <string>

using namespace gameLogic;

int main() {
    std::cout << "Welcome to Chkobba Game!\n";

    std::string player1Name, player2Name;
    std::cout << "Enter Player 1 name: ";
    std::getline(std::cin, player1Name);

    std::cout << "Enter Player 2 name: ";
    std::getline(std::cin, player2Name);

    Game game(player1Name, player2Name);

    game.runAuto();  // Start the game loop

    std::cout << "\nThank you for playing!\n";

    return 0;
}