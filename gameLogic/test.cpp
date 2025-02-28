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
    for (int i = 0; i < 4; i++) {
        table.push_back(deck.pop());
    }

}

Card popc(vector<Card> &v, int i) {
        Card c = v.at(i);
        v.erase(v.begin() + i);
        return c;
    }

void drawTable(Player &currentPlayer) {
    cout << "table: ";
    for (Card& card: table) cout << card.toString() << ", ";
    cout << endl;

    cout << currentPlayer.showHand() << endl;
    
}

void check(Player& currentPlayer, Card& card) {
    for (int i = 0; i < table.size(); i++) {
        if (table.at(i).getValue() == card.getValue()) {
            currentPlayer.capturedCards.push_back(card);
            currentPlayer.capturedCards.push_back(popc(table, i));
            return;
        }
    }

    table.push_back(card);

}

void makeMove(Player& currentPlayer) {
    int c;
    do {
        cout << "choice: " ;
        cin >> c;
    } while (c < 1 || c > currentPlayer.hand.size());
    Card ca = popc(currentPlayer.hand, c - 1);

    check(currentPlayer, ca);


}

bool haya(vector<Card> v) {
    for (Card card: v) {
        if (card.getSuit() == "Diamonds" && card.getValue() == 7) 
            return true;
    }

    return false;
}

bool dinari(vector<Card> v) {
    int d = 0;
    for (Card card: v) {
        if (card.getSuit() == "Diamonds") 
            d++;
    }

    return d > 5;
}

bool bermila(vector<Card> v) {
    int severns = 0, sixs = 0;

    for (Card card: v) {
        if (card.getValue() == 6) sixs++;
        else if (card.getValue() == 7) severns++;
    }

    return severns > 2 || (severns == 2 && sixs > 2);

}

int roundScore(Player& player) {
    int c, d, b, h;

    c = player.capturedCards.size() > 20;
    d = dinari(player.capturedCards);
    b = bermila(player.capturedCards);
    h = haya(player.capturedCards);

    return c + d + b + h;
}

void round() {
    bool roundOver = false;

    Deck deck = Deck();

    Player player1("hamza");
    Player player2("gadour");


    deck.shuffle();


    fillTheTable(deck);

    bool p = true;
    int r = 0;
    while (!roundOver) {
        if (r == 0) {
            servePlayers(deck, player1, player2);
        }
        r++;
        if (r == 6) r = 0;
        Player& currentPlayer = p ? player1 : player2;
        p = !p;

        drawTable(currentPlayer);
        makeMove(currentPlayer);
        if (table.size() == 0) currentPlayer.setScore(currentPlayer.getScore() + 1);
        if (deck.cards.size() == 0) roundOver = true;
    }

    player1.setScore(player1.getScore() + roundScore(player1));
    player2.setScore(player2.getScore() + roundScore(player2));

    cout << player1.getName() << ": " << player1.getScore() << endl;
    cout << player2.getName() << ": " << player2.getScore() << endl;

}

int main() {


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