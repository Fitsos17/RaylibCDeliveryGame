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

#ifndef HELPERS_H
#define HELPERS_H

#include"raylib.h"

// constants
#define MAX_VEHICLES 20
#define MAX_RESTAURANTS  8
#define MAX_HOUSES 20
#define minDistance 30 // Distance for colored pixels to be considered as one building
#define STOPPING_DISTANCE 20.0f
#define DISPLAY_MESSAGE_TIME 2.0f
extern const int weightRatio[3]; 
extern const Color defaultColors[5];
extern const char* restaurantNames[MAX_RESTAURANTS];

// integers
extern int restaurantCount;
extern int houseCount;
extern float difficultyFactor;


// type defs
typedef enum { STATE_MENU, STATE_GAMEPLAY, STATE_OPTIONS, STATE_GAME_OVER } GameState;

typedef enum { PENDING, SUCCESS, FAILURE } TypeOfMessage;

typedef struct {
    float timer;
    TypeOfMessage messageType;
} OrderStatusMessage;

typedef struct  {
    Vector2 pos;
    char name[50];
} Building;

extern Building houses[MAX_HOUSES];

typedef struct {
    Vector2 pickupLocation;
    Vector2 dropoffLocation;
    char restaurantName[50];
    bool isActive;
    bool foodPickedUp;
    float initialReward;
    float timeRemaining;
    float maxTimeAllowed;
} Order;

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
bool DrawButton(const char *text, Rectangle rec, int fontSize, Color color, Color hoverColor, Color textColor);
void InitMapLocations (Image map);
Order CreateNewOrder();
void updateOrder(Order *currentOrder, Vector2 bikePos, int *count, float *totalMoney, Building *houses, int houseCount, OrderStatusMessage *message, float *lastReward);
TYPE_OF_VEHICLE mapRandomToVehicleType(int random);
Color selectColor (TYPE_OF_VEHICLE selectedVehicle);
bool willTouchBorder(Image image, Vector2 point);
void getVehicleSize(TYPE_OF_VEHICLE type, float *w, float *h);
bool isVehiclePositionValid(Image image, float px, float py, TYPE_OF_VEHICLE type, int rotation);
void RenderVehicle(Vehicle v, RenderTexture2D carT, RenderTexture2D truckT, RenderTexture2D policeT);
void vehicleGenerator(int numOfVehicles, Vehicle vehicles[], int mapHeight, int mapWidth, Image mapWithBorders, Vector2 playerStartPos);
bool checkCollisionWithVehicles(Rectangle playerRect, Vehicle *vehicles, int maxVehicles, bool useMargin);
void updateTraffic(Vehicle *vehicles, int maxVehicles, Image mapWithBorders, Vector2 playerPos);
void displayOrderMessage(OrderStatusMessage *message, float lastReward);
Vector2 GetRandomValidPosition(Image map, Vehicle *vehicles, int maxVehicles, int mapWidth, int mapHeight);

#endif