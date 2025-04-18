#ifndef CARD_H
#define CARD_H

#include <string>

using namespace std;
    const char values[10] = {'A', '2', '3', '4', '5', '6', '7', 'Q', 'J', 'K'};
    const string suits[4] = {"Hearts", "Diamonds", "Clubs", "Spades"};
    const string symbol[4] = {"♥", "♦", "♣", "♠"};
class Card {
private:
    int value;
    int suit;


public:
    Card(int v, int s);

    int getVal() const;

    char getValue() const;

    string getSuit() const;
    
    string getSymbol() const;

    bool matches(const Card& other) const;

    string toString() const;

    void swap(Card& other);

};

#endif
