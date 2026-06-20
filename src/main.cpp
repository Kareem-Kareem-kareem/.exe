#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

// Game Screens
enum GameScreen { MENU, INFILTRATION, OWN_RESTAURANT, SHOP };

// Recipe Structure
struct Recipe {
    std::string name;
    std::string difficulty;
    int payout;
};

// Main Game State Variables
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
int selectedDifficulty = 1; // 1 = Easy, 2 = Normal, 3 = Hard

// Helper to handle simple button clicking
bool DrawButton(Rectangle rect, const char* text, Color baseColor, Color textColor) {
    Vector2 mousePos = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mousePos, rect);
    Color drawColor = hovered ? ColorAlpha(baseColor, 0.8f) : baseColor;
    
    DrawRectangleRec(rect, drawColor);
    DrawRectangleLinesEx(rect, 2, DARKGRAY);
    
    int fontSize = 20;
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, rect.x + (rect.width - textWidth)/2, rect.y + (rect.height - fontSize)/2, fontSize, textColor);
    
    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

// Visual layout rendering helper for stacking burger components
void DrawVisualBurger(int x, int startY) {
    int currentY = startY;
    // Walk backward to stack from bottom up visually
    for (int i = (int)activeBurgerLayers.size() - 1; i >= 0; i--) {
        std::string layer = activeBurgerLayers[i];
        if (layer == "Bun Base") {
            DrawRectangleRounded({(float)x - 60, (float)currentY, 120, 20}, 0.3f, 4, MAROON);
            DrawText("BUN BASE", x - 40, currentY + 3, 12, WHITE);
        } 
        else if (layer == "Grilled Patty") {
            DrawRectangle({x - 55, currentY, 110, 18}, DARKBROWN);
            DrawText("PATTY", x - 20, currentY + 2, 12, WHITE);
        } 
        else if (layer == "Melted Cheese") {
            DrawRectangle({x - 58, currentY + 4, 116, 10}, GOLD);
            DrawText("CHEESE", x - 25, currentY, 12, BLACK);
        } 
        else if (layer == "Fresh Tomato") {
            DrawRectangle({x - 50, currentY, 100, 14}, RED);
            DrawText("TOMATO", x - 25, currentY + 1, 12, WHITE);
        } 
        else if (layer == "Ketchup Squirt") {
            DrawRectangle({x - 40, currentY + 5, 80, 8}, RED);
            DrawText("KETCHUP", x - 25, currentY - 2, 10, WHITE);
        } 
        else if (layer == "Bun Top") {
            DrawRectangleRounded({(float)x - 60, (float)currentY - 5, 120, 25}, 0.6f, 4, ORANGE);
            DrawText("BUN TOP", x - 30, currentY + 2, 12, WHITE);
        }
        currentY -= 25; // Shift next ingredient position upward
    }
}

int main() {
    std::srand(std::time(nullptr));
    const int windowWidth = 900;
    const int windowHeight = 650;
    InitWindow(windowWidth, windowHeight, "Steal and Cook - Graphical Simulation");
    SetTargetFPS(60);

    GameScreen currentScreen = MENU;

    while (!WindowShouldClose()) {
        // ════════════════════════════════════════════════════════════════════
        //  UPDATE & GAMEPLAY SCENE LOGIC TRACKERS
        // ════════════════════════════════════════════════════════════════════
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

        // ════════════════════════════════════════════════════════════════════
        //  RENDER GRAPHICS LAYER
        // ════════════════════════════════════════════════════════════════════
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Render standard overhead resources status bar across management screens
        if (currentScreen != MENU) {
            DrawRectangle(0, 0, windowWidth, 45, DARKGRAY);
            DrawText(TextFormat("Gold: $%d", gold), 20, 12, 20, GREEN);
            DrawText(TextFormat("Stage: %d", currentStage), 200, 12, 20, WHITE);
            DrawText(TextFormat("Buns:%d | Patties:%d | Cheese:%d | Tomato:%d", inventoryBuns, inventoryPatties, inventoryCheese, inventoryTomatoes), 400, 14, 16, LIGHTGRAY);
            if (DrawButton({800, 7, 80, 30}, "HUB", REALGRAY, WHITE)) {
                currentScreen = MENU;
            }
        }

        switch (currentScreen) {
            case MENU: {
                DrawRectangle(0, 0, windowWidth, windowHeight, BLANK);
                DrawText("STEAL AND COOK", windowWidth/2 - MeasureText("STEAL AND COOK", 40)/2, 80, 40, FILECOLOR);
                DrawText("No Workers Allowed. Infiltrate competitors, secure recipes, automate production.", windowWidth/2 - MeasureText("No Workers Allowed. Infiltrate competitors, secure recipes, automate production.", 16)/2, 140, 16, GRAY);

                if (DrawButton({windowWidth/2 - 150, 240, 300, 50}, "1. INFILTRATE COMPETITORS", DERK), WHITE)) {
                    suspicion = 0.0f;
                    recipePiecesCollected = 0;
                    secretRoomBuilt = false;
                    activeBurgerLayers.clear();
                    
                    // Assign dynamic target recipe name
                    std::vector<std::string> pools = {"Classic Slider", "Bacon Deluxe", "Mega Colossus Tower"};
                    targetRecipeName = pools[std::rand() % 3];
                    currentScreen = INFILTRATION;
                }
                if (DrawButton({windowWidth/2 - 150, 310, 300, 50}, "2. MY RESTAURANT DESK", BLUE, WHITE)) {
                    currentScreen = OWN_RESTAURANT;
                }
                if (DrawButton({windowWidth/2 - 150, 380, 300, 50}, "3. SUPPLY WAREHOUSE SHOP", GOLD, BLACK)) {
                    currentScreen = SHOP;
                }
                if (DrawButton({windowWidth/2 - 150, 470, 300, 45}, "EXIT CORE ENGINE", RED, WHITE)) {
                    CloseWindow();
                    return 0;
                }
            } break;

            case INFILTRATION: {
                DrawText(TextFormat("Targeting Formula Extraction: %s", targetRecipeName.c_str()), 30, 70, 22, DARKBROWN);
                DrawText(TextFormat("Difficulty Configuration: Tier %d", selectedDifficulty), 30, 100, 16, GRAY);

                // Suspicion Meter UI Element
                DrawText("Chef Alert Level:", 30, 140, 16, DARKGRAY);
                DrawRectangle(30, 165, 300, 30, LIGHTGRAY);
                DrawRectangle(30, 165, (int)(3.0f * suspicion), 30, suspicion > 75.0f ? RED : suspicion > 45.0f ? ORANGE : GREEN);
                DrawText(TextFormat("%d %%", (int)suspicion), 160, 171, 18, BLACK);

                // Lab Structure Status Element
                DrawText("Secret Laboratory Module:", 30, 220, 16, DARKGRAY);
                DrawRectangleOutline(30, 245, 300, 60, DARKGRAY, 2);
                if (secretRoomBuilt) {
                    DrawRectangle(32, 247, 296, 56, ColorAlpha(GREEN, 0.2f));
                    DrawText("ROOM OPERATIONAL", 90, 265, 16, DARKGREEN);
                } else {
                    if (DrawButton({40, 252, 280, 45}, "Build Hidden Laboratory ($50)", BLACK, WHITE)) {
                        if (gold >= 50) { gold -= 50; secretRoomBuilt = true; }
                    }
                }

                // Objective Status Display Tracker
                DrawRectangle(30, 340, 300, 80, LIGHTGRAY);
                DrawText("Data Fragments Derived:", 45, 355, 15, DARKGRAY);
                DrawText(TextFormat("%d / 3 Modules Compiled", recipePiecesCollected), 45, 385, 20, BLACK);

                // Cooking assembly area visualization window rendering bounds
                DrawRectangle(420, 80, 430, 520, ColorAlpha(LIGHTGRAY, 0.4f));
                DrawRectangleLines(420, 80, 430, 520, GRAY);
                DrawText("KITCHEN ASSEMBLY STATION", 520, 95, 18, DARKGRAY);

                // Stacking visual rendering handler trigger
                DrawVisualBurger(635, 530);

                // Manual Assembly Addition Buttons Interface 
                if (DrawButton({440, 150, 160, 40}, "+ Add Bun Base", ORANGE, WHITE)) activeBurgerLayers.push_back("Bun Base");
                if (DrawButton({440, 200, 160, 40}, "+ Add Patty", DARKBROWN, WHITE)) activeBurgerLayers.push_back("Grilled Patty");
                if (DrawButton({440, 250, 160, 40}, "+ Add Cheese", GOLD, BLACK)) activeBurgerLayers.push_back("Melted Cheese");
                if (DrawButton({440, 300, 160, 40}, "+ Add Tomato", RED, WHITE)) activeBurgerLayers.push_back("Fresh Tomato");
                if (DrawButton({440, 350, 160, 40}, "+ Add Ketchup", RED, WHITE)) activeBurgerLayers.push_back("Ketchup Squirt");
                if (DrawButton({440, 400, 160, 40}, "+ Add Bun Top", ORANGE, WHITE)) activeBurgerLayers.push_back("Bun Top");

                // Check validation submission rules layout handles
                if (activeBurgerLayers.size() >= 6) {
                    if (DrawButton({440, 480, 220, 45}, "SERVE TO CUSTOMER", GREEN, WHITE)) {
                        suspicion = std::max(0.0f, suspicion - 15.0f);
                        activeBurgerLayers.clear();
                    }
                    if (DrawButton({440, 535, 220, 45}, "SNEAK TO SECRET LAB", PURPLE, WHITE)) {
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

                if (DrawButton({30, 560, 220, 40}, "Abort Mission Line", CHARCOAL, WHITE)) {
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

                        // Manual cooking verification checks
                        if (DrawButton({480, rowY + 20, 160, 40}, "Manual Cook", BLUE, WHITE)) {
                            if (inventoryBuns > 0 && inventoryPatties > 0 && inventoryCheese > 0 && inventoryTomatoes > 0) {
                                inventoryBuns--; inventoryPatties--; inventoryCheese--; inventoryTomatoes--;
                                gold += unlockedRecipes[i].payout;
                            }
                        }
                        
                        // Automated loop validation tracking mechanics
                        if (hasAutoCooker) {
                            if (DrawButton({660, rowY + 20, 160, 40}, "Run Auto Batch", GREEN, WHITE)) {
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

                // Resource Purchase Handles Matrix Row Layouts
                int startItemY = 140;
                auto DrawShopRow = [&](int idx, const char* name, int cost, int& targetInv) {
                    int yPos = startItemY + (idx * 60);
                    DrawText(TextFormat("%s (x5 Bulk pack) - Cost: $%d", name, cost), 40, yPos + 10, 18, BLACK);
                    if (DrawButton({450, (float)yPos, 140, 35}, "Purchase", ORANGE, WHITE)) {
                        if (gold >= cost) { gold -= cost; targetInv += 5; }
                    }
                };

                DrawShopRow(0, "Premium Dough Buns", 10, inventoryBuns);
                DrawShopRow(1, "Fresh Raw Patties", 20, inventoryPatties);
                DrawShopRow(2, "Cheddar Cheese Slices", 15, inventoryCheese);
                DrawShopRow(3, "Organic Tomatoes", 12, inventoryTomatoes);

                // Machine processing terminal modules setup fields
                DrawLine(30, 410, 850, 410, GRAY);
                DrawText("Automation Factory Hardware System upgrades (No Labor Mode):", 30, 430, 18, DARKGRAY);

                if (!hasAutoCooker) {
                    if (DrawButton({30, 470, 400, 50}, "Purchase Auto-Assembly Engine Setup ($100)", RED, WHITE)) {
                        if (gold >= 100) { gold -= 100; hasAutoCooker = true; }
                    }
                } else {
                    DrawText(TextFormat("Auto-Assembly Engine Rig: ONLINE (Processing Level: %d)", autoLevel), 30, 470, 18, GREEN);
                    int upgCost = (autoLevel + 1) * 50;
                    if (DrawButton({30, 510, 400, 45}, TextFormat("Upgrade Conveyor Feed Throughput ($%d)", upgCost), BLACK, WHITE)) {
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
