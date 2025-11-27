#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- CẤU HÌNH MÀU SẮC ---
const float COLOR_RED[] = {0.95f, 0.33f, 0.27f};
const float COLOR_WHITE[] = {0.90f, 0.90f, 0.90f};
const float COLOR_BROWN[] = {0.35f, 0.22f, 0.15f};
const float COLOR_SKY[] = {0.97f, 0.85f, 0.67f, 1.0f};
const float COLOR_SEA[] = {0.41f, 0.76f, 0.75f};
const float COLOR_GOLD[] = {1.0f, 0.84f, 0.0f};
const float COLOR_ENEMY[] = {0.3f, 0.3f, 0.3f};

// --- STRUCT ---
struct GameObject
{
    float x, y, z;
    bool active;
    float rotation;
    float scale;
};

// --- BIẾN TOÀN CỤC ---
float planeY = 100.0f;
float seaRotation = 0.0f;
float propellerAngle = 0.0f;
float coinSpinAngle = 0.0f;
float wavePhase = 0.0f;
bool isGameOver = false;

// ** BIẾN CAMERA MỚI (ĐIỀU KHIỂN BẰNG MŨI TÊN) **
float cameraAngle = 0.0f;    // Góc xoay quanh máy bay (Trái/Phải)
float cameraHeight = 150.0f; // Độ cao camera (Lên/Xuống)
float cameraDist = 250.0f;   // Khoảng cách từ camera đến tâm

int score = 0;
int spawnTimer = 0;
int coinSpawnTimer = 0;

std::vector<GameObject> bullets;
std::vector<GameObject> enemies;
std::vector<GameObject> coins;

// --- CẤU HÌNH GAMEPLAY ---
float fuel = 100.0f;
const float maxFuel = 100.0f;
const float COMMON_SIZE = 4.0f;
const float COIN_THICKNESS = 1.0f;

float gameSpeed = 0.6f;
const float MAX_SPEED = 2.5f;
const float SPEED_INC = 0.0005f;

const int COIN_SEQUENCE_MIN = 3;
const int COIN_SEQUENCE_MAX = 5;
const float SHOOT_FUEL_COST = 2.0f;
const float COIN_FUEL_GAIN = 10.0f;
const float HIT_ENEMY_FUEL_LOSS = 20.0f;

// --- SETUP MÔI TRƯỜNG ---
void setupEnvironment()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_FLAT);

    GLfloat lightPos[] = {150.0f, 350.0f, 150.0f, 1.0f};
    GLfloat ambient[] = {0.4f, 0.4f, 0.4f, 1.0f};
    GLfloat diffuse[] = {0.9f, 0.9f, 0.9f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    glEnable(GL_FOG);
    glFogfv(GL_FOG_COLOR, COLOR_SKY);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 200.0f);
    glFogf(GL_FOG_END, 950.0f);
}

// --- RESET GAME ---
void resetGame()
{
    isGameOver = false;
    fuel = 100.0f;
    score = 0;
    planeY = 100.0f;
    gameSpeed = 0.6f;

    // Reset camera về mặc định
    cameraAngle = 0.0f;
    cameraHeight = 150.0f;

    bullets.clear();
    enemies.clear();
    coins.clear();
}

// --- VẼ UI ---
void drawText(const char *text, float x, float y, void *font = GLUT_BITMAP_HELVETICA_18)
{
    glRasterPos2f(x, y);
    for (const char *c = text; *c != '\0'; c++)
    {
        glutBitmapCharacter(font, *c);
    }
}

void drawUI()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if (!isGameOver)
    {
        glColor3f(0.35f, 0.22f, 0.15f);
        std::string scoreStr = "Score: " + std::to_string(score);
        drawText(scoreStr.c_str(), 20, 560);

        // Hướng dẫn mới
        drawText("Move: Mouse | Fire: Space", 20, 530);
        drawText("Camera: Arrow Keys", 20, 510); // Hướng dẫn Camera

        std::string speedStr = "Speed: " + std::to_string((int)(gameSpeed * 100)) + "%";
        drawText(speedStr.c_str(), 20, 480);

        const float marginX = 20.0f;
        const float marginY = 450.0f;
        const float barWidth = 200.0f;
        const float barHeight = 20.0f;

        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(marginX, marginY);
        glVertex2f(marginX + barWidth, marginY);
        glVertex2f(marginX + barWidth, marginY + barHeight);
        glVertex2f(marginX, marginY + barHeight);
        glEnd();

        if (fuel > 0)
        {
            float currentWidth = (fuel / maxFuel) * (barWidth - 4);
            if (fuel > 50)
                glColor3f(0.3f, 0.8f, 0.3f);
            else if (fuel > 20)
                glColor3f(0.9f, 0.8f, 0.2f);
            else
                glColor3f(0.9f, 0.2f, 0.2f);

            glBegin(GL_QUADS);
            glVertex2f(marginX + 2, marginY + 2);
            glVertex2f(marginX + 2 + currentWidth, marginY + 2);
            glVertex2f(marginX + 2 + currentWidth, marginY + barHeight - 2);
            glVertex2f(marginX + 2, marginY + barHeight - 2);
            glEnd();
        }
        glColor3f(0.35f, 0.22f, 0.15f);
        drawText("FUEL", marginX, marginY - 15, GLUT_BITMAP_HELVETICA_12);
    }
    else
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(800, 0);
        glVertex2f(800, 600);
        glVertex2f(0, 600);
        glEnd();
        glDisable(GL_BLEND);

        glColor3f(1.0f, 0.3f, 0.3f);
        drawText("GAME OVER", 320, 350, GLUT_BITMAP_TIMES_ROMAN_24);

        glColor3f(1.0f, 1.0f, 1.0f);
        std::string finalScore = "Final Score: " + std::to_string(score);
        drawText(finalScore.c_str(), 335, 320);

        glColor3f(1.0f, 1.0f, 0.8f);
        drawText("Press 'R' to Play Again", 300, 250);
        drawText("Press 'ESC' to Quit", 310, 220);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

// --- VẼ MÁY BAY ---
void drawPlane()
{
    glPushMatrix();
    glTranslatef(-50.0f, planeY, 0.0f);
    glScalef(0.25f, 0.25f, 0.25f);

    if (isGameOver)
    {
        glRotatef(-45.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(propellerAngle * 0.5f, 1.0f, 0.0f, 0.0f);
    }
    else
    {
        glRotatef(sin(seaRotation * 0.1f) * 5.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(sin(seaRotation * 0.05f) * 5.0f, 0.0f, 0.0f, 1.0f);
    }

    glColor3fv(COLOR_RED);
    glPushMatrix();
    glScalef(2.0f, 1.0f, 1.0f);
    glutSolidCube(40.0);
    glPopMatrix();
    glColor3fv(COLOR_WHITE);
    glPushMatrix();
    glTranslatef(40.0f, 0.0f, 0.0f);
    glutSolidCube(40.0);
    glPopMatrix();
    glColor3fv(COLOR_RED);
    glPushMatrix();
    glTranslatef(-70.0f, 15.0f, 0.0f);
    glScalef(0.5f, 0.5f, 0.2f);
    glutSolidCube(40.0);
    glPopMatrix();
    glColor3fv(COLOR_RED);
    glPushMatrix();
    glTranslatef(0.0f, 10.0f, 0.0f);
    glScalef(0.8f, 0.1f, 4.0f);
    glutSolidCube(40.0);
    glPopMatrix();
    glColor3fv(COLOR_BROWN);
    glPushMatrix();
    glTranslatef(62.0f, 0.0f, 0.0f);
    glRotatef(propellerAngle, 1.0f, 0.0f, 0.0f);
    glScalef(0.1f, 2.5f, 0.3f);
    glutSolidCube(35.0);
    glPopMatrix();
    glPopMatrix();
}

// --- VẼ BIỂN ---
void drawSea()
{
    glPushMatrix();
    glTranslatef(0.0f, -600.0f, 0.0f);
    glRotatef(-seaRotation, 0.0f, 0.0f, 1.0f);

    int slices = 40;
    int stacks = 20;
    float radiusBase = 600.0f;
    float length = 800.0f;
    float zStart = -400.0f;

    glColor3fv(COLOR_SEA);
    glBegin(GL_QUADS);
    for (int i = 0; i < slices; i++)
    {
        float angle1 = (float)i / slices * 2.0f * M_PI;
        float angle2 = (float)(i + 1) / slices * 2.0f * M_PI;
        for (int j = 0; j < stacks; j++)
        {
            float z1 = zStart + (float)j / stacks * length;
            float z2 = zStart + (float)(j + 1) / stacks * length;

            auto getRadius = [&](float a, float z)
            {
                return radiusBase + sin(a * 7.0f + wavePhase) * 10.0f + cos(z * 0.05f + wavePhase * 1.5f) * 8.0f;
            };

            float r1 = getRadius(angle1, z1);
            float r2 = getRadius(angle2, z1);
            float r3 = getRadius(angle2, z2);
            float r4 = getRadius(angle1, z2);

            glNormal3f(cos(angle1), sin(angle1), 0.0f);
            glVertex3f(cos(angle1) * r1, sin(angle1) * r1, z1);
            glNormal3f(cos(angle2), sin(angle2), 0.0f);
            glVertex3f(cos(angle2) * r2, sin(angle2) * r2, z1);
            glNormal3f(cos(angle2), sin(angle2), 0.0f);
            glVertex3f(cos(angle2) * r3, sin(angle2) * r3, z2);
            glNormal3f(cos(angle1), sin(angle1), 0.0f);
            glVertex3f(cos(angle1) * r4, sin(angle1) * r4, z2);
        }
    }
    glEnd();
    glPopMatrix();
}

void drawCoinShape(float radius, float thickness)
{
    GLUquadricObj *q = gluNewQuadric();
    gluCylinder(q, radius, radius, thickness, 20, 1);
    gluDisk(q, 0.0f, radius, 20, 1);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, thickness);
    gluDisk(q, 0.0f, radius, 20, 1);
    glPopMatrix();
    gluDeleteQuadric(q);
}

void drawEntities()
{
    glColor3f(1.0f, 0.9f, 0.2f);
    for (const auto &b : bullets)
    {
        glPushMatrix();
        glTranslatef(b.x, b.y, b.z);
        glScalef(2.0f, 0.5f, 0.5f);
        glutSolidSphere(3.0, 8, 8);
        glPopMatrix();
    }

    glColor3fv(COLOR_ENEMY);
    for (const auto &e : enemies)
    {
        glPushMatrix();
        glTranslatef(e.x, e.y, e.z);
        glRotatef(e.rotation, 1.0f, 1.0f, 0.0f);
        float scaleFactor = COMMON_SIZE / 1.7f;
        glScalef(scaleFactor, scaleFactor, scaleFactor);
        glutSolidDodecahedron();
        glPopMatrix();
    }

    glColor3fv(COLOR_GOLD);
    for (const auto &c : coins)
    {
        glPushMatrix();
        glTranslatef(c.x, c.y, c.z);
        glRotatef(coinSpinAngle, 0.0f, 1.0f, 0.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, -COIN_THICKNESS / 2.0f);
        drawCoinShape(COMMON_SIZE, COIN_THICKNESS);
        glPopMatrix();
    }
}

void spawnCoinSequence(float startX)
{
    int count = COIN_SEQUENCE_MIN + (rand() % (COIN_SEQUENCE_MAX - COIN_SEQUENCE_MIN + 1));
    float gap = 15.0f;
    float baseY = 80.0f + (rand() % 100);

    for (int i = 0; i < count; ++i)
    {
        GameObject c;
        c.x = startX + i * gap;
        c.y = baseY + (rand() % 10 - 5);
        c.z = (rand() % 50) - 25.0f;
        c.rotation = 0.0f;
        c.active = true;
        c.scale = 1.0f;
        coins.push_back(c);
    }
}

// --- LOGIC GAME ---
void update(int value)
{
    if (isGameOver)
    {
        if (planeY > -100.0f)
        {
            planeY -= 2.0f;
            propellerAngle += 10.0f;
        }
        glutPostRedisplay();
        glutTimerFunc(16, update, 0);
        return;
    }

    if (gameSpeed < MAX_SPEED)
        gameSpeed += SPEED_INC;

    propellerAngle += 25.0f;
    seaRotation += (1.0f * gameSpeed);
    wavePhase += (0.08f * gameSpeed);
    coinSpinAngle += 3.0f;

    fuel -= (0.05f * gameSpeed);
    if (fuel <= 0.0f)
    {
        fuel = 0.0f;
        isGameOver = true;
    }

    spawnTimer++;
    float spawnThreshold = 50.0f / gameSpeed;
    if (spawnThreshold < 20.0f)
        spawnThreshold = 20.0f;

    if (spawnTimer > spawnThreshold)
    {
        GameObject e;
        e.x = 250.0f;
        e.y = (rand() % 140) + 30.0f;
        e.z = (rand() % 60) - 30.0f;
        e.rotation = 0.0f;
        e.active = true;
        e.scale = 1.0f;
        enemies.push_back(e);
        spawnTimer = 0;
    }

    coinSpawnTimer++;
    if (coinSpawnTimer > (90.0f / gameSpeed))
    {
        spawnCoinSequence(200.0f + (rand() % 60 - 30));
        coinSpawnTimer = 0;
    }

    for (auto &b : bullets)
        b.x += (8.0f * gameSpeed);
    for (auto &e : enemies)
    {
        e.x -= (3.0f * gameSpeed);
        e.rotation += 5.0f;
    }
    for (auto &c : coins)
        c.x -= (3.0f * gameSpeed);

    float collisionRadius = COMMON_SIZE * 3.0f;

    for (auto &e : enemies)
    {
        if (!e.active)
            continue;
        for (auto &b : bullets)
        {
            if (!b.active)
                continue;
            float dist = sqrtf(pow(e.x - b.x, 2) + pow(e.y - b.y, 2));
            if (dist < collisionRadius)
            {
                e.active = false;
                b.active = false;
                score += 10;
            }
        }
        float distPlane = sqrtf(pow(e.x - (-50.0f), 2) + pow(e.y - planeY, 2));
        if (distPlane < collisionRadius + 15.0f)
        {
            e.active = false;
            fuel -= HIT_ENEMY_FUEL_LOSS;
            if (fuel <= 0.0f)
            {
                fuel = 0.0f;
                isGameOver = true;
            }
        }
    }

    for (auto &c : coins)
    {
        if (!c.active)
            continue;
        float distC = sqrtf(pow(c.x - (-50.0f), 2) + pow(c.y - planeY, 2));
        if (distC < collisionRadius + 15.0f)
        {
            c.active = false;
            score += 5;
            fuel += COIN_FUEL_GAIN;
            if (fuel > maxFuel)
                fuel = maxFuel;
        }
    }

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const GameObject &b)
                                 { return !b.active || b.x > 400.0f; }),
                  bullets.end());
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const GameObject &e)
                                 { return !e.active || e.x < -220.0f; }),
                  enemies.end());
    coins.erase(std::remove_if(coins.begin(), coins.end(), [](const GameObject &c)
                               { return !c.active || c.x < -220.0f; }),
                coins.end());

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// --- INPUT CHUỘT (DI CHUYỂN MÁY BAY) ---
void mouseMotion(int x, int y)
{
    if (isGameOver)
        return;

    float normalizedY = (float)y / 600.0f;
    float targetY = 200.0f - (normalizedY * 180.0f);
    planeY += (targetY - planeY) * 0.1f;
}

// --- INPUT PHÍM MŨI TÊN (ĐIỀU KHIỂN CAMERA) ---
void specialKeys(int key, int x, int y)
{
    if (isGameOver)
        return;

    const float angleSpeed = 5.0f;
    const float heightSpeed = 10.0f;

    switch (key)
    {
    case GLUT_KEY_LEFT:
        cameraAngle -= angleSpeed;
        break;
    case GLUT_KEY_RIGHT:
        cameraAngle += angleSpeed;
        break;
    case GLUT_KEY_UP:
        cameraHeight += heightSpeed;
        break;
    case GLUT_KEY_DOWN:
        cameraHeight -= heightSpeed;
        break;
    }
}

// --- INPUT BÀN PHÍM THƯỜNG ---
void keyboard(unsigned char key, int x, int y)
{
    if (!isGameOver)
    {
        if (key == 32)
        { // Space - Bắn
            if (fuel >= SHOOT_FUEL_COST && fuel > 0.0f)
            {
                GameObject b;
                b.x = -35.0f;
                b.y = planeY;
                b.z = 0.0f;
                b.active = true;
                bullets.push_back(b);
                fuel -= SHOOT_FUEL_COST;
            }
        }
    }
    else
    {
        if (key == 'r' || key == 'R')
        {
            resetGame();
        }
    }

    if (key == 27)
        exit(0);
}

// --- HÀM HIỂN THỊ (CẬP NHẬT CAMERA ĐỘNG) ---
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Tính toán vị trí camera dựa trên góc và độ cao
    // Sử dụng công thức lượng giác để xoay camera quanh tâm
    float rad = cameraAngle * M_PI / 180.0f;

    // Mặc định Z = 250, xoay quanh trục Y
    float eyeX = sin(rad) * cameraDist;
    float eyeZ = cos(rad) * cameraDist;

    gluLookAt(eyeX, cameraHeight, eyeZ, // Vị trí Mắt (Eye)
              0.0f, planeY, 0.0f,       // Nhìn vào máy bay
              0.0f, 1.0f, 0.0f);        // Hướng lên

    drawSea();
    drawPlane();
    drawEntities();
    drawUI();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    if (h == 0)
        h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / h, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    srand((unsigned int)time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("The Aviator - Arrow Key Camera Control");

    glClearColor(COLOR_SKY[0], COLOR_SKY[1], COLOR_SKY[2], 1.0f);
    setupEnvironment();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutPassiveMotionFunc(mouseMotion);

    glutKeyboardFunc(keyboard);   // Phím thường (Space, R, Esc)
    glutSpecialFunc(specialKeys); // Phím đặc biệt (Mũi tên)

    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}