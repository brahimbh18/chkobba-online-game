#include "GameView.hpp"
#include <iostream>
#include <regex>

static const int WINDOW_WIDTH  = 800;
static const int WINDOW_HEIGHT = 600;
static const int CARD_WIDTH    = 60;
static const int CARD_HEIGHT   = 90;

GameView::GameView()
: handSelected(-1)
{
    background.setSize({float(WINDOW_WIDTH), float(WINDOW_HEIGHT)});
    background.setFillColor({34,139,34});
    textures.resize(10);
}

sf::Sprite GameView::loadCard(const std::string& code,
                              sf::Vector2f pos,
                              sf::Texture& tex)
{
    std::string path = "assets/cards/" + code + ".png";
    if (!tex.loadFromFile(path)) {
        std::cerr << "Failed to load card: " << path << "\n";
    }
    sf::Sprite s(tex);
    s.setPosition(pos);
    s.setScale(
        CARD_WIDTH  / float(tex.getSize().x),
        CARD_HEIGHT / float(tex.getSize().y)
    );
    return s;
}

void GameView::setCards(const std::vector<std::string>& table,
                        const std::vector<std::string>& hand)
{
    // --- table setup ---
    tableSprites.clear();
    tableOrig.clear();
    tableSelected.clear();
    for (size_t i = 0; i < table.size(); ++i) {
        float x = 250 + i*(CARD_WIDTH+20), y = 250;
        tableSprites.push_back(loadCard(table[i], {x,y}, textures[i]));
        tableOrig.push_back({x,y});
        tableSelected.push_back(false);
    }

    // --- hand setup ---
    playerSprites.clear();
    playerOrig.clear();
    playerCodes = hand;
    handSelected = -1;
    for (size_t i = 0; i < hand.size(); ++i) {
        float x = 250 + i*(CARD_WIDTH+20), y = 500;
        playerSprites.push_back(loadCard(hand[i], {x,y}, textures[4+i]));
        playerOrig.push_back({x,y});
    }

    // --- piles/back cards ---
    opponentCard = loadCard("card_back", {260,50}, textures[7]);
    playerPile   = loadCard("card_back", { 50,500}, textures[8]);
    opponentPile = loadCard("card_back", { 50, 50}, textures[9]);
}

void GameView::render(sf::RenderWindow& w) {
    w.draw(background);
    for (auto& s : tableSprites)  w.draw(s);
    for (auto& s : playerSprites) w.draw(s);
    w.draw(opponentCard);
    w.draw(playerPile);
    w.draw(opponentPile);
}

std::string GameView::handleClick(const sf::Vector2f& mouse) {
    // 1) Check hand cards (only one selected at a time)
    for (size_t i = 0; i < playerSprites.size(); ++i) {
        if (playerSprites[i].getGlobalBounds().contains(mouse)) {
            // if clicking same card again -> drop it
            if (int(i) == handSelected) {
                playerSprites[i].setPosition(playerOrig[i]);
                handSelected = -1;
            } else {
                // drop previous if any
                if (handSelected >= 0)
                    playerSprites[handSelected].setPosition(playerOrig[handSelected]);
                // lift new one
                auto p = playerOrig[i];
                playerSprites[i].setPosition(p.x, p.y - 20.f);
                handSelected = int(i);
            }
            return playerCodes[i];
        }
    }

    // 2) Check table cards (each toggles independently)
    for (size_t i = 0; i < tableSprites.size(); ++i) {
        if (tableSprites[i].getGlobalBounds().contains(mouse)) {
            if (tableSelected[i]) {
                // drop it
                tableSprites[i].setPosition(tableOrig[i]);
                tableSelected[i] = false;
            } else {
                // lift it
                auto p = tableOrig[i];
                tableSprites[i].setPosition(p.x, p.y - 20.f);
                tableSelected[i] = true;
            }
            return "";  // you could also return table code if you track it
        }
    }

    return "";
}
