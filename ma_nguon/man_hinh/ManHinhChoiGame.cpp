// ma_nguon/man_hinh/ManHinhChoiGame.cpp
#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include "../TrangThaiGame.h"
#include "../ve_hinh/TaiAnhPNG.cpp"
#include "../ve_hinh/VeSprite2D.cpp"
#include "../ve_hinh/Ve3D.cpp"
#include "../tien_ich/XuLyBanPhimChuoi.cpp"

extern int WINDOW_W;
extern int WINDOW_H;

// Game objects
struct Bullet
{
    float x, y;
    bool active;
};
struct Obstacle
{
    float x, y, z;
    float sx, sy, sz;
    bool active;
};

std::vector<Bullet> bullets;
std::vector<Obstacle> obstacles;
GLuint planeTex = 0;
float planeX = 400, planeY = 80;
float planeW = 60, planeH = 48;
float playerSpeed = 8.0f;

void initGame()
{
    bullets.clear();
    obstacles.clear();
    planeTex = loadTexture("tai_nguyen/anh/may_bay/maybay_macdinh.png");
    planeX = WINDOW_W / 2 - planeW / 2;
    planeY = 50;
    // spawn a few obstacles (3D) with chosen type
    for (int i = 0; i < 5; i++)
    {
        Obstacle o;
        o.x = (i % 5) * 120.0f + 60.0f;
        o.y = WINDOW_H + i * 100.0f;
        o.z = 0.0f;
        o.sx = 20 + (i % 3) * 10;
        o.sy = 20 + (i % 2) * 15;
        o.sz = 10 + (i % 2) * 5;
        o.active = true;
        obstacles.push_back(o);
    }
}

void updateGameLogic()
{
    // input move
    if (keyDown[GLUT_KEY_LEFT] || keyDown['a'])
    {
        planeX -= playerSpeed;
        if (planeX < 0)
            planeX = 0;
    }
    if (keyDown[GLUT_KEY_RIGHT] || keyDown['d'])
    {
        planeX += playerSpeed;
        if (planeX > WINDOW_W - planeW)
            planeX = WINDOW_W - planeW;
    }
    // shoot space
    static float cooldown = 0.0f;
    cooldown -= 0.016f;
    if (keyDown[' '])
    {
        if (cooldown <= 0.0f)
        {
            Bullet b;
            b.x = planeX + planeW / 2;
            b.y = planeY + planeH;
            b.active = true;
            bullets.push_back(b);
            cooldown = 0.2f;
        }
    }
    // update bullets
    for (auto &b : bullets)
        if (b.active)
        {
            b.y += 10.0f;
            if (b.y > WINDOW_H)
                b.active = false;
        }
    // update obstacles: move down in Z? here we move in Y towards player
    for (auto &o : obstacles)
        if (o.active)
        {
            o.y -= 1.5f; // falling
            if (o.y < -50)
                o.active = false;
        }
    // collisions: simple AABB 2D vs box projected -> approximate by distance
    for (auto &b : bullets)
        if (b.active)
        {
            for (auto &o : obstacles)
                if (o.active)
                {
                    float dx = b.x - o.x;
                    float dy = b.y - o.y;
                    if (std::sqrt(dx * dx + dy * dy) < 30.0f)
                    {
                        b.active = false;
                        o.active = false;
                    }
                }
        }
}

void drawGame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 3D view for obstacles
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)WINDOW_W / WINDOW_H, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(WINDOW_W / 2, WINDOW_H / 2, 600, WINDOW_W / 2, WINDOW_H / 2, 0, 0, 1, 0);

    // Draw obstacles as 3D boxes positioned in screen coords (transform)
    for (auto &o : obstacles)
        if (o.active)
        {
            // Map screen coords to world coords (simple mapping)
            float wx = o.x - WINDOW_W / 2;
            float wy = o.y - WINDOW_H / 2;
            glPushMatrix();
            glTranslatef(wx, wy, 0);
            drawBox3D(0, 0, 0, o.sx, o.sy, o.sz);
            glPopMatrix();
        }

    // 2D overlay for plane and bullets
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_W, 0, WINDOW_H);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // draw plane
    drawSprite(planeTex, planeX, planeY, planeW, planeH);

    // draw bullets as yellow quads
    glColor3f(1, 1, 0);
    glBegin(GL_QUADS);
    for (auto &b : bullets)
        if (b.active)
        {
            glVertex2f(b.x - 3, b.y);
            glVertex2f(b.x + 3, b.y);
            glVertex2f(b.x + 3, b.y + 10);
            glVertex2f(b.x - 3, b.y + 10);
        }
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glutSwapBuffers();
}

void timerGame(int)
{
    if (trangThaiHienTai != CHOI_GAME)
        return;
    updateGameLogic();
    glutPostRedisplay();
    glutTimerFunc(16, timerGame, 0);
}
