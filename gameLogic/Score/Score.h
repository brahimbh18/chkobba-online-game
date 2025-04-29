#ifndef SCORE_H
#define SCORE_H

#include "../Card/Card.h"
#include <vector>

namespace gameLogic {

class Score {
private:
    int bermila;
    int dinari;
    int haya;
    int carta;
    int chkobba;
    int finalScore;

public:
    Score();

    void reset();
    void addChkobba();
    void calculateAfterRound(const std::vector<Card>& capturedCards);

    int getFinalScore() const;
    int getChkobba() const;
    int getBermila() const;
    int getDinari() const;
    int getHaya() const;
    int getCarta() const;
};

} // namespace gameLogic

#endif // SCORE_H
