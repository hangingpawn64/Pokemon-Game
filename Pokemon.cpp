#include "Pokemon.h"
#include <iostream>
#include <cstdlib>

Pokemon::Pokemon(const std::string& name, Type type, int maxHP, int attack, int defense, int speed,
                 const std::string& spriteUrl)
    : name(name), spriteUrl(spriteUrl), type(type), maxHP(maxHP), currentHP(maxHP),
      attack(attack), defense(defense), speed(speed), status(Status::NONE) {}

void Pokemon::addMove(const Move& move) {
    if (moves.size() < 3) moves.push_back(move);
}

std::string Pokemon::getName() const { return name; }
std::string Pokemon::getSpriteUrl() const { return spriteUrl; }
void Pokemon::setSpriteUrl(const std::string& url) { spriteUrl = url; }
Type Pokemon::getType() const { return type; }
int Pokemon::getmaxHP() const { return maxHP; }
int Pokemon::getCurrentHP() const { return currentHP; }
int Pokemon::getAttack() const { return attack; }
int Pokemon::getDefense() const { return defense; }
int Pokemon::getSpeed() const { return speed; }
bool Pokemon::isFainted() const { return currentHP <= 0; }
const std::vector<Move>& Pokemon::getMoves() const { return moves; }

std::string Pokemon::takeDamage(int amount) {
    currentHP -= amount;
    if (currentHP < 0) currentHP = 0;
    std::string msg = name + " took damage: " + std::to_string(amount) + " HP: " + std::to_string(currentHP) + "/" + std::to_string(maxHP);
    std::cout << msg << std::endl;
    return msg;
}

void Pokemon::healFull() {
    currentHP = maxHP;
    status = Status::NONE;
}

std::vector<std::string> Pokemon::useMove(int index, Pokemon& target) {
    std::vector<std::string> msgs;
    if (index < 0 || index >= (int)moves.size()) return msgs;

    if (status == Status::PARALYZED) {
        int skipChance = rand() % 100;
        if (skipChance < 25) {
            std::string t = name + " is paralyzed and couldn't move!";
            std::cout << t << "\n";
            msgs.push_back(t);
            return msgs;
        }
    }

    const Move& move = moves[index];

    // Random flavorful message
    std::vector<std::string> flavors = {
        name + " launches a ferocious " + move.name + "!",
        name + " attacks with " + move.name + "!",
        name + " strikes hard using " + move.name + "!",
        name + " delivers a clean " + move.name + "!",
        name + " used " + move.name + " with determination!"
    };
    std::string usedMsg = flavors[rand() % flavors.size()];
    std::cout << usedMsg << "\n";
    msgs.push_back(usedMsg);

    int hitChance = rand() % 100;
    if (hitChance >= move.accuracy) {
        std::string t = "But it missed!";
        std::cout << t << "\n";
        msgs.push_back(t);
        return msgs;
    }

    double eff = 1.0;
    if (move.type == Type::Fire && target.type == Type::Grass) eff = 2.0;
    if (move.type == Type::Water && target.type == Type::Fire) eff = 2.0;
    if (move.type == Type::Grass && target.type == Type::Water) eff = 2.0;
    if (move.type == Type::Electric && target.type == Type::Water) eff = 2.0;
    if (move.type == Type::Grass && target.type == Type::Fire) eff = 0.5;
    if (move.type == Type::Fire && target.type == Type::Water) eff = 0.5;

    // Critical hit?
    bool crit = (rand() % 100 < 10); // 10% crit chance
    if (crit) eff *= 1.5;

    int rawDamage = (attack * move.power) / std::max(1, target.defense);
    int damage = std::max(1, (int)(rawDamage * eff));

    target.takeDamage(damage);

    if (crit) {
        std::string t = "A critical hit!";
        std::cout << t << "\n";
        msgs.push_back(t);
    }

    if (eff > 1.0) {
        std::string t = "It's Super Effective!";
        std::cout << t << "\n";
        msgs.push_back(t);
    }
    else if (eff < 1.0) {
        std::string t = "It's not very Effective...";
        std::cout << t << "\n";
        msgs.push_back(t);
    }

    // Thunderbolt paralysis
    if (move.name == "Thunderbolt") {
        int effectRoll = rand() % 100;
        if (effectRoll < 20 && target.getStatus() == Status::NONE) {
            std::string t = target.getName() + " was paralyzed!";
            std::cout << t << "\n";
            msgs.push_back(t);
            target.setStatus(Status::PARALYZED);
        }
    }

    return msgs;
}

void Pokemon::setStatus(Status s) { status = s; }
Status Pokemon::getStatus() const { return status; }

std::string statusToString(Status status) {
    switch (status) {
        case Status::NONE: return "None";
        case Status::PARALYZED: return "Paralyzed";
        default: return "Unknown";
    }
}
