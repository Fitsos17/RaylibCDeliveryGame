#ifndef HELPERS_H
#define HELPERS_H

#include"raylib.h"

// constants
#define MAX_VEHICLES 10
extern const int weightRatio[3]; 
#define STOPPING_DISTANCE 20.0f
extern const Color defaultColors[5];

// type defs
typedef enum { CAR, TRUCK, POLICE } TYPE_OF_VEHICLE;
typedef struct {
    TYPE_OF_VEHICLE type;
    Color vehicleColor;
    float posx;
    float posy;
    float speed;
    int rotation; 
} Vehicle;

// functions
TYPE_OF_VEHICLE mapRandomToVehicleType(int random);
Color selectColor (TYPE_OF_VEHICLE selectedVehicle);
bool willTouchBorder(Image image, Vector2 point);
void getVehicleSize(TYPE_OF_VEHICLE type, float *w, float *h);
bool isVehiclePositionValid(Image image, float px, float py, TYPE_OF_VEHICLE type, int rotation);
void RenderVehicle(Vehicle v, RenderTexture2D carT, RenderTexture2D truckT, RenderTexture2D policeT);
void vehicleGenerator(int numOfVehicles, Vehicle vehicles[], int mapHeight, int mapWidth, Image mapWithBorders);
bool checkCollisionWithVehicles(Rectangle playerRect, Vehicle *vehicles, int maxVehicles, bool useMargin);
void updateTraffic(Vehicle *vehicles, int maxVehicles, Image mapWithBorders, Vector2 playerPos);

#endif