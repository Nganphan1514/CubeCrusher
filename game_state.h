// game_state.h

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <vector>
#include <string>

using namespace std;

// ================== Globals ==================
extern int windowW;
extern int windowH;

enum Screen
{
    SCR_MENU,
    SCR_EQUIP,
    SCR_SELECT_PLANE,
    SCR_SELECT_OBS,
    SCR_PLAY
};
extern Screen cur;

struct Config
{
    int planeIdx;
    int obsIdx;
};
extern Config cfg;

const int NUM_PLANES = 3;
const int NUM_OBS = 3;

// Game State Globals
extern bool paused;
extern bool gameOver;
extern int score;

// Input Flags
extern bool keyLeftDown;
extern bool keyRightDown;

// Player state
extern float playerX;
extern float playerY;
extern float playerSpeed;

// Structures
struct Obstacle
{
    float x, y;
    float speed;
    int type;
    float rotationAngle;
    float rotationSpeed;
};
extern vector<Obstacle> obstacles;

struct Bullet
{
    float x, y;
    float speed;
};
extern vector<Bullet> bullets;

struct Button
{
    float x, y, w, h;
    string label;
};
extern vector<Button> currentButtons;

// Colors
extern float planeColors[NUM_PLANES][3];

// --- Utilities & Config (logic.cpp) ---
bool inside(float mx, float my, const Button &b);
void screenToWorld(int sx, int sy, float &wx, float &wy);
void saveConfig();
void loadConfig();

// --- Drawing & Render (drawing.cpp) ---
void drawText(float x, float y, const string &s);
void drawButton(const Button &b);
void drawPlanePreview(int idx, float cx, float cy, float scale);
void drawObstaclePreview(int type, float cx, float cy, float scale);
void drawObstacle(const Obstacle &o);
void drawBullet(const Bullet &b);
void renderMenu();
void renderEquip();
void renderSelectPlane();
void renderSelectObs();
void renderGame();

// --- Game Logic (logic.cpp) ---
void spawnObstacle();

// --- Core Functions (main.cpp) ---
void display();
void timer(int);
void init();

#endif // GAME_STATE_H