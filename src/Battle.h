// Step 3: Modularize Battle Class

#ifndef BATTLE_H
#define BATTLE_H

#include "Pokemon.h"
#include <string>

std::string generateHPBar(int current, int max, int width = 20);

class Battle {
private:
    Pokemon& player;
    Pokemon& enemy;

public:
    Battle(Pokemon& player, Pokemon& enemy);
    void run();
    void printStatus();
    void playerTurn();
    void aiturn();
};

#endif