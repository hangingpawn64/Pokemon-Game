#include "Pikachu.h"

Pikachu::Pikachu()
    : Pokemon("Pikachu", Type::Electric, 100, 55, 40, 90,
              "assets/sprites/pokemons/pikachu-hoenn-cap.png") {
    addMove(Move("Thunderbolt", Type::Electric, 90, 100));
    addMove(Move("Quick Attack", Type::Normal, 40, 100));
    addMove(Move("Iron Tail", Type::Normal, 70, 75));
}
