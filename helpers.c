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

void updateOrder(Order *currentOrder, Vector2 bikePos, int *count, float *totalMoney, Building *houses, int houseCount)   {
    if (currentOrder->isActive && currentOrder->foodPickedUp) {
        if (currentOrder->timeRemaining > 0) {
            currentOrder->timeRemaining -= GetFrameTime();
            float distToHouse = Vector2Distance(bikePos, currentOrder->dropoffLocation);
            float distRestToHouse = Vector2Distance(currentOrder->pickupLocation, currentOrder->dropoffLocation);
            
            if (distToHouse < 7.5f)    {
                (*count)++;
                float timePassed = currentOrder->maxTimeAllowed - currentOrder -> timeRemaining;
                float reward = currentOrder->initialReward - (currentOrder->initialReward / (currentOrder->maxTimeAllowed * currentOrder->maxTimeAllowed)) * (timePassed * timePassed);

                *totalMoney += reward;
                *currentOrder = CreateNewOrder();
            }
        } 
        else {
            currentOrder->timeRemaining = 0; // Time is up
            if (*totalMoney >= 20)  *totalMoney -= 20;
            else *totalMoney = 0;
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
            currentOrder->maxTimeAllowed = (distToHouse / 100.0f) * difficultyFactor + 10.0f;     
            currentOrder->timeRemaining = currentOrder->maxTimeAllowed;
        
        }
    }
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

void vehicleGenerator(int numOfVehicles, Vehicle vehicles[], int mapHeight, int mapWidth, Image mapWithBorders) {
    for (int i = 0; i < numOfVehicles; i++) {
        TYPE_OF_VEHICLE type = mapRandomToVehicleType(GetRandomValue(0, 10));
        bool found = false;
        float rx, ry;
        int rot = GetRandomValue(0, 3) * 90;

        while (!found) {
            rx = (float)GetRandomValue(100, mapWidth - 100);
            ry = (float)GetRandomValue(100, mapHeight - 100);

            if (isVehiclePositionValid(mapWithBorders, rx, ry, type, rot)) {
                found = true;
            }
        }

        vehicles[i].type = type;
        vehicles[i].posx = rx;
        vehicles[i].posy = ry;
        vehicles[i].vehicleColor = selectColor(type);
        vehicles[i].speed = (float)GetRandomValue(1, 2);
        vehicles[i].rotation = rot;
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

void updateTraffic(Vehicle *vehicles, int maxVehicles, Image mapWithBorders, Vector2 playerPos) {
    for (int i = 0; i < maxVehicles; i++) {
        
        float dx = vehicles[i].posx - playerPos.x;
        float dy = vehicles[i].posy - playerPos.y;
        // Compare squared distance to squared threshold (faster than sqrt)
        if ((dx*dx + dy*dy) < (STOPPING_DISTANCE * STOPPING_DISTANCE)) {
            continue; // Skip movement for this frame
        }

        float oldX = vehicles[i].posx;
        float oldY = vehicles[i].posy;

        // movement
        if (vehicles[i].rotation == 0)        vehicles[i].posy += vehicles[i].speed;
        else if (vehicles[i].rotation == 180) vehicles[i].posy -= vehicles[i].speed;
        else if (vehicles[i].rotation == 90)  vehicles[i].posx -= vehicles[i].speed;
        else if (vehicles[i].rotation == 270) vehicles[i].posx += vehicles[i].speed;

        // If move is invalid, revert and turn
        if (!isVehiclePositionValid(mapWithBorders, vehicles[i].posx, vehicles[i].posy, vehicles[i].type, vehicles[i].rotation)) {
            vehicles[i].posx = oldX;
            vehicles[i].posy = oldY;
            // Pick a new direction that isn't the one we just tried
            vehicles[i].rotation = GetRandomValue(0, 3) * 90;
        }
    }
}