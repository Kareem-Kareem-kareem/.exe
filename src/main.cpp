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

struct Station {
    Rectangle rect;
    std::string label;
    Color color;
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

Vector2 playerPos = { 100.0f, 300.0f };
float playerRadius = 15.0f;
float playerSpeed = 220.0f;

Vector2 cookPos = { 400.0f, 200.0f };
float cookSpeed = 120.0f;
bool cookMovingDown = true;

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
            DrawRectangleRounded(Rectangle{(float)x - 40.0f, (float)currentY, 80.0f, 14.0f}, 0.3f, 4, MAROON);
        } 
        else if (layer == "Grilled Patty") {
            DrawRectangleRec(Rectangle{(float)x - 36.0f, (float)currentY, 72.0f, 12.0f}, DARKBROWN);
        } 
        else if (layer == "Melted Cheese") {
            DrawRectangleRec(Rectangle{(float)x - 38.0f, (float)currentY + 2.0f, 76.0f, 8.0f}, GOLD);
        } 
        else if (layer == "Fresh Tomato") {
            DrawRectangleRec(Rectangle{(float)x - 34.0f, (float)currentY, 68.0f, 10.0f}, RED);
        } 
        else if (layer == "Ketchup Squirt") {
            DrawRectangleRec(Rectangle{(float)x - 30.0f, (float)currentY + 3.0f, 60.0f, 6.0f}, RED);
        } 
        else if (layer == "Bun Top") {
            DrawRectangleRounded(Rectangle{(float)x - 40.0f, (float)currentY - 3.0f, 80.0f, 16.0f}, 0.6f, 4, ORANGE);
        }
        currentY -= 16; 
    }
}

int main() {
    std::srand((unsigned int)std::time(nullptr));
    const int windowWidth = 950;
    const int windowHeight = 650;
    InitWindow(windowWidth, windowHeight, "Steal and Cook - 2D Top-Down Simulation");
    SetTargetFPS(60);

    GameScreen currentScreen = MENU;

    std::vector<Station> stations = {
        { { 500, 80, 80, 60 }, "Bun Base", ORANGE },
        { { 600, 80, 80, 60 }, "Patty", DARKBROWN },
        { { 700, 80, 80, 60 }, "Cheese", GOLD },
        { { 500, 160, 80, 60 }, "Tomato", RED },
        { { 600, 160, 80, 60 }, "Ketchup", MAROON },
        { { 700, 160, 80, 60 }, "Bun Top", ORANGE },
        { { 550, 320, 160, 60 }, "CUSTOMER COUNTER", GREEN }
    };

    Station secretLabStation = { { 50, 500, 140, 70 }, "SECRET LAB", PURPLE };

    float interactCooldown = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (interactCooldown > 0.0f) interactCooldown -= dt;

        switch (currentScreen) {
            case INFILTRATION: {
                if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) playerPos.y -= playerSpeed * dt;
                if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) playerPos.y += playerSpeed * dt;
                if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) playerPos.x -= playerSpeed * dt;
                if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) playerPos.x += playerSpeed * dt;

                if (playerPos.x < playerRadius) playerPos.x = playerRadius;
                if (playerPos.x > 820.0f - playerRadius) playerPos.x = 820.0f - playerRadius;
                if (playerPos.y < 65.0f + playerRadius) playerPos.y = 65.0f + playerRadius;
                if (playerPos.y > windowHeight - playerRadius) playerPos.y = windowHeight - playerRadius;

                if (cookMovingDown) {
                    cookPos.y += cookSpeed * dt;
                    if (cookPos.y > 450.0f) cookMovingDown = false;
                } else {
                    cookPos.y -= cookSpeed * dt;
                    if (cookPos.y < 120.0f) cookMovingDown = true;
                }

                if (interactCooldown <= 0.0f && IsKeyPressed(KEY_SPACE)) {
                    for (size_t i = 0; i < stations.size() - 1; i++) {
                        if (CheckCollisionCircleRec(playerPos, playerRadius, stations[i].rect)) {
                            activeBurgerLayers.push_back(stations[i].label);
                            interactCooldown = 0.3f;
                            break;
                        }
                    }

                    if (CheckCollisionCircleRec(playerPos, playerRadius, stations.back().rect)) {
                        if (activeBurgerLayers.size() >= 6) {
                            suspicion = std::max(0.0f, suspicion - 15.0f);
                            activeBurgerLayers.clear();
                            interactCooldown = 0.5f;
                        }
                    }

                    if (secretRoomBuilt && CheckCollisionCircleRec(playerPos, playerRadius, secretLabStation.rect)) {
                        if (activeBurgerLayers.size() >= 6) {
                            recipePiecesCollected++;
                            float cookDist = Vector2Distance(playerPos, cookPos);
                            if (cookDist < 250.0f) {
                                suspicion += (20.0f * selectedDifficulty);
                            } else {
                                suspicion += (5.0f * selectedDifficulty);
                            }
                            activeBurgerLayers.clear();
                            interactCooldown = 0.5f;
                        }
                    }
                }

                if (!secretRoomBuilt && CheckCollisionCircleRec(playerPos, playerRadius, secretLabStation.rect)) {
                    if (IsKeyPressed(KEY_SPACE) && gold >= 50 && interactCooldown <= 0.0f) {
                        gold -= 50;
                        secretRoomBuilt = true;
                        interactCooldown = 0.5f;
                    }
                }

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
            DrawText(TextFormat("Stage: %d", currentStage), 160, 12, 20, WHITE);
            DrawText(TextFormat("Buns:%d | Patties:%d | Cheese:%d | Tomato:%d", inventoryBuns, inventoryPatties, inventoryCheese, inventoryTomatoes), 310, 14, 16, LIGHTGRAY);
            if (DrawButton(Rectangle{850.0f, 7.0f, 80.0f, 30.0f}, "HUB", GRAY, WHITE)) {
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
                    playerPos = Vector2{ 150.0f, 300.0f };
                    
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
                DrawRectangle(0, 45, 820, windowHeight - 45, ColorAlpha(LIGHTGRAY, 0.2f));
                DrawRectangle(820, 45, 130, windowHeight - 45, DARKGRAY);

                DrawText("BURGER", 835, 60, 16, LIGHTGRAY);
                DrawVisualBurger(885, 450);
                DrawText(TextFormat("Pieces: %d/3", recipePiecesCollected), 830, 520, 14, WHITE);
                DrawText(TextFormat("Alert: %d%%", (int)suspicion), 830, 550, 14, suspicion > 70.0f ? RED : ORANGE);

                DrawText("CONTROLS: WASD / ARROWS to Move | SPACE to Interact at Tables", 40, 620, 15, DARKGRAY);
                DrawText(TextFormat("Targeting: %s", targetRecipeName.c_str()), 40, 60, 16, DARKBROWN);

                for (const auto& st : stations) {
                    DrawRectangleRec(st.rect, st.color);
                    DrawRectangleLinesEx(st.rect, 2.0f, BLACK);
                    DrawText(st.label.c_str(), (int)st.rect.x + 6, (int)st.rect.y + 20, 11, BLACK);
                }

                if (secretRoomBuilt) {
                    DrawRectangleRec(secretLabStation.rect, secretLabStation.color);
                    DrawRectangleLinesEx(secretLabStation.rect, 2.0f, BLACK);
                    DrawText("SECRET LAB", (int)secretLabStation.rect.x + 20, (int)secretLabStation.rect.y + 25, 14, WHITE);
                } else {
                    DrawRectangleRec(secretLabStation.rect, GRAY);
                    DrawRectangleLinesEx(secretLabStation.rect, 2.0f, DASHEDLINE);
                    DrawText("BUILD LAB [SPACE]", (int)secretLabStation.rect.x + 10, (int)secretLabStation.rect.y + 18, 11, BLACK);
                    DrawText("Cost: $50", (int)secretLabStation.rect.x + 35, (int)secretLabStation.rect.y + 40, 11, DARKGRAY);
                }

                DrawCircleV(cookPos, 16.0f, RED);
                DrawCircleLines((int)cookPos.x, (int)cookPos.y, 16.0f, BLACK);
                DrawText("COOK", (int)cookPos.x - 15, (int)cookPos.y - 5, 10, WHITE);

                DrawCircleV(playerPos, playerRadius, BLUE);
                DrawCircleLines((int)playerPos.x, (int)playerPos.y, playerRadius, BLACK);
                DrawText("YOU", (int)playerPos.x - 10, (int)playerPos.y - 5, 10, WHITE);

                if (DrawButton(Rectangle{710.0f, 580.0f, 100.0f, 35.0f}, "ABORT", BLACK, WHITE)) {
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
                        DrawRectangle(30, rowY, 890, 80, LIGHTGRAY);
                        DrawText(unlockedRecipes[i].name.c_str(), 50, rowY + 15, 20, BLACK);
                        DrawText(TextFormat("Tier: %s | Payout Base Value: $%d", unlockedRecipes[i].difficulty.c_str(), unlockedRecipes[i].payout), 50, rowY + 45, 14, DARKGRAY);

                        if (DrawButton(Rectangle{530.0f, (float)rowY + 20.0f, 160.0f, 40.0f}, "Manual Cook", BLUE, WHITE)) {
                            if (inventoryBuns > 0 && inventoryPatties > 0 && inventoryCheese > 0 && inventoryTomatoes > 0) {
                                inventoryBuns--; inventoryPatties--; inventoryCheese--; inventoryTomatoes--;
                                gold += unlockedRecipes[i].payout;
                            }
                        }
                        
                        if (hasAutoCooker) {
                            if (DrawButton(Rectangle{710.0f, (float)rowY + 20.0f, 160.0f, 40.0f}, "Run Auto Batch", GREEN, WHITE)) {
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
                if (DrawButton(Rectangle{520.0f, (float)startItemY, 140.0f, 35.0f}, "Purchase", ORANGE, WHITE)) {
                    if (gold >= 10) { gold -= 10; inventoryBuns += 5; }
                }
                
                DrawText("Fresh Raw Patties (x5 Bulk pack) - Cost: $20", 40, startItemY + 70, 18, BLACK);
                if (DrawButton(Rectangle{520.0f, (float)startItemY + 60.0f, 140.0f, 35.0f}, "Purchase", ORANGE, WHITE)) {
                    if (gold >= 20) { gold -= 20; inventoryPatties += 5; }
                }

                DrawText("Cheddar Cheese Slices (x5 Bulk pack) - Cost: $15", 40, startItemY + 130, 18, BLACK);
                if (DrawButton(Rectangle{520.0f, (float)startItemY + 120.0f, 140.0f, 35.0f}, "Purchase", ORANGE, WHITE)) {
                    if (gold >= 15) { gold -= 15; inventoryCheese += 5; }
                }

                DrawText("Organic Tomatoes (x5 Bulk pack) - Cost: $12", 40, startItemY + 190, 18, BLACK);
                if (DrawButton(Rectangle{520.0f, (float)startItemY + 180.0f, 140.0f, 35.0f}, "Purchase", ORANGE, WHITE)) {
                    if (gold >= 12) { gold -= 12; inventoryTomatoes += 5; }
                }

                DrawLine(30, 410, 910, 410, GRAY);
                DrawText("Automation Factory Hardware System upgrades (No Labor Mode):", 30, 430, 18, DARKGRAY);

                if (!hasAutoCooker) {
                    if (DrawButton(Rectangle{30.0f, 470.0f, 420.0f, 50.0f}, "Purchase Auto-Assembly Engine Setup ($100)", RED, WHITE)) {
                        if (gold >= 100) { gold -= 100; hasAutoCooker = true; }
                    }
                } else {
                    DrawText(TextFormat("Auto-Assembly Engine Rig: ONLINE (Processing Level: %d)", autoLevel), 30, 470, 18, GREEN);
                    int upgCost = (autoLevel + 1) * 50;
                    if (DrawButton(Rectangle{30.0f, 510.0f, 420.0f, 45.0f}, TextFormat("Upgrade Conveyor Feed Throughput ($%d)", upgCost), BLACK, WHITE)) {
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
