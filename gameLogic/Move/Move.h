#ifndef MOVE_H
#define MOVE_H

#include "../Card/Card.h"
#include <vector>
#include <string>

namespace gameLogic {

class Move {
private:
    std::string playerName;
    Card cardPlayed;
    std::vector<int> capturedIndices;
    int handCardIndex; // NEW

public:
    Move(const std::string& playerName, int handCardIndex, const Card& cardPlayed, const std::vector<int>& capturedIndices);

    const std::string& getPlayerName() const;
    const Card& getCardPlayed() const;
    const std::vector<int>& getCapturedIndices() const;
    int getPlayedCardIndex() const; // NEW
    bool isCapture() const;
    std::string toString() const;

    static bool validateMove(const Card& cardPlayed, const std::vector<int>& capturedIndices, const std::vector<Card>& table);
};

} // namespace gameLogic

#endif // MOVE_H
