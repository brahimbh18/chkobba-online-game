#include "Game.h"
#include "../Move/Move.h"
#include <iostream>
#include <limits>
#include <random>
namespace gameLogic {

Game::Game(const std::string& p1Name, const std::string& p2Name)
    : player1(p1Name), player2(p2Name), isGameOver(false) {}

void Game::run() {
    Round round(player1, player2); // Create round manually
    std::cout << round.startRound() << std::endl;

    while (!round.isRoundOver()) {
        std::cout << round.getGameState() << std::endl;
        std::cout << round.nextTurn() << std::endl;

        Player& currentPlayer = round.getCurrentPlayer();

        int handChoice;
        std::cout << "Choose a card to play (enter card index shown next to card): ";
        std::cin >> handChoice;

        std::vector<int> tableChoices;
        std::cout << "Enter indices of table cards to capture separated by space (enter -1 to finish): ";

        int choice;
        while (std::cin >> choice && choice != -1) {
            tableChoices.push_back(choice);
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (handChoice < 0 || handChoice >= static_cast<int>(currentPlayer.getHand().size())) {
            std::cout << "Invalid hand choice! Try again.\n";
            continue;
        }

        Card playedCard = currentPlayer.getHand()[handChoice];

        if (!Move::validateMove(playedCard, tableChoices, round.getTableCards())) {
            std::cout << "Invalid move! Try again.\n";
            continue;
        }

        Move move(currentPlayer.getName(), handChoice, playedCard, tableChoices);
        std::cout << round.processMove(move) << std::endl;
    }

    std::cout << "Round finished!\n";
    std::cout << round.getGameState() << std::endl;

    // Show final scores
    std::cout << "\n=== FINAL ROUND SCORES ===\n";
    std::cout << player1.getScore() << "\n";
    std::cout << player2.getScore() << "\n";
}
void Game::runAuto() {
    Round round(player1, player2);
    std::cout << round.startRound() << "\n";

    std::mt19937 rng(std::random_device{}());

    while (!round.isRoundOver()) {
        Player& currentPlayer = round.getCurrentPlayer();
        const auto& hand = currentPlayer.getHand();
        const auto& table = round.getTableCards();

        // Try each card in hand until a legal move is found
        bool movePlayed = false;
        for (size_t cardIndex = 0; cardIndex < hand.size(); ++cardIndex) {
            Card card = hand[cardIndex];

            // Try capturing each table card as a single match
            for (size_t tableIndex = 0; tableIndex < table.size(); ++tableIndex) {
                std::vector<int> singlePick = {static_cast<int>(tableIndex)};
                if (Move::validateMove(card, singlePick, table)) {
                    Move move(currentPlayer.getName(), cardIndex, card, singlePick);
                    std::cout << round.processMove(move) << "\n";
                    movePlayed = true;
                    break;
                }
            }

            if (movePlayed) break;

            // Try placing the card on table if no captures
            if (Move::validateMove(card, {}, table)) {
                Move move(currentPlayer.getName(), cardIndex, card, {});
                std::cout << round.processMove(move) << "\n";
                movePlayed = true;
                break;
            }
        }

        // Fallback (should never happen)
        if (!movePlayed) {
            Move move(currentPlayer.getName(), 0, hand[0], {});
            std::cout << round.processMove(move) << "\n";
        }
    }

    std::cout << "\n=== FINAL ROUND SCORES ===\n";
    std::cout << player1.getScore() << "\n";
    std::cout << player2.getScore() << "\n";
}


} // namespace gameLogic
