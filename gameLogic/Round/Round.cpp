#include "Round.h"
#include <sstream>

namespace gameLogic {

Round::Round(Player& p1, Player& p2)
    : player1(p1), player2(p2),
      lastCapturingPlayer(nullptr),
      roundOver(false),
      currentTurn(0),
      currentCycle(0)
{
    deck.shuffle();
    initializeRound();
}

void Round::initializeRound() {
    table.clear();
    for (int i = 0; i < 4; ++i) {
        table.addCard(deck.drawCard());
    }
    dealInitialCards();
}

void Round::dealInitialCards() {
    for (int i = 0; i < 3; ++i) {
        player1.drawCard(deck.drawCard());
        player2.drawCard(deck.drawCard());
    }
}

std::string Round::startRound() {
    std::stringstream ss;
    ss << "Starting new round!\nPlayers: " << player1.getName()
       << " vs " << player2.getName() << "\n";
    return ss.str();
}

std::string Round::getGameState() const {
    std::stringstream ss;
    ss << "=== Game State ===\n"
       << table.getTableState() << "\n"
       << player1.getName() << "'s hand: " << player1.getHandString() << "\n"
       << player2.getName() << "'s hand: " << player2.getHandString() << "\n";
    return ss.str();
}

std::string Round::nextTurn() {
    if (roundOver) return "Round has ended!";
    Player& player = getCurrentPlayer();
    std::stringstream ss;
    ss << "=== " << player.getName() << "'s turn ===\n"
       << table.getTableState() << "\nYour hand: " << player.getHandString();
    return ss.str();
}

std::string Round::processMove(const Move& move) {
    if (roundOver) return "Round has ended!";
    Player& current = getCurrentPlayer();

    if (!Move::validateMove(move.getCardPlayed(), move.getCapturedIndices(), table.getCards())) {
        return "Invalid move according to rules!";
    }

    executeMove(current, move);
    advanceTurn();
    checkRoundEnd();
    return move.toString();
}

void Round::executeMove(Player& player, const Move& move) {
    int playedCardIndex = move.getPlayedCardIndex();
    Card playedCard = player.removeCardFromHand(playedCardIndex);

    const std::vector<int>& indices = move.getCapturedIndices();

    if (indices.empty()) {
        table.addCard(playedCard);
    } else {
        std::vector<Card> captured = table.removeCards(indices);
        captured.push_back(playedCard);
        player.captureCards(captured);
        lastCapturingPlayer = &player;
        if (table.empty()) {
            player.addChkobba();
        }
    }
}

void Round::checkRoundEnd() {
    if (deck.isEmpty() && player1.getHand().empty() && player2.getHand().empty()) {
        finalizeRound();
    } else if (player1.getHand().empty() && player2.getHand().empty()) {
        dealInitialCards();
        ++currentCycle;
    }
}

void Round::finalizeRound() {
    if (!table.empty() && lastCapturingPlayer) {
        lastCapturingPlayer->captureCards(table.getCards());
        table.clear();
    }

    player1.calculateFinalScore();
    player2.calculateFinalScore();

    roundOver = true;
}

Player& Round::getCurrentPlayer() {
    return (currentTurn == 0) ? player1 : player2;
}

const std::vector<Card>& Round::getTableCards() const {
    return table.getCards();
}

void Round::advanceTurn() {
    currentTurn = (currentTurn + 1) % 2;
}

bool Round::isRoundOver() const {
    return roundOver;
}

} // namespace gameLogic
