// input.cpp

#include <GL/freeglut.h>
#include <algorithm>
#include "game_state.h"
#include <string>

// ============ Input handling - Mouse ============

void mouseClick(int button, int state, int sx, int sy)
{
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
        return;
    float wx, wy;
    screenToWorld(sx, sy, wx, wy);

    for (size_t i = 0; i < currentButtons.size(); ++i)
    {
        if (inside(wx, wy, currentButtons[i]))
        {
            string lbl = currentButtons[i].label;
            if (cur == SCR_MENU)
            {
                if (lbl == "START")
                {
                    bullets.clear();
                    obstacles.clear();
                    score = 0;
                    gameOver = false;
                    paused = false;
                    playerX = 0;
                    playerY = -3.5f;
                    spawnObstacle();
                    cur = SCR_PLAY;
                }
                else if (lbl == "EQUIP")
                {
                    cur = SCR_EQUIP;
                }
                else if (lbl == "EXIT")
                {
                    exit(0);
                }
            }
            else if (cur == SCR_EQUIP)
            {
                if (lbl == "Chon May Bay")
                {
                    cur = SCR_SELECT_PLANE;
                }
                else if (lbl == "Chon Vat Can")
                {
                    cur = SCR_SELECT_OBS;
                }
                else if (lbl == "BACK")
                {
                    cur = SCR_MENU;
                }
            }
            else if (cur == SCR_SELECT_PLANE)
            {
                if (lbl.rfind("Plane", 0) == 0)
                {
                    cfg.planeIdx = (int)stoi(lbl.substr(6));
                    saveConfig();
                    cur = SCR_EQUIP;
                }
                else if (lbl == "BACK")
                {
                    cur = SCR_EQUIP;
                }
            }
            else if (cur == SCR_SELECT_OBS)
            {
                if (lbl.rfind("Obs", 0) == 0)
                {
                    cfg.obsIdx = (int)stoi(lbl.substr(4));
                    saveConfig();
                    cur = SCR_EQUIP;
                }
                else if (lbl == "BACK")
                {
                    cur = SCR_EQUIP;
                }
            }
            glutPostRedisplay();
            return;
        }
    }

    if (cur == SCR_PLAY && !gameOver)
    {
        paused = !paused;
    }

    glutPostRedisplay();
}

// ============ Input handling - Special Keys Down/Up ============

void specialKeyDown(int key, int, int)
{
    if (cur == SCR_PLAY && !paused && !gameOver)
    {
        if (key == GLUT_KEY_LEFT)
        {
            keyLeftDown = true;
        }
        if (key == GLUT_KEY_RIGHT)
        {
            keyRightDown = true;
        }
    }
    glutPostRedisplay();
}

void specialKeyUp(int key, int, int)
{
    if (key == GLUT_KEY_LEFT)
    {
        keyLeftDown = false;
    }
    if (key == GLUT_KEY_RIGHT)
    {
        keyRightDown = false;
    }
    glutPostRedisplay();
}

// ============ Input handling - Keyboard ============

void keyboard(unsigned char key, int, int)
{
    if (key == 27)
    {
        if (cur == SCR_PLAY)
        {
            cur = SCR_MENU;
            gameOver = false;
            paused = false;
            bullets.clear();
            obstacles.clear();
        }
        else
        {
            exit(0);
        }
    }

    if (cur == SCR_PLAY)
    {
        if (key == 'p' || key == 'P')
        {
            if (!gameOver)
            {
                paused = !paused;
            }
        }

        if (gameOver && (key == 'r' || key == 'R'))
        {
            bullets.clear();
            obstacles.clear();
            playerX = 0.0f;
            score = 0;
            gameOver = false;
            paused = false;
            spawnObstacle();
        }

        if ((key == ' ') && !paused && !gameOver)
        {
            Bullet newBullet;
            newBullet.x = playerX;
            newBullet.y = playerY + 0.5f;
            newBullet.speed = 0.15f;
            bullets.push_back(newBullet);
        }
    }
    glutPostRedisplay();
}