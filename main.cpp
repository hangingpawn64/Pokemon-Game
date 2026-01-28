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
        Pokemon(string name, Type type, int maxHP, int attack, int defence, int speed)
                : name(name), type(type), maxHP(maxHP), currentHP(maxHP),
                attack(attack), defence(defence), speed(speed) {}


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

class Pikachu : public Pokemon {
public:
    Pikachu()
        : Pokemon("Pikachu", Type::Electric, 100, 55, 40, 90) {
        addMove(Move("Thunderbolt", Type::Electric, 90, 100));
        addMove(Move("Quick Attack", Type::Normal, 40, 100));
        addMove(Move("Iron Tail", Type::Normal, 70, 75));
    }
};

class Bulbasaur : public Pokemon {
public:
    Bulbasaur()
        : Pokemon("Bulbasaur", Type::Grass, 100, 49, 49, 45) {
        addMove(Move("Vine Whip", Type::Grass, 45, 100));
        addMove(Move("Tackle", Type::Normal, 40, 100));
        addMove(Move("Razor Leaf", Type::Grass, 55, 95));
    }
};

class Charmander : public Pokemon {
public:
    Charmander()
        : Pokemon("Charmander", Type::Fire, 100, 52, 43, 65) {
        addMove(Move("Ember", Type::Fire, 40, 100));
        addMove(Move("Scratch", Type::Normal, 40, 100));
        addMove(Move("Flame Burst", Type::Fire, 70, 85));
    }
};

class Squirtle : public Pokemon {
public:
    Squirtle()
        : Pokemon("Squirtle", Type::Water, 100, 48, 65, 43) {
        addMove(Move("Water Gun", Type::Water, 40, 100));
        addMove(Move("Tackle", Type::Normal, 40, 100));
        addMove(Move("Bubble Beam", Type::Water, 65, 95));
    }
};


class Battle{
    private:
        Pokemon& player;
        Pokemon& enemy;

    public:
        Battle(Pokemon& player, Pokemon& enemy) : player(player), enemy(enemy){}

        void run(){
            cout<<"\n Battle Start! ⚔️ 🛡️"<<player.getName()<<" vs "<< enemy.getName()<<endl;

            while(!player.isFainted() && !enemy.isFainted()){
                printStatus();

                if(player.getSpeed() >= enemy.getSpeed()) {
                    playerTurn();
                    if(enemy.isFainted()) break;
                    aiturn();
                }
                else{
                    aiturn();
                    if(player.isFainted()) break;
                    playerTurn();
                }
                cout<<"\n Round Ended\n";
            }

            if(player.isFainted()){
                cout<<player.getName()<<"Fainted! You lost the battle!"<<endl;
                cout<<"\n======== Battle Ended =========\n";
            }
            else{
                cout << enemy.getName() << " fainted! You win!\n"<<endl;
                cout<<"\n======== Battle Ended =========\n";
            }
        }
        void printStatus(){
            cout<<"\n Status: \n";
            cout<<player.getName()<<": "<<player.getCurrentHP()<<"/"<<player.getmaxHP()<<"\n";
            cout<<enemy.getName()<<": "<<enemy.getCurrentHP()<<"/"<<enemy.getmaxHP()<<"\n";
        }
        void playerTurn(){
            cout<<"\n Your Turn: choose a move\n";
            
            const vector<Move>& moves = player.getMoves();
            for(int i = 0; i< (int)moves.size(); i++){
                cout<<i<<") "<<moves[i].name<<" (Power: "<<moves[i].power
                    <<", Acc: "<< moves[i].accuracy<<" , Type: "
                    <<typeToString(moves[i].type)<<" )\n";
            }

            int choice = 0;
            cout<<"Enter choice index: ";
            cin>>choice;

            if(choice < 0 || choice >= moves.size()){
                cout<<"Invalid Choice. using Move 0. \n";
                choice = 0;
            }

            player.useMove(choice, enemy);
        }
        void aiturn(){
            cout<<"\n Enemy Turn: "<< enemy.getName()<<" is choosing a move...\n";
            const vector<Move>& moves = enemy.getMoves();
            int index = rand() % moves.size();
            enemy.useMove(index, player);
        }
};

int main() {
    srand((unsigned)time(0));

    Pikachu pikachu;
    Bulbasaur bulbasaur;

    Battle battle(pikachu, bulbasaur);
    battle.run();

    return 0;
}