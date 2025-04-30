#include "LobbyView.hpp"
#include <iostream>

void LobbyView::init() {
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "[LobbyView] Failed to load font!" << std::endl;
    }
    createBtn.setSize({200, 50});
    createBtn.setPosition(300, 150);
    createBtn.setFillColor(sf::Color::White);

    joinBtn.setSize({200, 50});
    joinBtn.setPosition(300, 230);
    joinBtn.setFillColor(sf::Color::White);

    quitBtn.setSize({200, 50});
    quitBtn.setPosition(300, 310);
    quitBtn.setFillColor(sf::Color::White);

    confirmBtn.setSize({200, 50});
    confirmBtn.setPosition(300, 260);
    confirmBtn.setFillColor(sf::Color::White);
}

void LobbyView::handleLobbyEvent(sf::Event& e, UIState& state) {
    if (e.type == sf::Event::MouseButtonPressed) {
        auto pos = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
        if (createBtn.getGlobalBounds().contains(pos))
            state = UIState::CreateForm;
        else if (joinBtn.getGlobalBounds().contains(pos))
            state = UIState::JoinForm;
        else if (quitBtn.getGlobalBounds().contains(pos))
            exit(0);
    }
}

void LobbyView::handleCreateEvent(sf::Event& e, sf::TcpSocket& sock, UIState& state, void(*connectFunc)()) {
    if (e.type == sf::Event::TextEntered) {
        char c = static_cast<char>(e.text.unicode);
        if (e.text.unicode == 8 && !nameInput.empty()) nameInput.pop_back();
        else if (isprint(c) && nameInput.size() < 16) nameInput += c;
    }

    if (e.type == sf::Event::MouseButtonPressed &&
        confirmBtn.getGlobalBounds().contains({(float)e.mouseButton.x, (float)e.mouseButton.y})) {
        connectFunc();
        std::string cmd = "CREATE " + nameInput + "\n";
        sock.send(cmd.c_str(), cmd.size());
    }
}

void LobbyView::handleJoinEvent(sf::Event& e, sf::TcpSocket& sock, UIState& state, void(*connectFunc)()) {
    if (e.type == sf::Event::TextEntered) {
        char c = static_cast<char>(e.text.unicode);
        if (isalpha(c) && codeInput.size() < 4) codeInput += toupper(c);
        else if (isprint(c) && nameInput.size() < 16) nameInput += c;
    }

    if (e.type == sf::Event::MouseButtonPressed &&
        confirmBtn.getGlobalBounds().contains({(float)e.mouseButton.x, (float)e.mouseButton.y})) {
        connectFunc();
        std::string cmd = "JOIN " + codeInput + " " + nameInput + "\n";
        sock.send(cmd.c_str(), cmd.size());
    }
}

void LobbyView::drawLobby(sf::RenderWindow& w) {
    sf::Text createLabel("Create Game", font, 20);
    createLabel.setFillColor(sf::Color::Black);
    createLabel.setPosition(340, 160);

    sf::Text joinLabel("Join Game", font, 20);
    joinLabel.setFillColor(sf::Color::Black);
    joinLabel.setPosition(350, 240);

    sf::Text quitLabel("Quit", font, 20);
    quitLabel.setFillColor(sf::Color::Black);
    quitLabel.setPosition(380, 320);

    w.draw(createBtn);
    w.draw(joinBtn);
    w.draw(quitBtn);
    w.draw(createLabel);
    w.draw(joinLabel);
    w.draw(quitLabel);
}

void LobbyView::drawCreateForm(sf::RenderWindow& w) {
    sf::Text label("Enter your name:", font, 18);
    label.setPosition(300, 100);
    w.draw(label);

    sf::Text input(nameInput, font, 20);
    input.setFillColor(sf::Color::Black);
    input.setPosition(305, 170);
    sf::RectangleShape box({200, 40});
    box.setPosition(300, 135);
    box.setFillColor(sf::Color::White);
    box.setOutlineThickness(2);
    box.setOutlineColor(sf::Color::Black);
    w.draw(box);
    w.draw(input);

    sf::Text btnLabel("Start", font, 20);
    btnLabel.setFillColor(sf::Color::Black);
    btnLabel.setPosition(360, 270);
    w.draw(confirmBtn);
    w.draw(btnLabel);
}

void LobbyView::drawJoinForm(sf::RenderWindow& w) {
    sf::Text codeL("Game code:", font, 18);
    codeL.setPosition(300, 40);
    w.draw(codeL);

    sf::Text code(codeInput, font, 20);
    code.setFillColor(sf::Color::Black);
    code.setPosition(305, 70);
    sf::RectangleShape codeBox({200, 40});
    codeBox.setPosition(300, 65);
    codeBox.setFillColor(sf::Color::White);
    codeBox.setOutlineThickness(2);
    codeBox.setOutlineColor(sf::Color::Black);
    w.draw(codeBox);
    w.draw(code);

    sf::Text nameL("Your name:", font, 18);
    nameL.setPosition(300, 120);
    w.draw(nameL);

    sf::Text name(nameInput, font, 20);
    name.setFillColor(sf::Color::Black);
    name.setPosition(305, 150);
    sf::RectangleShape nameBox({200, 40});
    nameBox.setPosition(300, 145);
    nameBox.setFillColor(sf::Color::White);
    nameBox.setOutlineThickness(2);
    nameBox.setOutlineColor(sf::Color::Black);
    w.draw(nameBox);
    w.draw(name);

    sf::Text joinBtnLabel("Join", font, 20);
    joinBtnLabel.setFillColor(sf::Color::Black);
    joinBtnLabel.setPosition(365, 270);
    w.draw(confirmBtn);
    w.draw(joinBtnLabel);
}

void LobbyView::drawWaiting(sf::RenderWindow& w) {
    if (!font.getInfo().family.empty()) {
        std::string msg = "Game Code: " + gameCode + "\nWaiting for another player...";
        if (!joinStatus.empty())
            msg += "\n" + joinStatus;

        sf::Text waitText(msg, font, 20);
        waitText.setFillColor(sf::Color::White);
        waitText.setPosition(200, 200);
        w.draw(waitText);
    } else {
        std::cerr << "[LobbyView] Warning: Font not loaded in drawWaiting()\n";
    }
}


void LobbyView::setGameCode(const std::string& code) {
    gameCode = code;
}

void LobbyView::setJoinStatus(const std::string& status) {
    joinStatus = status;
}
