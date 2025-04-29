#include "Card.h"
#include <utility>

namespace gameLogic {

const char Card::values[10] = {'A', '2', '3', '4', '5', '6', '7', 'Q', 'J', 'K'};
const std::string Card::suits[4] = {"Hearts", "Diamonds", "Clubs", "Spades"};
const std::string Card::symbols[4] = {"♥", "♦", "♣", "♠"};

Card::Card(int v, int s) : value(v), suit(s) {}

int Card::getVal() const {
    return value;
}

char Card::getValue() const {
    return values[value - 1];
}

std::string Card::getSuit() const {
    return suits[suit];
}

std::string Card::getSymbol() const {
    return symbols[suit];
}

bool Card::matches(const Card& other) const {
    return value == other.value && suit == other.suit;
}

std::string Card::toString() const {
    return std::string(1, getValue()) + ":" + getSymbol();
}

void Card::swap(Card& other) {
    std::swap(value, other.value);
    std::swap(suit, other.suit);
}

}
