#include "Move.h"
#include <sstream>

namespace gameLogic {

Move::Move(const std::string& playerName, int handCardIndex, const Card& cardPlayed, const std::vector<int>& capturedIndices)
    : playerName(playerName), handCardIndex(handCardIndex), cardPlayed(cardPlayed), capturedIndices(capturedIndices) {}

const std::string& Move::getPlayerName() const {
    return playerName;
}

const Card& Move::getCardPlayed() const {
    return cardPlayed;
}

const std::vector<int>& Move::getCapturedIndices() const {
    return capturedIndices;
}

int Move::getPlayedCardIndex() const {
    return handCardIndex;
}

bool Move::isCapture() const {
    return !capturedIndices.empty();
}

std::string Move::toString() const {
    std::stringstream ss;
    if (isCapture()) {
        ss << playerName << " plays " << cardPlayed.toString() << " capturing table cards at indices [";
        for (size_t i = 0; i < capturedIndices.size(); ++i) {
            ss << capturedIndices[i];
            if (i != capturedIndices.size() - 1) {
                ss << ", ";
            }
        }
        ss << "]";
    } else {
        ss << playerName << " plays " << cardPlayed.toString() << " placing card on table.";
    }
    return ss.str();
}


bool Move::validateMove(const Card& cardPlayed, const std::vector<int>& capturedIndices, const std::vector<Card>& table) {
    if (table.empty()) {
        return capturedIndices.empty(); // Only placement is allowed
    }

    bool exactMatchExists = false;
    for (const auto& tableCard : table) {
        if (tableCard.getVal() == cardPlayed.getVal()) {
            exactMatchExists = true;
            break;
        }
    }

    if (exactMatchExists) {
        // If exact match exists, player MUST capture it
        if (capturedIndices.size() != 1) return false;
        int idx = capturedIndices[0];
        if (idx < 0 || idx >= static_cast<int>(table.size())) return false;
        return table[idx].getVal() == cardPlayed.getVal();
    } else {
        // If no exact match, allow sum captures OR placing with no capture
        if (capturedIndices.empty()) {
            return true; // Placement allowed
        }
        
        int sum = 0;
        for (int idx : capturedIndices) {
            if (idx < 0 || idx >= static_cast<int>(table.size())) return false;
            sum += table[idx].getVal();
            if (sum > cardPlayed.getVal()) return false;
        }
        return sum == cardPlayed.getVal();
    }
}
}