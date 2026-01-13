/*
 * Πανεπιστήμιο: Αριστοτέλειο Πανεπιστήμιο Θεσσαλονίκης
 * Τμήμα: Τμήμα Ηλεκτρολόγων Μηχανικών και Μηχανικών Υπολογιστών
 * Μάθημα: Δομημένος Προγραμματισμός (004)
 * Τίτλος Εργασίας: Delivery Rush
 * Συγγραφείς: 
 * - Αντώνιος Καραφώτης (ΑΕΜ: 11891)
 * - Νικόλαος Αμοιρίδης (ΑΕΜ: 11836)
 * Άδεια Χρήσης: MIT License
 * (Δείτε το αρχείο LICENSE.txt για το πλήρες κείμενο)
 */

#include <time.h>
#include <stdio.h> // Required for sprintf
#include "raylib.h"
#include "raymath.h"
#include "helpers.h"
#include "drawTextures.h"

// --- GAME CONSTANTS ---
const int INITIAL_WINDOW_WIDTH = 1300; 
const int INITIAL_WINDOW_HEIGHT = 800;
const int DELIVERY_BIKE_RENDER_SIZE = 32;
const int DELIVERY_BIKE_SCALED_SIZE = 20;
const int SPEED_CONSTANT = 2;
const Color BACKGROUND_COLOR = DARKGRAY;

const int MINIMAP_WIDTH = 150;      
const int MINIMAP_HEIGHT = 150;     
const float MINIMAP_ZOOM = 0.3f;    
const int MINIMAP_BORDER = 2;

// --- UI CONSTANTS ---
const int BUTTON_WIDTH = 220;
const int BUTTON_HEIGHT = 50;
const int FONT_SIZE = 20;

// Global State Variables
bool showOrders = false;
int count = 0; // Number of orders completed
float totalMoney = 0.0f;
float difficultyFactor = 0.5f; 

/* Program's main function
Initiates window, loads media (image/sound) and runs game loop using the states Menu, Gameplay and Options
*/
int main(void) {
  
  SetRandomSeed(time(NULL)); 
  
  // This allows the game's internal resolution to update when entering Fullscreen
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
  InitWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "Delivery Rush");
  
  // --- LOAD ASSETS ---
  Texture2D background = LoadTexture("assets/map.jpg"); 
  Image backgroundWithBorders = LoadImage("assets/mapWithBorders.png");
  
  // Analyze map for houses/restaurants
  InitMapLocations(backgroundWithBorders);
  Order currentOrder = CreateNewOrder();
  
  SetTextureFilter(background, TEXTURE_FILTER_POINT);
  
  InitAudioDevice();
  Music backgroundMusic = LoadMusicStream("assets/background_music.mp3");
  Sound horn = LoadSound("assets/horn.mp3");

  // Volume State
  float musicVolume = 0.5f;
  float sfxVolume = 0.3f;
  SetMusicVolume(backgroundMusic, musicVolume);
  SetSoundVolume(horn, sfxVolume);
  
  int mapHeight = background.height;
  int mapWidth = background.width;
  
  SetTargetFPS(60);

  // --- PREPARE TEXTURES ---
  RenderTexture2D deliveryBikeRender = LoadRenderTexture(DELIVERY_BIKE_RENDER_SIZE, DELIVERY_BIKE_RENDER_SIZE);
  DrawDeliveryBike(deliveryBikeRender); 
  
  Rectangle bikeSource = {
    0, 0, deliveryBikeRender.texture.width, -deliveryBikeRender.texture.height
  };
  
  Rectangle deliveryBike = {mapWidth / 2.0f, mapHeight / 2.0f, DELIVERY_BIKE_SCALED_SIZE, DELIVERY_BIKE_SCALED_SIZE};
  
  // --- CAMERAS ---
  Camera2D cam = {0};
  cam.offset = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
  cam.zoom = 3;
  cam.rotation = 0;
  
  Camera2D minimapCam = {0};
  minimapCam.zoom = MINIMAP_ZOOM;
  
  // --- TRAFFIC GENERATION ---
  Vehicle vehicles[MAX_VEHICLES];
  // Passing player pos ensures cars don't spawn on top of you
  vehicleGenerator(MAX_VEHICLES, vehicles, background.height, background.width, backgroundWithBorders, (Vector2){deliveryBike.x, deliveryBike.y});

  RenderTexture2D carTex = LoadRenderTexture(40, 65);
  RenderTexture2D truckTex = LoadRenderTexture(65, 110);
  RenderTexture2D policeTex = LoadRenderTexture(40, 65);
  PrepareCarTexture(carTex);
  PrepareTruckTexture(truckTex);
  PreparePoliceTexture(policeTex);
  
  PlayMusicStream(backgroundMusic);

  // --- GAMEPLAY VARIABLES ---
  GameState currentState = STATE_MENU; // Start at Menu
  int rotation = 0;

  // timer to end run
  const float GAME_DURATION = 240.0f; // 4 Minutes in seconds
  float globalTimer = GAME_DURATION;
  
  OrderStatusMessage message;
  message.messageType = PENDING;
  message.timer = 0;

  float lastReward = 0;
  
  // Respawn / Stuck Logic
  float collisionDuration = 0.0f;
  float respawnTimer = 0.0f;
  bool isRespawning = false;

  bool exitRequest = false;
  bool running = true;

  // --- MAIN LOOP ---
  while (running) {
    UpdateMusicStream(backgroundMusic);

    float screenHeight = (float)GetScreenHeight();
    float screenWidth = (float)GetScreenWidth();

    if (IsKeyPressed(KEY_F)) ToggleFullscreen();

    if (IsWindowFullscreen()) {
        int monitor = GetCurrentMonitor();
        screenHeight = (float)GetMonitorHeight(monitor);
        screenWidth = (float)GetMonitorWidth(monitor);
    }

    if (WindowShouldClose()) {
      exitRequest = true;
    }

    Vector2 arrowPos = {0};
    Vector2 bikePos;
    float angleToTarget = 0.0f;

    // ==========================================
    // LOGIC UPDATES
    // ==========================================

    if (currentState == STATE_GAMEPLAY && !exitRequest) {
        
        globalTimer -= GetFrameTime();
        if (globalTimer <= 0) {
            globalTimer = 0;
            currentState = STATE_GAMEOVER;
        }

        // 1. Traffic & Orders
        updateTraffic(vehicles, MAX_VEHICLES, backgroundWithBorders, (Vector2){deliveryBike.x, deliveryBike.y});
        
        bikePos = (Vector2){ deliveryBike.x, deliveryBike.y };
        updateOrder(&currentOrder, bikePos, &count, &totalMoney, houses, houseCount, &message, &lastReward);
        
        // 2. Arrow Logic
        Vector2 currentTargetPos;
        if (!currentOrder.foodPickedUp) currentTargetPos = currentOrder.pickupLocation;
        else currentTargetPos = currentOrder.dropoffLocation;
        
        angleToTarget = atan2f(currentTargetPos.y - bikePos.y, currentTargetPos.x - bikePos.x);
        float arrowRadius = 45.0f;
        arrowPos = (Vector2){
            bikePos.x + cosf(angleToTarget) * arrowRadius,
            bikePos.y + sinf(angleToTarget) * arrowRadius
        };

        // 3. Movement & Physics
        float horizontalOffset, verticalOffset;

        if (rotation == 90 || rotation == 270) {
            horizontalOffset = deliveryBike.height / 2.0f; 
            verticalOffset = deliveryBike.width / 3.5f;
        } else {
            horizontalOffset = deliveryBike.width / 3.5f;
            verticalOffset = deliveryBike.height / 2.0f;
        }

        const Vector2 collisionPoints[4] = {
            {deliveryBike.x, deliveryBike.y - verticalOffset - 1},
            {deliveryBike.x + horizontalOffset + 1, deliveryBike.y},
            {deliveryBike.x, deliveryBike.y + verticalOffset + 1},
            {deliveryBike.x - horizontalOffset - 1, deliveryBike.y}
        };
          
        Rectangle futurePos = deliveryBike;
          
        // -- CONTROLS --
        // MOVE FORWARD (W)
        if (IsKeyDown(KEY_W)) {
            futurePos.y -= SPEED_CONSTANT; 
            bool hitCar = checkCollisionWithVehicles(futurePos, vehicles, MAX_VEHICLES, true);
            if (!willTouchBorder(backgroundWithBorders, collisionPoints[0]) && !hitCar) {
                rotation = 0;
                deliveryBike.y -= SPEED_CONSTANT;
                isRespawning = false;
            }
        }
        // MOVE BACKWARD (S)
        if (IsKeyDown(KEY_S)) {
            futurePos = deliveryBike; futurePos.y += SPEED_CONSTANT;
            bool hitCar = checkCollisionWithVehicles(futurePos, vehicles, MAX_VEHICLES, true);
            if (!willTouchBorder(backgroundWithBorders, collisionPoints[2]) && !hitCar) {
                rotation = 180;
                deliveryBike.y += SPEED_CONSTANT;
                isRespawning = false;
            }
        }
        // MOVE LEFT (A)
        if(IsKeyDown(KEY_A)) {
            futurePos = deliveryBike; futurePos.x -= SPEED_CONSTANT;
            bool hitCar = checkCollisionWithVehicles(futurePos, vehicles, MAX_VEHICLES, true);
            if (!willTouchBorder(backgroundWithBorders, collisionPoints[3]) && !hitCar) {
                rotation = 270;
                deliveryBike.x -= SPEED_CONSTANT;
                isRespawning = false;
            }
        }
        // MOVE RIGHT (D)
        if (IsKeyDown(KEY_D)) {
            futurePos = deliveryBike; futurePos.x += SPEED_CONSTANT;
            bool hitCar = checkCollisionWithVehicles(futurePos, vehicles, MAX_VEHICLES, true);
            if (!willTouchBorder(backgroundWithBorders, collisionPoints[1]) && !hitCar) {
                rotation = 90;
                deliveryBike.x += SPEED_CONSTANT;
                isRespawning = false;
            }
        }

        // 4. Collision / Stuck Logic
        bool isCollidingNow = checkCollisionWithVehicles(deliveryBike, vehicles, MAX_VEHICLES, false);
          
        if (isCollidingNow) {
            if (!IsSoundPlaying(horn)) PlaySound(horn);
            if (!isRespawning) collisionDuration += GetFrameTime();
        } else {
            collisionDuration = 0.0f;
        }
          
        if (collisionDuration > 1.5f && !isRespawning) {
            isRespawning = true;
            respawnTimer = 3.0f; 
        }
          
        if (isRespawning) {
            respawnTimer -= GetFrameTime();
            if (respawnTimer <= 0) {
                Vector2 newPos = GetRandomValidPosition(backgroundWithBorders, vehicles, MAX_VEHICLES, mapWidth, mapHeight);
                deliveryBike.x = newPos.x;
                deliveryBike.y = newPos.y;
                isRespawning = false;
                collisionDuration = 0.0f;
            }
        }

        // 5. Camera Update (With Clamping for Fullscreen)
        float visibleWidth = screenWidth / cam.zoom;
        float visibleHeight = screenHeight / cam.zoom;
        
        // Clamp Camera X
        if (mapWidth < visibleWidth) {
            cam.target.x = mapWidth / 2.0f;
        } else {
            cam.target.x = Clamp(deliveryBike.x, visibleWidth/2.0f, mapWidth - visibleWidth/2.0f);
        }

        // Clamp Camera Y
        if (mapHeight < visibleHeight) {
            cam.target.y = mapHeight / 2.0f;
        } else {
            cam.target.y = Clamp(deliveryBike.y, visibleHeight/2.0f, mapHeight - visibleHeight/2.0f);
        }
          
        if (cam.zoom >= 2 && GetMouseWheelMove() < 0) cam.zoom -= 0.2;
        else if (cam.zoom <= 3.6 && GetMouseWheelMove() > 0) cam.zoom += 0.2;

        minimapCam.target.x = deliveryBike.x;
        minimapCam.target.y = deliveryBike.y;
        
        // 6. Inputs
        if (IsKeyPressed(KEY_K)) showOrders = !showOrders;
    }

    // ==========================================
    // DRAWING
    // ==========================================
    BeginDrawing();
      ClearBackground((Color){0, 0, 0, 204});

      // --- COMMON MENU BACKGROUND DRAWING LOGIC ---
      // This is used for all states EXCEPT Gameplay.
      // It uses the Origin Vector to perfectly center the map without stretching.
      Rectangle bgSource = { 0.0f, 0.0f, (float)background.width, (float)background.height };
      Rectangle bgDest   = { screenWidth / 2.0f, screenHeight / 2.0f, (float)background.width, (float)background.height };
      Vector2   bgOrigin = { (float)background.width / 2.0f, (float)background.height / 2.0f };

      // --- STATE: GAMEPLAY ---
      if (currentState == STATE_GAMEPLAY) {
          
          // Re-center cam offset in case window resized
          cam.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};

          BeginMode2D(cam);
            DrawTexture(background, 0, 0, WHITE);
            
            // Draw Order Locations (Circles)
            if (currentOrder.isActive && !currentOrder.foodPickedUp) {
                DrawCircleV(currentOrder.pickupLocation, 7.5f, Fade(YELLOW, 0.6f));
            }
            if (currentOrder.isActive && currentOrder.foodPickedUp) {
                DrawCircleV(currentOrder.dropoffLocation, 7.5f, Fade(YELLOW, 0.6f));
            }
            
            // Draw Arrow
            if (currentOrder.isActive)  {
                float tipLength = 20.0f;
                float wingLength = 15.0f;
                float wingAngle = 5.0f; 
                Vector2 tip = { arrowPos.x + cosf(angleToTarget) * tipLength, arrowPos.y + sinf(angleToTarget) * tipLength };
                Vector2 leftWing = { arrowPos.x + cosf(angleToTarget + wingAngle) * wingLength, arrowPos.y + sinf(angleToTarget + wingAngle) * wingLength };
                Vector2 rightWing = { arrowPos.x + cosf(angleToTarget - wingAngle) * wingLength, arrowPos.y + sinf(angleToTarget - wingAngle) * wingLength };
                DrawTriangle(tip, leftWing, rightWing, WHITE);
                DrawTriangleLines(tip, leftWing, rightWing, BLACK); 
            }
            
            // Vehicles
            for (int i = 0; i < MAX_VEHICLES; i++) {
              RenderVehicle(vehicles[i], carTex, truckTex, policeTex);
            }
                    
            // Player
            Rectangle destRect = { deliveryBike.x, deliveryBike.y, DELIVERY_BIKE_SCALED_SIZE, DELIVERY_BIKE_SCALED_SIZE };
            Vector2 origin = { DELIVERY_BIKE_SCALED_SIZE / 2, DELIVERY_BIKE_SCALED_SIZE / 2 };
            DrawTexturePro(deliveryBikeRender.texture, bikeSource, deliveryBike, origin, rotation, WHITE);
        
          EndMode2D();

          // --- GLOBAL TIMER HUD ---
          int timerSec = (int)globalTimer % 60;
          int timerMin = (int)globalTimer / 60;
          const char* globalText = TextFormat("%02d:%02d", timerMin, timerSec);
          int gTimerW = 140;
          int gTimerX = screenWidth/2 - gTimerW/2;

          // Draw Box
          DrawRectangle(gTimerX, 20, gTimerW, 50, Fade(SKYBLUE, 0.9f));
          DrawRectangleLines(gTimerX, 20, gTimerW, 50, DARKBLUE);

          // Draw Text (Red if under 30 seconds, else Black)
          Color gColor = (globalTimer < 30.0f) ? RED : BLACK;
          DrawText(globalText, gTimerX + (gTimerW - MeasureText(globalText, 30))/2, 30, 30, gColor);
          
          // --- MINIMAP ---
          int mmX = screenWidth - MINIMAP_WIDTH - 30;
          int mmY = 20;
          
          minimapCam.offset = (Vector2){ mmX + MINIMAP_WIDTH/2, mmY + MINIMAP_HEIGHT/2 };
          
          DrawRectangle(mmX - MINIMAP_BORDER, mmY - MINIMAP_BORDER, MINIMAP_WIDTH + MINIMAP_BORDER*2, MINIMAP_HEIGHT + MINIMAP_BORDER*2, WHITE);
          DrawRectangle(mmX, mmY, MINIMAP_WIDTH, MINIMAP_HEIGHT, BLACK); 
          
          BeginScissorMode(mmX, mmY, MINIMAP_WIDTH, MINIMAP_HEIGHT);
            BeginMode2D(minimapCam);
                DrawTexture(background, 0, 0, LIGHTGRAY);
                for (int i=0; i<MAX_VEHICLES; i++) RenderVehicle(vehicles[i], carTex, truckTex, policeTex);
                if (currentOrder.isActive && !currentOrder.foodPickedUp) {
                    // Draw Restaurant (Yellow square)
                    DrawRectangle((int)currentOrder.pickupLocation.x - 10, (int)currentOrder.pickupLocation.y - 10, 20, 20, YELLOW);
                } else {
                    // Draw Delivery House (Yellow square)
                    DrawRectangle((int)currentOrder.dropoffLocation.x - 10, (int)currentOrder.dropoffLocation.y - 10, 20, 20, YELLOW);
                }
                DrawTexturePro(deliveryBikeRender.texture, bikeSource, destRect, origin, rotation, WHITE);
            EndMode2D();
          EndScissorMode(); 
          DrawRectangleLines(mmX, mmY, MINIMAP_WIDTH, MINIMAP_HEIGHT, BLACK);
                
          // --- HUD: ORDERS ---
          if (showOrders && currentOrder.foodPickedUp) {
            DrawRectangle (10, 10, 260, 150, Fade(WHITE, 0.9f));
            DrawRectangleLines (10, 10, 260, 150, BLACK);
            DrawText(TextFormat("Order %d:", count+1), 20, 20, 20, BLACK);
            DrawText(currentOrder.restaurantName, 20, 45, 15, BLACK);
            float distToHouse = Vector2Distance(bikePos, currentOrder.dropoffLocation);
            float distRestToHouse = Vector2Distance(currentOrder.pickupLocation, currentOrder.dropoffLocation);
            float reward = 5.0f + (distRestToHouse * 0.015f); // Approximation for display
            DrawText(TextFormat("Distance: %.1f m", distToHouse), 20, 70, 20, BLACK);
            DrawText(TextFormat("Max reward: $%.2f", reward), 20, 92.5f, 20, DARKGREEN);
            DrawText(TextFormat("Total Cash: $%.2f", totalMoney), 20, 115, 20, DARKGREEN);  
          }
          else if (showOrders)  {
            DrawRectangle (10, 10, 220, 100, WHITE);
            DrawText(TextFormat("%d orders completed", count), 20, 20, 20, BLACK);
            DrawText(TextFormat("Total Cash: $%.2f", totalMoney), 20, 50, 20, DARKGREEN);
        }
          
          // --- HUD: TIMER ---
          if (currentOrder.isActive && currentOrder.foodPickedUp) {
            int minutes = (int)currentOrder.timeRemaining / 60;
            int seconds = (int)currentOrder.timeRemaining % 60;
            const char* timerText = TextFormat("%02d:%02d", minutes, seconds);
            int boxWidth = 160;
            int boxHeight = 80;            
            int boxX = mmX - boxWidth - 20; 
            int boxY = mmY + (MINIMAP_HEIGHT / 2) - (boxHeight / 2);
              
            DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(WHITE, 0.9f));
            DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, BLACK); 
            Color timerColor = (currentOrder.timeRemaining < 10.0f) ? RED : BLACK;
            DrawText("Time:", boxX + 15, boxY + 5, 10, DARKGRAY);
            DrawText(timerText, boxX + 20, boxY + 25, 40, timerColor); 
          }

          // Messages (Success/Fail)
          if (message.messageType != PENDING) {
            displayOrderMessage(&message, lastReward);
          }
          
          // Respawn UI
          if (isRespawning) {
            const char* text1 = "CAN NOT MOVE";
            const char* text2 = TextFormat("Respawning in %.1f...", respawnTimer);
            DrawText(text1, screenWidth/2 - MeasureText(text1, 50)/2, 100, 50, RED);
            DrawText(text2, screenWidth/2 - MeasureText(text2, 40)/2, 160, 40, RED);
          }
      } 
      
      // --- STATE: GAME OVER ---
      else if (currentState == STATE_GAMEOVER) {
        // 1. Background (Centered)
        DrawTexturePro(background, bgSource, bgDest, bgOrigin, 0.0f, DARKGRAY);
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.4f));

        int centerX = screenWidth / 2;
        int centerY = screenHeight / 2;

        // 2. Main Title
        const char* title = "\"RUSH\" OVER!";
        DrawText(title, centerX - MeasureText(title, 60)/2, centerY - 150, 60, RED);

        // 3. Stats Panel (Dynamically Centered)
        int panelW = 500;
        int panelH = 200;
        int panelX = centerX - panelW/2;
        int panelY = centerY - 60;

        DrawRectangleRounded((Rectangle){panelX, panelY, panelW, panelH}, 0.1f, 10, Fade(DARKGRAY, 0.5f));
        DrawRectangleRoundedLines((Rectangle){panelX, panelY, panelW, panelH}, 0.1f, 10, WHITE);

        // Stats Text
        DrawText("SESSION RESULTS", centerX - MeasureText("SESSION RESULTS", 30)/2, panelY + 20, 30, GOLD);

        const char* txtOrders = TextFormat("Total Deliveries: %d", count);
        const char* txtMoney = TextFormat("Total Earnings: $%.2f", totalMoney);

        DrawText(txtOrders, centerX - MeasureText(txtOrders, 25)/2, panelY + 80, 25, WHITE);
        DrawText(txtMoney, centerX - MeasureText(txtMoney, 25)/2, panelY + 120, 25, GREEN);

        // 4. Buttons (Play Again / Menu)
        int btnY = panelY + panelH + 40;
        
        // PLAY AGAIN BUTTON
        if (DrawButton("PLAY AGAIN", (Rectangle){centerX - BUTTON_WIDTH - 20, btnY, BUTTON_WIDTH, BUTTON_HEIGHT}, FONT_SIZE, ORANGE, DARKGREEN, BLACK)) {
            // --- RESET VARIABLES ---
            count = 0;
            totalMoney = 0.0f;
            globalTimer = GAME_DURATION;
            currentOrder = CreateNewOrder();
            
            deliveryBike.x = mapWidth / 2.0f;
            deliveryBike.y = mapHeight / 2.0f;
            deliveryBikeRender = LoadRenderTexture(DELIVERY_BIKE_RENDER_SIZE, DELIVERY_BIKE_RENDER_SIZE);
            DrawDeliveryBike(deliveryBikeRender);
            rotation = 0;
            
            currentState = STATE_GAMEPLAY;
        }

        // MAIN MENU BUTTON
        if (DrawButton("MAIN MENU", (Rectangle){centerX + 20, btnY, BUTTON_WIDTH, BUTTON_HEIGHT}, FONT_SIZE, ORANGE, RED, BLACK)) {
            // --- RESET VARIABLES ---
            count = 0;
            totalMoney = 0.0f;
            globalTimer = GAME_DURATION;
            currentOrder = CreateNewOrder();
            
            // Reset Player Position
            deliveryBike.x = mapWidth / 2.0f;
            deliveryBike.y = mapHeight / 2.0f;
            rotation = 0;

            currentState = STATE_MENU;
        }
      }
      
      // --- STATE: MENU ---
      else if (currentState == STATE_MENU) {
          // 1. Background (Centered)
          DrawTexturePro(background, bgSource, bgDest, bgOrigin, 0.0f, DARKGRAY);
          
          const char* title = "DELIVERY RUSH";
          int titleWidth = MeasureText(title, 60);
          DrawText(title, screenWidth/2 - titleWidth/2, 100, 60, RAYWHITE);

          // Center Buttons
          float centerX = screenWidth / 2.0f - BUTTON_WIDTH / 2.0f;
          float startY = screenHeight / 2.0f - 100;

          if (DrawButton("START GAME", (Rectangle){centerX, startY, BUTTON_WIDTH, BUTTON_HEIGHT}, FONT_SIZE, LIGHTGRAY, WHITE, BLACK)) {
              currentState = STATE_GAMEPLAY;
          }
          if (DrawButton("CONTROLS", (Rectangle){centerX, startY + 70, BUTTON_WIDTH, BUTTON_HEIGHT}, FONT_SIZE, LIGHTGRAY, WHITE, BLACK)) {
            currentState = STATE_CONTROLS;
          }
          if (DrawButton("OPTIONS", (Rectangle){centerX, startY + 140, BUTTON_WIDTH, BUTTON_HEIGHT}, FONT_SIZE, LIGHTGRAY, WHITE, BLACK)) {
              currentState = STATE_OPTIONS;
          }
          if (DrawButton("ABOUT CREATORS", (Rectangle){centerX, startY + 210, BUTTON_WIDTH, BUTTON_HEIGHT}, FONT_SIZE, LIGHTGRAY, WHITE, BLACK)) {
              currentState = STATE_ABOUT_CREATORS;
          }
          if (DrawButton("EXIT", (Rectangle){centerX, startY + 280, BUTTON_WIDTH, BUTTON_HEIGHT}, FONT_SIZE, LIGHTGRAY, RED, BLACK)) {
              break; 
          }
      }

      // --- STATE: OPTIONS ---
      else if (currentState == STATE_OPTIONS) {
          DrawTexturePro(background, bgSource, bgDest, bgOrigin, 0.0f, DARKGRAY);
          DrawText("OPTIONS", screenWidth/2 - MeasureText("OPTIONS", 50)/2, 80, 50, RAYWHITE);

          float centerX = screenWidth / 2.0f - BUTTON_WIDTH / 2.0f;
          // Calculate dynamic vertical start point
          float y = screenHeight / 2.0f - 150; 

          // Screen Size
          DrawText("Display", centerX, y, 20, RAYWHITE);
          const char* fsText = IsWindowFullscreen() ? "Mode: Fullscreen" : "Mode: Windowed";
          if (DrawButton(fsText, (Rectangle){centerX, y + 30, BUTTON_WIDTH, 35}, 18, LIGHTGRAY, WHITE, BLACK)) {
              ToggleFullscreen();
          }
          
          if (!IsWindowFullscreen()) {
              if (DrawButton("1024 x 768", (Rectangle){centerX, y + 70, BUTTON_WIDTH, 35}, 18, LIGHTGRAY, WHITE, BLACK)) SetWindowSize(1024, 768);
              if (DrawButton("1536 x 864", (Rectangle){centerX, y + 110, BUTTON_WIDTH, 35}, 18, LIGHTGRAY, WHITE, BLACK)) SetWindowSize(1536, 864);
              if (DrawButton("1920 x 1080", (Rectangle){centerX, y + 150, BUTTON_WIDTH, 35}, 18, LIGHTGRAY, WHITE, BLACK)) SetWindowSize(1920, 1080);
              y += 200; 
          } else {
              DrawText("(Resolution locked in Fullscreen)", centerX, y + 80, 15, LIGHTGRAY);
              y += 120;
          }

          // Volume
          char volText[30];
          sprintf(volText, "Music Volume: %d%%", (int)(musicVolume*100));
          DrawText(volText, centerX, y, 20, RAYWHITE);
          
          if (DrawButton("-", (Rectangle){centerX, y + 25, 50, 35}, 20, LIGHTGRAY, WHITE, BLACK)) {
              if(musicVolume > 0.0f) musicVolume -= 0.1f;
              if(musicVolume < 0.0f) musicVolume = 0.0f; 
              SetMusicVolume(backgroundMusic, musicVolume);
          }
          if (DrawButton("+", (Rectangle){centerX + 170, y + 25, 50, 35}, 20, LIGHTGRAY, WHITE, BLACK)) {
              if(musicVolume < 1.0f) musicVolume += 0.1f;
              SetMusicVolume(backgroundMusic, musicVolume);
          }
          y += 100;

          char sfxText[30];
          sprintf(sfxText, "SFX Volume: %d%%", (int)(sfxVolume*100));
          DrawText(sfxText, centerX, y, 20, RAYWHITE);
          
          if (DrawButton("-", (Rectangle){centerX, y + 25, 50, 35}, 20, LIGHTGRAY, WHITE, BLACK)) {
              if(sfxVolume > 0.0f) sfxVolume -= 0.1f;
              if(sfxVolume < 0.0f) sfxVolume = 0.0f; 
              SetSoundVolume(horn, sfxVolume);
          }
          if (DrawButton("+", (Rectangle){centerX + 170, y + 25, 50, 35}, 20, LIGHTGRAY, WHITE, BLACK)) {
              if(sfxVolume < 1.0f) sfxVolume += 0.1f;
              SetSoundVolume(horn, sfxVolume);
          }
          y += 100;

          if (DrawButton("BACK", (Rectangle){centerX, y, BUTTON_WIDTH, BUTTON_HEIGHT}, FONT_SIZE, ORANGE, WHITE, BLACK)) {
              currentState = STATE_MENU;
          }
      
      }
      
      // --- STATE: CONTROLS ---
      else if (currentState == STATE_CONTROLS) {
            // 1. Draw Background
            DrawTexturePro(background, bgSource, bgDest, bgOrigin, 0.0f, DARKGRAY);
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.4f));

            int centerX = screenWidth / 2;
            int centerY = screenHeight / 2;

            // 2. Title
            const char* title = "HOW TO PLAY";
            DrawText(title, centerX - MeasureText(title, 50)/2, 60, 50, GOLD);

            // 3. Layout Coordinates (Dynamic centering)
            int leftColX = centerX - 250;
            int rightColX = centerX + 50;
            int startY = centerY - 180; // Adjusted for better vertical center

            // --- MOVEMENT SECTION (Left Side) ---
            DrawText("MOVEMENT", leftColX, startY, 25, LIGHTGRAY);
            DrawLine(leftColX, startY + 30, leftColX + 200, startY + 30, LIGHTGRAY);
            
            // Draw WASD Cluster
            int wasdY = startY + 50;
            DrawControlKey("W", "Move forward", leftColX , wasdY);
            DrawControlKey("A", "Turn Left", leftColX , wasdY + 60);
            DrawControlKey("S", "Reverse", leftColX , wasdY + 120);
            DrawControlKey("D", "Turn Right", leftColX , wasdY + 180);

            // --- ACTIONS SECTION (Right Side) ---
            DrawText("ACTIONS", rightColX, startY, 25, LIGHTGRAY);
            DrawLine(rightColX, startY + 30, rightColX + 200, startY + 30, LIGHTGRAY);

            int actionY = startY + 50;
            DrawControlKey("MW UP", "Zoom In", rightColX, actionY);
            DrawControlKey("MW DOWN", "Zoom Out", rightColX, actionY + 60);
            DrawControlKey("F", "Toggle Fullscreen", rightColX, actionY + 120);
            DrawControlKey("ESC", "Exit Game", rightColX, actionY + 180);

            // --- GAMEPLAY TIPS ---
            // Calculate tips relative to bottom of screen
            int tipY = centerY + 200;
            const char* tip1 = "DELIVERY TIP: Follow the White Arrow to find the restaurant and then the customer!";
            DrawRectangle(0, tipY, screenWidth, 40, Fade(DARKGREEN, 0.6f));
            DrawText(tip1, centerX - MeasureText(tip1, 20)/2, tipY + 10, 20, WHITE);

            tipY += 50;
            const char* tip2 = "BE CAREFUL: Deliver the food on time or else you are going to lose money!";
            DrawRectangle(0, tipY, screenWidth, 40, Fade(RED, 0.6f));
            DrawText(tip2, centerX - MeasureText(tip2, 20)/2, tipY + 10, 20, WHITE);

            // 4. Back Button
            if (DrawButton("BACK", (Rectangle){centerX - BUTTON_WIDTH/2, tipY + 55, BUTTON_WIDTH, BUTTON_HEIGHT}, FONT_SIZE, ORANGE, WHITE, BLACK)) {
                currentState = STATE_MENU;
            }
      }

    // --- STATE: ABOUT CREATORS ---
    else if (currentState == STATE_ABOUT_CREATORS) {
        // 1. Background
        DrawTexturePro(background, bgSource, bgDest, bgOrigin, 0.0f, DARKGRAY);
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f)); 

        int centerX = screenWidth / 2;
        int centerY = screenHeight / 2;
        
        // Define a central panel area
        int panelWidth = 700;
        int panelHeight = 510;
        // Dynamically calculate top-left corner
        int panelX = centerX - panelWidth / 2;
        int panelY = centerY - panelHeight / 2;

        // Draw the "Glass" Panel
        DrawRectangleRounded((Rectangle){panelX, panelY, panelWidth, panelHeight}, 0.1f, 10, Fade(SKYBLUE, 0.4f));
        DrawRectangleRoundedLines((Rectangle){panelX, panelY, panelWidth, panelHeight}, 0.1f, 10, LIGHTGRAY);

        // --- TEXT LAYOUT ---
        int y = panelY + 40; 
        int gapSmall = 30;
        int gapLarge = 50;

        // A. HEADER
        const char* header = "CREDITS & INFO";
        DrawText(header, centerX - MeasureText(header, FONT_SIZE)/2, y, FONT_SIZE, GOLD);
        y += gapLarge;

        // B. ACADEMIC INFO
        const char* uni = "Aristotle University of Thessaloniki";
        DrawText(uni, centerX - MeasureText(uni, FONT_SIZE)/2, y, FONT_SIZE, RAYWHITE);
        y += gapSmall;

        const char* dept = "Dept. of Electrical & Computer Engineering";
        DrawText(dept, centerX - MeasureText(dept, FONT_SIZE)/2, y, FONT_SIZE, LIGHTGRAY);
        y += gapSmall;

        const char* course = "Course: Structured Programming (004)";
        DrawText(course, centerX - MeasureText(course, FONT_SIZE)/2, y, FONT_SIZE, LIGHTGRAY);
        y += gapSmall;

        // Separator Line
        y += 10;
        DrawLine(centerX - 150, y, centerX + 150, y, GRAY);
        y += 20;

        // C. PROJECT TITLE
        const char* title = "Delivery Rush";
        DrawText(title, centerX - MeasureText(title, FONT_SIZE)/2, y, FONT_SIZE, SKYBLUE);
        y += gapLarge;

        // D. AUTHORS 
        const char* authorsLabel = "AUTHORS:";
        DrawText(authorsLabel, centerX - MeasureText(authorsLabel, FONT_SIZE)/2, y, FONT_SIZE, ORANGE);
        y += 35;

        // Author 1
        const char* auth1 = "Antonios Karafotis (AEM: 11891)";
        DrawText(auth1, centerX - MeasureText(auth1, FONT_SIZE)/2, y, FONT_SIZE, WHITE);
        y += 35; 

        // Author 2
        const char* auth2 = "Nikolaos Amoiridis (AEM: 11836)";
        DrawText(auth2, centerX - MeasureText(auth2, FONT_SIZE)/2, y, FONT_SIZE, WHITE);
        y += gapLarge;

        // E. LICENSE
        const char* licenceLabel = "LICENSE:";
        DrawText(licenceLabel, centerX - MeasureText(licenceLabel, FONT_SIZE)/2, y, FONT_SIZE, ORANGE);
        y += 35;
        const char* license = "MIT License";
        DrawText(license, centerX - MeasureText(license, FONT_SIZE)/2, y, FONT_SIZE, WHITE);
        y += 25;
        
        const char* note = "(See LICENSE.txt for full text)";
        DrawText(note, centerX - MeasureText(note, FONT_SIZE)/2, y, FONT_SIZE, WHITE);

        // 5. Back Button 
        if (DrawButton("BACK", (Rectangle){centerX - BUTTON_WIDTH/2, panelY + panelHeight + 20, BUTTON_WIDTH, BUTTON_HEIGHT}, FONT_SIZE, ORANGE, WHITE, BLACK)) {
            currentState = STATE_MENU;
        }
    }

      // --- OVERLAY: EXIT REQUEST ---
      if (exitRequest) {
        // A. Heavy Black Fade Background
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));

        // B. Large Central Box (Dynamically Centered)
        int boxWidth = 500;
        int boxHeight = 280;
        int boxX = (screenWidth - boxWidth) / 2;
        int boxY = (screenHeight - boxHeight) / 2;

        DrawRectangle(boxX, boxY, boxWidth, boxHeight, RAYWHITE);
        DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, BLACK);

        // C. Text & Styling
        const char* title = "EXIT GAME?";
        const char* sub = "Are you sure you want to quit?";
        
        DrawText(title, boxX + (boxWidth - MeasureText(title, 40))/2, boxY + 40, 40, MAROON);
        DrawText(sub, boxX + (boxWidth - MeasureText(sub, 20))/2, boxY + 90, 20, DARKGRAY);

        // D. Buttons (Spaced out)
        int btnW = 160;
        int btnH = 50;
        int btnY = boxY + 160;
        int spacing = 60; // Space between buttons

        // YES (Quit)
        if (DrawButton("YES, QUIT", (Rectangle){boxX + boxWidth/2 - btnW - spacing/2, btnY, btnW, btnH}, 20, LIGHTGRAY, RED, WHITE)) {
            running = false;
        }

        // NO (Resume)
        if (DrawButton("NO, STAY", (Rectangle){boxX + boxWidth/2 + spacing/2, btnY, btnW, btnH}, 20, LIGHTGRAY, DARKGREEN, WHITE)) {
            exitRequest = false;
        }
      } 
    EndDrawing();
  }
  
  // --- CLEANUP ---
  UnloadTexture(background);
  UnloadImage(backgroundWithBorders);
  UnloadRenderTexture(carTex);
  UnloadRenderTexture(truckTex);
  UnloadRenderTexture(policeTex);
  UnloadRenderTexture(deliveryBikeRender);
  UnloadMusicStream(backgroundMusic);
  UnloadSound(horn);
  CloseAudioDevice();
  CloseWindow();

  return 0;
}