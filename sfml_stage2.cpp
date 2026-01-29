#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Pokemon Battle");

    // Load Textures
    sf::Texture pikachuTexture, bulbasaurTexture;
    if (!pikachuTexture.loadFromFile("assets/pikachu.png") ||
        !bulbasaurTexture.loadFromFile("assets/bulbasaur.png"))
    {
        std::cerr << "Failed to load Pokémon images\n";
        return 1;
    }

    // Load Font
    sf::Font font;
    if (!font.loadFromFile("assets/ARIAL.TTF"))
    {
        std::cerr << "Failed to load font\n";
        return 1;
    }

    // Create Sprites
    sf::Sprite pikachuSprite(pikachuTexture);
    pikachuSprite.setScale(0.8f, 0.8f);
    pikachuSprite.setPosition(100, 300);

    sf::Sprite bulbasaurSprite(bulbasaurTexture);
    bulbasaurSprite.setScale(0.3f, 0.3f);
    bulbasaurSprite.setPosition(500, 100);

    // Text: Pokémon Names
    sf::Text pikachuText("Pikachu", font, 16);
    pikachuText.setPosition(200, 460);
    pikachuText.setFillColor(sf::Color::Black);

    sf::Text bulbasaurText("Bulbasaur", font, 16);
    bulbasaurText.setPosition(550, 250);
    bulbasaurText.setFillColor(sf::Color::Black);

    // Simulated move list (real game will pull from Pokemon object)
    std::vector<std::string> moves = {
        "Thunderbolt", "Quick Attack", "Iron Tail"};

    std::vector<sf::Text> moveTexts;
    for (size_t i = 0; i < moves.size(); ++i)
    {
        sf::Text moveText(moves[i], font, 16);
        moveText.setPosition(200, 500 + i * 20); // space them out
        moveText.setFillColor(sf::Color::Blue);
        moveTexts.push_back(moveText);
    }

    // Simulated HP values (later link these to real battle logic)
    int pikachuHP = 70;
    int pikachuMaxHP = 100;

    int bulbasaurHP = 40;
    int bulbasaurMaxHP = 100;

    // HP Bar dimensions
    sf::Vector2f barSize(100, 12);

    // Pikachu HP bar
    sf::RectangleShape pikachuBarBack(barSize);
    pikachuBarBack.setFillColor(sf::Color(50, 50, 50));
    pikachuBarBack.setPosition(180, 480);

    sf::RectangleShape pikachuBar;
    pikachuBar.setFillColor(sf::Color::Green);
    pikachuBar.setSize(sf::Vector2f((pikachuHP / (float)pikachuMaxHP) * barSize.x, barSize.y));
    pikachuBar.setPosition(180, 480);

    // Bulbasaur HP bar
    sf::RectangleShape bulbasaurBarBack(barSize);
    bulbasaurBarBack.setFillColor(sf::Color(50, 50, 50));
    bulbasaurBarBack.setPosition(530, 270);

    sf::RectangleShape bulbasaurBar;
    bulbasaurBar.setFillColor(sf::Color::Green);
    bulbasaurBar.setSize(sf::Vector2f((bulbasaurHP / (float)bulbasaurMaxHP) * barSize.x, barSize.y));
    bulbasaurBar.setPosition(530, 270);

    int selectedMove = -1; // -1 means no move selected

    // Render loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Num1)
                    selectedMove = 0;
                if (event.key.code == sf::Keyboard::Num2)
                    selectedMove = 1;
                if (event.key.code == sf::Keyboard::Num3)
                    selectedMove = 2;

                if (selectedMove >= 0 && selectedMove < (int)moves.size())
                {
                    std::cout << "You selected: " << moves[selectedMove] << "\n";
                }
            }
        }

        window.clear(sf::Color::White);
        window.draw(pikachuSprite);
        window.draw(bulbasaurSprite);
        window.draw(pikachuText);
        window.draw(bulbasaurText);
        window.draw(pikachuBarBack);
        window.draw(pikachuBar);
        window.draw(bulbasaurBarBack);
        window.draw(bulbasaurBar);

        for (size_t i = 0; i < moveTexts.size(); ++i)
        {
            if ((int)i == selectedMove)
                moveTexts[i].setFillColor(sf::Color::Red);
            else
                moveTexts[i].setFillColor(sf::Color::Blue);

            window.draw(moveTexts[i]);
        }

        for (const auto &moveText : moveTexts)
        {
            window.draw(moveText);
        }

        window.display();
    }

    return 0;
}
