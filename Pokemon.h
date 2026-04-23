#ifndef POKEMON_H
#define POKEMON_H

#include <string>
#include <vector>
#include "Move.h"
#include "Type.h"

enum class Status { NONE, PARALYZED };
std::string statusToString(Status status);

class Pokemon {
private:
    std::string name;
    std::string spritePath;
    Type type;
    int maxHP, currentHP;
    int attack, defense, speed;
    std::vector<Move> moves;
    Status status;

public:
    Pokemon(const std::string& name, Type type, int maxHP, int attack, int defense, int speed,
           const std::string& spriteUrl = "");

    void addMove(const Move& move);
    std::string takeDamage(int amount);
    void healFull();
    std::vector<std::string> useMove(int index, Pokemon& target);

    std::string getName() const;
    std::string getSpritePath() const;
    void setSpritePath(const std::string& path);
    Type getType() const;
    int getCurrentHP() const;
    int getmaxHP() const;
    int getAttack() const;
    int getDefense() const;
    int getSpeed() const;
    bool isFainted() const;
    const std::vector<Move>& getMoves() const;

    void setStatus(Status s);
    Status getStatus() const;
};

#endif
