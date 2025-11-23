// drawing.cpp

#include <GL/freeglut.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "game_state.h"

#define _USE_MATH_DEFINES

// ============== Drawing Primitives ==============

void drawText(float x, float y, const string &s)
{
    glRasterPos2f(x, y);
    for (char c : s)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

// Hàm drawButton đã được sửa lỗi màu
void drawButton(const Button &b)
{
    // 1. Thiết lập màu nền nút (Xám nhạt: 0.8, 0.8, 0.8)
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(b.x, b.y);
    glVertex2f(b.x + b.w, b.y);
    glVertex2f(b.x + b.w, b.y + b.h);
    glVertex2f(b.x, b.y + b.h);
    glEnd();

    // 2. Thiết lập màu chữ (Đen: 0.0, 0.0, 0.0)
    glColor3f(0.0f, 0.0f, 0.0f);
    drawText(b.x + 0.15f, b.y + 0.15f, b.label);
}

void drawPlanePreview(int idx, float cx, float cy, float scale)
{
    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);
    glScalef(scale, scale, scale);

    float r = planeColors[idx][0];
    float g = planeColors[idx][1];
    float b = planeColors[idx][2];

    GLfloat mat_ambient[] = {r * 0.5f, g * 0.5f, b * 0.5f, 1.0f};
    GLfloat mat_diffuse[] = {r, g, b, 1.0f};
    GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat mat_shininess[] = {50.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glColor3f(r, g, b);
    glRotatef(180, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.5, 1.0, 10, 10);

    glColor3f(r * 0.8f, g * 0.8f, b * 0.8f);
    glPushMatrix();
    glTranslatef(-0.6f, -0.2f, 0.0f);
    glScalef(0.8f, 0.2f, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.6f, -0.2f, 0.0f);
    glScalef(0.8f, 0.2f, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

void drawObstaclePreview(int type, float cx, float cy, float scale)
{
    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);
    glScalef(scale, scale, scale);

    GLfloat mat_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat mat_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat mat_shininess[] = {20.0f};

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    if (type == 0)
    {
        GLfloat mat_diffuse[] = {0.8f, 0.5f, 0.1f, 1.0f};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glutSolidCube(0.8);
    }
    else if (type == 1)
    {
        GLfloat mat_diffuse[] = {0.3f, 0.6f, 0.9f, 1.0f};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glutSolidSphere(0.4, 20, 20);
    }
    else
    {
        GLfloat mat_diffuse[] = {0.95f, 0.9f, 0.2f, 1.0f};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glutSolidCone(0.4, 0.8, 10, 10);
    }

    glPopMatrix();
}

void drawObstacle(const Obstacle &o)
{
    glPushMatrix();
    glTranslatef(o.x, o.y, 0.0f);
    glRotatef(o.rotationAngle, 0.5f, 1.0f, 0.0f);

    GLfloat mat_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat mat_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat mat_shininess[] = {20.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    if (o.type == 0)
    {
        GLfloat mat_diffuse[] = {0.8f, 0.5f, 0.1f, 1.0f};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glutSolidCube(0.8);
    }
    else if (o.type == 1)
    {
        GLfloat mat_diffuse[] = {0.3f, 0.6f, 0.9f, 1.0f};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glutSolidSphere(0.4, 20, 20);
    }
    else
    {
        GLfloat mat_diffuse[] = {0.95f, 0.9f, 0.2f, 1.0f};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glutSolidCone(0.4, 0.8, 10, 10);
    }
    glPopMatrix();
}

void drawBullet(const Bullet &b)
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glVertex2f(b.x, b.y);
    glEnd();
}

// ========= Screen Renderers ===========

void renderMenu()
{
    glColor3f(0.2f, 0.6f, 0.2f);
    drawText(-2.0f, 2.0f, "SPACE SHOOTER - MENU");
    Button bStart{-1.5f, 0.6f, 3.0f, 0.6f, "START"};
    Button bEquip{-1.5f, -0.1f, 3.0f, 0.6f, "EQUIP"};
    Button bExit{-1.5f, -0.8f, 3.0f, 0.6f, "EXIT"};
    // glColor3f(0.8f, 0.8f, 0.8f); <--- Đã được xóa
    drawButton(bStart);
    drawButton(bEquip);
    drawButton(bExit);
    currentButtons.clear();
    currentButtons.push_back(bStart);
    currentButtons.push_back(bEquip);
    currentButtons.push_back(bExit);
}

void renderEquip()
{
    glColor3f(0.9f, 0.9f, 0.9f);
    drawText(-3.5f, 2.3f, "EQUIP - Choose Plane and Obstacle");
    Button bPlanes{-3.4f, 1.6f, 3.2f, 0.5f, "Chon May Bay"};
    Button bObs{-3.4f, 0.9f, 3.2f, 0.5f, "Chon Vat Can"};
    Button bBack{-3.4f, 0.2f, 3.2f, 0.5f, "BACK"};
    // glColor3f(0.8f, 0.8f, 0.8f); <--- Đã được xóa
    drawButton(bPlanes);
    drawButton(bObs);
    drawButton(bBack);
    glColor3f(1, 1, 1);
    drawText(0.2f, 2.0f, "Hien tai:");
    drawPlanePreview(cfg.planeIdx, 1.2f, 1.0f, 1.0f);
    drawObstaclePreview(cfg.obsIdx, 2.5f, 1.0f, 1.0f);
    currentButtons.clear();
    currentButtons.push_back(bPlanes);
    currentButtons.push_back(bObs);
    currentButtons.push_back(bBack);
}

void renderSelectPlane()
{
    drawText(-3.4f, 2.3f, "Chon may bay (click de chon)");
    float cx = -2.5f;
    currentButtons.clear();
    for (int i = 0; i < NUM_PLANES; i++)
    {
        float x = cx + i * 2.3f;
        Button b{x - 0.7f, 0.5f, 1.4f, 1.4f, string("Plane ") + to_string(i)};
        glColor3f(0.9f, 0.9f, 0.9f);
        drawButton(b);
        drawPlanePreview(i, x, 1.2f, 0.8f);
        currentButtons.push_back(b);
    }
    currentButtons.push_back({-3.4f, -1.4f, 6.8f, 0.5f, "BACK"});
}

void renderSelectObs()
{
    drawText(-3.4f, 2.3f, "Chon vat can (click de chon)");
    float cx = -2.5f;
    currentButtons.clear();
    for (int i = 0; i < NUM_OBS; i++)
    {
        float x = cx + i * 2.3f;
        Button b{x - 0.7f, 0.5f, 1.4f, 1.4f, string("Obs ") + to_string(i)};
        glColor3f(0.9f, 0.9f, 0.9f);
        drawButton(b);
        drawObstaclePreview(i, x, 1.2f, 0.8f);
        currentButtons.push_back(b);
    }
    currentButtons.push_back({-3.4f, -1.4f, 6.8f, 0.5f, "BACK"});
}

void renderGame()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)windowW / (double)windowH, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 7.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    GLfloat light_position[] = {1.0f, 2.0f, 3.0f, 0.0f};
    GLfloat light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glDisable(GL_LIGHTING);
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
    glVertex3f(-4.0f, -3.8f, 0.0f);
    glVertex3f(4.0f, -3.8f, 0.0f);
    glVertex3f(4.0f, -3.8f, -1.0f);
    glVertex3f(-4.0f, -3.8f, -1.0f);
    glEnd();
    glEnable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -1.0f);
    drawPlanePreview(cfg.planeIdx, playerX, playerY, 1.0f);
    glPopMatrix();

    for (auto &o : obstacles)
    {
        drawObstacle(o);
    }

    glDisable(GL_LIGHTING);
    for (const auto &b : bullets)
    {
        drawBullet(b);
    }
    glEnable(GL_LIGHTING);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-4, 4, -3.5, 3.5, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1, 1, 1);
    drawText(-3.8f, 3.6f, "Score: " + to_string(score));
    drawText(-3.8f, 3.3f, "Press P/Click to Pause, ESC to Menu. SPACE to SHOOT.");

    if (paused)
    {
        glColor3f(0.8f, 0.8f, 0.2f);
        drawText(-1.0f, 0.0f, "PAUSED (P/Click to Resume)");
    }
    else if (gameOver)
    {
        glColor3f(1.0f, 0.2f, 0.2f);
        drawText(-2.0f, 0.5f, "GAME OVER!");
        drawText(-2.0f, 0.0f, "Final Score: " + to_string(score));
        drawText(-2.0f, -0.5f, "Press R to Restart or ESC to Menu.");
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
}