#include "Card.h"
#include <vector>

class Deck {
private:
    std::vector<Card> cards;
    
public:
    Deck();  // Initialize with standard Chkobba 40 cards
    std::vector<Card> getCards();
    void shuffle();
    Card drawCard();
    bool isEmpty() const;
    Card  pop();
};