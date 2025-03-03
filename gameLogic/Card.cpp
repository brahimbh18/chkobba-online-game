#include "Card.h"

Card::Card(int v, int s): value(v), suit(s) {}

char Card::getValue() const {
    return values[value - 1]; 
}

string Card::getSuit() const {
    return suits[suit];
}

bool Card::matches(const Card& other) const {
    return value == other.value && suit == other.suit;
}

string Card::toString() const {
    return string(1, getValue()) + ":" + getSuit();
}


void Card::swap(Card& other) {
      std::swap(value, other.value);
      std::swap(suit, other.suit);
}