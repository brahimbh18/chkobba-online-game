#include "Score.h"

namespace gameLogic {

Score::Score()
    : bermila(0), dinari(0), haya(0), carta(0), chkobba(0), finalScore(0) {}

void Score::reset() {
    bermila = dinari = haya = carta = chkobba = finalScore = 0;
}

void Score::addChkobba() {
    chkobba++;
}

void Score::calculateAfterRound(const std::vector<Card>& capturedCards) {
    int diamonds = 0;
    int sevens = 0;
    int sixes = 0;

    for (const auto& card : capturedCards) {
        if (card.getSuit() == "Diamonds") {
            diamonds++;
        }
        if (card.getVal() == 7) {
            sevens++;
        }
        if (card.getVal() == 6) {
            sixes++;
        }
    }

    if (diamonds >= 6) {
        dinari = 1;
    }

    for (const auto& card : capturedCards) {
        if (card.getSuit() == "Diamonds" && card.getVal() == 7) {
            haya = 1;
            break;
        }
    }

    if ((sevens >= 3) || (sevens == 2 && sixes >= 3)) {
        bermila = 1;
    }

    if (capturedCards.size() > 20) {
        carta = 1;
    }

    finalScore = bermila + dinari + haya + carta + chkobba;
}

int Score::getFinalScore() const {
    return finalScore;
}

int Score::getChkobba() const {
    return chkobba;
}

int Score::getBermila() const {
    return bermila;
}

int Score::getDinari() const {
    return dinari;
}

int Score::getHaya() const {
    return haya;
}

int Score::getCarta() const {
    return carta;
}

} // namespace gameLogic
