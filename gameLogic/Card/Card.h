#ifndef CARD_H
#define CARD_H

#include <string>

namespace gameLogic{
    
class Card {
private:
    int value;
    int suit;


public:
    Card(int v, int s);

    int getVal() const;

    char getValue() const;

    std::string getSuit() const;
    
    std::string getSymbol() const;

    bool matches(const Card& other) const;

    std::string toString() const;

    void swap(Card& other);

    static const char values[10];
    static const std::string suits[4];
    static const std::string symbols[4];
};
}
#endif
