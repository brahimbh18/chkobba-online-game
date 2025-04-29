#include "Table.h"
#include <sstream>
#include <algorithm>

namespace gameLogic {

Table::Table() {}

void Table::addCard(const Card& card) {
    cards.push_back(card);
}

void Table::addCards(const std::vector<Card>& newCards) {
    cards.insert(cards.end(), newCards.begin(), newCards.end());
}

std::vector<Card> Table::removeCards(const std::vector<int>& indices) {
    std::vector<Card> captured;
    std::vector<int> sortedIndices = indices;
    std::sort(sortedIndices.rbegin(), sortedIndices.rend());

    for (int idx : sortedIndices) {
        if (idx >= 0 && idx < static_cast<int>(cards.size())) {
            captured.push_back(cards[idx]);
            cards.erase(cards.begin() + idx);
        }
    }
    return captured;
}

bool Table::hasExactMatch(const Card& card) const {
    for (const auto& c : cards) {
        if (c.getVal() == card.getVal()) return true;
    }
    return false;
}

std::string Table::getTableState() const {
    std::stringstream ss;
    ss << "Table cards: ";
    for (size_t i = 0; i < cards.size(); ++i) {
        ss << "[" << i << "]" << cards[i].toString();
        if (i != cards.size() - 1) ss << ", ";
    }
    return ss.str();
}

const std::vector<Card>& Table::getCards() const {
    return cards;
}

bool Table::empty() const {
    return cards.empty();
}

void Table::clear() {
    cards.clear();
}

}