#ifndef PLAYER_H
#define PLAYER_H

#include "Card.h"
#include <vector>


class Player {
private:
  std::string name;
  int score;

public:
  std::vector<Card> hand;
  std::vector<Card> capturedCards;
  Player(const std::string& playerName);

  std::string getName();
};
#endif
