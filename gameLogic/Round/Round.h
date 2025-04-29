#ifndef ROUND_H
#define ROUND_H

#include "../Player/Player.h"
#include "../Deck/Deck.h"
#include "../Table/Table.h"
#include "../Move/Move.h"
#include <string>

namespace gameLogic {

class Round {
private:
    Player& player1;
    Player& player2;
    Deck deck;
    Table table;
    Player* lastCapturingPlayer;

    bool roundOver;
    int currentTurn;
    int currentCycle;

    void initializeRound();
    void dealInitialCards();
    void checkRoundEnd();
    void finalizeRound();
    void executeMove(Player& player, const Move& move);
    void advanceTurn();

public:
    Round(Player& p1, Player& p2);

    std::string startRound();
    std::string getGameState() const;
    std::string nextTurn();
    std::string processMove(const Move& move);
    bool isRoundOver() const;
    Player& getCurrentPlayer();
    const std::vector<Card>& getTableCards() const;
};

} // namespace gameLogic

#endif // ROUND_H
