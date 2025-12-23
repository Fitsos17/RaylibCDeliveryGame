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

void Draw8BitRoad() {
  // to be developed...
  DrawRectangle(500, 300, 100, 40, RED);
}