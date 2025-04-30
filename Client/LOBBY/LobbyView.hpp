#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "../UIState.hpp"

class LobbyView {
public:
    void init();

    void handleLobbyEvent(sf::Event&, UIState&);
    void handleCreateEvent(sf::Event&, sf::TcpSocket&, UIState&, void(*connectFunc)());
    void handleJoinEvent(sf::Event&, sf::TcpSocket&, UIState&, void(*connectFunc)());

    void drawLobby(sf::RenderWindow&);
    void drawCreateForm(sf::RenderWindow&);
    void drawJoinForm(sf::RenderWindow&);
    void drawWaiting(sf::RenderWindow&);

    void setGameCode(const std::string&);
    void setJoinStatus(const std::string&);

private:
    std::string nameInput, codeInput, joinStatus, gameCode;
    sf::Font font;
    sf::RectangleShape createBtn, joinBtn, quitBtn, confirmBtn;
};
