#include "Player.h"
#include <sstream>
#include <stdexcept>

namespace gameLogic {

Player::Player(const std::string& playerName)
    : name(playerName) {}

std::string Player::getName() const {
    return name;
}

const std::vector<Card>& Player::getHand() const {
    return hand;
}

const std::vector<Card>& Player::getCapturedCards() const {
    return capturedCards;
}

std::string Player::getHandString() const {
    std::stringstream ss;
    for (size_t i = 0; i < hand.size(); ++i) {
        ss << i << "-" << hand[i].toString();
        if (i != hand.size() - 1) {
            ss << ", ";
        }
    }
    return ss.str();
}

std::string Player::getCapturedCardsString() const {
    std::stringstream ss;
    ss << "Captured (" << capturedCards.size() << "): ";
    for (size_t i = 0; i < capturedCards.size(); ++i) {
        ss << capturedCards[i].toString();
        if (i != capturedCards.size() - 1) {
            ss << ", ";
        }
    }
    return ss.str();
}

void Player::captureCards(const std::vector<Card>& cards) {
    capturedCards.insert(capturedCards.end(), cards.begin(), cards.end());
}


std::string Player::getScore() const {
    std::stringstream ss;
    ss << "Score breakdown for " << name << ":\n";
    ss << " - Final score: " << score.getFinalScore() << "\n";
    ss << " - Chkobba: " << score.getChkobba() << "\n";
    ss << " - Carta: " << (score.getCarta() ? "Yes (+1)" : "No") << "\n";
    ss << " - Dinari: " << (score.getDinari() ? "Yes (+1)" : "No") << "\n";
    ss << " - Haya: " << (score.getHaya() ? "Yes (+1)" : "No") << "\n";
    ss << " - Bermila: " << (score.getBermila() ? "Yes (+1)" : "No") << "\n";
    return ss.str();
}


void Player::drawCard(const Card& card) {
    hand.push_back(card);
}

Card Player::removeCardFromHand(int index) {
    if (index < 0 || index >= static_cast<int>(hand.size())) {
        throw std::out_of_range("Invalid card index!");
    }
    Card c = hand.at(index);
    hand.erase(hand.begin() + index);
    return c;
}

void Player::addChkobba() {
    score.addChkobba();
}

void Player::calculateFinalScore() {
    score.calculateAfterRound(capturedCards);
}

const Score& Player::getScoreDetail() const {
    return score;
}

} // namespace gameLogic
