#include "raylib.h"
#include "drawTextures.h"
#include "helpers.h"

const int weightRatio[3] = {5, 3, 2}; // 0: Cars, 1: Trucks, 2: Policecars
const Color defaultColors[5] = {LIGHTGRAY, DARKGRAY, BLUE, RED, ORANGE};

TYPE_OF_VEHICLE mapRandomToVehicleType(int random) {    
    if (0 <= random && random < weightRatio[0]) return CAR;
    else if (weightRatio[0] <= random && random < weightRatio[0] + weightRatio[1]) return TRUCK;
    return POLICE;
}

Color selectColor (TYPE_OF_VEHICLE selectedVehicle) {   
    if (selectedVehicle == POLICE) return WHITE;
    return defaultColors[GetRandomValue(0, sizeof(defaultColors) / sizeof(Color) - 1)];
}

bool willTouchBorder(Image image, Vector2 point) {
    if (point.x < 0 || point.y < 0 || point.x >= image.width || point.y >= image.height) return true;
    Color c = GetImageColor(image, (int)point.x, (int)point.y);
    if (c.r > 150 && c.g < 100 && c.b < 100) return true;
    return false;
}

void getVehicleSize(TYPE_OF_VEHICLE type, float *w, float *h) {
    if (type == TRUCK) {
        *w = 13.0f;
        *h = 22.0f; 
    } else {
        *w = 8.0f;
        *h = 13.0f;
    }
}

bool isVehiclePositionValid(Image image, float px, float py, TYPE_OF_VEHICLE type, int rotation) {
    float w, h;
    getVehicleSize(type, &w, &h);

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

void RenderVehicle(Vehicle v, RenderTexture2D carT, RenderTexture2D truckT, RenderTexture2D policeT) {
    Texture2D tex;
    Rectangle source;
    float w, h;

    // Dimensions derived from your original RenderVehicle logic
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