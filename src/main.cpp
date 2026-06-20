#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

struct Player {
    int hp = 100;
    int maxHp = 100;
    int attack = 15;
    int gold = 0;
    int floor = 1;
};

struct Enemy {
    std::string name;
    int hp;
    int attack;
    int goldReward;
};

void battle(Player& player) {
    std::vector<std::string> names = {"Goblin", "Skeleton", "Orc", "Dark Knight"};
    int type = std::rand() % names.size();
    Enemy enemy;
    enemy.name = names[type];
    enemy.hp = 30 + (player.floor * 10);
    enemy.attack = 5 + (player.floor * 3);
    enemy.goldReward = 10 + (std::rand() % 20);

    std::cout << "\nAn enemy " << enemy.name << " appeared on Floor " << player.floor << "!\n";

    while (enemy.hp > 0 && player.hp > 0) {
        std::cout << "\nPlayer HP: " << player.hp << "/" << player.maxHp << " | Enemy HP: " << enemy.hp << "\n";
        std::cout << "1. Attack\n2. Run away\nChoice: ";
        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 1) {
            int pDamage = (std::rand() % player.attack) + 5;
            enemy.hp -= pDamage;
            std::cout << "You hit the " << enemy.name << " for " << pDamage << " damage!\n";

            if (enemy.hp > 0) {
                int eDamage = (std::rand() % enemy.attack) + 2;
                player.hp -= eDamage;
                std::cout << "The " << enemy.name << " hits you for " << eDamage << " damage!\n";
            }
        } else if (choice == 2) {
            if (std::rand() % 2 == 0) {
                std::cout << "You successfully escaped!\n";
                return;
            } else {
                std::cout << "Failed to escape!\n";
                int eDamage = (std::rand() % enemy.attack) + 2;
                player.hp -= eDamage;
                std::cout << "The " << enemy.name << " hits you for " << eDamage << " damage as you flee!\n";
            }
        }
    }

    if (player.hp <= 0) {
        std::cout << "\nYou died! Game Over.\n";
    } else if (enemy.hp <= 0) {
        std::cout << "\nYou defeated the " << enemy.name << " and found " << enemy.goldReward << " gold!\n";
        player.gold += enemy.goldReward;
        player.floor++;
    }
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    Player player;

    std::cout << "=================================\n";
    std::cout << "   WELCOME TO THE TEXT DUNGEON   \n";
    std::cout << "=================================\n";

    while (player.hp > 0) {
        std::cout << "\n--- Current Stats ---\n";
        std::cout << "Floor: " << player.floor << " | HP: " << player.hp << "/" << player.maxHp << " | Gold: " << player.gold << "\n";
        std::cout << "1. Venture Deeper\n2. Visit Shop\n3. Quit\nChoice: ";
        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 1) {
            battle(player);
        } else if (choice == 2) {
            std::cout << "\nWelcome to the shop! Potions cost 15 gold and heal 30 HP.\n";
            std::cout << "1. Buy Potion\n2. Leave\nChoice: ";
            int shopChoice;
            std::cin >> shopChoice;
            if (shopChoice == 1) {
                if (player.gold >= 15) {
                    player.gold -= 15;
                    player.hp = std::min(player.maxHp, player.hp + 30);
                    std::cout << "Healed! Current HP: " << player.hp << "\n";
                } else {
                    std::cout << "Not enough gold!\n";
                }
            }
        } else if (choice == 3) {
            break;
        }
    }

    std::cout << "\nThanks for playing! You reached floor " << player.floor << ".\n";
    std::cout << "Press Enter to close...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
    return 0;
}
