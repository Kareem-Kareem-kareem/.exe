#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// ════════════════════════════════════════════════════════════════════
//  Structures & Game State Data Configurations
// ════════════════════════════════════════════════════════════════════

struct StolenRecipe {
    std::string name;
    std::string difficulty;
    int buyPrice;
    int sellPayout;
};

struct Inventory {
    int gold = 150; // Starting money to allow buying materials or rooms
    int buns = 5;
    int patties = 5;
    int cheese = 5;
    int tomatoes = 5;
    int ketchup = 5;
};

struct Automation {
    bool hasAutoCooker = false;
    int autoSpeedLevel = 0; // Upgradable automation parameters
};

class GameEngine {
private:
    Inventory inv;
    Automation autoUpgrades;
    std::vector<StolenRecipe> unlockedRecipes;
    int currentStage = 1;
    bool quitGame = false;

    // Recipe pools split uniquely among difficulties
    std::vector<std::string> easyPool = { "Classic Slider", "Quick Bite Burger", "Junior Stack" };
    std::vector<std::string> normalPool = { "Bacon Deluxe", "Garden Avocado Burger", "Double Melt" };
    std::vector<std::string> hardPool = { "The Mega Colossus", "Trident Triple Tower", "Chef's Masterpiece" };

    void ShowOpeningCutscene() {
        std::cout << "\n================================═════════════════════════\n";
        std::cout << "                  STEAL AND COOK: PROLOGUE                \n";
        std::cout << "================================═════════════════════════\n";
        std::cout << " You open the doors to your dream restaurant...\n";
        std::cout << " But right across the street, the massive Mega-Burger Franchise\n";
        std::cout << " is completely destroying your business. Their food is flawless.\n";
        std::cout << " Your tables are completely empty. Failure is imminent.\n\n";
        std::cout << " Out of desperation, you hatch a devious plan:\n";
        std::cout << " You will infiltrate their kitchens, get hired as a cook,\n";
        std::cout << " sneak their burgers to a secret research center room,\n";
        std::cout << " reverse-engineer their secret formula recipes, and use their\n";
        std::cout << " own menus to automate your restaurant to total glory!\n";
        std::cout << "================================═════════════════════════\n";
        std::cout << " Press Enter to begin your journey of corporate espionage... ";
        std::cin.get();
    }

    void HandleSafeInput(int& choice) {
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            choice = -1;
        }
    }

    void RunShop() {
        while (true) {
            std::cout << "\n══════════════════════════════════════════════════\n";
            std::cout << "                  RESTAURANT SHOP                  \n";
            std::cout << "══════════════════════════════════════════════════\n";
            std::cout << " Current Gold: $" << inv.gold << "\n\n";
            std::cout << " Materials:\n";
            std::cout << " 1. Buy Buns (x5)       - $10  (You have: " << inv.buns << ")\n";
            std::cout << " 2. Buy Patties (x5)    - $20  (You have: " << inv.patties << ")\n";
            std::cout << " 3. Buy Cheese (x5)     - $15  (You have: " << inv.cheese << ")\n";
            std::cout << " 4. Buy Tomatoes (x5)   - $12  (You have: " << inv.tomatoes << ")\n";
            std::cout << " 5. Buy Ketchup (x5)    - $8   (You have: " << inv.ketchup << ")\n\n";
            std::cout << " Upgrades (No Workers Allowed - Tech Automation Only):\n";
            std::cout << " 6. Install Auto-Cooker Machine   - $100 [Status: " << (autoUpgrades.hasAutoCooker ? "OWNED" : "LOCKED") << "]\n";
            if (autoUpgrades.hasAutoCooker) {
                std::cout << " 7. Upgrade Machine Processing   - $" << (autoUpgrades.autoSpeedLevel + 1) * 50 << " (Level " << autoUpgrades.autoSpeedLevel << ")\n";
            }
            std::cout << " 8. Exit Shop\n";
            std::cout << " Choice: ";

            int choice;
            HandleSafeInput(choice);

            if (choice == 1 && inv.gold >= 10) { inv.gold -= 10; inv.buns += 5; }
            else if (choice == 2 && inv.gold >= 20) { inv.gold -= 20; inv.patties += 5; }
            else if (choice == 3 && inv.gold >= 15) { inv.gold -= 15; inv.cheese += 5; }
            else if (choice == 4 && inv.gold >= 12) { inv.gold -= 12; inv.tomatoes += 5; }
            else if (choice == 5 && inv.gold >= 8) { inv.gold -= 8; inv.ketchup += 5; }
            else if (choice == 6 && !autoUpgrades.hasAutoCooker) {
                if (inv.gold >= 100) { inv.gold -= 100; autoUpgrades.hasAutoCooker = true; std::cout << " Auto-Cooker system active!\n"; }
                else { std::cout << " Not enough gold!\n"; }
            }
            else if (choice == 7 && autoUpgrades.hasAutoCooker) {
                int cost = (autoUpgrades.autoSpeedLevel + 1) * 50;
                if (inv.gold >= cost) { inv.gold -= cost; autoUpgrades.autoSpeedLevel++; std::cout << " Automation efficiency increased!\n"; }
                else { std::cout << " Not enough gold!\n"; }
            }
            else if (choice == 8) {
                break;
            } else {
                std::cout << " Invalid selection or insufficient funds.\n";
            }
        }
    }

    void InfiltrateMission() {
        std::cout << "\n--- SELECT TARGET INFILTRATION DIFFICULTY ---\n";
        std::cout << "1. Easy   (Slower suspicion gain rate, unique reward tier)\n";
        std::cout << "2. Normal (Standard suspicion gain rate, unique reward tier)\n";
        std::cout << "3. Hard   (Fast suspicion gain rate, supreme reward tier)\n";
        std::cout << "4. Cancel Mission\n";
        std::cout << "Choice: ";
        
        int diffChoice;
        HandleSafeInput(diffChoice);
        if (diffChoice < 1 || diffChoice > 3) return;

        std::string diffStr = (diffChoice == 1) ? "Easy" : (diffChoice == 2) ? "Normal" : "Hard";
        int baseSusRate = diffChoice * 15; // Higher difficulty means higher suspicion speed increases

        // Pick target recipe name from designated tier pools
        std::string targetRecipeName;
        if (diffChoice == 1) targetRecipeName = easyPool[std::rand() % easyPool.size()];
        else if (diffChoice == 2) targetRecipeName = normalPool[std::rand() % normalPool.size()];
        else targetRecipeName = hardPool[std::rand() % hardPool.size()];

        bool researchRoomBuilt = false;
        int recipePieces = 0;
        int suspicionRate = 0;

        std::cout << "\n Infiltrating competitor kitchen on " << diffStr << " difficulty...\n";
        std::cout << " Objective: Reverse engineer: \"" << targetRecipeName << "\"\n";

        while (recipePieces < 3 && suspicionRate < 100) {
            std::cout << "\n================================═════════════════════════\n";
            std::cout << "                 COMPETITOR KITCHEN WORKPLACE            \n";
            std::cout << "================================═════════════════════════\n";
            std::cout << " Danger State Tracker: Suspicion Level: [" << suspicionRate << "%]\n";
            std::cout << " Formula Data Gathered: [" << recipePieces << "/3 Pieces Collected]\n";
            std::cout << " Research Laboratory Module: " << (researchRoomBuilt ? "Constructed & Functional" : "NOT BUILT YET (Requires $50 construction allocation)") << "\n\n";
            
            std::cout << " Competitor cooks around you are frying patties and slicing fresh tomatoes...\n";
            std::cout << " 1. Build Secret Research Room ($50)\n";
            std::cout << " 2. Start Assembling Target Recipe Burger\n";
            std::cout << " 3. Abandon Mission and Escape\n";
            std::cout << " Choice: ";

            int action;
            HandleSafeInput(action);

            if (action == 1) {
                if (researchRoomBuilt) {
                    std::cout << " Secret room is already built hidden behind the pantry walls!\n";
                } else if (inv.gold >= 50) {
                    inv.gold -= 50;
                    researchRoomBuilt = true;
                    std::cout << " You paid off-the-books builders. The secret research space is built!\n";
                } else {
                    std::cout << " You don't have enough personal funds to hide your setup here yet.\n";
                }
            }
            else if (action == 2) {
                // Realistic multi-tier cooking phase
                std::cout << "\n Preparing the order... Let's stack the ingredients!\n";
                std::vector<std::string> steps = { "Bun Base", "Grilled Patty", "Melted Cheese", "Fresh Sliced Tomato", "Ketchup squirt", "Bun Top" };
                bool cookSuccess = true;

                for (const auto& step : steps) {
                    std::cout << " Add " << step << "? (1: Yes, 2: No/Ruined): ";
                    int buildChoice;
                    HandleSafeInput(buildChoice);
                    if (buildChoice != 1) {
                        cookSuccess = false;
                        std::cout << " You dropped the burger items on the floor! Attempt failed.\n";
                        break;
                    }
                }

                if (!cookSuccess) continue;

                std::cout << "\n The premium burger stack is perfectly prepared!\n";
                std::cout << " Where are you taking it?\n";
                std::cout << " 1. Deliver to waiting line customer (Reduces suspicion by 10%)\n";
                std::cout << " 2. Sneak into the Secret Research Room\n";
                std::cout << " Choice: ";
                
                int deliveryChoice;
                HandleSafeInput(deliveryChoice);

                if (deliveryChoice == 1) {
                    std::cout << " Customer served nicely. Competitor cooks think you are hard at work!\n";
                    suspicionRate = std::max(0, suspicionRate - 10);
                }
                else if (deliveryChoice == 2) {
                    if (!researchRoomBuilt) {
                        std::cout << " You don't have a safe room built yet! You scrambled back out to the main lines in a panic.\n";
                        continue;
                    }
                    
                    // Risk element: High difficulty increases risk scaling
                    int checkChance = std::rand() % 100;
                    if (checkChance < (40 + baseSusRate)) {
                        suspicionRate += baseSusRate;
                        std::cout << " Warning! A line cook watched you walk towards the backup room without serving a customer!\n";
                        std::cout << " Suspicion grew by " << baseSusRate << "%.\n";
                    } else {
                        std::cout << " Clean exit. Nobody noticed your movement!\n";
                    }

                    recipePieces++;
                    std::cout << " Successfully analyzed a piece of the composition structure! (" << recipePieces << "/3)\n";
                }
            }
            else if (action == 3) {
                std::cout << " You slipped out the back window. Mission cancelled safely.\n";
                return;
            }
        }

        if (suspicionRate >= 100) {
            std::cout << "\n CRITICAL FAILURE: The Head Chef caught you red-handed copying technical specs.\n";
            std::cout << " You were fired instantly and kicked out onto the street! All collected data lost.\n";
        }
        else if (recipePieces >= 3) {
            std::cout << "\n VICTORY! You completely reverse-engineered the recipe for: " << targetRecipeName << "!\n";
            
            // Generate payout and entry metrics based on dynamic values
            int dynamicPayout = 30 + (diffChoice * 25);
            StolenRecipe newRecipe = { targetRecipeName, diffStr, 1, dynamicPayout };
            unlockedRecipes.push_back(newRecipe);
            
            std::cout << " This menu selection is now available to execute at your own home restaurant!\n";
            currentStage++;
        }
    }

    void RunOwnRestaurant() {
        std::cout << "\n══════════════════════════════════════════════════\n";
        std::cout << "                YOUR FLAVOR DINER CONSOLE          \n";
        std::cout << "══════════════════════════════════════════════════\n";

        if (unlockedRecipes.empty()) {
            std::cout << " You don't have any stolen recipes to cook yet!\n";
            std::cout << " Go infiltrate the competitors to secure menu files first.\n";
            return;
        }

        std::cout << " Active Stolen Formulations Available:\n";
        for (size_t i = 0; i < unlockedRecipes.size(); ++i) {
            std::cout << " " << i + 1 << ". " << unlockedRecipes[i].name << " [" << unlockedRecipes[i].difficulty << " Tier Payout: $" << unlockedRecipes[i].sellPayout << "]\n";
        }
        std::cout << " Choice of recipe to deploy: ";
        int recChoice;
        HandleSafeInput(recChoice);
        if (recChoice < 1 || recChoice > (int)unlockedRecipes.size()) return;

        StolenRecipe activeRec = unlockedRecipes[recChoice - 1];

        std::cout << "\n Execution Mode Settings:\n";
        std::cout << " 1. Run Manual Production Assembly Line Run\n";
        std::cout << " 2. Run Automated Machinery Batch Processing Loops\n";
        std::cout << " Choice: ";
        int runChoice;
        HandleSafeInput(runChoice);

        if (runChoice == 1) {
            // Check component inventory availability before running manual recipe items
            if (inv.buns > 0 && inv.patties > 0 && inv.cheese > 0 && inv.tomatoes > 0 && inv.ketchup > 0) {
                inv.buns--; inv.patties--; inv.cheese--; inv.tomatoes--; inv.ketchup--;
                inv.gold += activeRec.sellPayout;
                std::cout << " You compiled the ingredients safely and served it. Earned $" << activeRec.sellPayout << " gold!\n";
            } else {
                std::cout << " Missing necessary raw materials inventory ingredients! Visit the shop to replenish stock counts.\n";
            }
        }
        else if (runChoice == 2) {
            if (!autoUpgrades.hasAutoCooker) {
                std::cout << " Setup processing unit locked! Go purchase the Auto-Cooker terminal from the shop first.\n";
                return;
            }

            int batchRuns = 1 + autoUpgrades.autoSpeedLevel;
            int successfulRuns = 0;

            for (int i = 0; i < batchRuns; ++i) {
                if (inv.buns > 0 && inv.patties > 0 && inv.cheese > 0 && inv.tomatoes > 0 && inv.ketchup > 0) {
                    inv.buns--; inv.patties--; inv.cheese--; inv.tomatoes--; inv.ketchup--;
                    inv.gold += activeRec.sellPayout;
                    successfulRuns++;
                } else {
                    break;
                }
            }

            if (successfulRuns > 0) {
                std::cout << " Automated cycle executed " << successfulRuns << " iterations seamlessly.\n";
                std::cout << " Accumulated total gain of $" << (successfulRuns * activeRec.sellPayout) << " gold into accounts!\n";
            } else {
                std::cout << " Automated line stalled immediately due to total resource depletion.\n";
            }
        }
    }

public:
    void Start() {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        ShowOpeningCutscene();

        while (!quitGame) {
            std::cout << "\n================================═════════════════════════\n";
            std::cout << "           MAIN HUB: STEAL & COOK MANAGEMENT TERMINAL     \n";
            std::cout << "================================═════════════════════════\n";
            std::cout << " Current Operational Funds: $" << inv.gold << " | Current Campaign Tier: Stage " << currentStage << "\n";
            std::cout << " Storage Vault Stock: Buns: " << inv.buns << " | Patties: " << inv.patties << " | Cheese: " << inv.cheese << " | Tomatoes: " << inv.tomatoes << " | Ketchup: " << inv.ketchup << "\n";
            std::cout << " Automation Rig Status: " << (autoUpgrades.hasAutoCooker ? "Online (Level " + std::to_string(autoUpgrades.autoSpeedLevel) + ")" : "Offline") << "\n";
            std::cout << " Formulas Inverted: " << unlockedRecipes.size() << " unlocked\n";
            std::cout << "---------------------------------------------------------\n";
            std::cout << " 1. Go Infiltrate Competitor Kitchen (Steal Recipes)\n";
            std::cout << " 2. Access Management Desk (Run / Automate Your Restaurant)\n";
            std::cout << " 3. Visit Supplier Warehouse Shop (Buy Upgrades & Materials)\n";
            std::cout << " 4. Close Game Session\n";
            std::cout << " Choose Operation Code: ";

            int selection;
            HandleSafeInput(selection);

            switch (selection) {
                case 1: InfiltrateMission(); break;
                case 2: RunOwnRestaurant(); break;
                case 3: RunShop(); break;
                case 4: quitGame = true; break;
                default: std::cout << " Invalid control loop parameter target. Try again.\n"; break;
            }
        }
        std::cout << "\n Exiting game core terminal. Saving data configs... Done.\n";
    }
};

int main() {
    GameEngine game;
    game.Start();
    return 0;
}
