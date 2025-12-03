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
const float COLOR_SKY_DAY[] = {0.97f, 0.85f, 0.67f, 1.0f};
const float COLOR_SKY_NIGHT[] = {0.1f, 0.1f, 0.25f, 1.0f};

const float LIGHT_DAY_DIFFUSE[] = {0.9f, 0.9f, 0.9f, 1.0f};
const float LIGHT_NIGHT_DIFFUSE[] = {0.3f, 0.3f, 0.5f, 1.0f};

const float LIGHT_DAY_AMBIENT[] = {0.4f, 0.4f, 0.4f, 1.0f};
const float LIGHT_NIGHT_AMBIENT[] = {0.1f, 0.1f, 0.2f, 1.0f};

const float COLOR_SEA[] = {0.41f, 0.76f, 0.75f};
const float COLOR_ENEMY[] = {0.3f, 0.3f, 0.3f};
const float COLOR_GOLD[] = {1.0f, 0.84f, 0.0f};

// --- MÀU MÁY BAY ---
const float COLOR_PLANE_RED[] = {0.80f, 0.15f, 0.15f};
const float COLOR_PLANE_GRAY[] = {0.55f, 0.55f, 0.58f};
const float COLOR_BLACK_NEW[] = {0.0f, 0.0f, 0.0f};
const float COLOR_BROWN_NEW[] = {0.30f, 0.15f, 0.10f};
const float COLOR_SKIN[] = {0.90f, 0.75f, 0.6f};

// --- STRUCT ---
struct GameObject
{
    float x, y, z;
    bool active;
    float rotation;
    float scale;
    float vy;
    int state;
};

struct Particle
{
    float x, y, z;
    float vx, vy, vz;
    float life;
    float r, g, b;
    float size;
};

// --- BIẾN TOÀN CỤC ---
float planeY = 100.0f;
float seaRotation = 0.0f;
float propellerAngle = 0.0f;
float coinSpinAngle = 0.0f;
float wavePhase = 0.0f;
bool isGameOver = false;
bool isPaused = false;

int viewMode = 0;
float dayCycle = 0.0f;
float damageFlashAlpha = 0.0f;

// ** CAMERA **
float cameraAngle = 0.0f;
float cameraHeight = 150.0f;
float cameraDist = 250.0f;

int score = 0;
int spawnTimer = 0;
int coinSpawnTimer = 0;
int fishSpawnTimer = 0;
int cloudSpawnTimer = 0;

std::vector<GameObject> bullets;
std::vector<GameObject> enemies;
std::vector<GameObject> coins;
std::vector<GameObject> fishes;
std::vector<GameObject> clouds;
std::vector<Particle> particles;

// --- GAMEPLAY CONFIG (ĐÃ CHỈNH CHẬM LẠI) ---
float fuel = 100.0f;
const float maxFuel = 100.0f;
const float COMMON_SIZE = 4.0f;
const float COIN_THICKNESS = 1.0f;

// [EDIT] Tốc độ game khởi điểm chậm hơn (0.6 -> 0.3)
float gameSpeed = 0.3f;
const float MAX_SPEED = 2.0f;
// [EDIT] Tốc độ tăng độ khó chậm hơn (0.0005 -> 0.0001)
const float SPEED_INC = 0.0001f;

const int COIN_SEQUENCE_MIN = 3;
const int COIN_SEQUENCE_MAX = 5;
const float SHOOT_FUEL_COST = 2.0f;
const float COIN_FUEL_GAIN = 10.0f;
const float HIT_ENEMY_FUEL_LOSS = 20.0f;

// --- CONFIG CÁ (ĐÃ CHỈNH CHẬM LẠI) ---
const int FISH_JUMP_CHANCE = 40;
// [EDIT] Trọng lực giảm để cá rơi chậm như bay (0.25 -> 0.15)
const float FISH_GRAVITY = 0.15f;
// [EDIT] Lực nhảy giảm để phù hợp trọng lực mới (7.0 -> 5.0)
const float FISH_JUMP_POWER_BASE = 5.0f;
const float FISH_WATER_LEVEL = -5.0f;

// HÀM TRỘN MÀU
void mixColors(const float c1[], const float c2[], float t, float out[])
{
    for (int i = 0; i < 3; i++)
    {
        out[i] = c1[i] * (1.0f - t) + c2[i] * t;
    }
    out[3] = 1.0f;
}

// --- SETUP ---
void setupEnvironment()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_FLAT);

    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 400.0f);
    glFogf(GL_FOG_END, 1200.0f);
}

// --- RESET ---
void resetGame()
{
    isGameOver = false;
    isPaused = false;
    fuel = 100.0f;
    score = 0;
    planeY = 100.0f;

    // [EDIT] Reset về tốc độ chậm
    gameSpeed = 0.3f;

    damageFlashAlpha = 0.0f;
    dayCycle = 0.0f;
    viewMode = 0;

    cameraAngle = 0.0f;
    cameraHeight = 150.0f;

    bullets.clear();
    enemies.clear();
    coins.clear();
    fishes.clear();
    clouds.clear();
    particles.clear();
}

void spawnExplosion(float x, float y, float z, float r, float g, float b, int count = 15)
{
    for (int i = 0; i < count; i++)
    {
        Particle p;
        p.x = x;
        p.y = y;
        p.z = z;
        p.vx = (float)(rand() % 100 - 50) / 10.0f;
        p.vy = (float)(rand() % 100 - 50) / 10.0f;
        p.vz = (float)(rand() % 100 - 50) / 10.0f;
        p.life = 1.0f;
        p.r = r;
        p.g = g;
        p.b = b;
        p.size = (float)(rand() % 20 + 10) / 10.0f;
        particles.push_back(p);
    }
}

// --- UI ---
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

    if (damageFlashAlpha > 0.01f)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 0.0f, 0.0f, damageFlashAlpha);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(800, 0);
        glVertex2f(800, 600);
        glVertex2f(0, 600);
        glEnd();
        glDisable(GL_BLEND);
    }

    if (!isGameOver)
    {
        glColor3f(0.35f, 0.22f, 0.15f);
        std::string scoreStr = "Score: " + std::to_string(score);
        drawText(scoreStr.c_str(), 20, 560);

        drawText("Move: Mouse | Fire: Space", 20, 535);
        drawText("V: Pause | C: Change Camera", 20, 510);

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

        if (isPaused)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
            glBegin(GL_QUADS);
            glVertex2f(0, 0);
            glVertex2f(800, 0);
            glVertex2f(800, 600);
            glVertex2f(0, 600);
            glEnd();
            glDisable(GL_BLEND);
            glColor3f(1.0f, 1.0f, 1.0f);
            drawText("GAME PAUSED", 320, 310, GLUT_BITMAP_TIMES_ROMAN_24);
            drawText("Press 'V' to Resume", 330, 280);
        }
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
    glScalef(0.13f, 0.13f, 0.13f);

    if (isGameOver)
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    else
        glRotatef(sin(propellerAngle * 0.02f) * 3.0, 0.0, 0.0, 1.0);

    // THÂN
    glColor3fv(COLOR_PLANE_RED);
    glPushMatrix();
    glTranslatef(40.0f, 0.0f, 0.0f);
    glScalef(1.4f, 0.9f, 0.9f);
    glutSolidCube(50.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(1.5f, 1.0f, 1.0f);
    glutSolidCube(50.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-40.0f, 0.0f, 0.0f);
    glScalef(1.4f, 0.85f, 0.85f);
    glutSolidCube(50.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-80.0f, 0.0f, 0.0f);
    glScalef(1.0f, 0.6f, 0.6f);
    glutSolidCube(50.0);
    glPopMatrix();

    // ĐỘNG CƠ
    glColor3fv(COLOR_PLANE_GRAY);
    glPushMatrix();
    glTranslatef(90.0f, 0.0f, 0.0f);
    glScalef(1.0f, 1.0f, 1.0f);
    glutSolidCube(50.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(120.0f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glutSolidCone(20.0, 30.0, 12, 1);
    glPopMatrix();

    // CÁNH DƯỚI
    glColor3fv(COLOR_PLANE_RED);
    glPushMatrix();
    glTranslatef(10.0f, -25.0f, 80.0f);
    glScalef(1.2f, 0.12f, 2.5f);
    glutSolidCube(50.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(10.0f, -25.0f, -80.0f);
    glScalef(1.2f, 0.12f, 2.5f);
    glutSolidCube(50.0);
    glPopMatrix();
    glColor3fv(COLOR_BLACK_NEW);
    glPushMatrix();
    glTranslatef(10.0f, -25.0f, 130.0f);
    glScalef(1.2f, 0.08f, 0.2f);
    glutSolidCube(50.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(10.0f, -25.0f, -130.0f);
    glScalef(1.2f, 0.08f, 0.2f);
    glutSolidCube(50.0);
    glPopMatrix();

    // CÁNH TRÊN
    glColor3fv(COLOR_PLANE_RED);
    glPushMatrix();
    glTranslatef(5.0f, 55.0f, 70.0f);
    glScalef(1.1f, 0.1f, 2.2f);
    glutSolidCube(50.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(5.0f, 55.0f, -70.0f);
    glScalef(1.1f, 0.1f, 2.2f);
    glutSolidCube(50.0);
    glPopMatrix();

    // TRỤ NỐI
    glColor3fv(COLOR_BLACK_NEW);
    for (int side = -1; side <= 1; side += 2)
    {
        glPushMatrix();
        glTranslatef(25.0f, 15.0f, 60.0f * side);
        glScalef(0.08f, 1.6f, 0.08f);
        glutSolidCube(50.0);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-15.0f, 15.0f, 60.0f * side);
        glScalef(0.08f, 1.6f, 0.08f);
        glutSolidCube(50.0);
        glPopMatrix();
    }

    // ĐUÔI
    glColor3fv(COLOR_PLANE_RED);
    glPushMatrix();
    glTranslatef(-95.0f, 5.0f, 0.0f);
    glScalef(0.5f, 0.08f, 1.6f);
    glutSolidCube(50.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-95.0f, 25.0f, 0.0f);
    glScalef(0.5f, 0.8f, 0.1f);
    glutSolidCube(50.0);
    glPopMatrix();

    // CÁNH QUẠT
    glColor3fv(COLOR_BLACK_NEW);
    glPushMatrix();
    glTranslatef(130.0f, 0.0f, 0.0f);
    glutSolidSphere(4.0, 12, 12);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(130.0f, 0.0f, 0.0f);
    glRotatef(propellerAngle, 1.0f, 0.0f, 0.0f);
    for (int i = 0; i < 3; i++)
    {
        glPushMatrix();
        glRotatef(i * 120.0f, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 30.0f, 0.0f);
        glScalef(0.15f, 1.2f, 0.06f);
        glutSolidCube(50.0);
        glPopMatrix();
    }
    glPopMatrix();

    // BÁNH XE
    glColor3fv(COLOR_BLACK_NEW);
    glPushMatrix();
    glTranslatef(30.0f, -35.0f, 30.0f);
    glScalef(0.08f, 0.35f, 0.08f);
    glutSolidCube(50.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(30.0f, -50.0f, 30.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidTorus(3.0, 10.0, 8, 12);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(30.0f, -35.0f, -30.0f);
    glScalef(0.08f, 0.35f, 0.08f);
    glutSolidCube(50.0);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(30.0f, -50.0f, -30.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidTorus(3.0, 10.0, 8, 12);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-75.0f, -35.0f, 0.0f);
    glutSolidSphere(6.0, 10, 10);
    glPopMatrix();
    glPopMatrix();
}

// --- VẼ BIỂN ---
void drawSea()
{
    glPushMatrix();
    float radiusBase = 2500.0f;
    glTranslatef(0.0f, -radiusBase - 50.0f, 0.0f);
    glRotatef(-seaRotation * 0.25f, 0.0f, 0.0f, 1.0f);

    int slices = 60;
    int stacks = 30;
    float length = 6000.0f;
    float zStart = -3000.0f;

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
                return radiusBase +
                       sin(a * 20.0f + wavePhase) * 20.0f +
                       cos(z * 0.02f + wavePhase * 1.5f) * 15.0f;
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

// --- VẼ CÁ ---
void drawFish(const GameObject &f)
{
    glPushMatrix();
    glTranslatef(f.x, f.y, f.z);

    if (f.state != 1)
        glRotatef(sin(wavePhase * 2.0f) * 10.0f, 0.0f, 0.0f, 1.0f);
    else
    {
        float angle = atan2(f.vy, 5.0f) * 180.0f / M_PI;
        glRotatef(angle, 0.0f, 0.0f, 1.0f);
    }

    glColor3f(0.2f, 0.4f, 0.8f);
    glPushMatrix();
    glScalef(1.5f, 0.8f, 0.5f);
    glutSolidSphere(10.0, 10, 10);
    glPopMatrix();
    glColor3f(0.1f, 0.3f, 0.6f);
    glPushMatrix();
    glTranslatef(-10.0f, 0.0f, 0.0f);
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
    glutSolidCone(5.0, 15.0, 10, 1);
    glPopMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(8.0f, 3.0f, 3.0f);
    glutSolidSphere(2.0, 5, 5);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(8.0f, 3.0f, -3.0f);
    glutSolidSphere(2.0, 5, 5);
    glPopMatrix();
    glPopMatrix();
}

// --- VẼ MÂY ---
void drawClouds()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    for (const auto &c : clouds)
    {
        if (!c.active)
            continue;
        glPushMatrix();
        glTranslatef(c.x, c.y, c.z);
        glScalef(c.scale, c.scale, c.scale);
        glutSolidSphere(10.0, 8, 8);
        glPushMatrix();
        glTranslatef(8.0f, 0.0f, 0.0f);
        glutSolidSphere(7.0, 8, 8);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-8.0f, 0.0f, 0.0f);
        glutSolidSphere(7.0, 8, 8);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(4.0f, 5.0f, 0.0f);
        glutSolidSphere(6.0, 8, 8);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-4.0f, 4.0f, 3.0f);
        glutSolidSphere(5.0, 8, 8);
        glPopMatrix();
        glPopMatrix();
    }
}

// VẼ CÁC HẠT NỔ
void drawParticles()
{
    for (const auto &p : particles)
    {
        if (p.life <= 0)
            continue;
        glColor3f(p.r, p.g, p.b);
        glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        float currentScale = p.size * p.life;
        glScalef(currentScale, currentScale, currentScale);
        glutSolidCube(1.0);
        glPopMatrix();
    }
}

// VẼ MẶT TRỜI VÀ MẶT TRĂNG
void drawSunAndMoon()
{
    float distanceFromCenter = 700.0f;

    glPushMatrix();
    glRotatef(-dayCycle * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);

    // --- VẼ MẶT TRỜI (Phía trên) ---
    glPushMatrix();
    glTranslatef(0.0f, distanceFromCenter, 0.0f);

    glDisable(GL_LIGHTING);
    glColor4f(1.0f, 0.8f, 0.0f, 0.3f);
    glEnable(GL_BLEND);
    glutSolidSphere(60.0, 20, 20);
    glDisable(GL_BLEND);

    glColor3f(1.0f, 0.9f, 0.0f);
    glutSolidSphere(40.0, 20, 20);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // --- VẼ MẶT TRĂNG (Đối diện - Phía dưới) ---
    glPushMatrix();
    glTranslatef(0.0f, -distanceFromCenter, 0.0f);

    glDisable(GL_LIGHTING);
    glColor3f(0.9f, 0.9f, 0.9f);
    glutSolidSphere(30.0, 20, 20);

    glColor3f(0.7f, 0.7f, 0.7f);
    glTranslatef(10.0f, 5.0f, 25.0f);
    glutSolidSphere(5.0, 10, 10);
    glTranslatef(-15.0f, -10.0f, 0.0f);
    glutSolidSphere(7.0, 10, 10);

    glEnable(GL_LIGHTING);
    glPopMatrix();

    glPopMatrix();
}

void drawEnemyModel(int type)
{
    // === OPTION 1: THỦY LÔI GAI ===
    glScalef(1.5f, 1.5f, 1.5f);
    glColor3f(0.3f, 0.3f, 0.3f);
    glutSolidSphere(2.5, 12, 12);
    glColor3f(0.8f, 0.2f, 0.2f);
    float spikeLen = 4.0f;
    glPushMatrix();
    glRotatef(0, 1, 0, 0);
    glTranslatef(0, 0, 2.0f);
    glutSolidCone(0.8, spikeLen, 10, 1);
    glPopMatrix();
    glPushMatrix();
    glRotatef(180, 1, 0, 0);
    glTranslatef(0, 0, 2.0f);
    glutSolidCone(0.8, spikeLen, 10, 1);
    glPopMatrix();
    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glTranslatef(0, 0, 2.0f);
    glutSolidCone(0.8, spikeLen, 10, 1);
    glPopMatrix();
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0, 0, 2.0f);
    glutSolidCone(0.8, spikeLen, 10, 1);
    glPopMatrix();
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glTranslatef(0, 0, 2.0f);
    glutSolidCone(0.8, spikeLen, 10, 1);
    glPopMatrix();
    glPushMatrix();
    glRotatef(-90, 0, 1, 0);
    glTranslatef(0, 0, 2.0f);
    glutSolidCone(0.8, spikeLen, 10, 1);
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
    for (const auto &e : enemies)
    {
        glPushMatrix();
        glTranslatef(e.x, e.y, e.z);
        glRotatef(e.rotation, 1.0f, 1.0f, 0.0f);
        drawEnemyModel(1);
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
    for (const auto &f : fishes)
    {
        drawFish(f);
    }
    drawParticles();
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
    if (isPaused)
    {
        glutPostRedisplay();
        glutTimerFunc(16, update, 0);
        return;
    }

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

    if (damageFlashAlpha > 0.0f)
        damageFlashAlpha -= 0.05f;
    if (gameSpeed < MAX_SPEED)
        gameSpeed += SPEED_INC;

    // [EDIT] Mặt trời quay chậm hơn (0.002 -> 0.0005)
    dayCycle += 0.0005f;
    if (dayCycle > 2 * M_PI)
        dayCycle -= 2 * M_PI;

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
    if (spawnTimer > (50.0f / gameSpeed))
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

    fishSpawnTimer++;
    if (fishSpawnTimer > (120.0f / gameSpeed))
    {
        GameObject f;
        f.x = 400.0f;
        f.y = FISH_WATER_LEVEL;
        f.z = (rand() % 100) - 50.0f;
        f.active = true;
        f.scale = 1.0f;
        f.vy = 0.0f;
        f.state = 0;
        fishes.push_back(f);
        fishSpawnTimer = 0;
    }

    cloudSpawnTimer++;
    if (cloudSpawnTimer > (80.0f / gameSpeed))
    {
        GameObject c;
        c.x = 450.0f;
        c.y = (rand() % 200) + 50.0f;
        c.z = (rand() % 150) - 200.0f;
        c.active = true;
        c.scale = 1.0f + (rand() % 10) / 10.0f;
        clouds.push_back(c);
        cloudSpawnTimer = 0;
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

    for (auto &p : particles)
    {
        p.x += p.vx;
        p.y += p.vy;
        p.z += p.vz;
        p.vy -= 0.1f;
        p.life -= 0.05f;
    }

    for (auto &f : fishes)
    {
        if (f.state == 0 || f.state == 2)
        {
            f.x -= (3.0f * gameSpeed);

            if (f.state == 0)
            {
                if (f.x < 50.0f && f.x > -50.0f)
                {
                    if ((rand() % 100) < FISH_JUMP_CHANCE)
                    {
                        f.state = 1;
                        f.vy = FISH_JUMP_POWER_BASE + (float)(rand() % 6);
                    }
                    else
                    {
                        f.state = 2;
                    }
                }
            }
        }
        else if (f.state == 1)
        {
            f.x -= (3.0f * gameSpeed);
            f.y += f.vy;
            f.vy -= FISH_GRAVITY;

            if (f.y < FISH_WATER_LEVEL && f.vy < 0)
            {
                f.state = 2;
                f.y = FISH_WATER_LEVEL;
                f.vy = 0;
            }
        }
    }

    for (auto &c : clouds)
    {
        c.x -= (1.5f * gameSpeed);
        c.rotation += 0.5f;
        if (c.x < -450.0f)
            c.active = false;
    }

    float collisionRadius = COMMON_SIZE * 3.0f;
    float fishRadius = 15.0f;

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
                spawnExplosion(e.x, e.y, e.z, 0.4f, 0.4f, 0.4f);
            }
        }
        float distPlane = sqrtf(pow(e.x - (-50.0f), 2) + pow(e.y - planeY, 2));
        if (distPlane < collisionRadius + 15.0f)
        {
            e.active = false;
            fuel -= HIT_ENEMY_FUEL_LOSS;
            damageFlashAlpha = 0.6f;
            spawnExplosion(e.x, e.y, e.z, 1.0f, 0.5f, 0.0f, 25);
            if (fuel <= 0.0f)
            {
                fuel = 0.0f;
                isGameOver = true;
            }
        }
    }

    for (auto &f : fishes)
    {
        if (!f.active)
            continue;
        float distPlane = sqrtf(pow(f.x - (-50.0f), 2) + pow(f.y - planeY, 2));
        if (distPlane < fishRadius + 15.0f)
        {
            f.active = false;
            fuel -= 15.0f;
            damageFlashAlpha = 0.6f;
            spawnExplosion(f.x, f.y, f.z, 0.2f, 0.4f, 0.8f, 20);
            if (fuel <= 0.0f)
            {
                fuel = 0.0f;
                isGameOver = true;
            }
        }
        for (auto &b : bullets)
        {
            if (!b.active)
                continue;
            float dist = sqrtf(pow(f.x - b.x, 2) + pow(f.y - b.y, 2));
            if (dist < fishRadius)
            {
                f.active = false;
                b.active = false;
                score += 15;
                spawnExplosion(f.x, f.y, f.z, 0.2f, 0.4f, 0.8f);
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
    fishes.erase(std::remove_if(fishes.begin(), fishes.end(), [](const GameObject &f)
                                { return !f.active || f.x < -220.0f; }),
                 fishes.end());
    clouds.erase(std::remove_if(clouds.begin(), clouds.end(), [](const GameObject &c)
                                { return !c.active; }),
                 clouds.end());
    particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle &p)
                                   { return p.life <= 0.0f; }),
                    particles.end());

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// --- DISPLAY ---
void display()
{
    float dayFactor = (cos(dayCycle) + 1.0f) / 2.0f;

    float currentSky[4];
    float currentLightDiffuse[4];
    float currentLightAmbient[4];

    mixColors(COLOR_SKY_NIGHT, COLOR_SKY_DAY, dayFactor, currentSky);
    mixColors(LIGHT_NIGHT_DIFFUSE, LIGHT_DAY_DIFFUSE, dayFactor, currentLightDiffuse);
    mixColors(LIGHT_NIGHT_AMBIENT, LIGHT_DAY_AMBIENT, dayFactor, currentLightAmbient);

    glClearColor(currentSky[0], currentSky[1], currentSky[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glFogfv(GL_FOG_COLOR, currentSky);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, currentLightDiffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, currentLightAmbient);

    float lightPos[] = {0.0f, 500.0f, 0.0f, 0.0f};
    glPushMatrix();
    glRotatef(-dayCycle * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glPopMatrix();

    if (viewMode == 0)
    {
        float rad = cameraAngle * M_PI / 180.0f;
        float eyeX = sin(rad) * cameraDist;
        float eyeZ = cos(rad) * cameraDist;
        gluLookAt(eyeX, cameraHeight, eyeZ, 0.0f, planeY, 0.0f, 0.0f, 1.0f, 0.0f);
    }
    else
    {
        gluLookAt(-160.0f, planeY + 30.0f, 0.0f,
                  200.0f, planeY, 0.0f,
                  0.0f, 1.0f, 0.0f);
    }

    drawSunAndMoon();

    drawSea();
    drawClouds();
    drawEntities();
    drawPlane();
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

// --- INPUT ---
void mouseMotion(int x, int y)
{
    if (isGameOver || isPaused)
        return;
    float normalizedY = (float)y / 600.0f;
    float targetY = 200.0f - (normalizedY * 180.0f);
    planeY += (targetY - planeY) * 0.1f;
}

void specialKeys(int key, int x, int y)
{
    if (isGameOver || isPaused)
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

void keyboard(unsigned char key, int x, int y)
{
    if (!isGameOver)
    {
        if (key == 'v' || key == 'V')
            isPaused = !isPaused;

        if (key == 'c' || key == 'C')
        {
            viewMode = (viewMode + 1) % 2;
        }

        if (!isPaused && key == 32 && fuel >= SHOOT_FUEL_COST && fuel > 0.0f)
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
    else if (key == 'r' || key == 'R')
        resetGame();
    if (key == 27)
        exit(0);
}

int main(int argc, char **argv)
{
    srand((unsigned int)time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("The Aviator - Full Features");

    setupEnvironment();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutPassiveMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}