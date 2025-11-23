// logic.cpp

#include <GL/freeglut.h>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <iostream>
#include "game_state.h"

// ================== Utility ==================

bool inside(float mx, float my, const Button &b)
{
    return (mx >= b.x && mx <= b.x + b.w && my >= b.y && my <= b.y + b.h);
}

void screenToWorld(int sx, int sy, float &wx, float &wy)
{
    wx = (sx / (float)windowW) * 8.0f - 4.0f;
    wy = (1.0f - sy / (float)windowH) * 6.0f - 3.0f;
}

// save/load config
void saveConfig()
{
    ofstream f("config.cfg");
    if (!f)
        return;
    f << cfg.planeIdx << " " << cfg.obsIdx << "\n";
}
void loadConfig()
{
    ifstream f("config.cfg");
    if (!f)
        return;
    f >> cfg.planeIdx >> cfg.obsIdx;
}

// ================== Game Logic ==================

void spawnObstacle()
{
    Obstacle o;
    o.x = -3.6f + (rand() % 720) / 100.0f;
    o.y = 4.0f;
    o.speed = 0.01f + (rand() % 50) / 1000.0f;
    o.type = cfg.obsIdx;
    o.rotationAngle = (float)(rand() % 360);
    o.rotationSpeed = 0.5f + (rand() % 100) / 100.0f;
    obstacles.push_back(o);
}