#include "Pikachu.h"

Pikachu::Pikachu()
    : Pokemon("Pikachu", Type::Electric, 100, 55, 40, 90,
              "https://img.pokemondb.net/sprites/scarlet-violet/normal/pikachu-hoenn-cap.png") {
    addMove(Move("Thunderbolt", Type::Electric, 90, 100));
    addMove(Move("Quick Attack", Type::Normal, 40, 100));
    addMove(Move("Iron Tail", Type::Normal, 70, 75));
}
