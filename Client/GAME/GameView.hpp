#ifndef GAME_GAMEVIEW_HPP
#define GAME_GAMEVIEW_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class GameView {
public:
    GameView();

    // Supply table & hand codes like {"C4","R10",...}
    void setCards(const std::vector<std::string>& table,
                  const std::vector<std::string>& hand);

    // Draw everything
    void render(sf::RenderWindow& window);

    // Handles a click: lifts/drops and returns the code clicked ("" if none)
    std::string handleClick(const sf::Vector2f& mousePos);
    int getHandSelected() const;
    std::vector<bool> getTableSelected() const;
    void clearSelection();


private:
    sf::RectangleShape background;
    std::vector<sf::Texture> textures;

    std::vector<sf::Sprite> tableSprites;
    std::vector<sf::Vector2f> tableOrig;      // original positions
    std::vector<bool>         tableSelected;  // toggle state

    std::vector<sf::Sprite>   playerSprites;
    std::vector<sf::Vector2f> playerOrig;     // original positions
    std::vector<std::string>  playerCodes;    // code for each sprite
    int                        handSelected;  // index of lifted hand card, or -1

    sf::Sprite opponentCard, playerPile, opponentPile;

    // load "assets/cards/<code>.png", scale & position
    sf::Sprite loadCard(const std::string& code,
                        sf::Vector2f position,
                        sf::Texture& tex);
};

#endif // GAME_GAMEVIEW_HPP
