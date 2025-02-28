#ifndef CARD_H
#define CARD_H

#include <string>

using namespace std;

class Card {
private:
    int value;
    int suit;

    const char values[10] = {'A', '2', '3', '4', '5', '6', '7', 'Q', 'J', 'K'};
    const string suits[4] = {"Hearts", "Diamonds", "Clubs", "Spades"};

public:
    Card(int v, int s);

    char getValue() const;

    string getSuit() const;

    bool matches(const Card& other) const;

    string toString() const;

    void swap(Card& other);

};

#endif // CARD_H
