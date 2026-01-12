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

#include "raylib.h"
#include "drawTextures.h"

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

// CAR: 40x65 base
void PrepareCarTexture(RenderTexture2D target) {
    BeginTextureMode(target);
        ClearBackground(BLANK);
        
        // Main Body (White will be tinted by vehicleColor)
        DrawRectangle(0, 0, 40, 65, WHITE); 
        
        // Shading/Detail (Darken the bottom/rear slightly for depth)
        DrawRectangle(0, 55, 40, 10, CLITERAL(Color){ 0, 0, 0, 40 }); 

        // Windows
        DrawRectangle(5, 12, 30, 15, SKYBLUE);   // Windshield
        DrawRectangle(5, 35, 30, 10, SKYBLUE);   // Rear window
        
        // Lights
        DrawRectangle(5, 2, 10, 5, YELLOW);      // Left Headlight
        DrawRectangle(25, 2, 10, 5, YELLOW);     // Right Headlight
        DrawRectangle(5, 62, 10, 3, RED);        // Left Brake
        DrawRectangle(25, 62, 10, 3, RED);       // Right Brake

        // Bumpers
        DrawRectangle(0, 0, 40, 3, DARKGRAY);    // Front Bumper
        DrawRectangle(0, 63, 40, 2, DARKGRAY);   // Rear Bumper
    EndTextureMode();
}

// TRUCK: 65x110 base
void PrepareTruckTexture(RenderTexture2D target) {
    BeginTextureMode(target);
        ClearBackground(BLANK); 
        
        // 1. Cab
        DrawRectangle(5, 0, 55, 35, WHITE); 
        DrawRectangle(10, 5, 45, 10, SKYBLUE); 
        
        // 2. Mirrors
        DrawRectangle(0, 10, 5, 8, DARKGRAY);   // Left
        DrawRectangle(60, 10, 5, 8, DARKGRAY);  // Right

        // 3. Trailer
        DrawRectangle(5, 38, 55, 72, WHITE); 
    EndTextureMode();
}

// POLICE: 40x65 base
void PreparePoliceTexture(RenderTexture2D target) {
    BeginTextureMode(target);
        ClearBackground(BLANK);
        
        // Body (White)
        DrawRectangle(0, 0, 40, 65, WHITE); 
        
        // Black sides to make it look like a police interceptor
        DrawRectangle(0, 15, 5, 35, BLACK);
        DrawRectangle(35, 15, 5, 35, BLACK);
        
        // Windows
        DrawRectangle(5, 12, 30, 12, SKYBLUE);
        
        // Siren Bar
        DrawRectangle(5, 30, 15, 8, RED);
        DrawRectangle(20, 30, 15, 8, BLUE);
        
        // Push-bar at front
        DrawRectangle(10, 0, 20, 4, BLACK);
    EndTextureMode();
}