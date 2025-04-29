#ifndef TABLE_H
#define TABLE_H

#include "../Card/Card.h"
#include <vector>
#include <string>

namespace gameLogic {

class Table {
private:
    std::vector<Card> cards;

public:
    Table();

    void addCard(const Card& card);
    void addCards(const std::vector<Card>& newCards);
    std::vector<Card> removeCards(const std::vector<int>& indices); // return removed cards
    bool hasExactMatch(const Card& card) const;
    std::string getTableState() const;
    const std::vector<Card>& getCards() const;
    bool empty() const;
    void clear();
};

} // namespace gameLogic

#endif // TABLE_H
