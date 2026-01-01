#include"raylib.h"
#include <time.h>

#include"helpers.c"
#include"drawTextures.c"

const int INITIAL_WINDOW_WIDTH = 1000; 
const int INITIAL_WINDOW_HEIGHT = 550;
const int DELIVERY_BIKE_RENDER_SIZE = 32;
const int DELIVERY_BIKE_SCALED_SIZE = 128;
const Color BACKGROUND_COLOR = DARKGRAY;

const int MINIMAP_WIDTH = 150;      
const int MINIMAP_HEIGHT = 150;     
const float MINIMAP_ZOOM = 0.3f;    
const int MINIMAP_BORDER = 2;

bool showOrders=false;
int count=0; //Number of order
int main(void) {
  
  SetRandomSeed(time(NULL)); 
  Vehicle vehicles[MAX_VEHICLES];
  vehicleGenerator(MAX_VEHICLES, vehicles, 80, 80); // posx , posy != 0 else bug. Needs to be offsected
  
  InitWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "RaylibProjectAuth");
  Texture2D background = LoadTexture("map.jpg"); 
  SetTextureFilter(background, TEXTURE_FILTER_POINT);
  
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
  cam.zoom = 1;
  cam.rotation = 0;
  
  // initialize minimap camera
  Camera2D minimapCam = {0};
  minimapCam.zoom = MINIMAP_ZOOM;

  while (!WindowShouldClose()) {
    
    updateTraffic(vehicles, MAX_VEHICLES, cam, mapHeight, mapWidth);
    BeginDrawing();

      ClearBackground(DARKGRAY);
    
      /*** MOVEMENT ***/ 
      // move forward
      if (IsKeyDown(KEY_W)) {
        deliveryBike.y -= 4;
      }
      // move backward (S)
      if (IsKeyDown(KEY_S)) {
        deliveryBike.y += 4;
      }
      // move left (A)
      if(IsKeyDown(KEY_A)) {
        deliveryBike.x -= 4;
      }
      // move right (D)
      if (IsKeyDown(KEY_D)) {
        deliveryBike.x += 4;
      }

      /*** CAMERA ***/
      // Set camera to follow the bike
      cam.target.x = deliveryBike.x;
      cam.target.y = deliveryBike.y;
      
      // Only if 0.8 <= cam.zoom <= 1.4 we want to change cam.zoom. Otherwise,
      // bike will become very small/big.
      if (cam.zoom >= 0.6 && GetMouseWheelMove() < 0) {
        cam.zoom -= 0.2;
      } else if (cam.zoom <= 1.6 && GetMouseWheelMove() > 0) {
        cam.zoom += 0.2;
      }
      
      minimapCam.target.x = deliveryBike.x;
      minimapCam.target.y = deliveryBike.y;
      
      BeginMode2D(cam);
        
        DrawTexture(background, 0, 0, WHITE);
        for (int i=0; i<MAX_VEHICLES; i++)  {
            mapVehicleToDrawFunction(vehicles[i].type, vehicles[i].posx, vehicles[i].posy, vehicles[i].vehicleColor);
        }
        
                
        Rectangle destRect = { deliveryBike.x, deliveryBike.y, DELIVERY_BIKE_SCALED_SIZE, DELIVERY_BIKE_SCALED_SIZE };
        Vector2 origin = { DELIVERY_BIKE_SCALED_SIZE / 2, DELIVERY_BIKE_SCALED_SIZE / 2 };
        DrawTexturePro(deliveryBikeRender.texture, bikeSource, deliveryBike, (Vector2) { DELIVERY_BIKE_SCALED_SIZE / 2, DELIVERY_BIKE_SCALED_SIZE / 2 }, 180, WHITE);
    
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
                if(vehicles[i].type == POLICE)  {
                    DrawRectangle(vehicles[i].posx, vehicles[i].posy, 40, 60, BLUE);
                }
            }

             DrawTexturePro(deliveryBikeRender.texture, bikeSource, destRect, origin, 180, WHITE);

        EndMode2D();
        
      EndScissorMode(); 
      
      DrawRectangleLines(mmX, mmY, MINIMAP_WIDTH, MINIMAP_HEIGHT, BLACK);
      
      if (IsKeyPressed(KEY_K)) {
            showOrders = !showOrders;
      }
      
      if (showOrders)   {
          DrawRectangle (10, 10, 140, 170, WHITE);
          DrawText("Orders", 15, 15 + count*25, 20, BLACK);
      }
    EndDrawing();
  }
  
  UnloadTexture (background);
  UnloadRenderTexture(deliveryBikeRender);
  CloseWindow();

  return 0;
}
