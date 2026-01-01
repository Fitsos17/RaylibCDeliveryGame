#include "raylib.h"

const int MAX_VEHICLES = 10; 
int weightRatio[3] = {5, 3, 2}; // 0:Cars, 1:Trucks, 2:Policecars

typedef enum {CAR, TRUCK, POLICE} TYPE_OF_VEHICLE;
Color defaultColors[6] = {LIGHTGRAY, DARKGRAY, BLUE, RED, ORANGE, BLACK};

typedef struct {
    TYPE_OF_VEHICLE type;
    Color vehicleColor;
    int posx;
    int posy;
} Vehicle;

TYPE_OF_VEHICLE mapRandomToVehicleType(int random) {    // Select random vehicle type
    if (0 <= random && random < weightRatio[0]) return CAR;
    else if (weightRatio[0] <= random && random < weightRatio[0] + weightRatio[1]) return TRUCK;
    return POLICE;
}

Color selectColor (TYPE_OF_VEHICLE selectedVehicle) {   // Select random color
    if (selectedVehicle == POLICE) return BLACK;
    // get size of array - 1, because GetRandomValue is inclusive
    return defaultColors[GetRandomValue(0, sizeof(defaultColors) / sizeof(Color) - 1)];
}

void vehicleGenerator (int numOfVehicles, Vehicle vehicles[numOfVehicles], int mapHeight, int mapWidth) {
    // Draw vehicles (num is for number of vehicles, posx and posy initial coordinates)
    for (int i=0; i < numOfVehicles; i++) {
        int rand = GetRandomValue(0, MAX_VEHICLES - 1);
        TYPE_OF_VEHICLE selectedVehicleType = mapRandomToVehicleType(rand);
        Color selectedColor = selectColor(selectedVehicleType);
        int randomX = GetRandomValue (50, mapWidth - 50);
        int randomY = GetRandomValue (50, mapHeight - 50);
        
        vehicles[i] = (Vehicle){selectedVehicleType, selectedColor, randomX, randomY};
    }
}

void updateTraffic (Vehicle *vehicle, int maxVehicles, Camera2D cam, int mapHeight, int mapWidth) { 
    // Vehicles move to a random location when they go out of camera view
    Vector2 startOfWorld = GetScreenToWorld2D((Vector2){0,0}, cam);
    Vector2 endOfWorld = GetScreenToWorld2D((Vector2){(float)GetScreenWidth(), (float)GetScreenHeight()}, cam);
    
    float buffer = 50.0f; // Vehicles respawn only when they are completely out of camera view
    
    for (int i=0; i<maxVehicles; i++) {
        
        bool isOut = false;
        
        if (vehicle[i].posx < startOfWorld.x - buffer || vehicle[i].posx > endOfWorld.x + buffer)  isOut = true; 
        else if (vehicle[i].posy < startOfWorld.y - buffer || vehicle[i].posy > endOfWorld.y + buffer) isOut = true;
        
        if (isOut) {
            
            int rand = GetRandomValue(0, 3); // 0: Up, 1: Down, 2:Right, 3:Left
            
            switch(rand) {
                case 0:
                    vehicle[i].posx = cam.target.x;
                    vehicle[i].posy = cam.target.y - 200;
                    break;
                
                case 1:
                    vehicle[i].posx = cam.target.x;
                    vehicle[i].posy = cam.target.y + 200;
                    break;
                    
                case 2:
                    vehicle[i].posx = cam.target.x + 300;
                    vehicle[i].posy = cam.target.y;
                    break;
                    
                case 3:
                    vehicle[i].posx = cam.target.x - 300;
                    vehicle[i].posy = cam.target.y;
                    break;
            }
            
            // Check if respawned vehicle is out of bounds
            if (vehicle[i].posx < 20) vehicle[i].posx = 20; 
            if (vehicle[i].posx > mapWidth - 60) vehicle[i].posx = mapWidth - 60;
            
            if (vehicle[i].posy < 20) vehicle[i].posy = 20;
            if (vehicle[i].posy > mapHeight - 100) vehicle[i].posy = mapHeight - 100;
            
            vehicle[i].vehicleColor = selectColor(vehicle[i].type);
        }
    }
}