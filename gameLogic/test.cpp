#include "Deck.cpp"
#include "Card.h"
#include "Player.h"
#include <cmath>
#include <string>
#include <iostream>
#include <vector>
#include <ctime>


using namespace std;


vector<Card> table;

void makeMove(Player& currentPlayer);
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

void check(Player& currentPlayer, int c, vector<int> v) {
    if (v.empty()) {
        table.push_back(popc(currentPlayer.hand,c - 1));
        return;
    }

    Card card = currentPlayer.hand.at(c - 1);
  
    int s = 0;
    for (int i: v) {
        s += table.at(i - 1).getVal();
        if (s > card.getVal()) break;
    }

    if (s == card.getVal()) {
        card = popc(currentPlayer.hand, c - 1);
        currentPlayer.capturedCards.push_back(card);
        for (int i: v) {
            Card t = popc(table, i - 1);
            currentPlayer.capturedCards.push_back(t);
        }
        
    } else {
        cout << s << endl;
        cout << card.getVal() << endl;
        cout << "Invalid move" << endl;
        makeMove(currentPlayer);
    } 

}

int getChoice(int min, int max) {
    int c;
    do {
        cout << " ->choice: " ;
        cin >> c;
    } while (c < min || c > max );

    return c;
}

void makeMove(Player& currentPlayer) {
    int c = getChoice(1, currentPlayer.hand.size());

    vector<int> a;
    int k, l = table.size(); 
    cout << "cards to eat card by card :\n"; 
    cout << "0 for finishing the list\n";
    for (int i = 0; a.size() <= table.size(); i++) { 
        k = getChoice(0, table.size()); 
        if (k == 0) goto skip;
        a.push_back(k);
    }
    skip:
    for (int i : a) cout << i << ", ";
    cout << endl;

    check(currentPlayer, c, a);
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

int roundScore(vector<Card> v) {
    int c, d, b, h;

    c = v.size() > 20;
    d = dinari(v);
    b = bermila(v);
    h = haya(v);
    cout << "Calculating round score - " 
         << " Cards: " << c << " Dinari: " << d 
         << " Bermila: " << b << " Haya: " << h << endl;
    return c + d + b + h;
}

void turn(Player& player) {
    drawTable(player);
    makeMove(player);
    
    if (table.size() == 0) player.setScore(player.getScore() + 1);
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

    for (int i = 0; i < 6; i++) {
        servePlayers(deck, player1, player2);
        for (int j = 0; j < 3; j++) {
            turn(player1);
            turn(player2);
        }
    }


    player1.setScore(player1.getScore() + roundScore(player1.capturedCards));
    player2.setScore(player2.getScore() + roundScore(player2.capturedCards));


    cout << player1.getName() << ": " << player1.getScore() << endl;
    cout << player2.getName() << ": " << player2.getScore() << endl;

}

int main() {

srand(time(NULL));
    round(); 
    return 0;
}
