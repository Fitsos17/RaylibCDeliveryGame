/*
 * Πανεπιστήμιο: Αριστοτέλειο Πανεπιστήμιο Θεσσαλονίκης
 * Τμήμα: Τμήμα Ηλεκτρολόγων Μηχανικών και Μηχανικών Υπολογιστών
 * Μάθημα: Δομημένος Προγραμματισμός (004)
 * Τίτλος Εργασίας: Raylib Food Delivery Game
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
float difficultyFactor = 0.8f; 

int main(void) {
  
  SetRandomSeed(time(NULL)); 
  
  InitWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "Raylib Delivery Rush");
  
  // --- LOAD ASSETS ---
  // Note: Using the file extensions from main.c 1 as they seemed more specific (png for borders)
  Texture2D background = LoadTexture("assets/map.jpg"); 
  Image backgroundWithBorders = LoadImage("assets/mapWithBorders.png");
  
  // Analyze map for houses/restaurants (From main.c 1)
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

        // 5. Camera Update
        cam.target.x = deliveryBike.x;
        cam.target.y = deliveryBike.y;
          
        if (cam.zoom >= 2 && GetMouseWheelMove() < 0) cam.zoom -= 0.2;
        else if (cam.zoom <= 3.6 && GetMouseWheelMove() > 0) cam.zoom += 0.2;

        minimapCam.target.x = deliveryBike.x;
        minimapCam.target.y = deliveryBike.y;
        
        // 6. Inputs
        if (IsKeyPressed(KEY_K)) showOrders = !showOrders;
        if (IsKeyPressed(KEY_F)) ToggleFullscreen();
    }

    // ==========================================
    // DRAWING
    // ==========================================
    BeginDrawing();
      ClearBackground(DARKGRAY);
      // --- STATE: GAMEPLAY ---
      if (currentState == STATE_GAMEPLAY) {
          
          // Re-center cam in case window resized
          cam.offset = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};

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
          
          // --- MINIMAP ---
          int mmX = GetScreenWidth() - MINIMAP_WIDTH - 20;
          int mmY = 20;
          
          minimapCam.offset = (Vector2){ mmX + MINIMAP_WIDTH/2, mmY + MINIMAP_HEIGHT/2 };
          
          DrawRectangle(mmX - MINIMAP_BORDER, mmY - MINIMAP_BORDER, MINIMAP_WIDTH + MINIMAP_BORDER*2, MINIMAP_HEIGHT + MINIMAP_BORDER*2, WHITE);
          DrawRectangle(mmX, mmY, MINIMAP_WIDTH, MINIMAP_HEIGHT, BLACK); 
          
          BeginScissorMode(mmX, mmY, MINIMAP_WIDTH, MINIMAP_HEIGHT);
            BeginMode2D(minimapCam);
                DrawTexture(background, 0, 0, LIGHTGRAY);
                for (int i=0; i<MAX_VEHICLES; i++) RenderVehicle(vehicles[i], carTex, truckTex, policeTex);
                if (currentOrder.isActive && !currentOrder.foodPickedUp) {
                    DrawRectangle((int)currentOrder.pickupLocation.x - 10, (int)currentOrder.pickupLocation.y - 10, 20, 20, YELLOW);
                }
                DrawTexturePro(deliveryBikeRender.texture, bikeSource, destRect, origin, rotation, WHITE);
            EndMode2D();
          EndScissorMode(); 
          DrawRectangleLines(mmX, mmY, MINIMAP_WIDTH, MINIMAP_HEIGHT, BLACK);
                
          // --- HUD: ORDERS ---
          if (showOrders && currentOrder.foodPickedUp) {
            DrawRectangle (10, 10, 260, 125, Fade(WHITE, 0.9f));
            DrawRectangleLines (10, 10, 260, 125, BLACK);
            DrawText(TextFormat("Order %d:", count+1), 20, 20, 20, BLACK);
            DrawText(currentOrder.restaurantName, 20, 45, 15, BLACK);
            float distToHouse = Vector2Distance(bikePos, currentOrder.dropoffLocation);
            float distRestToHouse = Vector2Distance(currentOrder.pickupLocation, currentOrder.dropoffLocation);
            float reward = 5.0f + (distRestToHouse * 0.015f); // Approximation for display
            DrawText(TextFormat("Distance: %.1f m", distToHouse), 20, 70, 20, BLACK);
            DrawText(TextFormat("Reward: $%.2f", reward), 20, 90, 20, DARKGREEN);
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
            DrawText(text1, GetScreenWidth()/2 - MeasureText(text1, 50)/2, 100, 50, RED);
            DrawText(text2, GetScreenWidth()/2 - MeasureText(text2, 40)/2, 160, 40, RED);
          }
      } 
      
      // --- STATE: MENU ---
      else if (currentState == STATE_MENU) {
          // Draw dimmed game in background
          DrawTexturePro(background, (Rectangle){0,0,background.width,background.height}, (Rectangle){0,0,GetScreenWidth(),GetScreenHeight()}, (Vector2){0,0}, 0, DARKGRAY);
          
          const char* title = "DELIVERY RUSH";
          int titleWidth = MeasureText(title, 60);
          DrawText(title, GetScreenWidth()/2 - titleWidth/2, 100, 60, RAYWHITE);

          float centerX = GetScreenWidth() / 2.0f - BUTTON_WIDTH / 2.0f;
          float startY = GetScreenHeight() / 2.0f - 50;

          if (DrawButton("START GAME", (Rectangle){centerX, startY, BUTTON_WIDTH, BUTTON_HEIGHT}, FONT_SIZE, LIGHTGRAY, WHITE, BLACK)) {
              currentState = STATE_GAMEPLAY;
          }
          if (DrawButton("OPTIONS", (Rectangle){centerX, startY + 70, BUTTON_WIDTH, BUTTON_HEIGHT}, FONT_SIZE, LIGHTGRAY, WHITE, BLACK)) {
              currentState = STATE_OPTIONS;
          }
          if (DrawButton("EXIT", (Rectangle){centerX, startY + 140, BUTTON_WIDTH, BUTTON_HEIGHT}, FONT_SIZE, LIGHTGRAY, RED, BLACK)) {
              break; 
          }
      }

      // --- STATE: OPTIONS ---
      else if (currentState == STATE_OPTIONS) {
          DrawTexturePro(background, (Rectangle){0,0,background.width,background.height}, (Rectangle){0,0,GetScreenWidth(),GetScreenHeight()}, (Vector2){0,0}, 0, DARKGRAY);
          DrawText("OPTIONS", GetScreenWidth()/2 - MeasureText("OPTIONS", 50)/2, 80, 50, RAYWHITE);

          float centerX = GetScreenWidth() / 2.0f - BUTTON_WIDTH / 2.0f;
          float y = 180;

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
      
      if (exitRequest) {
        // A. Heavy Black Fade Background
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));

        // B. Large Central Box
        int boxWidth = 500;
        int boxHeight = 280;
        int boxX = GetScreenWidth() / 2 - boxWidth / 2;
        int boxY = GetScreenHeight() / 2 - boxHeight / 2;

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