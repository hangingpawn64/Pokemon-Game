#include "Battle.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
using namespace std;

std::string generateHPBar(int current, int max, int width) {
    int filled = (int)((double)current / max * width);
    std::string bar = "[";
    for (int i = 0; i < width; ++i) {
        bar += (i < filled) ? "=" : " ";
    }
    bar += "]";
    return bar;
}

Battle::Battle(Pokemon& player, Pokemon& enemy) : player(player), enemy(enemy) {}

void Battle::run() {
    cout << "\n Battle Start! ⚔️  🛡️"<<"  "<< player.getName() << " vs " << enemy.getName() << endl;

    while (!player.isFainted() && !enemy.isFainted()) {
        printStatus();

        if (player.getSpeed() >= enemy.getSpeed()) {
            playerTurn();
            if (enemy.isFainted()) break;
            aiturn();
        } else {
            aiturn();
            if (player.isFainted()) break;
            playerTurn();
        }
    }

    cout << "\n======== Battle Ended =========\n";

    if (player.isFainted()) {
        cout << player.getName() << " fainted! You lost the battle!\n";
    } else {
        cout << enemy.getName() << " fainted! You win!\n";
    }
}

void Battle::printStatus() {
    cout << "\n📊 Battle Status:\n";

    cout << "🧍 You: " << player.getName() << " (" << typeToString(player.getType()) << ")\n";
    cout << "HP: " << generateHPBar(player.getCurrentHP(), player.getmaxHP())
         << " " << player.getCurrentHP() << "/" << player.getmaxHP() << "\n";
    cout << "Status: " << statusToString(player.getStatus()) << "\n";

    cout << "\n🤖 Enemy: " << enemy.getName() << " (" << typeToString(enemy.getType()) << ")\n";
    cout << "HP: " << generateHPBar(enemy.getCurrentHP(), enemy.getmaxHP())
         << " " << enemy.getCurrentHP() << "/" << enemy.getmaxHP() << "\n";
    cout << "Status: " << statusToString(enemy.getStatus()) << "\n";
}

void Battle::playerTurn() {
    cout << "\n Your Turn: choose a move\n";
    const vector<Move>& moves = player.getMoves();
    for (int i = 0; i < (int)moves.size(); ++i) {
        cout << i << ") " << moves[i].name << " (Power: " << moves[i].power
             << ", Acc: " << moves[i].accuracy << ", Type: "
             << typeToString(moves[i].type) << ")\n";
    }

    int choice;
    cout << "Enter choice index: ";
    cin >> choice;

    if (choice < 0 || choice >= (int)moves.size()) {
        cout << "Invalid Choice. Using Move 0.\n";
        choice = 0;
    }

    player.useMove(choice, enemy);
}

void Battle::aiturn() {
    cout << "\n Enemy Turn: " << enemy.getName() << " is choosing a move...\n";
    const vector<Move>& moves = enemy.getMoves();
    int index = rand() % moves.size();
    enemy.useMove(index, player);
}