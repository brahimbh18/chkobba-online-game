#include "Deck.cpp"
#include "Card.h"
#include "Player.h"
#include <cmath>
#include <string>
#include <iostream>
#include <vector>


using namespace std;


vector<Card> table;

void servePlayers(Deck& deck, Player& player1, Player& player2) {
    for (int j = 0; j < 3; j++) {
        player1.hand.push_back(deck.pop());
    }

    for (int j = 0; j < 3; j++) {
        player2.hand.push_back(deck.pop());
    }
}

void  fillTheTable(Deck &deck) {
    for (int i = 0; i <= 4; i++) {
        table.push_back(deck.pop());
    }

}

void drawTable(Player &currentPlayer) {
    cout << "table: " << endl;
    for (Card& card: table) cout << card.toString();

    
}

void round() {
    bool roundOver = false;

    Deck deck = Deck();

    Player player1("hamza");
    Player player2("gadour");

    Player currentPlayer = player1;

    servePlayers(deck, player1, player2);

    fillTheTable(deck);

    while (!roundOver) {
        drawTable(currentPlayer);
        roundOver = true;
    }
}

int main() {
    Deck deck = Deck();
    deck.shuffle();

    round(); 
    //deck.toString();
    // vector<int> v = {1, 2, 3};

    // for (int i: v) cout << i << ", ";
    // cout << endl;

    // int a = v.back();
    // v.pop_back();

    // for (int i: v) cout << i << ", ";
    // cout << endl;

    return 0;
}