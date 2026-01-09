#include <time.h>
#include"raylib.h"
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
int count = 0; //Number of order

int main(void) {
  
  SetRandomSeed(time(NULL)); 
  
  InitWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "RaylibProjectAuth");
  Texture2D background = LoadTexture("assets/map.jpg"); 
  Image backgroundWithBorders = LoadImage("assets/mapWithBorders.jpg");
  
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
  vehicleGenerator(MAX_VEHICLES, vehicles, background.height, background.width, backgroundWithBorders); // posx , posy != 0 else bug. Needs to be offsected
  

  int rotation = 180;

  KeyboardKey activeKey = KEY_NULL;

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
      // goes clockwise: top, right, bottom, left
      const Vector2 collisionPoints[4] = {
          {deliveryBike.x, deliveryBike.y - verticalOffset - 1},
          {deliveryBike.x + horizontalOffset + 1, deliveryBike.y},
          {deliveryBike.x, deliveryBike.y + verticalOffset + 1},
          {deliveryBike.x - horizontalOffset - 1, deliveryBike.y}
      };
      
      
      /*** MOVEMENT ***/ 
      // Create a "future" rectangle to test if the move is valid before moving
      Rectangle futurePos = deliveryBike;

      // MOVE FORWARD (W)
      if (IsKeyDown(KEY_W)) {
        futurePos.y -= SPEED_CONSTANT; // Calculate target position
        
        // Check specifically if we hit a CAR
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
      // Set camera to follow the bike
      cam.target.x = deliveryBike.x;
      cam.target.y = deliveryBike.y;
      
      // Create bounds for cam.zoom, so that the
      // bike won't become very small/big.
      if (cam.zoom >= 2 && GetMouseWheelMove() < 0) {
        cam.zoom -= 0.2;
      } else if (cam.zoom <= 3.6 && GetMouseWheelMove() > 0) {
        cam.zoom += 0.2;
      }

      minimapCam.target.x = deliveryBike.x;
      minimapCam.target.y = deliveryBike.y;
      
      BeginMode2D(cam);
        
        DrawTexture(background, 0, 0, WHITE);
        
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
      DrawRectangle(mmX, mmY, MINIMAP_WIDTH, MINIMAP_HEIGHT, BLACK); // Black if out of bounds
      
      // Minimap initilization
      BeginScissorMode(mmX, mmY, MINIMAP_WIDTH, MINIMAP_HEIGHT);
        
        BeginMode2D(minimapCam);
            
            DrawTexture(background, 0, 0, LIGHTGRAY);
            
            for (int i=0; i<MAX_VEHICLES; i++) {
              RenderVehicle(vehicles[i], carTex, truckTex, policeTex);
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
      
      if (showOrders) {
        DrawRectangle (10, 10, 140, 170, WHITE);
        DrawText("Orders", 15, 15 + count*25, 20, BLACK);
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
