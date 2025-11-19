// ma_nguon/main.cpp
#include <GL/freeglut.h>
#include <iostream>
#include "TrangThaiGame.h"

// Window size globals
int WINDOW_W = 800;
int WINDOW_H = 600;

#include "man_hinh/ManHinhMenu.cpp"
#include "man_hinh/ManHinhChonMayBay.cpp"
#include "man_hinh/ManHinhChonVatCan.cpp"
#include "man_hinh/ManHinhChoiGame.cpp"
#include "tien_ich/XuLyBanPhimChuoi.cpp"

// Forward declarations for callbacks depending on state
void displayRouter();
void mouseRouter(int button, int state, int x, int y);
void keyboardRouter(unsigned char key, int x, int y);
void keyboardUpRouter(unsigned char key, int x, int y);
void specialDownRouter(int key, int x, int y);
void specialUpRouter(int key, int x, int y);
void idleRouter();

void initAll()
{
    initMenu();
    initPlaneSelect();
    initObstacleSelect();
    initGame();
}

void displayRouter()
{
    switch (trangThaiHienTai)
    {
    case MENU:
        displayMenu();
        break;
    case CHON_MAY_BAY:
        displayPlaneSelect();
        break;
    case CHON_VAT_CAN:
        displayObstacleSelect();
        break;
    case CHOI_GAME:
        drawGame();
        break;
    }
}

void mouseRouter(int button, int state, int x, int y)
{
    switch (trangThaiHienTai)
    {
    case MENU:
        mouseMenu(button, state, x, y);
        break;
    case CHON_MAY_BAY:
        mousePlaneSelect(button, state, x, y);
        break;
    case CHON_VAT_CAN:
        mouseObstacleSelect(button, state, x, y);
        break;
    case CHOI_GAME: /* could implement shooting with mouse */
        break;
    }
}

void keyboardRouter(unsigned char key, int x, int y)
{
    keyboardDownFunc(key, x, y);
    switch (trangThaiHienTai)
    {
    case MENU:
        break;
    case CHON_MAY_BAY:
        keyboardPlaneSelect(key, x, y);
        break;
    case CHON_VAT_CAN:
        if (key == 'b' || key == 'B')
            trangThaiHienTai = MENU;
        break;
    case CHOI_GAME:
        if (key == 'r' || key == 'R')
        {
            initGame();
            trangThaiHienTai = MENU;
        }
        break;
    }
    if (key == 27)
        exit(0);
}

void keyboardUpRouter(unsigned char key, int x, int y)
{
    keyboardUpFunc(key, x, y);
}

void specialDownRouter(int key, int x, int y)
{
    specialDownFunc(key, x, y);
}

void specialUpRouter(int key, int x, int y)
{
    specialUpFunc(key, x, y);
}

void idleRouter()
{
    if (trangThaiHienTai == CHON_VAT_CAN)
        idleObstacleSelect();
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_W, WINDOW_H);
    glutCreateWindow("DuAnGameMayBay");

    glEnable(GL_DEPTH_TEST);

    initAll();

    glutDisplayFunc(displayRouter);
    glutMouseFunc(mouseRouter);
    glutKeyboardFunc(keyboardRouter);
    glutKeyboardUpFunc(keyboardUpRouter);
    glutSpecialFunc(specialDownRouter);
    glutSpecialUpFunc(specialUpRouter);
    glutIdleFunc(idleRouter);
    glutTimerFunc(16, timerGame, 0);

    glutMainLoop();
    return 0;
}
