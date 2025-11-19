// ma_nguon/tien_ich/XuLyBanPhimChuoi.cpp
#include <GL/freeglut.h>

bool keyDown[256] = {false};
bool specialDown[256] = {false};

void keyboardDownFunc(unsigned char key, int x, int y)
{
    keyDown[key] = true;
}

void keyboardUpFunc(unsigned char key, int x, int y)
{
    keyDown[key] = false;
}

void specialDownFunc(int key, int x, int y)
{
    specialDown[key] = true;
}

void specialUpFunc(int key, int x, int y)
{
    specialDown[key] = false;
}
