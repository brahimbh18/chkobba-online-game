#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const int CARD_WIDTH = 60;
const int CARD_HEIGHT = 90;

// Load a card sprite from file
sf::Sprite loadCard(const std::string& filename, sf::Vector2f position, sf::Texture& textureOut) {
    textureOut.loadFromFile(filename);
    sf::Sprite sprite(textureOut);
    sprite.setPosition(position);
    sprite.setScale(
        CARD_WIDTH / static_cast<float>(textureOut.getSize().x),
        CARD_HEIGHT / static_cast<float>(textureOut.getSize().y)
    );
    return sprite;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Chkobba GUI - With Cards");

    // Green felt background
    sf::RectangleShape background(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    background.setFillColor(sf::Color(34, 139, 34));

    // Store textures (must remain in scope)
    std::vector<sf::Texture> textures(10); // 3 player + 4 table + 1 opponent + 2 extra

    // File paths to player’s hand cards (bottom)
    std::vector<std::string> playerCardFiles = {
        "assets/cards/R1.png",   // Ace of Hearts
        "assets/cards/C5.png",   // 5 of Diamonds
        "assets/cards/B7.png"    // 7 of Spades 
    };

    // File paths to cards on the table (center)
    std::vector<std::string> tableCardFiles = {
        "assets/cards/T9.png",   // 9 of Clubs
        "assets/cards/C8.png",   // Queen of Diamonds
        "assets/cards/R10.png",  // King of Hearts
        "assets/cards/B3.png"    // 3 of Spades
    };

    // Load player cards
    std::vector<sf::Sprite> playerCards;
    for (int i = 0; i < 3; ++i) {
        float x = 250 + i * (CARD_WIDTH + 20);
        float y = 500;
        playerCards.push_back(loadCard(playerCardFiles[i], {x, y}, textures[i]));
    }

    // Load table cards
    std::vector<sf::Sprite> tableCards;
    for (int i = 0; i < 4; ++i) {
        float x = 250 + i * (CARD_WIDTH + 20);
        float y = 250;
        tableCards.push_back(loadCard(tableCardFiles[i], {x, y}, textures[3 + i]));
    }

    // Opponent's face-down card (top center)
    sf::Sprite opponentCard = loadCard("assets/cards/card_back.png", {260, 50}, textures[7]);

    // Your score pile card (flipped left)
    sf::Sprite playerPile = loadCard("assets/cards/card_back.png", {50, 500}, textures[8]);

    // Opponent's score pile card (flipped top-left)
    sf::Sprite oppPile = loadCard("assets/cards/card_back.png", {50, 50}, textures[9]);

    // Game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(background);

        // Draw everything
        for (auto& sprite : playerCards) window.draw(sprite);
        for (auto& sprite : tableCards) window.draw(sprite);
        window.draw(opponentCard);
        window.draw(playerPile);
        window.draw(oppPile);

        window.display();
    }

    return 0;
}
