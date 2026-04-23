#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <queue>
#include "Pokemon.h"
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

sf::Color getTypeColor(Type t) {
    switch (t) {
        case Type::Fire:     return sf::Color(255, 80, 50);   // Vibrant Orange-Red
        case Type::Water:    return sf::Color(50, 120, 240);  // Deep Sky Blue
        case Type::Grass:    return sf::Color(80, 200, 80);   // Forest Green
        case Type::Electric: return sf::Color(255, 220, 30);  // Electric Yellow
        case Type::Normal:   return sf::Color(50, 220, 220);  // Vibrant Teal/Cyan for Normal
        default:             return sf::Color(140, 140, 140); // Neutral Grey (Steel/Etc)
    }
}

void startBattle(sf::RenderWindow& window, Pokemon* wildPokemon)
{
    // Reset global message state for fresh battle
    while(!messageQueue.empty()) messageQueue.pop();
    currentMessage = "";
    shownMessage = "";
    messageIndex = 0;
    messageTimer = 0.0f;
    messageHoldTimer = 0.0f;
    messageDone = true;

    window.setTitle("Pokemon Battle");

    sf::Clock clock;

    // Load Font
    sf::Font minecraftFont;
    if (!minecraftFont.loadFromFile("assets/Minecraftia-Regular.ttf"))
    {
        std::cerr << "Failed to load font\n";
        return;
    }

    // Create roster with all Pokemon
    std::vector<Pokemon*> roster = {
        new Pikachu(), new Bulbasaur(), new Charmander(), new Squirtle()
    };

    // Load textures for all Pokemon from local files
    std::vector<sf::Texture> textures(roster.size());
    for (size_t i = 0; i < roster.size(); ++i) {
        if (!textures[i].loadFromFile(roster[i]->getSpritePath())) {
            std::cerr << "Failed to load " << roster[i]->getName() << " sprite from: " << roster[i]->getSpritePath() << "\n";
            return;
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
    sf::Text playerText("", minecraftFont, 20);
    playerText.setFillColor(sf::Color::White);
    playerText.setOutlineColor(sf::Color::Black);
    playerText.setOutlineThickness(2.0f);

    sf::Text enemyText("", minecraftFont, 20);
    enemyText.setFillColor(sf::Color::White);
    enemyText.setOutlineColor(sf::Color::Black);
    enemyText.setOutlineThickness(2.0f);

    // Selection screen title
    sf::Text selectTitle("Choose Your Pokemon!", minecraftFont, 28);
    selectTitle.setFillColor(sf::Color::Black);
    sf::FloatRect stBounds = selectTitle.getLocalBounds();
    selectTitle.setOrigin(stBounds.left + stBounds.width / 2.0f, stBounds.top + stBounds.height / 2.0f);
    selectTitle.setPosition(500, 45);

    sf::Text selectHint("Up/Down to navigate, Enter to select", minecraftFont, 14);
    selectHint.setFillColor(sf::Color(120, 120, 120));
    sf::FloatRect shBounds = selectHint.getLocalBounds();
    selectHint.setOrigin(shBounds.left + shBounds.width / 2.0f, shBounds.top + shBounds.height / 2.0f);
    selectHint.setPosition(500, 80);



    
    sf::RectangleShape newGameButton(sf::Vector2f(160, 40));
    newGameButton.setFillColor(sf::Color(100, 180, 255));
    newGameButton.setPosition(420, 550);  // Centered in 1000x800

    sf::Text newGameText("Return (R)", minecraftFont, 20);

    newGameText.setFillColor(sf::Color::White);
    sf::FloatRect ngBounds = newGameText.getLocalBounds();
    newGameText.setOrigin(std::floor(ngBounds.left + ngBounds.width / 2.0f), std::floor(ngBounds.top + ngBounds.height / 2.0f));
    newGameText.setPosition(500, 570);
    
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
    enemyBarBack.setPosition(650, 40);

    sf::RectangleShape enemyBar;
    enemyBar.setFillColor(sf::Color::Green);
    enemyBar.setPosition(650, 40);

    // Moves box background
    sf::RectangleShape movesBg(sf::Vector2f(220, 100));
    movesBg.setFillColor(sf::Color(0, 0, 0, 200));
    movesBg.setOutlineThickness(2.0f);
    movesBg.setOutlineColor(sf::Color(255, 255, 255));
    movesBg.setPosition(50, 680);

    float totalTime = 0.0f;
    float popupScale = 0.0f;

    // Render loop
    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        totalTime += deltaTime;
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
                        // Reset all state for potential next run within same call (though we return now)
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

                        // Return to overworld
                        return;

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

                        // Enemy is the wild pokemon passed to the function
                        enemy = wildPokemon;
                        if (!enemyTexture.loadFromFile(enemy->getSpritePath())) {
                            std::cerr << "Failed to load wild pokemon sprite\n";
                        }


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
                            backgroundSprite.setScale(1000.0f / texSize.x, 800.0f / texSize.y);
                        } else {
                            backgroundSprite.setTexture(sf::Texture(), true);
                        }

                        // Set up player sprite
                        playerSprite.setTexture(playerTexture, true);
                        {
                            sf::Vector2u texSize = playerTexture.getSize();
                            float scale = playerTargetSize / std::max(texSize.x, texSize.y);
                            playerSprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f); 
                            playerSprite.setScale(-scale, scale); // Flip horizontally
                        }
                        playerSprite.setPosition(200, 400);

                        // Set up enemy sprite
                        enemySprite.setTexture(enemyTexture, true);
                        {
                            sf::Vector2u texSize = enemyTexture.getSize();
                            float scale = enemyTargetSize / std::max(texSize.x, texSize.y);
                            enemySprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
                            enemySprite.setScale(scale, scale);
                        }
                        enemySprite.setPosition(800, 400);

                        // Set up name texts (top bar centering)
                        playerText.setString(player->getName());
                        sf::FloatRect ptBounds = playerText.getLocalBounds();
                        playerText.setOrigin(0, std::floor(ptBounds.top + ptBounds.height / 2.0f));
                        playerText.setPosition(50, 22); // Fine-tuned for top bar

                        enemyText.setString(enemy->getName());
                        sf::FloatRect etBounds = enemyText.getLocalBounds();
                        enemyText.setOrigin(etBounds.width, std::floor(etBounds.top + etBounds.height / 2.0f));
                        enemyText.setPosition(950, 22);

                        // Build move buttons and texts from player's moves
                        moveTexts.clear();
                        moveButtons.clear();
                        const std::vector<Move>& moves = player->getMoves();
                        for (size_t i = 0; i < moves.size(); ++i) {
                            // Column layout position
                            float bx = 60;
                            float by = 688 + i * 32;

                            // Create button
                            sf::RectangleShape btn(sf::Vector2f(200, 28));
                            btn.setPosition(bx, by);
                            btn.setFillColor(sf::Color(40, 40, 40));
                            btn.setOutlineColor(sf::Color::White);
                            btn.setOutlineThickness(1.0f);
                            moveButtons.push_back(btn);

                            // Create text centered in button
                            sf::Text moveText(moves[i].name, minecraftFont, 14);
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
                else if (gameState == GameState::End) {
                    if (event.key.code == sf::Keyboard::R) {
                        return; // Return to overworld
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
                // Row background highlight for selected
                if ((int)i == selectedIndex) {
                    sf::RectangleShape rowHighlight(sf::Vector2f(700, 80));
                    rowHighlight.setOrigin(350, 40);
                    rowHighlight.setPosition(500, 150 + i * 100);
                    rowHighlight.setFillColor(sf::Color(240, 240, 240, 150));
                    rowHighlight.setOutlineColor(sf::Color(100, 180, 255));
                    rowHighlight.setOutlineThickness(2.0f);
                    window.draw(rowHighlight);
                }

                // Sprite preview (vertically centered in 100px row)
                sf::Sprite preview(textures[i]);
                sf::Vector2u texSize = textures[i].getSize();
                float scale = 60.0f / std::max(texSize.x, texSize.y);
                
                // Selection scale effect
                if ((int)i == selectedIndex) scale *= (1.0f + 0.1f * std::sin(totalTime * 5.0f));
                
                preview.setScale(scale, scale);
                preview.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
                preview.setPosition(390, 150 + i * 100);

                // Name + type text (vertically centered in 100px row)
                std::string label = roster[i]->getName() + "  (" + typeToString(roster[i]->getType()) + ")";
                sf::Text optionText(label, minecraftFont, 20);
                sf::FloatRect otBounds = optionText.getLocalBounds();
                optionText.setOrigin(0, std::floor(otBounds.top + otBounds.height / 2.0f));
                optionText.setPosition(440, 150 + i * 100);

                // Highlight selected
                if ((int)i == selectedIndex) {
                    optionText.setFillColor(sf::Color(0, 100, 255));
                    optionText.setStyle(sf::Text::Bold);

                    // Draw selection arrow with pulse
                    sf::Text arrow(">", minecraftFont, 24);
                    sf::FloatRect arBounds = arrow.getLocalBounds();
                    arrow.setOrigin(arBounds.width / 2.0f, std::floor(arBounds.top + arBounds.height / 2.0f));
                    float xPulse = 345 + 5.0f * std::sin(totalTime * 8.0f);
                    arrow.setPosition(xPulse, 150 + i * 100);
                    arrow.setFillColor(sf::Color(0, 100, 255));
                    window.draw(arrow);
                } else {
                    optionText.setFillColor(sf::Color(80, 80, 80));
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
                        enemySprite.setPosition(800 + offset, 400);
                    else
                        playerSprite.setPosition(200 + offset, 400);

                    if (t > animationDuration) {
                        // reset positions
                        playerSprite.setPosition(200, 400);
                        enemySprite.setPosition(800, 400);
                        
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
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

                window.draw(movesBg);
                for (size_t i = 0; i < moveButtons.size(); ++i)
                {
                    sf::RectangleShape& btn = moveButtons[i];
                    sf::Text& txt = moveTexts[i];
                    
                    // Force non-hovered state first to ensure bounds check is against base position
                    btn.setScale(1.0f, 1.0f);
                    btn.setOrigin(0, 0);
                    btn.setPosition(60, 688 + i * 32);
                    
                    bool hovered = btn.getGlobalBounds().contains(mousePos);
                    bool selected = ((int)i == selectedMove);
                    bool active = (selected || hovered);

                    Type moveType = player->getMoves()[i].type;
                    sf::Color moveColor = getTypeColor(moveType);
                    
                    // Base styling reset
                    btn.setOutlineThickness(2.0f);
                    
                    if (active) {
                        // Subtle selection pulse
                        float pulseFactor = 0.01f * std::sin(totalTime * 10.0f);
                        btn.setScale(1.03f + pulseFactor, 1.05f);
                        btn.setOrigin(btn.getSize().x / 2.0f, btn.getSize().y / 2.0f);
                        btn.setPosition(160, 688 + i * 32 + 14);
                        
                        btn.setFillColor(moveColor);
                        btn.setOutlineColor(sf::Color::White);
                        btn.setOutlineThickness(3.0f);
                        
                        txt.setFillColor(sf::Color::White);
                        txt.setOutlineColor(sf::Color::Black);
                        txt.setOutlineThickness(1.5f);
                        txt.setStyle(sf::Text::Bold);
                        txt.setPosition(btn.getPosition());
                    } else {
                        // btn already set to base pos/scale above
                        sf::Color uniformBase = sf::Color(45, 45, 45);
                        btn.setFillColor(uniformBase);
                        btn.setOutlineColor(sf::Color(100, 100, 100));
                        btn.setOutlineThickness(1.0f);
                        
                        txt.setFillColor(sf::Color(180, 180, 180));
                        txt.setOutlineThickness(0);
                        txt.setStyle(sf::Text::Regular);
                        txt.setPosition(60 + 100, 688 + i * 32 + 14);
                    }

                    // Draw 3D Bevel/Shadow
                    sf::RectangleShape shadow(btn.getSize());
                    shadow.setOrigin(btn.getOrigin());
                    shadow.setPosition(btn.getPosition().x + 2, btn.getPosition().y + 2);
                    shadow.setScale(btn.getScale());
                    shadow.setFillColor(sf::Color(0, 0, 0, 100));
                    window.draw(shadow);

                    window.draw(btn);
                    
                    // Gloss highlight
                    sf::RectangleShape gloss(sf::Vector2f(btn.getGlobalBounds().width, btn.getGlobalBounds().height / 2.0f));
                    gloss.setPosition(btn.getGlobalBounds().left, btn.getGlobalBounds().top);
                    gloss.setFillColor(sf::Color(255, 255, 255, 40));
                    window.draw(gloss);
                    
                    window.draw(txt);
                }
                
                if (gameState == GameState::End) {
                    // New Game Button styling
                    newGameButton.setOutlineThickness(3.0f);
                    newGameButton.setOutlineColor(sf::Color::White);
                    float ngScale = 1.0f + 0.05f * std::abs(std::sin(totalTime * 3.0f));
                    newGameButton.setScale(ngScale, ngScale);
                    newGameButton.setOrigin(80, 20);
                    newGameButton.setPosition(500, 570);
                    
                    window.draw(newGameButton);
                    
                    newGameText.setPosition(newGameButton.getPosition());
                    newGameText.setScale(ngScale, ngScale);
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
                msgBox.setPosition(50, 700);
                window.draw(msgBox);

                sf::Text typedText(shownMessage, minecraftFont, 18);
                sf::FloatRect ttBounds = typedText.getLocalBounds();
                typedText.setOrigin(0, ttBounds.top + ttBounds.height / 2.0f);
                typedText.setPosition(60, 730);
                typedText.setFillColor(sf::Color::Black);
                window.draw(typedText);
            }

            if (gameState == GameState::End && messageQueue.empty() && messageDone) {
                // Pop-up animation
                if (popupScale < 1.0f) popupScale += deltaTime * 4.0f;
                if (popupScale > 1.0f) popupScale = 1.0f;

                float displayScale = popupScale * (1.0f + 0.05f * std::sin(totalTime * 2.0f));

                // Multi-layered stylish frame
                sf::RectangleShape outerBox(sf::Vector2f(400, 120));
                outerBox.setOrigin(200, 60);
                outerBox.setPosition(500, 300);
                outerBox.setScale(displayScale, displayScale);
                outerBox.setFillColor(sf::Color(0, 50, 100)); // Dark navy
                outerBox.setOutlineThickness(4);
                outerBox.setOutlineColor(sf::Color::White);

                sf::RectangleShape innerBox(sf::Vector2f(380, 100));
                innerBox.setOrigin(190, 50);
                innerBox.setPosition(500, 300);
                innerBox.setScale(displayScale, displayScale);
                innerBox.setFillColor(sf::Color(240, 240, 240));
                innerBox.setOutlineThickness(2);
                innerBox.setOutlineColor(sf::Color::Black);

                window.draw(outerBox);
                window.draw(innerBox);

                // Result text with shadow for depth
                sf::Text resTextShadow(battleResult, minecraftFont, 44);
                resTextShadow.setStyle(sf::Text::Bold);
                resTextShadow.setFillColor(sf::Color(0, 0, 0, 100));
                sf::FloatRect rsBounds = resTextShadow.getLocalBounds();
                resTextShadow.setOrigin(std::floor(rsBounds.left + rsBounds.width / 2.f), std::floor(rsBounds.top + rsBounds.height / 2.f));
                resTextShadow.setPosition(500 + 4, 300 + 4);
                resTextShadow.setScale(displayScale, displayScale);
                window.draw(resTextShadow);

                sf::Text resText(battleResult, minecraftFont, 44);
                resText.setStyle(sf::Text::Bold);
                resText.setFillColor(battleResult == "YOU WIN!" ? sf::Color(0, 150, 0) : sf::Color(150, 0, 0));
                sf::FloatRect resBounds = resText.getLocalBounds();
                resText.setOrigin(std::floor(resBounds.left + resBounds.width / 2.f), std::floor(resBounds.top + resBounds.height / 2.f));
                resText.setPosition(500, 300);
                resText.setScale(displayScale, displayScale);
                window.draw(resText);
            } else {
                popupScale = 0.0f; // Reset for next time
            }
        }

        window.display();
    }

    // Clean up roster memory
    for (auto* p : roster)
        delete p;
    if (player) delete player;
    // Note: enemy (wildPokemon) is deleted by the overworld or at the end of this function
    if (enemy) delete enemy;

    return;
}

