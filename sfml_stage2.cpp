#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <queue>
#include "SpriteLoader.h"
#include "pokemons/Pikachu.h"
#include "pokemons/Bulbasaur.h"
#include "pokemons/Charmander.h"
#include "pokemons/Squirtle.h"

enum class GameState {
    Selecting,
    Battling,
    End
};

enum class BattlePhase {
    Idle,
    Attacking,
    Hit,
    Waiting
};

std::queue<std::string> messageQueue;
std::string currentMessage = "";
std::string shownMessage = "";
size_t messageIndex = 0;
float charDelay = 0.035f;
float messageTimer = 0.0f;
float messageHoldTimer = 0.0f;
bool messageDone = true;

void pushMessage(const std::string& msg) {
    messageQueue.push(msg);
    if (messageDone && messageQueue.size() == 1) {
        currentMessage = messageQueue.front();
        shownMessage.clear();
        messageIndex = 0;
        messageTimer = 0.0f;
        messageHoldTimer = 0.0f;
        messageDone = false;
    }
}

int main()
{
    srand((unsigned)time(0));
    sf::RenderWindow window(sf::VideoMode(800, 600), "Pokemon Battle");
    sf::Clock clock;

    // Load Font
    sf::Font font;
    if (!font.loadFromFile("assets/ARIAL.TTF"))
    {
        std::cerr << "Failed to load font\n";
        return 1;
    }

    // Create roster with all Pokemon
    std::vector<Pokemon*> roster = {
        new Pikachu(), new Bulbasaur(), new Charmander(), new Squirtle()
    };

    // Load textures for all Pokemon from their URLs
    std::vector<sf::Texture> textures(roster.size());
    std::cout << "Downloading Pokemon sprites from URLs...\n";
    for (size_t i = 0; i < roster.size(); ++i) {
        if (!loadTextureFromURL(textures[i], roster[i]->getSpriteUrl())) {
            std::cerr << "Failed to load " << roster[i]->getName() << " sprite from URL\n";
            return 1;
        }
    }
    std::cout << "All sprites loaded successfully!\n";

    // Game state variables
    GameState gameState = GameState::Selecting;
    int selectedIndex = 0;
    int selectedMove = -1;
    
    BattlePhase battlePhase = BattlePhase::Idle;
    bool isPlayerTurn = true;
    sf::Clock phaseClock;
    float animationDuration = 0.2f;
    bool attackResolved = false;
    int pendingEnemyMove = -1;
    
    std::string battleResult = "";

    Pokemon* player = nullptr;
    Pokemon* enemy = nullptr;

    // Target sprite sizes (in pixels) - 2x size for fighting game style
    float playerTargetSize = 300.0f;
    float enemyTargetSize = 300.0f;

    // Persistent textures for battle (must outlive sprites)
    sf::Texture playerTexture;
    sf::Texture enemyTexture;
    sf::Texture backgroundTexture;

    // Sprites for battle (set up after selection)
    sf::Sprite playerSprite;
    sf::Sprite enemySprite;
    sf::Sprite backgroundSprite;

    // Text objects
    sf::Text playerText("", font, 20);
    playerText.setFillColor(sf::Color::White);
    playerText.setOutlineColor(sf::Color::Black);
    playerText.setOutlineThickness(2.0f);

    sf::Text enemyText("", font, 20);
    enemyText.setFillColor(sf::Color::White);
    enemyText.setOutlineColor(sf::Color::Black);
    enemyText.setOutlineThickness(2.0f);

    // Selection screen title
    sf::Text selectTitle("Choose Your Pokemon!", font, 28);
    selectTitle.setFillColor(sf::Color::Black);
    selectTitle.setPosition(240, 30);

    sf::Text selectHint("Up/Down to navigate, Enter to select", font, 14);
    selectHint.setFillColor(sf::Color(120, 120, 120));
    selectHint.setPosition(240, 65);

    // Result text (shown when battle ends)
    sf::Text resultText("", font, 40);
    resultText.setFillColor(sf::Color::Red);

    
    sf::RectangleShape newGameButton(sf::Vector2f(160, 40));
    newGameButton.setFillColor(sf::Color(100, 180, 255));
    newGameButton.setPosition(320, 350);  // Centered

    sf::Text newGameText("New Game", font, 20);
    newGameText.setFillColor(sf::Color::White);
    newGameText.setPosition(340, 360);
    
    // Move UI components (rebuilt after selection)
    std::vector<sf::Text> moveTexts;
    std::vector<sf::RectangleShape> moveButtons;

    // HP bar variables
    float playerCurrentHP = 0;
    float enemyCurrentHP = 0;
    float hpChangeSpeed = 60.0f;
    sf::Vector2f barSize(300, 20);

    sf::RectangleShape playerBarBack(barSize);
    playerBarBack.setFillColor(sf::Color(220, 220, 220));
    playerBarBack.setOutlineColor(sf::Color::Black);
    playerBarBack.setOutlineThickness(2.0f);
    playerBarBack.setPosition(50, 40);

    sf::RectangleShape playerBar;
    playerBar.setFillColor(sf::Color::Green);
    playerBar.setPosition(50, 40);

    sf::RectangleShape enemyBarBack(barSize);
    enemyBarBack.setFillColor(sf::Color(220, 220, 220));
    enemyBarBack.setOutlineColor(sf::Color::Black);
    enemyBarBack.setOutlineThickness(2.0f);
    enemyBarBack.setPosition(450, 40);

    sf::RectangleShape enemyBar;
    enemyBar.setFillColor(sf::Color::Green);
    enemyBar.setPosition(450, 40);

    // Moves box background
    sf::RectangleShape movesBg(sf::Vector2f(220, 100));
    movesBg.setFillColor(sf::Color(0, 0, 0, 200));
    movesBg.setOutlineThickness(2.0f);
    movesBg.setOutlineColor(sf::Color(255, 255, 255));
    movesBg.setPosition(50, 480);

    // Render loop
    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && gameState == GameState::End) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    if (newGameButton.getGlobalBounds().contains(mousePos)) {
                        // Reset state
                        // Reset all state
                        gameState = GameState::Selecting;
                        selectedIndex = 0;
                        selectedMove = -1;
                        battlePhase = BattlePhase::Idle;
                        isPlayerTurn = true;
                        battleResult = "";
                        while(!messageQueue.empty()) messageQueue.pop();
                        shownMessage = "";
                        messageIndex = 0;
                        messageDone = false;

                        // Clean up independent battle objects
                        if (player) { delete player; player = nullptr; }
                        if (enemy) { delete enemy; enemy = nullptr; }
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (gameState == GameState::Selecting)
                {
                    if (event.key.code == sf::Keyboard::Up)
                        selectedIndex = (selectedIndex - 1 + roster.size()) % roster.size();
                    else if (event.key.code == sf::Keyboard::Down)
                        selectedIndex = (selectedIndex + 1) % roster.size();
                    else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Return)
                    {
                        // Player picks their Pokemon (clone an independent copy)
                        player = new Pokemon(*roster[selectedIndex]);
                        playerTexture = textures[selectedIndex];

                        // Enemy is randomly assigned from all choices (clone independent copy)
                        int enemyIdx = rand() % roster.size();
                        enemy = new Pokemon(*roster[enemyIdx]);
                        enemyTexture = textures[enemyIdx];

                        // Load dynamic background based on enemy type
                        std::string bgPath = "assets/backgrounds/default.png";
                        switch (enemy->getType()) {
                            case Type::Fire: bgPath = "assets/backgrounds/fire.png"; break;
                            case Type::Water: bgPath = "assets/backgrounds/water.png"; break;
                            case Type::Grass: bgPath = "assets/backgrounds/grass.png"; break;
                            case Type::Electric: bgPath = "assets/backgrounds/electric.png"; break;
                            default: break;
                        }
                        if (backgroundTexture.loadFromFile(bgPath)) {
                            backgroundSprite.setTexture(backgroundTexture, true);
                            sf::Vector2u texSize = backgroundTexture.getSize();
                            backgroundSprite.setScale(800.0f / texSize.x, 600.0f / texSize.y);
                        } else {
                            backgroundSprite.setTexture(sf::Texture(), true);
                        }

                        // Set up player sprite
                        playerSprite.setTexture(playerTexture, true);
                        {
                            sf::Vector2u texSize = playerTexture.getSize();
                            float scale = playerTargetSize / std::max(texSize.x, texSize.y);
                            playerSprite.setOrigin(texSize.x, 0); // Set origin to right edge for flipping
                            playerSprite.setScale(-scale, scale); // Flip horizontally
                        }
                        playerSprite.setPosition(50, 180);

                        // Set up enemy sprite
                        enemySprite.setTexture(enemyTexture, true);
                        {
                            sf::Vector2u texSize = enemyTexture.getSize();
                            float scale = enemyTargetSize / std::max(texSize.x, texSize.y);
                            enemySprite.setScale(scale, scale);
                        }
                        enemySprite.setPosition(450, 180);

                        // Set up name texts
                        playerText.setString(player->getName());
                        playerText.setPosition(50, 12);
                        enemyText.setString(enemy->getName());
                        enemyText.setPosition(750 - enemyText.getLocalBounds().width, 12);

                        // Build move buttons and texts from player's moves
                        moveTexts.clear();
                        moveButtons.clear();
                        const std::vector<Move>& moves = player->getMoves();
                        for (size_t i = 0; i < moves.size(); ++i) {
                            // Column layout position
                            float bx = 60;
                            float by = 488 + i * 32;

                            // Create button
                            sf::RectangleShape btn(sf::Vector2f(200, 28));
                            btn.setPosition(bx, by);
                            btn.setFillColor(sf::Color(40, 40, 40));
                            btn.setOutlineColor(sf::Color::White);
                            btn.setOutlineThickness(1.0f);
                            moveButtons.push_back(btn);

                            // Create text centered in button
                            sf::Text moveText(moves[i].name, font, 14);
                            sf::FloatRect tr = moveText.getLocalBounds();
                            moveText.setOrigin(tr.left + tr.width / 2.0f, tr.top + tr.height / 2.0f);
                            moveText.setPosition(bx + 200.0f / 2.0f, by + 28.0f / 2.0f);
                            moveText.setFillColor(sf::Color::White);
                            moveTexts.push_back(moveText);
                        }

                        // Initialize HP tracking
                        playerCurrentHP = player->getCurrentHP();
                        enemyCurrentHP = enemy->getCurrentHP();
                        playerBar.setSize(sf::Vector2f(barSize.x, barSize.y));
                        enemyBar.setSize(sf::Vector2f(barSize.x, barSize.y));

                        selectedMove = 0; // Default to first move so it's highlighted
                        gameState = GameState::Battling;
                        
                        pushMessage("A wild " + enemy->getName() + " appeared!");
                        pushMessage("Go, " + player->getName() + "!");

                        std::cout << "You chose: " << player->getName() << "!\n";
                        std::cout << "Enemy chose: " << enemy->getName() << "!\n";
                    }
                }
                else if (gameState == GameState::Battling)
                {
                    if (battlePhase == BattlePhase::Idle && !player->isFainted() && !enemy->isFainted() && messageQueue.empty() && messageDone)
                    {
                        int moveCount = (int)player->getMoves().size();
                        if (event.key.code == sf::Keyboard::Up)
                            selectedMove = (selectedMove - 1 + moveCount) % moveCount;
                        else if (event.key.code == sf::Keyboard::Down)
                            selectedMove = (selectedMove + 1) % moveCount;
                        else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Return)
                        {
                            if (selectedMove >= 0 && selectedMove < moveCount)
                            {
                                std::cout << "You selected: " << player->getMoves()[selectedMove].name << "\n";
                                battlePhase = BattlePhase::Attacking;
                                isPlayerTurn = true;
                                phaseClock.restart();
                            }
                        }
                    }
                }
            }
        }

        window.clear(sf::Color::White);

        // ==================== SELECTING STATE ====================
        if (gameState == GameState::Selecting)
        {
            window.draw(selectTitle);
            window.draw(selectHint);

            // Draw each Pokemon option with its sprite preview
            for (size_t i = 0; i < roster.size(); ++i)
            {
                // Sprite preview (small)
                sf::Sprite preview(textures[i]);
                sf::Vector2u texSize = textures[i].getSize();
                float scale = 60.0f / std::max(texSize.x, texSize.y);
                preview.setScale(scale, scale);
                preview.setPosition(260, 100 + i * 100);

                // Name + type text
                std::string label = roster[i]->getName() + "  (" + typeToString(roster[i]->getType()) + ")";
                sf::Text optionText(label, font, 20);
                optionText.setPosition(340, 115 + i * 100);

                // Highlight selected
                if ((int)i == selectedIndex) {
                    optionText.setFillColor(sf::Color::Red);

                    // Draw selection arrow
                    sf::Text arrow(">", font, 24);
                    arrow.setFillColor(sf::Color::Red);
                    arrow.setPosition(235, 112 + i * 100);
                    window.draw(arrow);
                } else {
                    optionText.setFillColor(sf::Color::Black);
                }

                window.draw(preview);
                window.draw(optionText);
            }
        }
        // ==================== BATTLING STATE ====================
        else if (gameState == GameState::Battling || gameState == GameState::End)
        {
            if (backgroundSprite.getTexture() != nullptr) {
                window.draw(backgroundSprite);
            }

            if (gameState == GameState::Battling) {
                if (battlePhase == BattlePhase::Attacking) {
                    float t = phaseClock.getElapsedTime().asSeconds();
                    if (t < animationDuration) {
                        float lunge = 80.0f * deltaTime * (isPlayerTurn ? 1 : -1);
                        if (isPlayerTurn) playerSprite.move(lunge, 0);
                        else enemySprite.move(lunge, 0);
                    } else {
                        battlePhase = BattlePhase::Hit;
                        phaseClock.restart();
                        attackResolved = false;
                    }
                }
                else if (battlePhase == BattlePhase::Hit) {
                    float t = phaseClock.getElapsedTime().asSeconds();
                    if (!attackResolved) {
                        if (isPlayerTurn) {
                            std::vector<std::string> msgs = player->useMove(selectedMove, *enemy);
                            for (const auto& msg : msgs) pushMessage(msg);
                            if (enemy->isFainted()) {
                                pushMessage(enemy->getName() + " fainted!");
                            }
                        } else {
                            std::vector<std::string> msgs = enemy->useMove(pendingEnemyMove, *player);
                            for (const auto& msg : msgs) pushMessage(msg);
                            if (player->isFainted()) {
                                pushMessage(player->getName() + " fainted!");
                            }
                        }
                        attackResolved = true;
                    }

                    // shake effect
                    float offset = std::sin(t * 40.0f) * 2.0f;
                    if (isPlayerTurn)
                        enemySprite.setPosition(450 + offset, 180);
                    else
                        playerSprite.setPosition(50 + offset, 180);

                    if (t > animationDuration) {
                        // reset positions
                        playerSprite.setPosition(50, 180);
                        enemySprite.setPosition(450, 180);
                        
                        battlePhase = BattlePhase::Waiting;
                        phaseClock.restart();
                    }
                }
                else if (battlePhase == BattlePhase::Waiting) {
                    if (phaseClock.getElapsedTime().asSeconds() > 1.0f) {
                        if (player->isFainted() || enemy->isFainted()) {
                            if (enemy->isFainted()) battleResult = "YOU WIN!";
                            else battleResult = "YOU LOST!";
                            std::cout << battleResult << "\n";
                            gameState = GameState::End;
                        } else {
                            if (isPlayerTurn) {
                                // Switch to enemy turn
                                isPlayerTurn = false;
                                pendingEnemyMove = rand() % enemy->getMoves().size();
                                battlePhase = BattlePhase::Attacking;
                                phaseClock.restart();
                            } else {
                                // Back to player
                                selectedMove = -1;
                                isPlayerTurn = true;
                                battlePhase = BattlePhase::Idle;
                            }
                        }
                    }
                }
            }
            
            // (Removed manual user snippet overlay to place properly at end of draw)

            // Smooth animation toward real HP values
            if (player) {
                float playerTargetHP = player->getCurrentHP();
                if (playerCurrentHP > playerTargetHP)
                    playerCurrentHP -= hpChangeSpeed * deltaTime;
                if (playerCurrentHP < playerTargetHP)
                    playerCurrentHP = playerTargetHP;
            }
            if (enemy) {
                float enemyTargetHP = enemy->getCurrentHP();
                if (enemyCurrentHP > enemyTargetHP)
                    enemyCurrentHP -= hpChangeSpeed * deltaTime;
                if (enemyCurrentHP < enemyTargetHP)
                    enemyCurrentHP = enemyTargetHP;
            }

            // Update HP bar sizes and colors
            if (player) {
                float pRatio = playerCurrentHP / player->getmaxHP();
                playerBar.setSize(sf::Vector2f(pRatio * barSize.x, barSize.y));
                if (pRatio > 0.5f) playerBar.setFillColor(sf::Color::Green);
                else if (pRatio > 0.2f) playerBar.setFillColor(sf::Color::Yellow);
                else playerBar.setFillColor(sf::Color::Red);
            }
            if (enemy) {
                float eRatio = enemyCurrentHP / enemy->getmaxHP();
                enemyBar.setSize(sf::Vector2f(eRatio * barSize.x, barSize.y));
                if (eRatio > 0.5f) enemyBar.setFillColor(sf::Color::Green);
                else if (eRatio > 0.2f) enemyBar.setFillColor(sf::Color::Yellow);
                else enemyBar.setFillColor(sf::Color::Red);
            }

            // Draw drop shadows for 3D depth effect
            sf::Sprite pShadow = playerSprite;
            pShadow.setColor(sf::Color(0, 0, 0, 130)); // Perfect black silhouette
            pShadow.move(8.0f, 8.0f); // Cast shadow down-right
            window.draw(pShadow);

            sf::Sprite eShadow = enemySprite;
            eShadow.setColor(sf::Color(0, 0, 0, 130));
            eShadow.move(8.0f, 8.0f);
            window.draw(eShadow);

            // Draw actual sprites
            window.draw(playerSprite);
            window.draw(enemySprite);
            window.draw(playerText);
            window.draw(enemyText);
            window.draw(playerBarBack);
            window.draw(playerBar);
            window.draw(enemyBarBack);
            window.draw(enemyBar);

            // Draw moves only if queue is empty (avoids visual overlap)
            if (messageQueue.empty() && messageDone) {
                window.draw(movesBg);
                for (size_t i = 0; i < moveButtons.size(); ++i)
                {
                    if ((int)i == selectedMove) {
                        moveButtons[i].setOutlineColor(sf::Color::Yellow);
                        moveButtons[i].setOutlineThickness(2.0f);
                        moveButtons[i].setFillColor(sf::Color(80, 80, 80));
                        moveTexts[i].setFillColor(sf::Color::Yellow);
                    } else {
                        moveButtons[i].setOutlineColor(sf::Color::White);
                        moveButtons[i].setOutlineThickness(1.0f);
                        moveButtons[i].setFillColor(sf::Color(40, 40, 40));
                        moveTexts[i].setFillColor(sf::Color::White);
                    }

                    window.draw(moveButtons[i]);
                    window.draw(moveTexts[i]);
                }
                if (gameState == GameState::End) {
                    window.draw(newGameButton);
                    window.draw(newGameText);
                }
            }

            // Typewriter rendering
            if (!messageDone || !messageQueue.empty()) {
                if (!messageDone) {
                    messageTimer += deltaTime;
                    if (messageTimer >= charDelay && messageIndex < currentMessage.size()) {
                        shownMessage += currentMessage[messageIndex++];
                        messageTimer = 0.0f;
                    }
                    if (messageIndex >= currentMessage.size()) {
                        messageHoldTimer += deltaTime;
                        if (messageHoldTimer >= 1.0f) { // hold for 1s
                            messageDone = true;
                            messageQueue.pop();
                            if (!messageQueue.empty()) {
                                currentMessage = messageQueue.front();
                                shownMessage.clear();
                                messageIndex = 0;
                                messageTimer = 0.0f;
                                messageHoldTimer = 0.0f;
                                messageDone = false;
                            }
                        }
                    }
                }

                sf::RectangleShape msgBox(sf::Vector2f(700, 60));
                msgBox.setFillColor(sf::Color(240, 240, 240));
                msgBox.setOutlineThickness(2);
                msgBox.setOutlineColor(sf::Color::Black);
                msgBox.setPosition(50, 500);
                window.draw(msgBox);

                sf::Text typedText(shownMessage, font, 18);
                typedText.setPosition(60, 510);
                typedText.setFillColor(sf::Color::Black);
                window.draw(typedText);
            }

            if (gameState == GameState::End && messageQueue.empty() && messageDone) {
                // Outer black border
                sf::RectangleShape outerBox(sf::Vector2f(300, 80));
                outerBox.setOrigin(150, 40);
                outerBox.setPosition(400, 200); // top center
                outerBox.setFillColor(sf::Color::Black);

                // Inner white box layer
                sf::RectangleShape midBox(sf::Vector2f(292, 72)); // 4px border
                midBox.setOrigin(146, 36);
                midBox.setPosition(400, 200);
                midBox.setFillColor(sf::Color::White);

                // Inner black border layer
                sf::RectangleShape innerBorder(sf::Vector2f(284, 64)); // 4px padding
                innerBorder.setOrigin(142, 32);
                innerBorder.setPosition(400, 200);
                innerBorder.setFillColor(sf::Color::Black);

                // Innermost background layer
                sf::RectangleShape innerBox(sf::Vector2f(276, 56)); // 4px border
                innerBox.setOrigin(138, 28);
                innerBox.setPosition(400, 200);
                innerBox.setFillColor(sf::Color(240, 240, 240));

                window.draw(outerBox);
                window.draw(midBox);
                window.draw(innerBorder);
                window.draw(innerBox);

                // Result text
                sf::Text resText(battleResult, font, 36);
                resText.setStyle(sf::Text::Bold);
                resText.setFillColor(sf::Color::Black);
                sf::FloatRect bounds = resText.getLocalBounds();
                resText.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
                resText.setPosition(400, 200);
                window.draw(resText);
            }
        }

        window.display();
    }

    // Clean up roster memory
    for (auto* p : roster)
        delete p;
    if (player) delete player;
    if (enemy) delete enemy;

    return 0;
}
