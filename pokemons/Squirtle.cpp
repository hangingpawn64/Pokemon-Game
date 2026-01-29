#include "Squirtle.h"

Squirtle::Squirtle()
        : Pokemon("Squirtle", Type::Water, 100, 48, 65, 43) {
        addMove(Move("Water Gun", Type::Water, 40, 100));
        addMove(Move("Tackle", Type::Normal, 40, 100));
        addMove(Move("Bubble Beam", Type::Water, 65, 95));
    }
