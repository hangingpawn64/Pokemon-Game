#include <bits/stdc++.h>
#include "Move.h"
#include "Type.h"
#include "Pokemon.h"
#include "Battle.h"
#include "pokemons/Pikachu.h"
#include "pokemons/Bulbasaur.h"
#include "pokemons/Squirtle.h"
#include "pokemons/Charmander.h"

using namespace std;

int main() {
    srand((unsigned)time(0));

    Pikachu pikachu;
    Bulbasaur bulbasaur;
    Charmander charmander;
    Squirtle squirtle;

    vector<Pokemon*> roster = { &pikachu, &bulbasaur, &charmander, &squirtle};
    cout<<"Choose Your Pokemon!\n";
    for(int i=0; i<(int)roster.size(); ++i){
        cout<< i <<") "<<roster[i]->getName()<<" (Type: "
        <<typeToString(roster[i]->getType())<<")\n";
    }

    int choice = 0;
    cout << "Enter the number of your choice: ";
    cin >> choice;
    if (choice < 0 || choice >= (int)roster.size()) {
        cout << "Invalid choice, defaulting to Pikachu.\n";
        choice = 0;
    }

    Pokemon* player =roster[choice];

    roster.erase(roster.begin() + choice);

    int enemyIndex = rand() % roster.size();
    Pokemon* enemy = roster[enemyIndex];

    cout<< "\nYou Chose: "<<player->getName()<<"!\n";
    cout<< "\nEnemy Chose: "<<enemy->getName()<<"!\n";    

    Battle battle(*player, *enemy);
    battle.run();

    return 0;
}