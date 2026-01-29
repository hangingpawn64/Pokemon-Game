#ifndef TYPE_H
#define TYPE_H

#include <string>

enum class Type { Normal, Fire, Water, Grass, Electric };

inline std::string typeToString(Type type) {
    switch (type) {
        case Type::Normal: return "Normal";
        case Type::Fire: return "Fire";
        case Type::Water: return "Water";
        case Type::Grass: return "Grass";
        case Type::Electric: return "Electric";
        default: return "Unknown";
    }
}

#endif
