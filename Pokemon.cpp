#include "Pokemon.h"
#include <iostream>
#include <cstdlib>

Pokemon::Pokemon(const std::string& name, Type type, int maxHP, int attack, int defense, int speed)
    : name(name), type(type), maxHP(maxHP), currentHP(maxHP),
      attack(attack), defense(defense), speed(speed), status(Status::NONE) {}

void Pokemon::addMove(const Move& move) {
    if (moves.size() < 3) moves.push_back(move);
}

std::string Pokemon::getName() const { return name; }
Type Pokemon::getType() const { return type; }
int Pokemon::getmaxHP() const { return maxHP; }
int Pokemon::getCurrentHP() const { return currentHP; }
int Pokemon::getAttack() const { return attack; }
int Pokemon::getDefense() const { return defense; }
int Pokemon::getSpeed() const { return speed; }
bool Pokemon::isFainted() const { return currentHP <= 0; }
const std::vector<Move>& Pokemon::getMoves() const { return moves; }

void Pokemon::takeDamage(int amount) {
    currentHP -= amount;
    if (currentHP < 0) currentHP = 0;
    std::cout << name << " took damage: " << amount << " HP: " << currentHP << "/" << maxHP << std::endl;
}

void Pokemon::useMove(int index, Pokemon& target) {
    if (index < 0 || index >= (int)moves.size()) return;

    if (status == Status::PARALYZED) {
        int skipChance = rand() % 100;
        if (skipChance < 25) {
            std::cout << name << " is paralyzed and couldn't move!\n";
            return;
        }
    }

    Move move = moves[index];
    std::cout << name << " used " << move.name << "!\n";

    int hitChance = rand() % 100;
    if (hitChance >= move.accuracy) {
        std::cout << "But it missed!\n";
        return;
    }

    double eff = 1.0;
    if (move.type == Type::Fire && target.type == Type::Grass) eff = 2.0;
    if (move.type == Type::Water && target.type == Type::Fire) eff = 2.0;
    if (move.type == Type::Grass && target.type == Type::Water) eff = 2.0;
    if (move.type == Type::Electric && target.type == Type::Water) eff = 2.0;
    if (move.type == Type::Grass && target.type == Type::Fire) eff = 0.5;
    if (move.type == Type::Fire && target.type == Type::Water) eff = 0.5;

    int rawDamage = (attack * move.power) / std::max(1, target.defense);
    int damage = std::max(1, (int)(rawDamage * eff));

    target.takeDamage(damage);

    if (eff > 1.0) std::cout << "It's Super Effective!\n";
    else if (eff < 1.0) std::cout << "It's not very Effective...\n";

    if (move.name == "Thunderbolt") {
        int effectRoll = rand() % 100;
        if (effectRoll < 20 && target.getStatus() == Status::NONE) {
            std::cout << target.getName() << " was paralyzed!\n";
            target.setStatus(Status::PARALYZED);
        }
    }
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
