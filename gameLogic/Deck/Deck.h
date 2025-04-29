#ifndef DECK_H
#define DECK_H

#include "../Card/Card.h"
#include <vector>

namespace gameLogic {

class Deck {
private:
    std::vector<Card> cards;

public:
    Deck();
    void shuffle();
    Card drawCard();
    bool isEmpty() const;
    const std::vector<Card>& getCards() const;
};

} 

#endif 