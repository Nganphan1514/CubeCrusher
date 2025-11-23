// main.cpp

#include <GL/freeglut.h>
#include <iostream>
#include <algorithm> // Bổ sung: Cho std::remove_if, std::max/min
#include <cmath>     // Bổ sung: Cho std::fabs

#include "game_state.h"
#include "input.h"

using namespace std;

// ================== Globals Definitions ==================
int windowW = 900, windowH = 650;
Screen cur = SCR_MENU;
Config cfg = {0, 0};

bool paused = false;
bool gameOver = false;
int score = 0;

bool keyLeftDown = false;
bool keyRightDown = false;

float playerX = 0.0f;
float playerY = -3.5f;
float playerSpeed = 0.1f;

vector<Obstacle> obstacles;
vector<Bullet> bullets;
vector<Button> currentButtons;

float planeColors[NUM_PLANES][3] = {{0.9f, 0.2f, 0.2f}, {0.2f, 0.8f, 0.2f}, {0.2f, 0.4f, 0.9f}};

// =========== Main display =============
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (cur == SCR_MENU)
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-4, 4, -3.5, 3.5, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        renderMenu();
    }
    else if (cur == SCR_EQUIP)
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-4, 4, -3.5, 3.5, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        renderEquip();
    }
    else if (cur == SCR_SELECT_PLANE)
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-4, 4, -3.5, 3.5, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        renderSelectPlane();
    }
    else if (cur == SCR_SELECT_OBS)
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-4, 4, -3.5, 3.5, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        renderSelectObs();
    }
    else if (cur == SCR_PLAY)
    {
        renderGame();
    }

    glutSwapBuffers();
}

// ========== Idle / Timer: update game world ===========
void timer(int)
{
    if (cur == SCR_PLAY)
    {
        if (!paused && !gameOver)
        {
            // Cập nhật Di chuyển Player
            if (keyLeftDown)
                playerX = std::max(-3.6f, playerX - playerSpeed);
            if (keyRightDown)
                playerX = std::min(3.6f, playerX + playerSpeed);

            // 1. Update Bullets
            for (auto &b : bullets)
            {
                b.y += b.speed;
            }
            bullets.erase(remove_if(bullets.begin(), bullets.end(), [](const Bullet &b)
                                    { return b.y > 4.0f; }),
                          bullets.end());

            // 2. Update Obstacles
            for (auto &o : obstacles)
            {
                o.y -= o.speed;
                o.rotationAngle += o.rotationSpeed;
                if (o.rotationAngle > 360.0f)
                    o.rotationAngle -= 360.0f;
            }

            obstacles.erase(remove_if(obstacles.begin(), obstacles.end(), [&](const Obstacle &o)
                                      { 
                                         if (o.y < -4.5f) {
                                             score += 10;
                                             return true; 
                                         }
                                         return false; }),
                            obstacles.end());

            if ((rand() % 100) < 4)
                spawnObstacle();

            // 3. Collision Checks

            // Va chạm Player vs Obstacle
            for (auto &o : obstacles)
            {
                if (fabs(o.x - playerX) < (0.5f + 0.4f) &&
                    fabs(o.y - playerY) < (0.5f + 0.4f))
                {
                    gameOver = true;
                    break;
                }
            }
            // Va chạm Bullet vs Obstacle
            for (auto bit = bullets.begin(); bit != bullets.end();)
            {
                bool hit = false;
                for (auto oit = obstacles.begin(); oit != obstacles.end();)
                {
                    if (fabs(bit->x - oit->x) < 0.45f && fabs(bit->y - oit->y) < 0.45f)
                    {
                        oit = obstacles.erase(oit);
                        score += 50;
                        hit = true;
                        break;
                    }
                    else
                    {
                        ++oit;
                    }
                }
                if (hit)
                {
                    bit = bullets.erase(bit);
                }
                else
                {
                    ++bit;
                }
            }
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void init()
{
    loadConfig();
    glClearColor(0.05f, 0.05f, 0.08f, 1);

    // Kích hoạt các tính năng 3D
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    GLfloat global_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowW, windowH);
    glutCreateWindow("Space Shooter - 3D Rotating Obstacles");

    init();
    glutDisplayFunc(display);

    // Input Handlers
    glutMouseFunc(mouseClick);
    glutSpecialFunc(specialKeyDown);
    glutSpecialUpFunc(specialKeyUp);
    glutKeyboardFunc(keyboard);

    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}