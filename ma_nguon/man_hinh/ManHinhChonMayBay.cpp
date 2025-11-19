// ma_nguon/man_hinh/ManHinhChonMayBay.cpp
#include <GL/freeglut.h>
#include <vector>
#include <string>

extern int WINDOW_W;
extern int WINDOW_H;

// trạng thái màn hình (ví dụ MENU, CHON_MAY_BAY...)
extern int trangThaiHienTai;
extern const int MENU = 0;
extern const int CHON_MAY_BAY = 1;
extern const int CHON_VAT_CAN = 2;

// Máy bay tam giác
std::vector<std::string> planeNames = {"Mac dinh", "Xanh", "Do"};
int selectedPlaneIndex = 0;

// Background texture
GLuint bgTexture = 0;

// ------------------- Load background -------------------
#define STB_IMAGE_IMPLEMENTATION
#include "../thu_vien/stb_image.h"

GLuint loadTexture(const char *path)
{
    int width, height, channels;
    unsigned char *data = stbi_load(path, &width, &height, &channels, 4);
    if (!data)
        return 0;

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}

void loadBackground()
{
    bgTexture = loadTexture("tai_nguyen/anh/background/background.png");
}

// ------------------- Vẽ tam giác máy bay -------------------
void drawPlaneOption(float x, float y, int type)
{
    switch (type)
    {
    case 0:
        glColor3f(0.2f, 0.8f, 1.0f);
        break; // Mac dinh
    case 1:
        glColor3f(0.0f, 0.5f, 1.0f);
        break; // Xanh
    case 2:
        glColor3f(1.0f, 0.2f, 0.2f);
        break; // Do
    }
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 40);      // đỉnh
    glVertex2f(x - 20, y - 20); // trái dưới
    glVertex2f(x + 20, y - 20); // phải dưới
    glEnd();
}

// ------------------- Vẽ background -------------------
void drawBackground()
{
    if (bgTexture == 0)
        return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bgTexture);
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    glTexCoord2f(1, 0);
    glVertex2f(WINDOW_W, 0);
    glTexCoord2f(1, 1);
    glVertex2f(WINDOW_W, WINDOW_H);
    glTexCoord2f(0, 1);
    glVertex2f(0, WINDOW_H);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// ------------------- Hiển thị màn hình chọn máy bay -------------------
void displayPlaneSelect()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_W, 0, WINDOW_H);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawBackground();

    // Tiêu đề
    glColor3f(1, 1, 1);
    glRasterPos2f(20, WINDOW_H - 30);
    std::string t = "Chon may bay";
    for (char c : t)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

    // Vẽ các lựa chọn máy bay
    float startX = 150;
    float startY = WINDOW_H / 2;
    for (size_t i = 0; i < planeNames.size(); ++i)
    {
        float x = startX + i * 200;
        drawPlaneOption(x, startY, (int)i);

        // Vẽ tên máy bay
        glRasterPos2f(x - 30, startY - 40);
        std::string name = planeNames[i];
        for (char c : name)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    // Thông tin
    glRasterPos2f(10, 10);
    std::string info = "Click de chon. Back tro ve Menu: phai nhan 'b'.";
    for (char c : info)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);

    glutSwapBuffers();
}

// ------------------- Chuột chọn máy bay -------------------
void mousePlaneSelect(int button, int state, int x, int y)
{
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
        return;

    int yy = WINDOW_H - y;
    float startX = 150;
    float startY = WINDOW_H / 2;
    float w = 40, h = 60; // kích thước tam giác
    for (size_t i = 0; i < planeNames.size(); ++i)
    {
        float px = startX + i * 200;
        if (x >= px - 20 && x <= px + 20 && yy >= startY - 20 && yy <= startY + 40)
        {
            selectedPlaneIndex = (int)i;
            trangThaiHienTai = CHON_VAT_CAN; // tiếp tục chọn vật cản
        }
    }
}

// ------------------- Keyboard -------------------
void keyboardPlaneSelect(unsigned char key, int x, int y)
{
    if (key == 'b' || key == 'B')
    {
        trangThaiHienTai = MENU;
    }
}
