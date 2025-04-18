#include "Player.h"
#include "Card.h"


Player::Player(const std::string& n): name(n) {
    score = 0;
}

string Player::getName() {
    return name;
}

string Player::showHand() {
    string h = name + ": ";

    for (Card& card: hand) h += card.toString() + ", ";
    
    return h;
}

void Player::setScore(int s) {
    score = s;
}

int Player::getScore() {
    return score;
}

void Player::addScore(int s) {
    score += s;
}

void Player::drawCard(Card card) {
    hand.push_back(card);
}