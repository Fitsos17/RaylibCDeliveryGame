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
#include "raymath.h"
#include <string.h>
#include "helpers.h"

const int weightRatio[3] = {5, 3, 2}; // 0:Cars, 1:Trucks, 2:Policecars
    
const Color defaultColors[5] = {LIGHTGRAY, DARKGRAY, BLUE, RED, ORANGE};

const char* restaurantNames[MAX_RESTAURANTS] = {
    "Pizzeria Antonio",
    "Papa Nick's Burger House",
    "Hoy Ming Sushi",
    "Gyros 101",
    "Pizza-Pasta Di Italia",
    "Street food on the go",
    "Mark's Fish and Chips 1965",
    "Big Patty Burgers"
};

Building restaurants[MAX_RESTAURANTS];
int restaurantCount;

Building houses[MAX_HOUSES];
int houseCount;

// Scans map for restaurants and houses
void InitMapLocations(Image map) {
    restaurantCount = 0;
    houseCount = 0;
    
    Color *pixels = LoadImageColors(map);
    
    for (int y = 0; y < map.height; y += 5) {
        for (int x = 0; x < map.width; x += 5) {
            Color c = GetImageColor(map, x, y);
            
            // Scan for restaurants (GREEN)
            if (c.g > 200 && c.r < 100 && c.b < 100) {
                bool exists = false;
            
                for (int i = 0; i < restaurantCount; i++) {
                    if (Vector2Distance((Vector2){(float)x, (float)y}, restaurants[i].pos) < minDistance) {
                        exists = true;
                        break;
                    }
                }
                if (!exists && restaurantCount < MAX_RESTAURANTS) {
                    restaurants[restaurantCount].pos = (Vector2){(float)x, (float)y};
                    strcpy(restaurants[restaurantCount].name, restaurantNames[restaurantCount]);
                    restaurantCount++;
                }
            }   
        
            // Scan for houses (BLUE)
            if (c.b > 200 && c.g < 30) {
                bool exists = false;
                for (int i = 0; i < houseCount; i++) {
                    if (Vector2Distance((Vector2){(float)x, (float)y}, houses[i].pos) < minDistance) {
                        exists = true;
                        break;
                    }
                }
                if (!exists && houseCount < MAX_HOUSES) {
                    houses[houseCount].pos = (Vector2){(float)x, (float)y};
                    houseCount++;
                }
            }
        }
    }
    
    UnloadImageColors(pixels);
}

Order CreateNewOrder()  {
    Order newOrder = {0};
    
    // Get random restaurant
    int restaurantIndex = GetRandomValue(0, restaurantCount - 1);
    newOrder.pickupLocation = restaurants[restaurantIndex].pos;
        
    strcpy(newOrder.restaurantName, restaurants[restaurantIndex].name);
        
    // Get random house
    int houseIndex = GetRandomValue(0, houseCount - 1);
    newOrder.dropoffLocation = (Vector2){0, 0};
        
    newOrder.isActive = true;
    newOrder.foodPickedUp = false;
    return newOrder;
}

void updateOrder(Order *currentOrder, Vector2 bikePos, int *count, float *totalMoney, Building *houses, int houseCount, OrderStatusMessage *message, float *lastReward)   {
    if (currentOrder->isActive && currentOrder->foodPickedUp) {
        if (currentOrder->timeRemaining > 0) {
            currentOrder->timeRemaining -= GetFrameTime();
            float distToHouse = Vector2Distance(bikePos, currentOrder->dropoffLocation);
            float distRestToHouse = Vector2Distance(currentOrder->pickupLocation, currentOrder->dropoffLocation);
            
            if (distToHouse < 7.5f)    {
                (*count)++;
                float timeFraction = currentOrder->timeRemaining / currentOrder->maxTimeAllowed;
                float reward = currentOrder->initialReward * timeFraction;
                
                *totalMoney += reward;
                *lastReward = reward;
                message->messageType = SUCCESS;
                message->timer = DISPLAY_MESSAGE_TIME;
                *currentOrder = CreateNewOrder();
            }
        } 
        else {
            currentOrder->timeRemaining = 0; // Time is up
            if (*totalMoney >= 5)  {
                *lastReward = -5;
                *totalMoney -= 5;
            }
            else {
                *lastReward = 0;
                *totalMoney = 0;
            }

            message->messageType = FAILURE;
            message->timer = DISPLAY_MESSAGE_TIME;
            *currentOrder = CreateNewOrder();
            
        }
    }
    
    if (currentOrder->isActive && !currentOrder->foodPickedUp)    {
        float distToRestaurant = Vector2Distance(bikePos, currentOrder->pickupLocation);
        
        if (distToRestaurant < 7.5f)   { // If close to restaurant
            currentOrder->foodPickedUp = true;
        
            int randomHouse = GetRandomValue(0, houseCount - 1);
            currentOrder->dropoffLocation = houses[randomHouse].pos;
            
            float distToHouse = Vector2Distance(currentOrder->pickupLocation, currentOrder->dropoffLocation);
            
            currentOrder->initialReward = 5.0f + (distToHouse * 0.02f);
            currentOrder->maxTimeAllowed = (distToHouse / 100.0f) * difficultyFactor + 11.0f;     
            currentOrder->timeRemaining = currentOrder->maxTimeAllowed;
            
        }
    }
}

void displayOrderMessage(OrderStatusMessage *message, float lastReward) {
    if (message->messageType == FAILURE) {
        // Calculate positions to center text
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();
        
        int fontSizeMain = 40;
        int fontSizeSub = 20;
        
        // Draw a dark semi-transparent background for readability
        DrawRectangle(0, screenH / 2 - 50, screenW, 100, Fade(BLACK, 0.6f));
        if (lastReward == 0) {
            const char* header = "DELIVERY FAILED!";
            const char* subtext = "You have no money left. WORK HARDER!!";
            int textWidthMain = MeasureText(header, fontSizeMain);
            int textWidthSub = MeasureText(subtext, fontSizeSub);
            
            // Draw the red failure text
            DrawText(header, screenW / 2 - textWidthMain / 2, screenH / 2 - 30, fontSizeMain, RED);
            DrawText(subtext, screenW / 2 - textWidthSub / 2, screenH / 2 + 15, fontSizeSub, WHITE);
        } else {
            const char* header = "DELIVERY FAILED!";
            const char* subtext = "The company charged you $5.00 for failing";
            int textWidthMain = MeasureText(header, fontSizeMain);
            int textWidthSub = MeasureText(subtext, fontSizeSub);
            
            // Draw the red failure text
            DrawText(header, screenW / 2 - textWidthMain / 2, screenH / 2 - 30, fontSizeMain, RED);
            DrawText(subtext, screenW / 2 - textWidthSub / 2, screenH / 2 + 15, fontSizeSub, WHITE);
        }
        message->timer -= GetFrameTime();
    }

    else if (message->messageType == SUCCESS) {
        const char* header = "DELIVERY SUCCESSFUL!";
        
        const char* subtext = TextFormat("You earned: $%.2f", lastReward);
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();
        
        int fontSizeMain = 45; // Slightly larger for success
        int fontSizeSub = 25;
        
        int textWidthMain = MeasureText(header, fontSizeMain);
        int textWidthSub = MeasureText(subtext, fontSizeSub);

        // Dark green semi-transparent bar
        DrawRectangle(0, screenH / 2 - 60, screenW, 120, Fade(DARKGREEN, 0.7f));
        
        // Extra "glow" border for the success bar
        DrawRectangleLinesEx((Rectangle){0, (float)screenH / 2 - 60, (float)screenW, 120}, 3, GOLD);

        // Text
        DrawText(header, screenW / 2 - textWidthMain / 2, screenH / 2 - 40, fontSizeMain, LIME);
        DrawText(subtext, screenW / 2 - textWidthSub / 2, screenH / 2 + 10, fontSizeSub, GOLD);
        message->timer -= GetFrameTime();

    }

    if (message->timer <= 0) {
        message->messageType = PENDING;
    }
}

bool DrawButton(const char *text, Rectangle rec, int fontSize, Color color, Color hoverColor, Color textColor) {
    bool pressed = false;
    Vector2 mousePoint = GetMousePosition();
    bool isHover = CheckCollisionPointRec(mousePoint, rec);

    // Draw Button Background
    if (isHover) {
        DrawRectangleRec(rec, hoverColor);
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            pressed = true;
        }
    } else {
        DrawRectangleRec(rec, color);
    }
    
    // Draw Border
    DrawRectangleLinesEx(rec, 2, BLACK);

    // Center Text
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, rec.x + (rec.width - textWidth) / 2, rec.y + (rec.height - fontSize) / 2, fontSize, textColor);

    return pressed;
}

TYPE_OF_VEHICLE mapRandomToVehicleType(int random) {    // Select random vehicle type
    if (0 <= random && random < weightRatio[0]) return CAR;
    else if (weightRatio[0] <= random && random < weightRatio[0] + weightRatio[1]) return TRUCK;
    return POLICE;
}

Color selectColor (TYPE_OF_VEHICLE selectedVehicle) {   // Select random color
    if (selectedVehicle == POLICE) return WHITE;
    // get size of array - 1, because GetRandomValue is inclusive
    return defaultColors[GetRandomValue(0, sizeof(defaultColors) / sizeof(Color) - 1)];
}

bool willTouchBorder(Image image, Vector2 point) {
    // Safety check to prevent crashing if coordinates are off-map
    if (point.x < 0 || point.y < 0 || point.x >= image.width || point.y >= image.height) return true;

    Color c = GetImageColor(image, (int)point.x, (int)point.y);
    
    // If the pixel is "Too Red" 
    if (c.r > 150 && c.g < 100 && c.b < 100) return true;
    return false;
}

void getVehicleSize(TYPE_OF_VEHICLE type, float *w, float *h) {
    float s = 0.2f; // Your scale constant
    if (type == TRUCK) {
        *w = 55.0f * s;
        *h = 110.0f * s; 
    } else {
        *w = 40.0f * s;
        *h = 65.0f * s;
    }
}

bool isVehiclePositionValid(Image image, float px, float py, TYPE_OF_VEHICLE type, int rotation) {
    // Scaled dimensions for collision check
    float w = (type == TRUCK) ? 11.0f : 8.0f;
    float h = (type == TRUCK) ? 22.0f : 13.0f;

    // Swap box dimensions if horizontal
    if (rotation == 90 || rotation == 270) {
        float temp = w; w = h; h = temp;
    }

    Vector2 corners[4] = {
        { px - w/2, py - h/2 }, { px + w/2, py - h/2 },
        { px - w/2, py + h/2 }, { px + w/2, py + h/2 }
    };

    // check center
    if (willTouchBorder(image, (Vector2){px, py})) return false;

    for (int i = 0; i < 4; i++) {
        if (willTouchBorder(image, corners[i])) return false;
    }
    return true;
}

// Global drawing helper using DrawTexturePro
void RenderVehicle(Vehicle v, RenderTexture2D carT, RenderTexture2D truckT, RenderTexture2D policeT) {
    Texture2D tex;
    Rectangle source;
    float w, h;

    if (v.type == TRUCK) {
        tex = truckT.texture;
        source = (Rectangle){ 0, 0, 65, 110 }; 
        w = 13.0f; h = 22.0f; 
    } else {
        tex = (v.type == POLICE) ? policeT.texture : carT.texture;
        source = (Rectangle){ 0, 0, 40, 65 };
        w = 8.0f; h = 13.0f;
    }

    Rectangle dest = { v.posx, v.posy, w, h };
    Vector2 origin = { w / 2, h / 2 };

    DrawTexturePro(tex, source, dest, origin, (float)v.rotation, v.vehicleColor);
}

void vehicleGenerator(int numOfVehicles, Vehicle vehicles[], int mapHeight, int mapWidth, Image mapWithBorders, Vector2 playerStartPos) {
    for (int i = 0; i < numOfVehicles; i++) {
        TYPE_OF_VEHICLE type = mapRandomToVehicleType(GetRandomValue(0, 10));
        bool found = false;
        float rx, ry;
        int rotation;

        int attempts = 0;
        
        while (!found && attempts < 1000) { // Safety break to prevent infinite loops
            rx = (float)GetRandomValue(100, mapWidth - 100);
            ry = (float)GetRandomValue(100, mapHeight - 100);
            rotation = GetRandomValue(0, 3) * 90;

            // Check distance to player (Safe Zone of 250 pixels)
            float distToPlayer = Vector2Distance((Vector2){rx, ry}, playerStartPos);

            if (distToPlayer > 250.0f && isVehiclePositionValid(mapWithBorders, rx, ry, type, rotation)) {
                found = true;
            }
            attempts++;
        }

        vehicles[i].type = type;
        vehicles[i].posx = rx;
        vehicles[i].posy = ry;
        vehicles[i].vehicleColor = selectColor(type);
        vehicles[i].speed = (float)GetRandomValue(8, 16) / 10.0f;
        vehicles[i].rotation = rotation;
    }
}

void updateTraffic(Vehicle *vehicles, int maxVehicles, Image mapWithBorders, Vector2 playerPos) {
    for (int i = 0; i < maxVehicles; i++) {
        
        // Collision with player logic (Stop if close)
        float dx = vehicles[i].posx - playerPos.x;
        float dy = vehicles[i].posy - playerPos.y;
        if ((dx*dx + dy*dy) < (STOPPING_DISTANCE * STOPPING_DISTANCE)) {
            continue; 
        }

        float oldX = vehicles[i].posx;
        float oldY = vehicles[i].posy;

        // Apply movement
        if (vehicles[i].rotation == 0)        vehicles[i].posy += vehicles[i].speed;
        else if (vehicles[i].rotation == 180) vehicles[i].posy -= vehicles[i].speed;
        else if (vehicles[i].rotation == 90)  vehicles[i].posx -= vehicles[i].speed;
        else if (vehicles[i].rotation == 270) vehicles[i].posx += vehicles[i].speed;

        // If the new move is invalid (hit a wall)
        if (!isVehiclePositionValid(mapWithBorders, vehicles[i].posx, vehicles[i].posy, vehicles[i].type, vehicles[i].rotation)) {
            
            // 1. Reset position immediately so they don't clip into the wall
            vehicles[i].posx = oldX;
            vehicles[i].posy = oldY;

            // 2. Try to find a valid direction instead of purely random
            // Current rotation
            int currentRot = vehicles[i].rotation;
            
            // Define relative turns: [Left, Right, Back]
            // We favor turning 90 degrees over going fully backwards
            int candidates[3];
            candidates[0] = (currentRot + 90) % 360;  // Turn Right
            candidates[1] = (currentRot + 270) % 360; // Turn Left
            candidates[2] = (currentRot + 180) % 360; // Reverse

            bool directionFound = false;

            // Check which of these directions is free
            for (int d = 0; d < 3; d++) {
                int testRot = candidates[d];
                float testX = oldX;
                float testY = oldY;
                float lookAhead = 10.0f; // Look a bit ahead to see if it's clear

                if (testRot == 0) testY += lookAhead;
                else if (testRot == 180) testY -= lookAhead;
                else if (testRot == 90) testX -= lookAhead;
                else if (testRot == 270) testX += lookAhead;

                if (isVehiclePositionValid(mapWithBorders, testX, testY, vehicles[i].type, testRot)) {
                    vehicles[i].rotation = testRot;
                    directionFound = true;
                    break; // Stop looking, we found a path
                }
            }

            // If completely stuck (boxed in), pick a random one as a last resort
            if (!directionFound) {
                 vehicles[i].rotation = candidates[GetRandomValue(0, 2)];
            }
        }
    }
}

bool checkCollisionWithVehicles(Rectangle playerRect, Vehicle *vehicles, int maxVehicles, bool useMargin) {
    
    Rectangle playerBox = {
        playerRect.x - playerRect.width/2,
        playerRect.y - playerRect.height/2,
        playerRect.width,
        playerRect.height
    };

    // Only apply the "shrink" margin if we are checking for movement.
    // If we are checking for Sound/Impact, we want the full size.
    if (useMargin) {
        float margin = 4.0f; 
        playerBox.x += margin;
        playerBox.y += margin;
        playerBox.width -= (margin * 2);
        playerBox.height -= (margin * 2);
    }

    for (int i = 0; i < maxVehicles; i++) {
        float w, h;
        getVehicleSize(vehicles[i].type, &w, &h);

        if (vehicles[i].rotation == 90 || vehicles[i].rotation == 270) {
            float temp = w; w = h; h = temp;
        }

        Rectangle npcBox = {
            vehicles[i].posx - w/2,
            vehicles[i].posy - h/2,
            w,
            h
        };

        if (CheckCollisionRecs(playerBox, npcBox)) {
            return true;
        }
    }
    return false;
}

Vector2 GetRandomValidPosition(Image map, Vehicle *vehicles, int maxVehicles, int mapWidth, int mapHeight) {
    int attempts = 0;
    while (attempts < 1000) {
        float rx = (float)GetRandomValue(100, mapWidth - 100);
        float ry = (float)GetRandomValue(100, mapHeight - 100);
        
        // 1. Check if inside border
        if (willTouchBorder(map, (Vector2){rx, ry})) {
            attempts++;
            continue;
        }
        
        // 2. Check if overlapping with any existing vehicle
        bool hitVehicle = false;
        for(int i=0; i<maxVehicles; i++) {
            if (Vector2Distance((Vector2){rx, ry}, (Vector2){vehicles[i].posx, vehicles[i].posy}) < 50.0f) {
                hitVehicle = true;
                break;
            }
        }
        
        if (!hitVehicle) return (Vector2){rx, ry};
        
        attempts++;
    }
    
    return (Vector2){ (float)mapWidth/2, (float)mapHeight/2 }; // Fallback
}