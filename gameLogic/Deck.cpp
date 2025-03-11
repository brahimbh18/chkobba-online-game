#include "Card.h"
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <bits/stdc++.h>
#include <random>


using namespace std;

class Deck {
    
public:
    vector<Card> cards;
    Deck() {
        for (int i = 1; i <= 10; i++) {
            for (int j = 0; j < 4; j++) {
                cards.push_back(Card(i, j));
            }
        }
    }
    
    std::vector<Card> getCards() {
        return cards;
    }

    // Card drawCard()

    bool isEmpty() {
        return cards.size() == 0;
    }

    void toString() {
        for (Card& card: cards) {
            cout << card.toString() << endl;
        }
    }

    void shuffle() {

        for (int i = cards.size() - 1 - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            cards[i].swap(cards[j]);
        }
    }

    Card pop() {
        Card c = cards.back();
        cards.pop_back();
        return c;
    }
};