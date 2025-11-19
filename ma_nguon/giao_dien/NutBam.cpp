// ma_nguon/giao_dien/NutBam.cpp
#include <GL/freeglut.h>
#include <string>
#include <functional>

struct Nut
{
    float x, y, w, h;
    std::string label;
    bool hovered;
    std::function<void()> onClick;
};

void drawTextSimple(float x, float y, const std::string &s)
{
    glRasterPos2f(x, y);
    for (char c : s)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

void drawButton(const Nut &n)
{
    glColor3f(n.hovered ? 0.2f : 0.1f, n.hovered ? 0.7f : 0.4f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(n.x, n.y);
    glVertex2f(n.x + n.w, n.y);
    glVertex2f(n.x + n.w, n.y + n.h);
    glVertex2f(n.x, n.y + n.h);
    glEnd();
    glColor3f(1, 1, 1);
    drawTextSimple(n.x + 10, n.y + n.h / 2 - 6, n.label);
}
