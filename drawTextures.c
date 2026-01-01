#include"raylib.h"

void DrawDeliveryBike(RenderTexture2D target) {
  // DRAW THE SPRITE ONCE to the texture
  BeginTextureMode(target);
    ClearBackground(BLANK); // Transparent background
    
    // 1. Main Bike Frame (Grey)
    DrawRectangle(13, 5, 6, 20, DARKBROWN);
    
    // 2. Handlebars (Black)
    DrawRectangle(8, 7, 16, 6, DARKGRAY); 

    // Left Mirror Stem & Glass
    DrawRectangle(8, 5, 4, 2, LIGHTGRAY);  // Mirror glass moved down to Y=4

    // Right Mirror (Connected at the edge of handlebars)
    DrawRectangle(20, 5, 4, 2, LIGHTGRAY);  // Mirror glass moved down to Y=4

    // 3. Driver's Helmet (Blue)
    DrawCircle(16, 14, 5, BLUE); 
    
    // 4. Delivery Box (Red/Yellow)
    DrawRectangle(10, 20, 12, 10, RED);    // The box
    DrawRectangle(12, 22, 8, 2, YELLOW);   // "Logo" on the box
    
    // 5. Headlight (Yellow)
    DrawRectangle(15, 3, 2, 2, YELLOW);
    
  EndTextureMode();
}

void DrawCar(int posx, int posy, Color color) {    // Draw car
  DrawRectangle(posx, posy, 40, 65, color); // Main frame

  DrawRectangle(posx + 5, posy + 12, 30, 10, SKYBLUE);    // Dashscreen

  DrawRectangle(posx + 5, posy + 45, 30, 8, SKYBLUE);   // Backscreen
  
  DrawRectangle(posx + 2, posy - 2, 8, 4, YELLOW);    //Light left
  
  DrawRectangle(posx + 30, posy - 2, 8, 4, YELLOW);    //Light right
}

void DrawPoliceCar(int posx, int posy) {    // Draw policecar
  DrawRectangle(posx, posy, 40, 65, WHITE); // Main frame

  DrawRectangle(posx, posy, 40, 12, BLACK);    // Front

  DrawRectangle(posx, posy + 53, 40, 12, BLACK);   // Back
  
  DrawRectangle(posx + 8, posy + 30, 12, 6, RED);    // Police lights
  
  DrawRectangle(posx + 20, posy + 30, 12, 6, BLUE);    // Police lights
  
  DrawRectangle(posx + 5, posy + 15, 30, 8, YELLOW);    // Dashscreen
}

void DrawTruck (int posx, int posy, Color color)    {   // Draw truck
  DrawRectangle(posx, posy, 55, 35, WHITE);  // Main frame
  
  DrawRectangle(posx + 5, posy + 5, 45, 10, SKYBLUE);    // Dashscreen
  
  DrawRectangle(posx, posy + 38, 55, 72, color);  // Trailer
  
  DrawRectangle(posx - 5, posy + 10, 5, 8, DARKGRAY); // Left mirror
  
  DrawRectangle(posx + 55, posy + 10, 5, 8, DARKGRAY);    // Right mirror
}

void mapVehicleToDrawFunction(TYPE_OF_VEHICLE selectedVehicle, int posx, int posy, Color color) {
  switch (selectedVehicle)    {
    case CAR:
        DrawCar(posx, posy, color);
        break;
    case TRUCK:
        DrawTruck(posx, posy, color);
        break;
    case POLICE:
        DrawPoliceCar(posx, posy);
        break;
  }
}
          
void DrawTrafficLight (int posx, int posy)  {   // Draw traffic light
  Color color;

  DrawRectangle(posx, posy, 20, 20, LIGHTGRAY);   // Pole
  DrawCircleSector((Vector2){posx + 10, posy + 20}, 10, 0, 180, 0, color); //Light
}
