#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Pokemon.h"
#include "pokemons/Pikachu.h"
#include "pokemons/Bulbasaur.h"
#include "pokemons/Charmander.h"
#include "pokemons/Squirtle.h"

extern void startBattle(sf::RenderWindow& window, Pokemon* wildPokemon);  // Declare the battle function

enum class MapState { Exploring, EncounterPopup };

const int TILE_SIZE = 50;
const int MAP_WIDTH = 20;
const int MAP_HEIGHT = 16;

std::vector<std::string> tileMap = {
    "####################", // 0
    "#..............1.4.#", // 1
    "#.VVV$........4.3.4#", // 2
    "#.VVV.......?..1.3.#", // 3
    "#.VVV...........4.1#", // 4
    "#.$.!............3.#", // 5
    "#.......@@@........#", // 6
    "#.......@@@........#", // 7
    "#.......@@@w.......#", // 8
    "#..................#", // 9
    "#............ff6789#", // 10
    "#..E.E.......7LLLL6#", // 11
    "#.E.E.......f9LLLL7#", // 12
    "#..E.E.......6LLLL8#", // 13
    "#.E.E........8LLLL9#", // 14
    "####################"  // 15
};


int main() {
    srand((unsigned)time(0));
    sf::RenderWindow window(sf::VideoMode(1000, 800), "Overworld Map");
    window.setFramerateLimit(60);

    sf::Clock clock;
    MapState mapState = MapState::Exploring;

    // View to follow player
    sf::View worldView(sf::FloatRect(0, 0, 1000, 800));

    // Fade variables
    sf::RectangleShape fadeRect(sf::Vector2f(1000, 800));
    fadeRect.setFillColor(sf::Color::Transparent);
    float fadeAlpha = 0.0f;
    bool fadingOut = false;
    bool fadingIn = false;

    // Popup variables
    std::string encounterName = "";
    sf::Texture encounterTexture;
    sf::Sprite encounterSprite;
    sf::Font font;
    sf::Text messageText, optionBattle, optionRun;
    int selectedOption = 0;
    Pokemon* pendingEncounter = nullptr;
    sf::Vector2f lastSafePos(400, 300);

    // Background Dim
    sf::RectangleShape dimOverlay(sf::Vector2f(1000, 800));
    dimOverlay.setFillColor(sf::Color(0, 0, 0, 150));

    if (!font.loadFromFile("assets/Minecraftia-Regular.ttf")) {
        std::cerr << "Failed to load assets/Minecraftia-Regular.ttf\n";
        return 1;
    }

    // Tile Textures
    std::map<char, sf::Texture> tileTextures;
    std::map<char, std::string> tilePaths = {
        {'.', "assets/tiles/grass.png"},
        {'#', "assets/tiles/longGrass.png"},
        {'@', "assets/tiles/house.png"},
        {'V', "assets/tiles/volcano.png"},
        {'E', "assets/tiles/electricCloud.png"},
        {'L', "assets/tiles/water.png"},
        {'!', "assets/tiles/volcanicRock.png"},
        {'$', "assets/tiles/volcanicRock2.png"}, 
        {'w', "assets/tiles/well.png"},
        {'1', "assets/tiles/Plants1.png"},
        {'3', "assets/tiles/Plants3.png"},
        {'4', "assets/tiles/Plants4.png"},
        {'6', "assets/tiles/rock1.png"},
        {'7', "assets/tiles/rock2.png"},
        {'8', "assets/tiles/rock3.png"},
        {'9', "assets/tiles/rock4.png"},
        {'f', "assets/tiles/rockcrumbsmall.png"},
        {'g', "assets/tiles/rockcrumblong.png"},
        {'?', "assets/tiles/flower.png"},
    };

    for (auto const& [symbol, path] : tilePaths) {
        if (!tileTextures[symbol].loadFromFile(path)) {
            std::cerr << "Failed to load tile asset: " << path << "\n";
        }
    }

    sf::Texture playerTextures[4][3];
    std::string charPaths[4][3] = {
        {"assets/character/walk_down_0.png", "assets/character/walk_down_1.png", "assets/character/walk_down_2.png"},
        {"assets/character/walk_up_0.png", "assets/character/walk_up_1.png", "assets/character/walk_up_2.png"},
        {"assets/character/walk_left_0.png", "assets/character/walk_left_1.png", "assets/character/walk_left_2.png"},
        {"assets/character/walk_right_0.png", "assets/character/walk_right_1.png", "assets/character/walk_right_2.png"}
    };

    for (int d = 0; d < 4; ++d) {
        for (int f = 0; f < 3; ++f) {
            if (!playerTextures[d][f].loadFromFile(charPaths[d][f])) {
                std::cerr << "Failed to load " << charPaths[d][f] << "\n";
                return 1;
            }
        }
    }

    sf::Sprite player(playerTextures[0][0]); // Initialize with down idle
    sf::Vector2u pTexSize = playerTextures[0][0].getSize();
    player.setOrigin(pTexSize.x / 2.0f, pTexSize.y / 2.0f);
    
    // Find spawn point '@' (3x3 house)
    sf::Vector2f spawnPos(400, 300);
    bool spawnFound = false;
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (tileMap[y][x] == '@' && !spawnFound) {
                // Spawn below the 3x3 house: x+1 is center column, y+3 is row below the house
                spawnPos = sf::Vector2f((x + 1) * TILE_SIZE + TILE_SIZE / 2.0f, (y + 3) * TILE_SIZE + TILE_SIZE / 2.0f);
                spawnFound = true;
            }
        }
    }
    player.setPosition(spawnPos);
    lastSafePos = spawnPos;
    player.setScale(0.15f, 0.15f); // Adjusted scale for new character sprites

    int currentDirIdx = 0; // 0=down, 1=up, 2=left, 3=right
    int animFrame = 0;
    float animTimer = 0.0f;

    messageText.setFont(font);
    messageText.setCharacterSize(22);
    messageText.setFillColor(sf::Color::White);
    messageText.setString("A wild Pokemon appeared!");

    optionBattle.setFont(font);
    optionBattle.setCharacterSize(20);
    optionBattle.setString("▶ Battle");

    optionRun.setFont(font);
    optionRun.setCharacterSize(20);
    optionRun.setString("   Run");


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (mapState == MapState::EncounterPopup) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down) {
                        selectedOption = 1 - selectedOption;
                    } else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Return) {
                        if (selectedOption == 0) {
                            fadingOut = true;
                            fadeAlpha = 0.0f;
                        } else {
                            mapState = MapState::Exploring;
                            player.setPosition(lastSafePos); // Retreat to last safe location
                            if (pendingEncounter) {
                                delete pendingEncounter;
                                pendingEncounter = nullptr;
                            }
                        }
                    }
                }
            }
        }

        float deltaTime = clock.restart().asSeconds();

        // Update Fade
        if (fadingOut) {
            fadeAlpha += 255 * deltaTime;
            if (fadeAlpha >= 255) {
                fadeAlpha = 255;
                fadingOut = false;
                
                if (pendingEncounter) {
                    startBattle(window, pendingEncounter);
                    window.setTitle("Overworld Map");
                    player.setPosition(400, 300);
                    pendingEncounter = nullptr;
                }
                
                fadingIn = true;
                mapState = MapState::Exploring;
            }
        } else if (fadingIn) {
            fadeAlpha -= 255 * deltaTime;
            if (fadeAlpha <= 0) {
                fadeAlpha = 0;
                fadingIn = false;
            }
        }
        fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(fadeAlpha)));

        if (mapState == MapState::Exploring && !fadingOut) {
            lastSafePos = player.getPosition(); // Store position before moving
            sf::Vector2f pos = player.getPosition();
            bool moved = false;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && pos.y > 0) {
                player.move(0, -5);
                currentDirIdx = 1;
                moved = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && pos.y < MAP_HEIGHT * TILE_SIZE) {
                player.move(0, 5);
                currentDirIdx = 0;
                moved = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && pos.x > 0) {
                player.move(-5, 0);
                currentDirIdx = 2;
                moved = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && pos.x < MAP_WIDTH * TILE_SIZE) {
                player.move(5, 0);
                currentDirIdx = 3;
                moved = true;
            }

            if (moved) {
                // Check for tile collision
                sf::Vector2f nextPos = player.getPosition();
                int tx = static_cast<int>(nextPos.x / TILE_SIZE);
                int ty = static_cast<int>(nextPos.y / TILE_SIZE);

                if (tx >= 0 && tx < MAP_WIDTH && ty >= 0 && ty < MAP_HEIGHT) {
                    char tile = tileMap[ty][tx];
                    if (tile == '#' || tile == '@' || tile == 'w') {
                        player.setPosition(lastSafePos); // Blocked
                        moved = false; // Stop animation if blocked
                    }
                }

                if (moved) {
                    animTimer += deltaTime;
                    if (animTimer >= 0.15f) {
                        animFrame = (animFrame == 1) ? 2 : 1;
                        animTimer = 0.0f;
                    }
                }
            }
            
            if (!moved) {
                animFrame = 0;
                animTimer = 0.0f;
            }

            player.setTexture(playerTextures[currentDirIdx][animFrame], true);
        }

        // Update Camera View
        sf::Vector2f pPos = player.getPosition();
        sf::Vector2f viewCenter = pPos;
        
        // Clamp view to map boundaries (1000x800 world, 1000x800 window)
        float worldW = MAP_WIDTH * TILE_SIZE;
        float worldH = MAP_HEIGHT * TILE_SIZE;
        
        if (viewCenter.x < 500) viewCenter.x = 500;
        if (viewCenter.x > worldW - 500) viewCenter.x = worldW - 500;
        if (viewCenter.y < 400) viewCenter.y = 400;
        if (viewCenter.y > worldH - 400) viewCenter.y = worldH - 400;
        
        worldView.setCenter(viewCenter);



        if (mapState == MapState::Exploring && !fadingOut) {
            sf::Vector2f center = player.getPosition();
            int tx = static_cast<int>(center.x / TILE_SIZE);
            int ty = static_cast<int>(center.y / TILE_SIZE);

            if (tx >= 0 && tx < MAP_WIDTH && ty >= 0 && ty < MAP_HEIGHT) {
                char tile = tileMap[ty][tx];
                bool encounterTriggered = false;
                
                if (tile == 'V') {
                    pendingEncounter = new Charmander();
                    encounterTriggered = true;
                } else if (tile == 'L') {
                    pendingEncounter = new Squirtle();
                    encounterTriggered = true;
                } else if (tile == '1' || tile == '3' || tile == '4') {
                    pendingEncounter = new Bulbasaur();
                    encounterTriggered = true;
                } else if (tile == 'E') {
                    pendingEncounter = new Pikachu();
                    encounterTriggered = true;
                }

                if (encounterTriggered) {
                    encounterName = pendingEncounter->getName();
                    if (encounterTexture.loadFromFile(pendingEncounter->getSpritePath())) {
                        encounterSprite.setTexture(encounterTexture, true);
                        sf::Vector2u eSize = encounterTexture.getSize();
                        encounterSprite.setOrigin(eSize.x / 2.0f, eSize.y / 2.0f);
                        
                        // Fixed target scaling (approx 180px)
                        float scale = 180.0f / std::max(eSize.x, eSize.y);
                        encounterSprite.setScale(scale, scale);
                        encounterSprite.setPosition(650, 400); // Right side of the 1000x800 window
                    }
                    mapState = MapState::EncounterPopup;
                    selectedOption = 0;
                }
            }
        }


        window.clear(sf::Color::White);
        
        // Use worldView for map and player
        window.setView(worldView);

        // Pass 1: Global Ground Layer (Lush Grass Base)
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            for (int x = 0; x < MAP_WIDTH; ++x) {
                sf::Sprite grassBase(tileTextures['.']);
                sf::Vector2u gSize = tileTextures['.'].getSize();
                grassBase.setScale((float)TILE_SIZE / gSize.x, (float)TILE_SIZE / gSize.y);
                grassBase.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                window.draw(grassBase);
            }
        }

        // Pass 2: Object & Encounter Zone Overlays
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            for (int x = 0; x < MAP_WIDTH; ++x) {
                char symbol = tileMap[y][x];
                if (tileTextures.count(symbol) && symbol != '.') {
                    // Check if this is a special 3x3 area (Zones or House)
                    bool isZone = (symbol == 'V' || symbol == '@');
                    
                    if (isZone) {
                        // Anchor detection: Only draw if the tile above and to the left are different symbols
                        bool hasSameAbove = (y > 0 && tileMap[y-1][x] == symbol);
                        bool hasSameLeft = (x > 0 && tileMap[y][x-1] == symbol);
                        
                        if (hasSameAbove || hasSameLeft) {
                            continue; // Skip rendering for interior zone tiles
                        }
                    }

                    sf::Sprite tileSprite(tileTextures[symbol]);
                    tileSprite.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                    
                    sf::Vector2u texSize = tileTextures[symbol].getSize();
                    if (isZone) {
                        // Scale to cover 3x3 tiles (150x150 pixels)
                        tileSprite.setScale((float)(3 * TILE_SIZE) / texSize.x, (float)(3 * TILE_SIZE) / texSize.y);
                    } else {
                        // Standard 1x1 tile scaling
                        tileSprite.setScale((float)TILE_SIZE / texSize.x, (float)TILE_SIZE / texSize.y);
                    }
                    window.draw(tileSprite);
                }
            }
        }

        window.draw(player);

        // Switch to default view for UI (fixed on screen)
        window.setView(window.getDefaultView());

        if (mapState == MapState::EncounterPopup) {
            window.draw(dimOverlay); // Dim the background

            sf::RectangleShape box(sf::Vector2f(500, 300));
            box.setOrigin(250, 150);
            box.setPosition(500, 400);
            box.setFillColor(sf::Color(0, 0, 0, 230));
            box.setOutlineColor(sf::Color::White);
            box.setOutlineThickness(3.0f);
            window.draw(box);

            // Position UI relative to box
            messageText.setPosition(270, 280);
            optionBattle.setPosition(290, 340);
            optionRun.setPosition(290, 380);

            window.draw(encounterSprite);
            window.draw(messageText);
            
            optionBattle.setString((selectedOption == 0 ? "▶ " : "   ") + std::string("Battle"));
            optionRun.setString((selectedOption == 1 ? "▶ " : "   ") + std::string("Run"));
            optionBattle.setFillColor(selectedOption == 0 ? sf::Color::Yellow : sf::Color::White);
            optionRun.setFillColor(selectedOption == 1 ? sf::Color::Yellow : sf::Color::White);
            
            window.draw(optionBattle);
            window.draw(optionRun);
        }

        if (fadingOut || fadeAlpha > 0) {
            window.draw(fadeRect);
        }

        window.display();
    }

    return 0;
}
