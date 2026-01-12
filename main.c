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
#include"raylib.h"
#include "raymath.h"
#include"helpers.h"
#include"drawTextures.h"

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

bool showOrders = false;
int count = 0; // Number of order
float totalMoney = 0.0f;
float difficultyFactor = 0.8f; // Smaller equals less time

int main(void) {
  
  SetRandomSeed(time(NULL)); 
  
  InitWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "RaylibProjectAuth");
  Texture2D background = LoadTexture("assets/map.jpg"); 
  
  Image backgroundWithBorders = LoadImage("assets/mapWithBorders.png");
  
  InitMapLocations(backgroundWithBorders);
  Order currentOrder = CreateNewOrder();
  
  SetTextureFilter(background, TEXTURE_FILTER_POINT);
  
  InitAudioDevice();
  Music backgroundMusic = LoadMusicStream("assets/background_music.mp3");
  Sound horn = LoadSound("assets/horn.mp3");

  SetMusicVolume(backgroundMusic, 0.5f);
  SetSoundVolume(horn, 0.3f);
  
  int mapHeight = background.height;
  int mapWidth = background.width;
  
  SetTargetFPS(60);

  RenderTexture2D deliveryBikeRender = LoadRenderTexture(DELIVERY_BIKE_RENDER_SIZE, DELIVERY_BIKE_RENDER_SIZE);

  DrawDeliveryBike(deliveryBikeRender); 
  
  Rectangle bikeSource = {
    0, 0, deliveryBikeRender.texture.width, deliveryBikeRender.texture.height
  };
  
  Rectangle deliveryBike = {mapWidth / 2.0f, mapHeight / 2.0f, DELIVERY_BIKE_SCALED_SIZE, DELIVERY_BIKE_SCALED_SIZE};
  
  // initialize main camera
  Camera2D cam = {0};
  cam.offset = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
  cam.zoom = 3;
  cam.rotation = 0;
  
  // initialize minimap camera
  Camera2D minimapCam = {0};
  minimapCam.zoom = MINIMAP_ZOOM;
  
  // generate random vehicles inside the borders
  Vehicle vehicles[MAX_VEHICLES];
  vehicleGenerator(MAX_VEHICLES, vehicles, background.height, background.width, backgroundWithBorders); 
  
  int rotation = 180;

  RenderTexture2D carTex = LoadRenderTexture(40, 65);
  RenderTexture2D truckTex = LoadRenderTexture(65, 110);
  RenderTexture2D policeTex = LoadRenderTexture(40, 65);
  PrepareCarTexture(carTex);
  PrepareTruckTexture(truckTex);
  PreparePoliceTexture(policeTex);
  PlayMusicStream(backgroundMusic);

  while (!WindowShouldClose()) {
    UpdateMusicStream(backgroundMusic);
    updateTraffic(vehicles, MAX_VEHICLES, backgroundWithBorders, (Vector2){deliveryBike.x, deliveryBike.y});
    
    Vector2 bikePos = { deliveryBike.x, deliveryBike.y };
    updateOrder(&currentOrder, bikePos, &count, &totalMoney, houses, houseCount);
    
    Vector2 currentTargetPos;
    
    if (!currentOrder.foodPickedUp) {
        currentTargetPos = currentOrder.pickupLocation; // Target is restaurant
    }
    else currentTargetPos = currentOrder.dropoffLocation; // Target is house
    
    // Initiate arrow to target
    float angleToTarget = atan2f(currentTargetPos.y - bikePos.y, currentTargetPos.x - bikePos.x);
    
    float arrowRadius = 45.0f;
    Vector2 arrowPos = {
        bikePos.x + cosf(angleToTarget) * arrowRadius,
        bikePos.y + sinf(angleToTarget) * arrowRadius
    };
    
    BeginDrawing();

      ClearBackground(DARKGRAY);

      float horizontalOffset, verticalOffset;

      if (rotation == 90 || rotation == 270) {
          // Vehicle is horizontal -> swap offsets
          horizontalOffset = deliveryBike.height / 2.0f; 
          verticalOffset = deliveryBike.width / 3.5f;
      } else {
          // Vehicle is vertical -> standard offsets
          horizontalOffset = deliveryBike.width / 3.5f;
          verticalOffset = deliveryBike.height / 2.0f;
      }

      // points of bike we need to track for collisions
      const Vector2 collisionPoints[4] = {
          {deliveryBike.x, deliveryBike.y - verticalOffset - 1},
          {deliveryBike.x + horizontalOffset + 1, deliveryBike.y},
          {deliveryBike.x, deliveryBike.y + verticalOffset + 1},
          {deliveryBike.x - horizontalOffset - 1, deliveryBike.y}
      };
      
      
      /*** MOVEMENT ***/ 
      Rectangle futurePos = deliveryBike;
      
      // MOVE FORWARD (W)
      if (IsKeyDown(KEY_W)) {
        futurePos.y -= SPEED_CONSTANT; 
        
        bool hitCar = checkCollisionWithVehicles(futurePos, vehicles, MAX_VEHICLES, true);

        if (!willTouchBorder(backgroundWithBorders, collisionPoints[0]) && !hitCar) {
          rotation = 180;
          deliveryBike.y -= SPEED_CONSTANT;
        }
      }
      
      // MOVE BACKWARD (S)
      if (IsKeyDown(KEY_S)) {
        futurePos = deliveryBike; 
        futurePos.y += SPEED_CONSTANT;

        bool hitCar = checkCollisionWithVehicles(futurePos, vehicles, MAX_VEHICLES, true);

        if (!willTouchBorder(backgroundWithBorders, collisionPoints[2]) && !hitCar) {
          rotation = 0;
          deliveryBike.y += SPEED_CONSTANT;
        }
      }

      // MOVE LEFT (A)
      if(IsKeyDown(KEY_A)) {
        futurePos = deliveryBike;
        futurePos.x -= SPEED_CONSTANT;

        bool hitCar = checkCollisionWithVehicles(futurePos, vehicles, MAX_VEHICLES, true);

        if (!willTouchBorder(backgroundWithBorders, collisionPoints[3]) && !hitCar) {
          rotation = 90;
          deliveryBike.x -= SPEED_CONSTANT;
        }
      }
      
      // MOVE RIGHT (D)
      if (IsKeyDown(KEY_D)) {
        futurePos = deliveryBike;
        futurePos.x += SPEED_CONSTANT;

        bool hitCar = checkCollisionWithVehicles(futurePos, vehicles, MAX_VEHICLES, true);

        if (!willTouchBorder(backgroundWithBorders, collisionPoints[1]) && !hitCar) {
          rotation = 270;
          deliveryBike.x += SPEED_CONSTANT;
        }
      }

      // check for current collision to play the horn sound
      bool isCollidingNow = checkCollisionWithVehicles(deliveryBike, vehicles, MAX_VEHICLES, false);
      
      if (isCollidingNow) {
          if (!IsSoundPlaying(horn)) {
              PlaySound(horn);
          }
      }

      /*** CAMERA ***/
      cam.target.x = deliveryBike.x;
      cam.target.y = deliveryBike.y;
      
      if (cam.zoom >= 2 && GetMouseWheelMove() < 0) {
        cam.zoom -= 0.2;
      } else if (cam.zoom <= 3.6 && GetMouseWheelMove() > 0) {
        cam.zoom += 0.2;
      }

      minimapCam.target.x = deliveryBike.x;
      minimapCam.target.y = deliveryBike.y;
      
      BeginMode2D(cam);
        
        DrawTexture(background, 0, 0, WHITE);
        
        if (currentOrder.isActive && !currentOrder.foodPickedUp) {
                DrawCircleV(currentOrder.pickupLocation, 7.5f, Fade(YELLOW, 0.6f)); // Semi transparent circle at restaurant
            }
            
        if (currentOrder.isActive && currentOrder.foodPickedUp) {
            DrawCircleV(currentOrder.dropoffLocation, 7.5f, Fade(YELLOW, 0.6f)); // Semi transparent circle at house
        }
        
        // Draw arrow pointing to target
        if (currentOrder.isActive)  {
            float tipLength = 20.0f;
            float wingLength = 15.0f;
            float wingAngle = 5.0f; 

            Vector2 tip = {
                arrowPos.x + cosf(angleToTarget) * tipLength,
                arrowPos.y + sinf(angleToTarget) * tipLength
            };

            Vector2 leftWing = {
                arrowPos.x + cosf(angleToTarget + wingAngle) * wingLength,
                arrowPos.y + sinf(angleToTarget + wingAngle) * wingLength
            };

            Vector2 rightWing = {
                arrowPos.x + cosf(angleToTarget - wingAngle) * wingLength,
                arrowPos.y + sinf(angleToTarget - wingAngle) * wingLength
            };

            DrawTriangle(tip, leftWing, rightWing, WHITE);
            DrawTriangleLines(tip, leftWing, rightWing, BLACK); 
        }
        
        for (int i = 0; i < MAX_VEHICLES; i++) {
          RenderVehicle(vehicles[i], carTex, truckTex, policeTex);
        }
                
        Rectangle destRect = { deliveryBike.x, deliveryBike.y, DELIVERY_BIKE_SCALED_SIZE, DELIVERY_BIKE_SCALED_SIZE };
        Vector2 origin = { DELIVERY_BIKE_SCALED_SIZE / 2, DELIVERY_BIKE_SCALED_SIZE / 2 };
        DrawTexturePro(deliveryBikeRender.texture, bikeSource, deliveryBike, (Vector2) { DELIVERY_BIKE_SCALED_SIZE / 2, DELIVERY_BIKE_SCALED_SIZE / 2 }, rotation, WHITE);
    
      EndMode2D();
      
      int mmX = GetScreenWidth() - MINIMAP_WIDTH - 20;
      int mmY = 20;
      
      minimapCam.offset = (Vector2){ mmX + MINIMAP_WIDTH/2, mmY + MINIMAP_HEIGHT/2 };
      
      DrawRectangle(mmX - MINIMAP_BORDER, mmY - MINIMAP_BORDER, MINIMAP_WIDTH + MINIMAP_BORDER*2, MINIMAP_HEIGHT + MINIMAP_BORDER*2, WHITE);
      DrawRectangle(mmX, mmY, MINIMAP_WIDTH, MINIMAP_HEIGHT, BLACK); 
      
      // Minimap initilization
      BeginScissorMode(mmX, mmY, MINIMAP_WIDTH, MINIMAP_HEIGHT);
        
        BeginMode2D(minimapCam);
            
            DrawTexture(background, 0, 0, LIGHTGRAY);
            
            for (int i=0; i<MAX_VEHICLES; i++) {
              RenderVehicle(vehicles[i], carTex, truckTex, policeTex);
            }

            if (currentOrder.isActive && !currentOrder.foodPickedUp)  {
                DrawRectangle(
                    (int)currentOrder.pickupLocation.x - 10,
                    (int)currentOrder.pickupLocation.y - 10,
                    20,
                    20,
                    YELLOW
                );
            }
            
            DrawTexturePro(deliveryBikeRender.texture, bikeSource, destRect, origin, rotation, WHITE);
            

        EndMode2D();
        
        EndScissorMode(); 
      
      DrawRectangleLines(mmX, mmY, MINIMAP_WIDTH, MINIMAP_HEIGHT, BLACK);
            
      if (IsKeyPressed(KEY_F)) {
        ToggleFullscreen();
      }

      if (IsKeyPressed(KEY_K)) {
        showOrders = !showOrders;
      }
      
      if (showOrders && currentOrder.foodPickedUp) {
        DrawRectangle (10, 10, 260, 125, Fade(WHITE, 0.9f));
        DrawRectangleLines (10, 10, 260, 125, BLACK);
        
        DrawText(TextFormat("Order %d:", count+1), 20, 20, 20, BLACK);
        
        DrawText(currentOrder.restaurantName, 20, 45, 15, BLACK);
        
        float distRestToHouse = Vector2Distance(currentOrder.pickupLocation, currentOrder.dropoffLocation);
        float distToHouse = Vector2Distance(bikePos, currentOrder.dropoffLocation);
        float reward = 5.0f + (distRestToHouse * 0.015f);
        
        DrawText(TextFormat("Distance: %.1f m", distToHouse), 20, 70, 20, BLACK);
        DrawText(TextFormat("Reward: $%.2f", reward), 20, 90, 20, DARKGREEN);
      }
      
      else if (showOrders)  {
        DrawRectangle (10, 10, 220, 100, WHITE);
        DrawText(TextFormat("%d orders completed", count), 20, 20, 20, BLACK);
        DrawText(TextFormat("Total Cash: $%.2f", totalMoney), 20, 50, 20, DARKGREEN);
      }
      
      // Initialize timer
      if (currentOrder.isActive && currentOrder.foodPickedUp) {
            int minutes = (int)currentOrder.timeRemaining / 60;
            int seconds = (int)currentOrder.timeRemaining % 60;
            const char* timerText = TextFormat("%02d:%02d", minutes, seconds);
            const char* labelText = "Time:";
                
            int fontSize = 40;
            int boxWidth = 160;
            int boxHeight = 80;            
                
            // Timer coordinates
            int boxX = mmX - boxWidth - 20; 
            int boxY = mmY + (MINIMAP_HEIGHT / 2) - (boxHeight / 2);
              
            DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(WHITE, 0.9f));
            DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, BLACK); 
                
            Color timerColor;
 
            // Red timer if time remaining is under 10 seconds
            if (currentOrder.timeRemaining < 10.0f) timerColor = RED;
            else timerColor = BLACK;
                
            DrawText(labelText, boxX + 15, boxY + 5, 10, DARKGRAY);
            DrawText(timerText, boxX + 20, boxY + 25, fontSize, timerColor); 
            }

    EndDrawing();
  }
  
  UnloadTexture(background);
  UnloadImage(backgroundWithBorders);
  UnloadRenderTexture(carTex);
  UnloadRenderTexture(truckTex);
  UnloadRenderTexture(policeTex);
  UnloadRenderTexture(deliveryBikeRender);
  CloseWindow();

  return 0;
}