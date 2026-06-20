#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

enum GameScreen { MENU, INFILTRATION, OWN_RESTAURANT, SHOP };

struct Recipe {
    std::string name;
    std::string difficulty;
    int payout;
};

int gold = 150;
int inventoryBuns = 5;
int inventoryPatties = 5;
int inventoryCheese = 5;
int inventoryTomatoes = 5;
int inventoryKetchup = 5;

bool hasAutoCooker = false;
int autoLevel = 0;
int currentStage = 1;

std::vector<Recipe> unlockedRecipes;
std::vector<std::string> activeBurgerLayers; 
float suspicion = 0.0f;
bool secretRoomBuilt = false;
int recipePiecesCollected = 0;
std::string targetRecipeName = "Classic Slider";
int selectedDifficulty = 1; 

bool DrawButton(Rectangle rect, const char* text, Color baseColor, Color textColor) {
    Vector2 mousePos = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mousePos, rect);
    Color drawColor = hovered ? ColorAlpha(baseColor, 0.8f) : baseColor;
    
    DrawRectangleRec(rect, drawColor);
    DrawRectangleLinesEx(rect, 2.0f, DARKGRAY);
    
    int fontSize = 20;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, (int)(rect.x + (rect.width - textWidth)/2), (int)(rect.y + (rect.height - fontSize)/2), fontSize, textColor);
    
    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void DrawVisualBurger(int x, int startY) {
    int currentY = startY;
    for (int i = (int)activeBurgerLayers.size() - 1; i >= 0; i--) {
        std::string layer = activeBurgerLayers[i];
        if (layer == "Bun Base") {
            DrawRectangleRounded(Rectangle{(float)x - 60.0f, (float)currentY, 120.0f, 20.0f}, 0.3f, 4, MAROON);
            DrawText("BUN BASE", x - 40, currentY + 3, 12, WHITE);
        } 
        else if (layer == "Grilled Patty") {
            DrawRectangleRec(Rectangle{(float)x - 55.0f, (float)currentY, 110.0f, 18.0f}, DARKBROWN);
            DrawText("PATTY", x - 20, currentY + 2, 12, WHITE);
        } 
        else if (layer == "Melted Cheese") {
            DrawRectangleRec(Rectangle{(float)x - 58.0f, (float)currentY + 4.0f, 116.0f, 10.0f}, GOLD);
            DrawText("CHEESE", x - 25, currentY, 12, BLACK);
        } 
        else if (layer == "Fresh Tomato") {
            DrawRectangleRec(Rectangle{(float)x - 50.0f, (float)currentY, 100.0f, 14.0f}, RED);
            DrawText("TOMATO", x - 25, currentY + 1, 12, WHITE);
        } 
        else if (layer == "Ketchup Squirt") {
            DrawRectangleRec(Rectangle{(float)x - 40.0f, (float)currentY + 5.0f, 80.0f, 8.0f}, RED);
            DrawText("KETCHUP", x - 25, currentY - 2, 10, WHITE);
        } 
        else if (layer == "Bun Top") {
            DrawRectangleRounded(Rectangle{(float)x - 60.0f, (float)currentY - 5.0f, 120.0f, 25.0f}, 0.6f, 4, ORANGE);
            DrawText("BUN TOP", x - 30, currentY + 2, 12, WHITE);
        }
        currentY -= 25; 
    }
}

int main() {
    std::srand((unsigned int)std::time(nullptr));
    const int windowWidth = 900;
    const int windowHeight = 650;
    InitWindow(windowWidth, windowHeight, "Steal and Cook - Graphical Simulation");
    SetTargetFPS(60);

    GameScreen currentScreen = MENU;

    while (!WindowShouldClose()) {
        switch (currentScreen) {
            case INFILTRATION: {
                if (suspicion >= 100.0f) {
                    activeBurgerLayers.clear();
                    suspicion = 0.0f;
                    secretRoomBuilt = false;
                    recipePiecesCollected = 0;
                    currentScreen = MENU;
                }
                if (recipePiecesCollected >= 3) {
                    unlockedRecipes.push_back({targetRecipeName, (selectedDifficulty == 1 ? "Easy" : selectedDifficulty == 2 ? "Normal" : "Hard"), 30 + (selectedDifficulty * 25)});
                    activeBurgerLayers.clear();
                    suspicion = 0.0f;
                    secretRoomBuilt = false;
                    recipePiecesCollected = 0;
                    currentStage++;
                    currentScreen = MENU;
                }
            } break;
            default: break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentScreen != MENU) {
            DrawRectangle(0, 0, windowWidth, 45, DARKGRAY);
            DrawText(TextFormat("Gold: $%d", gold), 20, 12, 20, GREEN);
            DrawText(TextFormat("Stage: %d", currentStage), 200, 12, 20, WHITE);
            DrawText(TextFormat("Buns:%d | Patties:%d | Cheese:%d | Tomato:%d", inventoryBuns, inventoryPatties, inventoryCheese, inventoryTomatoes), 400, 14, 16, LIGHTGRAY);
            if (DrawButton(Rectangle{800.0f, 7.0f, 80.0f, 30.0f}, "HUB", GRAY, WHITE)) {
                currentScreen = MENU;
            }
        }

        switch (currentScreen) {
            case MENU: {
                DrawText("STEAL AND COOK", windowWidth/2 - MeasureText("STEAL AND COOK", 40)/2, 80, 40, RED);
                DrawText("No Workers Allowed. Infiltrate competitors, secure recipes, automate production.", windowWidth/2 - MeasureText("No Workers Allowed. Infiltrate competitors, secure recipes, automate production.", 16)/2, 140, 16, GRAY);

                if (DrawButton(Rectangle{(float)windowWidth/2 - 150.0f, 240.0f, 300.0f, 50.0f}, "1. INFILTRATE COMPETITORS", DARKGRAY, WHITE)) {
                    suspicion = 0.0f;
                    recipePiecesCollected = 0;
                    secretRoomBuilt = false;
                    activeBurgerLayers.clear();
                    
                    std::vector<std::string> pools = {"Classic Slider", "Bacon Deluxe", "Mega Colossus Tower"};
                    targetRecipeName = pools[std::rand() % 3];
                    currentScreen = INFILTRATION;
                }
                if (DrawButton(Rectangle{(float)windowWidth/2 - 150.0f, 310.0f, 300.0f, 50.0f}, "2. MY RESTAURANT DESK", BLUE, WHITE)) {
                    currentScreen = OWN_RESTAURANT;
                }
                if (DrawButton(Rectangle{(float)windowWidth/2 - 150.0f, 380.0f, 300.0f, 50.0f}, "3. SUPPLY WAREHOUSE SHOP", GOLD, BLACK)) {
                    currentScreen = SHOP;
                }
                if (DrawButton(Rectangle{(float)windowWidth/2 - 150.0f, 470.0f, 300.0f, 45.0f}, "EXIT CORE ENGINE", RED, WHITE)) {
                    CloseWindow();
                    return 0;
                }
            } break;

            case INFILTRATION: {
                DrawText(TextFormat("Targeting Formula Extraction: %s", targetRecipeName.c_str()), 30, 70, 22, DARKBROWN);
                DrawText(TextFormat("Difficulty Configuration: Tier %d", selectedDifficulty), 30, 100, 16, GRAY);

                DrawText("Chef Alert Level:", 30, 140, 16, DARKGRAY);
                DrawRectangle(30, 165, 300, 30, LIGHTGRAY);
                DrawRectangle(30, 165, (int)(3.0f * suspicion), 30, suspicion > 75.0f ? RED : suspicion > 45.0f ? ORANGE : GREEN);
                DrawText(TextFormat("%d %%", (int)suspicion), 160, 171, 18, BLACK);

                DrawText("Secret Laboratory Module:", 30, 220, 16, DARKGRAY);
                DrawRectangleLines(30, 245, 300, 60, DARKGRAY);
                if (secretRoomBuilt) {
                    DrawRectangle(32, 247, 296, 56, ColorAlpha(GREEN, 0.2f));
                    DrawText("ROOM OPERATIONAL", 90, 265, 16, DARKGREEN);
                } else {
                    if (DrawButton(Rectangle{40.0f, 252.0f, 280.0f, 45.0f}, "Build Hidden Laboratory ($50)", BLACK, WHITE)) {
                        if (gold >= 50) { gold -= 50; secretRoomBuilt = true; }
                    }
                }

                DrawRectangle(30, 340, 300, 80, LIGHTGRAY);
                DrawText("Data Fragments Derived:", 45, 355, 15, DARKGRAY);
                DrawText(TextFormat("%d / 3 Modules Compiled", recipePiecesCollected), 45, 385, 20, BLACK);

                DrawRectangle(420, 80, 430, 520, ColorAlpha(LIGHTGRAY, 0.4f));
                DrawRectangleLines(420, 80, 430, 520, GRAY);
                DrawText("KITCHEN ASSEMBLY STATION", 520, 95, 18, DARKGRAY);

                DrawVisualBurger(635, 530);

                if (DrawButton(Rectangle{440.0f, 150.0f, 160.0f, 40.0f}, "+ Add Bun Base", ORANGE, WHITE)) activeBurgerLayers.push_back("Bun Base");
                if (DrawButton(Rectangle{440.0f, 200.0f, 160.0f, 40.0f}, "+ Add Patty", DARKBROWN, WHITE)) activeBurgerLayers.push_back("Grilled Patty");
                if (DrawButton(Rectangle{440.0f, 250.0f, 160.0f, 40.0f}, "+ Add Cheese", GOLD, BLACK)) activeBurgerLayers.push_back("Melted Cheese");
                if (DrawButton(Rectangle{440.0f, 300.0f, 160.0f, 40.0f}, "+ Add Tomato", RED, WHITE)) activeBurgerLayers.push_back("Fresh Tomato");
                if (DrawButton(Rectangle{440.0f, 350.0f, 160.0f, 40.0f}, "+ Add Ketchup", RED, WHITE)) activeBurgerLayers.push_back("Ketchup Squirt");
                if (DrawButton(Rectangle{440.0f, 400.0f, 160.0f, 40.0f}, "+ Add Bun Top", ORANGE, WHITE)) activeBurgerLayers.push_back("Bun Top");

                if (activeBurgerLayers.size() >= 6) {
                    if (DrawButton(Rectangle{440.0f, 480.0f, 220.0f, 45.0f}, "SERVE TO CUSTOMER", GREEN, WHITE)) {
                        suspicion = std::max(0.0f, suspicion - 15.0f);
                        activeBurgerLayers.clear();
                    }
                    if (DrawButton(Rectangle{440.0f, 535.0f, 220.0f, 45.0f}, "SNEAK TO SECRET LAB", PURPLE, WHITE)) {
                        if (!secretRoomBuilt) {
                            suspicion += 25.0f;
                        } else {
                            recipePiecesCollected++;
                            if ((std::rand() % 100) < (30 + (selectedDifficulty * 15))) {
                                suspicion += (20.0f * selectedDifficulty);
                            }
                        }
                        activeBurgerLayers.clear();
                    }
                }

                if (DrawButton(Rectangle{30.0f, 560.0f, 220.0f, 40.0f}, "Abort Mission Line", DARKGRAY, WHITE)) {
                    currentScreen = MENU;
                }
            } break;

            case OWN_RESTAURANT: {
                DrawText("MANAGEMENT DESK OPERATIONS", 30, 70, 26, DARKGRAY);

                if (unlockedRecipes.empty()) {
                    DrawText("You have no stolen recipes available! Infiltrate targets first.", 30, 150, 18, RED);
                } else {
                    DrawText("Stolen Blueprints Online Deployment Selector:", 30, 130, 18, DARKGRAY);
                    
                    for (int i = 0; i < (int)unlockedRecipes.size() && i < 4; i++) {
                        int rowY = 170 + (i * 95);
                        DrawRectangle(30, rowY, 820, 80, LIGHTGRAY);
                        DrawText(unlockedRecipes[i].name.c_str(), 50, rowY + 15, 20, BLACK);
                        DrawText(TextFormat("Tier: %s | Payout Base Value: $%d", unlockedRecipes[i].difficulty.c_str(), unlockedRecipes[i].payout), 50, rowY + 45, 14, DARKGRAY);

                        if (DrawButton(Rectangle{480.0f, (float)rowY + 20.0f, 160.0f, 40.0f}, "Manual Cook", BLUE, WHITE)) {
                            if (inventoryBuns > 0 && inventoryPatties > 0 && inventoryCheese > 0 && inventoryTomatoes > 0) {
                                inventoryBuns--; inventoryPatties--; inventoryCheese--; inventoryTomatoes--;
                                gold += unlockedRecipes[i].payout;
                            }
                        }
                        
                        if (hasAutoCooker) {
                            if (DrawButton(Rectangle{660.0f, (float)rowY + 20.0f, 160.0f, 40.0f}, "Run Auto Batch", GREEN, WHITE)) {
                                int loops = 1 + autoLevel;
                                for (int k = 0; k < loops; k++) {
                                    if (inventoryBuns > 0 && inventoryPatties > 0 && inventoryCheese > 0 && inventoryTomatoes > 0) {
                                        inventoryBuns--; inventoryPatties--; inventoryCheese--; inventoryTomatoes--;
                                        gold += unlockedRecipes[i].payout;
                                    }
                                }
                            }
                        }
                    }
                }
            } break;

            case SHOP: {
                DrawText("MATERIAL SUPPLIER & AUTOMATION RIGS", 30, 70, 26, DARKGRAY);

                int startItemY = 140;
                
                DrawText("Premium Dough Buns (x5 Bulk pack) - Cost: $10", 40, startItemY + 10, 18, BLACK);
                if (DrawButton(Rectangle{450.0f, (float)startItemY, 140.0f, 35.0f}, "Purchase", ORANGE, WHITE)) {
                    if (gold >= 10) { gold -= 10; inventoryBuns += 5; }
                }
                
                DrawText("Fresh Raw Patties (x5 Bulk pack) - Cost: $20", 40, startItemY + 70, 18, BLACK);
                if (DrawButton(Rectangle{450.0f, (float)startItemY + 60.0f, 140.0f, 35.0f}, "Purchase", ORANGE, WHITE)) {
                    if (gold >= 20) { gold -= 20; inventoryPatties += 5; }
                }

                DrawText("Cheddar Cheese Slices (x5 Bulk pack) - Cost: $15", 40, startItemY + 130, 18, BLACK);
                if (DrawButton(Rectangle{450.0f, (float)startItemY + 120.0f, 140.0f, 35.0f}, "Purchase", ORANGE, WHITE)) {
                    if (gold >= 15) { gold -= 15; inventoryCheese += 5; }
                }

                DrawText("Organic Tomatoes (x5 Bulk pack) - Cost: $12", 40, startItemY + 190, 18, BLACK);
                if (DrawButton(Rectangle{450.0f, (float)startItemY + 180.0f, 140.0f, 35.0f}, "Purchase", ORANGE, WHITE)) {
                    if (gold >= 12) { gold -= 12; inventoryTomatoes += 5; }
                }

                DrawLine(30, 410, 850, 410, GRAY);
                DrawText("Automation Factory Hardware System upgrades (No Labor Mode):", 30, 430, 18, DARKGRAY);

                if (!hasAutoCooker) {
                    if (DrawButton(Rectangle{30.0f, 470.0f, 400.0f, 50.0f}, "Purchase Auto-Assembly Engine Setup ($100)", RED, WHITE)) {
                        if (gold >= 100) { gold -= 100; hasAutoCooker = true; }
                    }
                } else {
                    DrawText(TextFormat("Auto-Assembly Engine Rig: ONLINE (Processing Level: %d)", autoLevel), 30, 470, 18, GREEN);
                    int upgCost = (autoLevel + 1) * 50;
                    if (DrawButton(Rectangle{30.0f, 510.0f, 400.0f, 45.0f}, TextFormat("Upgrade Conveyor Feed Throughput ($%d)", upgCost), BLACK, WHITE)) {
                        if (gold >= upgCost) { gold -= upgCost; autoLevel++; }
                    }
                }
            } break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
