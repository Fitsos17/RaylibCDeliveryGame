#include "raylib.h"
#include"drawTextures.c"

const int MAX_VEHICLES = 10; 
int weightRatio[3] = {5, 3, 2}; // 0:Cars, 1:Trucks, 2:Policecars

typedef enum {CAR, TRUCK, POLICE} TYPE_OF_VEHICLE;
Color defaultColors[5] = {LIGHTGRAY, DARKGRAY, BLUE, RED, ORANGE};

typedef struct {
    TYPE_OF_VEHICLE type;
    Color vehicleColor;
    float posx;
    float posy;
    float speed;
    int rotation; 
} Vehicle;

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

bool isAreaColliding(Image image, Rectangle rect) {
    // Check 4 corners + center to ensure the whole car fits
    Vector2 points[5] = {
        { rect.x, rect.y },                               // Top Left
        { rect.x + rect.width, rect.y },                  // Top Right
        { rect.x, rect.y + rect.height },                 // Bottom Left
        { rect.x + rect.width, rect.y + rect.height },    // Bottom Right
        { rect.x + rect.width/2, rect.y + rect.height/2 } // Center
    };

    for (int i = 0; i < 5; i++) {
        if (willTouchBorder(image, points[i])) return true;
    }
    return false;
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

void updateTraffic(Vehicle *vehicles, int maxVehicles, Image mapWithBorders) {
    for (int i = 0; i < maxVehicles; i++) {
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