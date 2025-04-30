#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <vector>

// Constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int CARD_WIDTH = 60;
const int CARD_HEIGHT = 90;

// === Utility to load and scale card ===
sf::Sprite loadCard(const std::string& filename, sf::Vector2f position, sf::Texture& textureOut) {
    if (!textureOut.loadFromFile(filename)) {
        std::cerr << "Failed to load image: " << filename << std::endl;
    }
    sf::Sprite sprite(textureOut);
    sprite.setPosition(position);
    sprite.setScale(
        CARD_WIDTH / static_cast<float>(textureOut.getSize().x),
        CARD_HEIGHT / static_cast<float>(textureOut.getSize().y)
    );
    return sprite;
}

// === Parse HAND:... or TABLE:... string into vector of card codes ===
std::vector<std::string> parseCards(const std::string& line, const std::string& prefix) {
    if (line.find(prefix) != 0) return {};
    std::string list = line.substr(prefix.length());
    std::vector<std::string> result;
    size_t pos = 0, comma;
    while ((comma = list.find(',', pos)) != std::string::npos) {
        result.push_back(list.substr(pos, comma - pos));
        pos = comma + 1;
    }
    if (pos < list.size()) result.push_back(list.substr(pos));
    return result;
}

// === Renders hand and table ===
void renderGame(std::vector<std::string> hand, std::vector<std::string> table) {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Chkobba - GUI");

    sf::RectangleShape background(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    background.setFillColor(sf::Color(34, 139, 34));

    std::vector<sf::Texture> textures(hand.size() + table.size() + 3); // reserve all textures
    std::vector<sf::Sprite> playerCards;
    std::vector<sf::Sprite> tableCards;

    // Load hand cards
    for (int i = 0; i < hand.size(); ++i) {
        std::string path = "Client/assets/cards/" + hand[i] + ".png";
        float x = 250 + i * (CARD_WIDTH + 20);
        float y = 500;
        playerCards.push_back(loadCard(path, {x, y}, textures[i]));
    }

    // Load table cards
    for (int i = 0; i < table.size(); ++i) {
        std::string path = "Client/assets/cards/" + table[i] + ".png";
        float x = 250 + i * (CARD_WIDTH + 20);
        float y = 250;
        tableCards.push_back(loadCard(path, {x, y}, textures[hand.size() + i]));
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(background);
        for (auto& c : playerCards) window.draw(c);
        for (auto& c : tableCards) window.draw(c);
        window.display();
    }
}

// === Main client networking ===
int main() {
    sf::TcpSocket socket;
    std::string ip = "127.0.0.1";
    unsigned short port = 4000;

    std::cout << "Connecting to " << ip << ":" << port << "...\n";
    if (socket.connect(ip, port) != sf::Socket::Done) {
        std::cerr << "Connection failed!\n";
        return 1;
    }

    std::cout << "Connected to server.\n";

    char buffer[1024];
    std::size_t received;

    while (true) {
        if (socket.receive(buffer, sizeof(buffer), received) != sf::Socket::Done) {
            std::cerr << "Disconnected or error.\n";
            break;
        }

        std::string msg(buffer, received);
        std::cout << "[Server] " << msg << "\n";

        std::vector<std::string> hand   = parseCards(msg, "HAND:");
        std::vector<std::string> table  = parseCards(msg, "TABLE:");

        if (!hand.empty() || !table.empty()) {
            renderGame(hand, table);
        }
    }

    return 0;
}
