#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <regex>
#include "GAME/GameView.hpp"

const int WINDOW_WIDTH  = 800;
const int WINDOW_HEIGHT = 600;
const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = 50;
bool myTurn = false;
std::string p1ScoreText, p2ScoreText;
bool score1Complete = false, score2Complete = false;
bool inScoreBlock = false;
std::string currentScoreBlock;


enum class UIState { LobbyMenu, CreateForm, JoinForm, Waiting, InGame, ScoreScreen };
enum class ActiveField { None, Name, Code };

GameView gameView;
sf::TcpSocket socket;
sf::SocketSelector selector;

std::string nameInput, codeInput, lobbyCode;
UIState state = UIState::LobbyMenu;
ActiveField activeField = ActiveField::None;

sf::Font font;

// UI elements
sf::RectangleShape createBtn, joinBtn, quitBtn, inputBoxName, inputBoxCode, startBtn, joinConfirmBtn, backBtn;
sf::Text createText, joinText, quitText, inputTextName, inputTextCode, backText;
sf::Text nameLabel, codeLabel, startText, joinConfirmText, waitingText;

// Utility: create button + center text
sf::RectangleShape createButton(float x, float y, const std::string& label, sf::Text& outText) {
    sf::RectangleShape button(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
    button.setPosition(x, y);
    button.setFillColor({200, 200, 200});
    button.setOutlineThickness(2);
    button.setOutlineColor(sf::Color::Black);

    outText.setFont(font);
    outText.setString(label);
    outText.setCharacterSize(20);
    outText.setFillColor(sf::Color::Black);
    sf::FloatRect tb = outText.getLocalBounds();
    outText.setPosition(x + (BUTTON_WIDTH - tb.width) / 2 - tb.left,
                        y + (BUTTON_HEIGHT - tb.height) / 2 - tb.top);
    return button;
}

void layoutCentered(sf::Text& label, sf::RectangleShape& box, sf::Text& text, float y, const std::string& labelStr) {
    label.setString(labelStr);
    label.setPosition((WINDOW_WIDTH - BUTTON_WIDTH) / 2, y);
    box.setPosition((WINDOW_WIDTH - BUTTON_WIDTH) / 2, y + 35);
    text.setPosition((WINDOW_WIDTH - BUTTON_WIDTH) / 2 + 5, y + 40);
}

void drawLobbyMenu(sf::RenderWindow& window) {
    window.draw(createBtn);
    window.draw(joinBtn);
    window.draw(quitBtn);
    window.draw(createText);
    window.draw(joinText);
    window.draw(quitText);
}

void drawCreateForm(sf::RenderWindow& window) {
    window.draw(nameLabel);
    window.draw(inputBoxName);
    window.draw(inputTextName);
    window.draw(startBtn);
    window.draw(startText);
    window.draw(backBtn);
    window.draw(backText);
}

void drawJoinForm(sf::RenderWindow& window) {
    window.draw(codeLabel);
    window.draw(inputBoxCode);
    window.draw(inputTextCode);
    window.draw(nameLabel);
    window.draw(inputBoxName);
    window.draw(inputTextName);
    window.draw(joinConfirmBtn);
    window.draw(joinConfirmText);
    window.draw(backBtn);
    window.draw(backText);
}

void drawWaitingScreen(sf::RenderWindow& window) {
    window.draw(waitingText);
}

void setupScoreScreenUI() {
    backBtn.setSize({150, 40});
    backBtn.setPosition((WINDOW_WIDTH - 150) / 2, 500);
    backBtn.setFillColor({200, 200, 200});
    backBtn.setOutlineThickness(2);
    backBtn.setOutlineColor(sf::Color::Black);

    backText.setFont(font);
    backText.setCharacterSize(18);
    backText.setFillColor(sf::Color::Black);
    backText.setString("Back to Lobby");

    sf::FloatRect textBounds = backText.getLocalBounds();
    backText.setPosition(
        backBtn.getPosition().x + (150 - textBounds.width) / 2 - textBounds.left,
        backBtn.getPosition().y + (40 - textBounds.height) / 2 - textBounds.top
    );
}

void drawScoreScreen(sf::RenderWindow& window) {
    sf::Text header, player1, player2;

    header.setFont(font);
    header.setCharacterSize(28);
    header.setFillColor(sf::Color::White);
    header.setString("Round Results");
    header.setPosition(280, 40);

    player1.setFont(font);
    player1.setCharacterSize(16);
    player1.setFillColor(sf::Color::White);
    player1.setString(p1ScoreText);
    player1.setPosition(80, 100);

    player2.setFont(font);
    player2.setCharacterSize(16);
    player2.setFillColor(sf::Color::White);
    player2.setString(p2ScoreText);
    player2.setPosition(420, 100);

    window.draw(header);
    window.draw(player1);
    window.draw(player2);
    window.draw(backBtn);
    window.draw(backText);
}

void handleScoreScreenClick(const sf::Vector2f& mouse) {
    if (backBtn.getGlobalBounds().contains(mouse)) {
        std::cout << "[UI] Back to Lobby clicked\n";
        state = UIState::LobbyMenu;
        p1ScoreText.clear();
        p2ScoreText.clear();
        score1Complete = score2Complete = false;
    }
}


static std::vector<std::string> parseCardCodes(const std::string& raw) {
    std::vector<std::string> codes;
    // Match a capital letter followed by one or more digits
    static const std::regex re(R"(([A-Z][0-9]+))");
    auto begin = std::sregex_iterator(raw.begin(), raw.end(), re);
    auto end   = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
        codes.push_back((*it)[1].str());
    }
    return codes;
}

void sendCreateRequest() {
    std::string cmd = "CREATE " + nameInput + "\n";
    socket.send(cmd.c_str(), cmd.size());
}

void sendJoinRequest() {
    if (codeInput.size() != 4 || nameInput.empty()) {
        std::cout << "[CLIENT] Cannot send JOIN: invalid name or code (code must be 4 chars).\n";
        return;
    }

    std::string cmd = "JOIN " + codeInput + " " + nameInput + "\n";
    socket.send(cmd.c_str(), cmd.size());
    std::cout << "[CLIENT] Sent: " << cmd;
}

void handleServerResponse() {
    static std::vector<std::string> tableRaw, handRaw;

    if (selector.wait(sf::milliseconds(10))) {
        char buffer[512];
        std::size_t received;
        if (socket.receive(buffer, sizeof(buffer), received) == sf::Socket::Done) {
            std::string response(buffer, received);
            std::istringstream iss(response);
            std::string line;

            bool seenTable = false;
            bool seenHand = false;

            while (std::getline(iss, line)) {
                std::cout << "[SERVER] " << line << "\n";

                if (line == "INFO:INVALID_CODE") {
                    std::cout << "[CLIENT] Invalid game code received.\n";
                    state = UIState::JoinForm;

                    // Keep name; reset only the code
                    codeInput.clear();
                    inputTextCode.setString("");
                    waitingText.setString("Invalid code. Try again.");
                    continue;
                }

                if (line.rfind("GAME_CREATED", 0) == 0) {
                    lobbyCode = line.substr(13);
                    lobbyCode.erase(std::remove(lobbyCode.begin(), lobbyCode.end(), '\n'), lobbyCode.end());
                    waitingText.setString("Game Code: " + lobbyCode + "\nWaiting for another player...");
                    state = UIState::Waiting;
                }

                else if (line.rfind("JOINED_GAME", 0) == 0) {
                    waitingText.setString("Successfully joined game!\nWaiting for other player...");
                    state = UIState::Waiting;
                }

                else if (line == "INFO:GAME_STARTED") {
                    tableRaw.clear();
                    handRaw.clear();
                }

                else if (line.rfind("TABLE:", 0) == 0) {
                    tableRaw = parseCardCodes(line.substr(6));
                    seenTable = true;
                }

                else if (line.rfind("HAND:", 0) == 0) {
                    handRaw = parseCardCodes(line.substr(5));
                    seenHand = true;
                }

                else if (line == "TURN") {
                    myTurn = true;
                    std::cout << "[SERVER] It's your turn.\n";
                }

                else if (line.rfind("SCORE:", 0) == 0) {
                    currentScoreBlock = line.substr(6) + "\n";
                    inScoreBlock = true;
                }

                else if (inScoreBlock && (line.find(" - ") == 0 || line.empty())) {
                    currentScoreBlock += line + "\n";
                    if (line.find("Bermila") != std::string::npos || line.empty()) {
                        if (!score1Complete) {
                            p1ScoreText = currentScoreBlock;
                            score1Complete = true;
                        } else {
                            p2ScoreText = currentScoreBlock;
                            score2Complete = true;
                        }
                        inScoreBlock = false;
                    }
                }
            }

            if (seenTable && seenHand) {
                gameView.setCards(tableRaw, handRaw);
                state = UIState::InGame;
            }
            else if (seenTable) {
                gameView.setCards(tableRaw, handRaw);
            }

            if (score1Complete && score2Complete) {
                state = UIState::ScoreScreen;
            }
        }
    }
}


int main() {
    if (socket.connect("127.0.0.1", 4000) != sf::Socket::Done) {
        std::cerr << "Could not connect to server.\n";
        return 1;
    }
    selector.add(socket);

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Chkobba Lobby");

    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Font missing.\n";
        return 1;
    }
    setupScoreScreenUI();

    // Setup UI buttons
    createBtn       = createButton(200, 100, "Create Game", createText);
    joinBtn         = createButton(200, 180, "Join Game",  joinText);
    quitBtn         = createButton(200, 260, "Quit",       quitText);
    startBtn        = createButton(200, 260, "Start",      startText);
    joinConfirmBtn  = createButton(200, 320, "Join",       joinConfirmText);

    // Common label/input styling
    auto setupInput = [&](sf::Text& label, sf::RectangleShape& box, sf::Text& text){
        label.setFont(font);
        label.setCharacterSize(18);
        label.setFillColor(sf::Color::White);
        box.setSize({BUTTON_WIDTH, BUTTON_HEIGHT});
        box.setFillColor(sf::Color::White);
        box.setOutlineThickness(2);
        box.setOutlineColor(sf::Color::Black);
        text.setFont(font);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);
    };
    setupInput(nameLabel, inputBoxName, inputTextName);
    setupInput(codeLabel, inputBoxCode, inputTextCode);

    waitingText.setFont(font);
    waitingText.setCharacterSize(20);
    waitingText.setFillColor(sf::Color::White);
    waitingText.setPosition(150, 160);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            sf::Vector2f mouse{float(event.mouseButton.x), float(event.mouseButton.y)};
            if (event.type == sf::Event::Closed)
                window.close();

            // Lobby menu clicks
            if (state == UIState::LobbyMenu && event.type == sf::Event::MouseButtonPressed) {
                if (createBtn.getGlobalBounds().contains(mouse)) {
                    nameInput.clear(); inputTextName.setString("");
                    activeField = ActiveField::Name;
                    state = UIState::CreateForm;
                }
                else if (joinBtn.getGlobalBounds().contains(mouse)) {
                    nameInput.clear(); codeInput.clear();
                    inputTextName.setString(""); inputTextCode.setString("");
                    activeField = ActiveField::None;
                    state = UIState::JoinForm;
                }
                else if (quitBtn.getGlobalBounds().contains(mouse)) {
                    window.close();
                }
            }

            // Text entry and clicks in forms
            if ((state == UIState::CreateForm || state == UIState::JoinForm) &&
                (event.type == sf::Event::TextEntered || event.type == sf::Event::MouseButtonPressed))
            {
                // Mouse-based focus
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (inputBoxName.getGlobalBounds().contains(mouse))
                        activeField = ActiveField::Name;
                    else if (inputBoxCode.getGlobalBounds().contains(mouse))
                        activeField = ActiveField::Code;
                    else
                        activeField = ActiveField::None;
                }

                // Text input
                if (event.type == sf::Event::TextEntered) {
                    char c = static_cast<char>(event.text.unicode);
                    if (c == 8) { // backspace
                        if (activeField == ActiveField::Name && !nameInput.empty())
                            nameInput.pop_back();
                        else if (activeField == ActiveField::Code && !codeInput.empty())
                            codeInput.pop_back();
                    }
                    else if (std::isprint(static_cast<unsigned char>(c))) {
                        if (activeField == ActiveField::Name && nameInput.size() < 16)
                            nameInput += c;
                        else if (activeField == ActiveField::Code && codeInput.size() < 4)
                            codeInput += std::toupper(c);
                    }
                }

                // Button clicks
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (state == UIState::CreateForm && startBtn.getGlobalBounds().contains(mouse)) {
                        sendCreateRequest();
                        state = UIState::Waiting;
                    }
                    else if (state == UIState::JoinForm && joinConfirmBtn.getGlobalBounds().contains(mouse)) {
                        sendJoinRequest();
                        
                    }
                }

                // Update input text
                inputTextName.setString(nameInput);
                inputTextCode.setString(codeInput);
            }
            if (state==UIState::InGame && event.type==sf::Event::MouseButtonPressed) {
                auto m = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                auto sel = gameView.handleClick(m);
                if (!sel.empty())
                    std::cout << "Selected: " << sel << "\n";
            }
            else if (state == UIState::InGame && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter && myTurn) {
                    int handIdx = gameView.getHandSelected();
                    if (handIdx != -1) {
                        auto tableSel = gameView.getTableSelected();
                        std::ostringstream mv;
                        mv << "MOVE " << handIdx;
                        for (int i = 0; i < tableSel.size(); ++i)
                            if (tableSel[i]) mv << ' ' << i;
                        mv << '\n';
                        socket.send(mv.str().c_str(), mv.str().size());
                        myTurn = false; 
                        std::cout << "[DEBUG] Sending command: " << mv.str();
                        gameView.clearSelection();
                    }
                }
            } else if (state == UIState::ScoreScreen &&
                event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mouse{float(event.mouseButton.x), float(event.mouseButton.y)};
                if (backBtn.getGlobalBounds().contains(mouse)) {
                    state = UIState::LobbyMenu;
                    p1ScoreText.clear();
                    p2ScoreText.clear();
                    score1Complete = score2Complete = false;
                }
            } else if ((state == UIState::CreateForm || state == UIState::JoinForm) && event.type == sf::Event::MouseButtonPressed) {
                if (backBtn.getGlobalBounds().contains(mouse)) {
                    std::cout << "[UI] Back to Lobby clicked\n";
                    state = UIState::LobbyMenu;
                    nameInput.clear();
                    codeInput.clear();
                    inputTextName.setString("");
                    inputTextCode.setString("");
                }
            }
            
        }

       
        handleServerResponse();
        

        // Layout updates for forms
        if (state == UIState::CreateForm) {
            nameLabel.setString("Enter your name:");
            nameLabel.setPosition(200, 100);
            inputBoxName.setPosition(200, 135);
            inputTextName.setPosition(200, 140);
        }
        else if (state == UIState::JoinForm) {
            codeLabel.setString("Game Code:");
            codeLabel.setPosition(200, 30);
            inputBoxCode.setPosition(200, 60);
            inputTextCode.setPosition(200, 65);

            nameLabel.setString("Enter your name:");
            nameLabel.setPosition(200, 130);
            inputBoxName.setPosition(200, 160);
            inputTextName.setPosition(200, 165);
        }

        // Draw
        window.clear({50, 120, 50});
        if      (state == UIState::LobbyMenu)  drawLobbyMenu(window);
        else if (state == UIState::CreateForm) drawCreateForm(window);
        else if (state == UIState::JoinForm)   drawJoinForm(window);
        else if (state == UIState::Waiting)    drawWaitingScreen(window);
        else if (state == UIState::InGame)     gameView.render(window);
        else if (state == UIState::ScoreScreen) drawScoreScreen(window);

        window.display();
    }

    return 0;
}
