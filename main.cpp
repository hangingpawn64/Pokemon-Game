#include <bits/stdc++.h>
using namespace std;

enum class Type {Normal, Fire, Water, Grass, Electric};

// Utility to convert enum to string (for display)
string typeToString(Type type) {
    switch (type) {
        case Type::Normal: return "Normal";
        case Type::Fire: return "Fire";
        case Type::Water: return "Water";
        case Type::Grass: return "Grass";
        case Type::Electric: return "Electric";
        default: return "Unknown";
    }
}

class Move {
    public:
        string name;
        Type type;
        int power;
        int accuracy;

        Move(string name, Type type, int power, int accuracy){
            this->name = name;
            this->type = type;
            this->power = power; // i will keep power 1-255
            this->accuracy = accuracy;
        }

};

class Pokemon {
    private:
    // i will keep the int values between 1-255;
        string name;
        Type type;
        int maxHP;
        int currentHP;
        int attack;
        int defence;
        int speed;
        vector<Move> moves;
    
    public:
        Pokemon(string name, Type type, int maxHP, int currentHP,
            int attack, int defence, int speed, vector<Move> moves){
                this->name = name;
                this->type = type;
                this->maxHP = maxHP;
                this->currentHP = currentHP;
                this->attack = attack;
                this->defence = defence;
                this->speed = speed;
            }

            void addMove(Move move){
                if(moves.size()<3) {moves.push_back(move);}
            }

            string getName() {return name;}
            Type getType() {return type;}
            int getmaxHP() {return maxHP;}
            int getCurrentHP() {return currentHP;}
            int getAttack() {return attack;}
            int getDefence() {return defence;}
            int getSpeed() {return speed;}
            bool isFainted() {return currentHP <= 0;}
            vector<Move>& getMoves() {return moves;}

            void takeDamage(int amount){
                currentHP -= amount;
                if (currentHP < 0) currentHP = 0;
                cout<<name<<" took "<<"damage: "<<amount<<"HP: "<<currentHP<<"/"<<maxHP<<endl;
            }

            void useMove(int index, Pokemon& target){
                if(index < 0 || index >= int(moves.size())) return;

                Move move = moves[index];
                cout<<name<<" used "<<move.name<<"!\n";
            
                int hitChance = rand() % 100;
                if(hitChance >= move.accuracy){
                    cout<<"But it missed!\n";
                    return;
                }

                double eff = 1.0;
                if(move.type == Type::Fire && target.type == Type::Grass) eff = 2.0;
                if (move.type == Type::Water && target.type == Type::Fire) eff = 2.0;
                if (move.type == Type::Grass && target.type == Type::Water) eff = 2.0;
                if (move.type == Type::Electric && target.type == Type::Water) eff = 2.0;
                if (move.type == Type::Grass && target.type == Type::Fire) eff = 0.5;
                if (move.type == Type::Fire && target.type == Type::Water) eff = 0.5;

                int rawDamage = (attack * move.power) / max(1, target.defence);
                int damage = max(1, (int)(rawDamage * eff));

                target.takeDamage(damage);

                if(eff > 1.0) cout<<"It's Super Effective!\n";
                else if(eff < 1.0) cout<<"It's not very Effective...\n";
            }
};

int main(){
    srand(time(0)); // for randomness

    Move thunderbolt("Thunderbolt", Type::Electric, 90, 100);
    Move quickAttack("Quick Attack", Type::Normal, 40, 100);
    Move ironTail("Iron Tail", Type::Normal, 70, 75);

    Pokemon pikachu("Pikachu", Type::Electric, 100, 100, 55, 40, 90, vector<Move>());
    Pokemon bulbasaur("Bulbasaur", Type::Grass, 100, 100, 49, 49, 45, vector<Move>());

    pikachu.addMove(thunderbolt);
    pikachu.addMove(quickAttack);
    pikachu.addMove(ironTail);

    pikachu.useMove(0, bulbasaur); // Thunderbolt on Bulbasaur
}