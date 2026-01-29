#include "Charmander.h"

Charmander::Charmander()
    : Pokemon("Charmander", Type::Fire, 100, 52, 43, 65) {
    addMove(Move("Ember", Type::Fire, 40, 100));
    addMove(Move("Scratch", Type::Normal, 40, 100));
    addMove(Move("Flame Burst", Type::Fire, 70, 85));
}
