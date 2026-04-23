#ifndef MOVE_H
#define MOVE_H

#include <string>
#include "Type.h"

class Move {
public:
    std::string name;
    Type type;
    int power;
    int accuracy;

    Move(std::string name, Type type, int power, int accuracy);
};

#endif
