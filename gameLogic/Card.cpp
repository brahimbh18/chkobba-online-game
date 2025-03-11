#include "Card.h"

Card::Card(int v, int s): value(v), suit(s) {}

int Card::getVal() const {
    return value; 
}

char Card::getValue() const {
    return values[value - 1]; 
}

string Card::getSuit() const {
    return suits[suit];
}

string Card::getSymbol() const {
    return symbol[suit];
}

bool Card::matches(const Card& other) const {
    return value == other.value && suit == other.suit;
}

string Card::toString() const {
    return string(1, getValue()) + ":" + getSymbol();
}


void Card::swap(Card& other) {
      std::swap(value, other.value);
      std::swap(suit, other.suit);
}