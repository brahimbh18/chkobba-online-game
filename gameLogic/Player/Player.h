#ifndef PLAYER_H
#define PLAYER_H

#include "../Card/Card.h"
#include "../Score/Score.h"
#include <vector>
#include <string>

namespace gameLogic {

    class Player {
        private:
            std::string name;
            std::vector<Card> hand;
            std::vector<Card> capturedCards;
            Score score;

        public:
            Player(const std::string& playerName);

            std::string getName() const;

            const std::vector<Card>& getHand() const;
            const std::vector<Card>& getCapturedCards() const;

            std::string getHandString() const;
            std::string getCapturedCardsString() const;
            std::string getScore() const;

            void captureCards(const std::vector<Card>& cards);

            void drawCard(const Card& card);
            Card removeCardFromHand(int index);

            void addChkobba();
            void calculateFinalScore();
            const Score& getScoreDetail() const;
    };

} // namespace gameLogic

#endif // PLAYER_H
