#include "Deck.h"
#include <random> 
#include <algorithm>
#include <stdexcept>

namespace gameLogic {

    Deck::Deck() {
    for (int i = 1; i <= 10; ++i) {
        for (int j = 0; j < 4; ++j) {
            cards.emplace_back(i, j);
        }
    }
}


void Deck::shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);
}


Card Deck::drawCard() {
    if (cards.empty()) {
        throw std::runtime_error("Deck is empty!");
    }
    Card c = cards.back();
    cards.pop_back();
    return c;
}


bool Deck::isEmpty() const {
    return cards.empty();
}


const std::vector<Card>& Deck::getCards() const {
    return cards;
}

}