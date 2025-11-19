// ma_nguon/man_hinh/ManHinhMenu.cpp
#include <GL/freeglut.h>
#include <iostream>
#include "../TrangThaiGame.h"
#include "../ve_hinh/TaiAnhPNG.cpp"
#include "../ve_hinh/VeSprite2D.cpp"
#include "../giao_dien/NutBam.cpp"
#include "../am_thanh/TrinhPhatAmThanh.cpp"

// Globals for screen size
extern int WINDOW_W;
extern int WINDOW_H;

GLuint texBtnStart = 0;
GLuint texIconSoundOn = 0;
GLuint texIconSoundOff = 0;

void initMenu()
{
    texBtnStart = loadTexture("tai_nguyen/anh/giao_dien/nut_start.png");
    texIconSoundOn = loadTexture("tai_nguyen/anh/giao_dien/icon_am_thanh_bat.png");
    texIconSoundOff = loadTexture("tai_nguyen/anh/giao_dien/icon_am_thanh_tat.png");
}

void displayMenu()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_W, 0, WINDOW_H);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Title
    glColor3f(1, 1, 1);
    glRasterPos2f(WINDOW_W / 2 - 80, WINDOW_H - 80);
    std::string title = "DU AN GAME MAY BAY";
    for (char c : title)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

    // Draw Start button (use sprite or simple quad)
    float bw = 200, bh = 60;
    float bx = WINDOW_W / 2 - bw / 2;
    float by = WINDOW_H / 2;
    Nut startBtn = {bx, by, bw, bh, "START", false, []() { /* handled in mouse */ }};
    drawButton(startBtn);

    // Draw Equip button
    Nut equipBtn = {bx, by - 80, bw, bh, "TRANG BI", false, []() {}};
    drawButton(equipBtn);

    // Draw sound toggle icon
    if (musicOn)
    {
        drawSprite(texIconSoundOn, 20, WINDOW_H - 60, 48, 48);
    }
    else
    {
        drawSprite(texIconSoundOff, 20, WINDOW_H - 60, 48, 48);
    }

    // Small instructions
    glColor3f(1, 1, 1);
    glRasterPos2f(10, 10);
    std::string instr = "ESC de thoat. Nhap R de reset game (sau khi choi).";
    for (char c : instr)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);

    glutSwapBuffers();
}

// Mouse handling for menu - simple coordinate checks
void mouseMenu(int button, int state, int x, int y)
{
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
        return;
    int yy = WINDOW_H - y;
    float bw = 200, bh = 60;
    float bx = WINDOW_W / 2 - bw / 2;
    float by = WINDOW_H / 2;
    // START
    if (x >= bx && x <= bx + bw && yy >= by && yy <= by + bh)
    {
        trangThaiHienTai = CHOI_GAME;
    }
    // TRANG BI
    if (x >= bx && x <= bx + bw && yy >= by - 80 && yy <= by - 80 + bh)
    {
        trangThaiHienTai = CHON_MAY_BAY;
    }
    // sound icon
    if (x >= 20 && x <= 20 + 48 && yy >= WINDOW_H - 60 && yy <= WINDOW_H - 12)
    {
        toggleMusic();
    }
}
