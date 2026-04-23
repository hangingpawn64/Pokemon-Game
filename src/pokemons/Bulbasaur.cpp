#include "Bulbasaur.h"

Bulbasaur::Bulbasaur()
        : Pokemon("Bulbasaur", Type::Grass, 100, 49, 49, 45,
                  "assets/sprites/pokemons/bulbasaur.png") {
        addMove(Move("Vine Whip", Type::Grass, 45, 100));
        addMove(Move("Tackle", Type::Normal, 40, 100));
        addMove(Move("Razor Leaf", Type::Grass, 55, 95));
    }
