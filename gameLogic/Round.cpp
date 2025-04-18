#include "Deck.cpp"
#include "Card.h"
#include "Player.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>

class Round {
private:
    Deck deck;
    Player player1;
    Player player2;
    std::vector<Card> table;
    int currentPlayerIndex;
    Player* lastCapturingPlayer;  // New tracking variable

    void fillTable() {
        table.clear();
        for (int i = 0; i < 4; i++) {
            table.push_back(deck.pop());
        }
    }

    void servePlayers() {
        for (int j = 0; j < 3; j++) {
            player1.drawCard(deck.pop());
            player2.drawCard(deck.pop());
        }
    }

    Card popCard(std::vector<Card>& cards, int index) {
        Card c = cards.at(index);
        cards.erase(cards.begin() + index);
        return c;
    }

    void drawTable(Player& currentPlayer) {
        std::cout << "\nTable: ";
        for (Card& card : table) std::cout << card.toString() << ", ";
        std::cout << "\n" << currentPlayer.showHand() << std::endl;
    }

    bool validateMove(Card& playerCard, const std::vector<int>& selectedTableCards) {
        if (selectedTableCards.size() == 1 && table[selectedTableCards[0] - 1].getVal() == playerCard.getVal()) return true;

        for (Card& c: table) {
            if (c.getVal() == playerCard.getVal()) return false;
        }

        int sum = 0;
        for (int i : selectedTableCards) {
            sum += table.at(i - 1).getVal();
            if (sum > playerCard.getVal()) break;
        }
        return sum == playerCard.getVal();
    }

    void processCapture(Player& player, int handIndex, const std::vector<int>& tableIndices) {
        std::vector<int> sortedIndices(tableIndices);
        std::sort(sortedIndices.rbegin(), sortedIndices.rend());
        
        Card playedCard = popCard(player.hand, handIndex - 1);
        player.capturedCards.push_back(playedCard);
        
        // Update last capturing player
        lastCapturingPlayer = &player;
        
        // Remove table cards from highest index to lowest
        for (int i : sortedIndices) {
            if (i-1 < table.size()) {
                Card t = popCard(table, i-1);
                player.capturedCards.push_back(t);
            }
        }
        
        if (table.empty()) {
            player.addScore(1);
        }
    }

    int getChoice(int min, int max) {
        int c;
        do {
            std::cout << " -> Choice: ";
            std::cin >> c;
        } while (c < min || c > max);
        return c;
    }

    void makeMove(Player& currentPlayer) {
        drawTable(currentPlayer);
        
        std::cout << "Choose a card to play (1-" << currentPlayer.hand.size() << "):\n";
        int cardChoice = getChoice(1, currentPlayer.hand.size());
        
        std::vector<int> tableChoices;
        std::cout << "Select table cards to capture (1-" << table.size() << "), 0 to finish:\n";
        while (tableChoices.size() < table.size()) {
            int choice = getChoice(0, table.size());
            if (choice == 0) break;
            if (std::find(tableChoices.begin(), tableChoices.end(), choice) == tableChoices.end()) {
                tableChoices.push_back(choice);
            }
        }

        Card& chosenCard = currentPlayer.hand[cardChoice - 1];
        if (tableChoices.empty()) {
            table.push_back(popCard(currentPlayer.hand, cardChoice - 1));
        } else if (validateMove(chosenCard, tableChoices)) {
            processCapture(currentPlayer, cardChoice, tableChoices);
            if (table.empty()) currentPlayer.addScore(1);
        } else {
            std::cout << "Invalid move! Try again.\n";
            makeMove(currentPlayer);
        }
    }

    bool checkHaya(const std::vector<Card>& cards) {
        return std::any_of(cards.begin(), cards.end(), [](const Card& c) {
            return c.getSuit() == "Diamonds" && c.getValue() == 7;
        });
    }
    int checkBermila(const std::vector<Card>& p1Cards) {
        int p1_7 = 0, p1_6 = 0;
    
        for (const Card& c : p1Cards) {
            if (c.getValue() == 7) p1_7++;
            else if (c.getValue() == 6) p1_6++;
        }
        if (p1_7 >= 3 || (p1_7 == 2 && p1_6 >= 3)) return 1;
        if (p1_7 == 2 && p1_6 == 2) return 0;
        else return 2;
    }

    int calculateRoundScore(Player& player) {
        int score = 0;
        if (player.capturedCards.size() > 20) score += 1;
        if (checkHaya(player.capturedCards)) score += 1;
        return score;
    }

public:
    Round(std::string p1Name, std::string p2Name) 
        : player1(p1Name), player2(p2Name), currentPlayerIndex(0), lastCapturingPlayer(nullptr) {
        deck.shuffle();
        fillTable();
    }

    void start() {
        for (int cycle = 0; cycle < 6; cycle++) {
            servePlayers();
            for (int turn = 0; turn < 3; turn++) {
                playTurn(player1);
                playTurn(player2);
            }
        }
        
        // Add remaining table cards to last capturer
        if (!table.empty() && lastCapturingPlayer != nullptr) {
            lastCapturingPlayer->capturedCards.insert(
                lastCapturingPlayer->capturedCards.end(),
                table.begin(),
                table.end()
            );
            table.clear();
        }
        
        player1.addScore(calculateRoundScore(player1));
        player2.addScore(calculateRoundScore(player2));
        
        int bermilaWinner = checkBermila(player1.capturedCards);
        if (bermilaWinner == 1) player1.addScore(1);
        else if (bermilaWinner == 2) player2.addScore(1);

        std::cout << "\nFinal Scores:\n";
        std::cout << player1.getName() << ": " << player1.getScore() << "\n";
        std::cout << player2.getName() << ": " << player2.getScore() << "\n";
    }

    void playTurn(Player& player) {
        makeMove(player);
    }
};

int main() {
    srand(time(NULL));
    Round gameRound("hamza", "gadour");
    gameRound.start();
    return 0;
}