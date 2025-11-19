// ma_nguon/man_hinh/ManHinhChonVatCan.cpp
#include <GL/freeglut.h>
#include <vector>
#include "../TrangThaiGame.h"
#include "../doi_tuong_3d/HopChuNhat3D.cpp"
#include "../doi_tuong_3d/KhoiLapPhuong3D.cpp"

extern int WINDOW_W;
extern int WINDOW_H;

int previewRotate = 0;

void initObstacleSelect()
{
    // nothing heavy to load
    previewRotate = 0;
}

void displayObstacleSelect()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 3D view for preview
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)WINDOW_W / WINDOW_H, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // camera
    gluLookAt(0, 0, 80, 0, 0, 0, 0, 1, 0);

    glPushMatrix();
    glRotatef(previewRotate, 0, 1, 0);
    // Draw options as 3 items: cube, rectangular prism, thin plate
    glPushMatrix();
    glTranslatef(-30, 0, 0);
    drawKhoiLapPhuong3D(0, 0, 0, 12.0f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0, 0, 0);
    drawHopChuNhat3D(0, 0, 0, 20.0f, 10.0f, 8.0f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(30, 0, 0);
    drawHopChuNhat3D(0, 0, 0, 6.0f, 18.0f, 6.0f);
    glPopMatrix();
    glPopMatrix();

    // 2D overlay for text
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_W, 0, WINDOW_H);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(1, 1, 1);
    glRasterPos2f(10, WINDOW_H - 20);
    std::string s = "Chon vat can: Click tren doi tuong (left click). B: tro ve Menu.";
    for (char c : s)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);

    glutSwapBuffers();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void idleObstacleSelect()
{
    previewRotate = (previewRotate + 1) % 360;
    glutPostRedisplay();
}

void mouseObstacleSelect(int button, int state, int x, int y)
{
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
        return;
    int yy = WINDOW_H - y;
    // map x,y to which item roughly (we showed -30,0,30 in world coords)
    // Simplified: split window into three vertical areas
    float third = WINDOW_W / 3.0f;
    if (x < third)
        selectedObstacleIndex = 0;
    else if (x < 2 * third)
        selectedObstacleIndex = 1;
    else
        selectedObstacleIndex = 2;
    // After selecting go to game
    trangThaiHienTai = CHOI_GAME;
}
