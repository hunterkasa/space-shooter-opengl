#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "spaceship2d.h"
#include "audio.h"
#include "nightmare_level.h"
#include "reverse_level.h"
#include "cutscene.h"

/* =========================================================
   SIMPLE 3 LEVEL 2D SPACE SHOOTER TEMPLATE

   Controls:
     Menu:
       1 / 2 / 3 / 4 / 5  - start an unlocked level

     Gameplay:
       Arrow keys / WASD  - move
       Space              - shoot
       U                  - use ult when charged
       M                  - return to menu
       R                  - restart selected level
       Q / Esc            - quit

     Cutscenes:
       Space / Enter      - continue dialogue

   How to tweak this template:
     1. Change the constants below for global game feel.
     2. Change levels[] for each level's color and difficulty.
     3. Change the draw functions if you want a different look.
     4. Change the update functions if you want different behavior.
   ========================================================= */

/* WORLD_W and WORLD_H are the visible 2D game area. */
const float WORLD_W = 10.0f;
const float WORLD_H = 7.0f;

/* Player settings. Increase health or speed to make the game easier. */
const float PLAYER_SPEED = 0.16f;
const int PLAYER_MAX_HEALTH = 100;  //////////////////////////////////////////////////////////////////////////////////////
const int REVERSE_PLAYER_MAX_HEALTH = 170;

/* These limits keep the code simple by using fixed-size arrays. */
const int MAX_STARS = 75;
const int MAX_PLAYER_BULLETS = 28;
const int MAX_ENEMY_BULLETS = 48;
const int MAX_ENEMIES = 12;
const int MAX_ASTEROIDS = 10;

/* Enemy projectile settings. These are easy knobs for balancing. */
const float INTERCEPTOR_BULLET_SPEED = 0.075f;
const int INTERCEPTOR_BULLET_DAMAGE = 7;
const int INTERCEPTOR_EXTRA_DELAY = 0;

const float BOMBER_BULLET_SPEED = 0.052f; // Slower but heavier.
const int BOMBER_BULLET_DAMAGE = 12;
const int BOMBER_EXTRA_DELAY = 65;        // Bomber fires slower.

const float BOSS_BULLET_SPEED = 0.105f;
const int BOSS_CENTER_DAMAGE = 10;
const int BOSS_SIDE_DAMAGE = 9;

/* Ult settings. Land this many hits, then press U for a stronger shot. */
const int ULT_HITS_REQUIRED = 7;
const int ULT_DAMAGE = 10;
const float ULT_BULLET_SPEED = 0.115f;
const float REVERSE_BOSS_BULLET_SPEED = 0.135f;

struct Color {
    float r, g, b;
};

struct Star {
    float x, y;
    float speed;
    float size;
};

struct Bullet {
    bool active;
    float x, y;
    float speed;
    float speedX;
    float speedY;
    int damage;
    int type;       // 0 = player, 1 = interceptor, 2 = bomber, 3 = boss, 4 = player ult
};

struct Enemy {
    bool active;
    float x, y;
    float speed;
    int health;
    int maxHealth;
    int type;       // 0 = interceptor, 1 = bomber, 2 = boss
    int shootTimer; // Counts down each frame before this enemy can shoot.
};

struct Asteroid {
    bool active;
    float x, y;
    float speed;
    float speedX;
    float scale;
    float spin;
    float angle;
};

struct LevelSettings {
    Color background;
    Color starColor;
    int enemyCount;
    int enemyHealth;
    float enemySpeed;
    int enemyShootDelay;   // Higher value = enemies shoot less often.
    int asteroidDelay;     // Higher value = asteroids spawn less often.
    bool bossLevel;
    int bossHealth;
    int bossShootDelay;    // Only used in level 3.
};

enum GameState {
    STATE_INTRO_CUTSCENE,
    STATE_MENU,
    STATE_PLAYING,
    STATE_LEVEL_CLEAR,
    STATE_GAME_OVER,
    STATE_END_CUTSCENE
};

/*
   Main level editor:
   { background, starColor, enemyCount, enemyHealth, enemySpeed,
     enemyShootDelay, asteroidDelay, bossLevel, bossHealth, bossShootDelay }

   Notes:
   - Color values are RGB from 0.0 to 1.0.
   - Lower shoot/asteroid delays make the level harder.
   - bossHealth and bossShootDelay only matter when bossLevel is true.
 */
LevelSettings levels[5] = {
    {{0.010f, 0.018f, 0.055f}, {0.72f, 0.92f, 1.00f}, 5, 2, 0.020f, 115, 105, false,  0,  0}, // Level 1
    {{0.060f, 0.014f, 0.045f}, {1.00f, 0.70f, 0.78f}, 7, 3, 0.028f,  85,  78, false,  0,  0}, // Level 2
    {{0.022f, 0.016f, 0.085f}, {0.76f, 0.96f, 1.00f}, 5, 4, 0.032f,  62,  48, true, 80, 24},  // Level 3 boss
    {{0.015f, 0.018f, 0.065f}, {0.62f, 0.92f, 1.00f}, 8, 5, 0.035f,  45,  25, true, 150, 15}, // Level 4 Nightmare
    {{0.008f, 0.008f, 0.040f}, {0.92f, 0.52f, 0.95f}, 10, 7, 0.042f, 36,  22, true, 170, 18}  // Level 5 Reverse Boss: command the villain against hero squadrons
};

Star stars[MAX_STARS];
Bullet playerBullets[MAX_PLAYER_BULLETS];
Bullet enemyBullets[MAX_ENEMY_BULLETS];
Enemy enemies[MAX_ENEMIES];
Asteroid asteroids[MAX_ASTEROIDS];

GameState gameState = STATE_INTRO_CUTSCENE;
int currentLevel = 0;
int unlockedLevelCount = 1; // Level 1 is unlocked at the start.
int playerHealth = PLAYER_MAX_HEALTH;
int asteroidTimer = 0;
int ultHits = 0;
bool ultReady = false;
int cutsceneLine = 0;
int cutsceneVisibleChars = 0;
int cutsceneTimer = 0;
float playerX = 0.0f;
float playerY = -2.65f;
bool keyDown[256] = {false};
bool specialDown[256] = {false};
int audioCooldown = 0;
int loseTimer = 0;

/* =========================================================
   BASIC HELPERS
   ========================================================= */
float randomRange(float minValue, float maxValue)
{
    float t = (float)rand() / (float)RAND_MAX;
    return minValue + t * (maxValue - minValue);
}

float clampFloat(float value, float minValue, float maxValue)
{
    if(value < minValue) return minValue;
    if(value > maxValue) return maxValue;
    return value;
}

int playerMaxHealthForCurrentLevel()
{
    return (currentLevel == 4) ? REVERSE_PLAYER_MAX_HEALTH : PLAYER_MAX_HEALTH;
}

void drawText(float x, float y, const char *text)
{
    glRasterPos2f(x, y);
    for(int i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

int textLength(const char *text)
{
    int count = 0;
    while(text[count] != '\0')
        count++;
    return count;
}

void drawTextPartial(float x, float y, const char *text, int visibleChars)
{
    glRasterPos2f(x, y);
    for(int i = 0; text[i] != '\0' && i < visibleChars; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

void drawBar(float x, float y, float w, float h, float percent, Color fill)
{
    percent = clampFloat(percent, 0.0f, 1.0f);

    /* Dark background of the health bar. */
    glColor3f(0.08f, 0.08f, 0.10f);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();

    /* Filled part of the health bar. */
    glColor3f(fill.r, fill.g, fill.b);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w * percent, y);
        glVertex2f(x + w * percent, y + h);
        glVertex2f(x, y + h);
    glEnd();

    /* Thin outline so the bar is visible on every background. */
    glColor3f(0.82f, 0.90f, 1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();
}

/* =========================================================
   SETUP AND SPAWNING
   ========================================================= */
void resetStars()
{
    for(int i = 0; i < MAX_STARS; i++) {
        stars[i].x = randomRange(-WORLD_W / 2.0f, WORLD_W / 2.0f);
        stars[i].y = randomRange(-WORLD_H / 2.0f, WORLD_H / 2.0f);
        stars[i].speed = randomRange(0.015f, 0.055f);
        stars[i].size = randomRange(1.0f, 3.0f);
    }
}

void clearGameObjects()
{
    /* Reset all active objects when starting/restarting a level. */
    for(int i = 0; i < MAX_PLAYER_BULLETS; i++) playerBullets[i].active = false;
    for(int i = 0; i < MAX_ENEMY_BULLETS; i++) enemyBullets[i].active = false;
    for(int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
    for(int i = 0; i < MAX_ASTEROIDS; i++) asteroids[i].active = false;
}

void spawnAsteroid()
{
    /* Find one unused asteroid slot and reuse it. */
    for(int i = 0; i < MAX_ASTEROIDS; i++) {
        if(!asteroids[i].active) {
            asteroids[i].active = true;
            asteroids[i].x = randomRange(-4.7f, 4.7f);
            asteroids[i].y = 3.8f;
            asteroids[i].speed = randomRange(0.030f, 0.075f) + currentLevel * 0.012f;
            if(currentLevel >= 3) {
                asteroids[i].speedX = randomRange(-0.015f, 0.015f);
            } else {
                asteroids[i].speedX = 0.0f;
            }
            asteroids[i].scale = randomRange(0.18f, 0.36f);
            asteroids[i].spin = randomRange(-2.0f, 2.0f);
            asteroids[i].angle = randomRange(0.0f, 360.0f);
            return;
        }
    }
}

void spawnEnemy(int i, float x, float y, int type)
{
    LevelSettings level = levels[currentLevel];

    /* Interceptors and bombers share this simple setup function. */
    enemies[i].active = true;
    enemies[i].x = x;
    enemies[i].y = y;
    enemies[i].speed = level.enemySpeed;
    enemies[i].type = type;

    if(type == 1) enemies[i].maxHealth = level.enemyHealth + 2;
    else if(type == 3) enemies[i].maxHealth = level.enemyHealth - 1; // Laser enemy is fragile
    else enemies[i].maxHealth = level.enemyHealth;

    enemies[i].health = enemies[i].maxHealth;
    enemies[i].shootTimer = level.enemyShootDelay + i * 18;
}

void spawnEnemiesForLevel()
{
    LevelSettings level = levels[currentLevel];

    if(level.bossLevel) {
        if(currentLevel == 4) {
            /* Reverse level: the player controls the boss, so spawn hero enemies instead. */
            for(int i = 0; i < level.enemyCount && i < MAX_ENEMIES; i++) {
                int eType = (i % 3 == 0) ? 3 : (i % 2);
                spawnEnemy(i, randomRange(-4.0f, 4.0f), randomRange(0.3f, 2.8f), eType);
            }
            return;
        }

        /* Slot 0 is reserved for the boss in level 3. */
        enemies[0].active = true;
        enemies[0].x = 0.0f;
        enemies[0].y = 1.75f;
        enemies[0].speed = 0.040f;
        enemies[0].type = 2;
        enemies[0].maxHealth = level.bossHealth;
        enemies[0].health = level.bossHealth;
        enemies[0].shootTimer = 35;

        /* Extra smaller enemies make the boss fight harder. */
        int nightmareLaserSpawned = 0;
        int nightmareBomberSpawned = 0;
        const int nightmareLaserCap = 2;
        const int nightmareBomberCap = 2;
        for(int i = 1; i <= level.enemyCount && i < MAX_ENEMIES; i++) {
            int eType = ((currentLevel == 3 || currentLevel == 4) && i % 2 == 0) ? 3 : (i % 2);

            if(currentLevel == 3) {
                if(eType == 3) {
                    if(nightmareLaserSpawned >= nightmareLaserCap) continue;
                    nightmareLaserSpawned++;
                } else if(eType == 1) {
                    if(nightmareBomberSpawned >= nightmareBomberCap) continue;
                    nightmareBomberSpawned++;
                }
            }

            spawnEnemy(i, randomRange(-4.0f, 4.0f), randomRange(0.3f, 2.8f), eType);
        }

        return;
    }

    for(int i = 0; i < level.enemyCount && i < MAX_ENEMIES; i++)
        spawnEnemy(i, randomRange(-4.0f, 4.0f), randomRange(0.3f, 2.9f), i % 2);
}

void startLevel(int levelIndex)
{
    if(levelIndex < 0) levelIndex = 0;
    if(levelIndex > 4) levelIndex = 4;

    if(levelIndex != 3 && levelIndex != 4 && levelIndex >= unlockedLevelCount) return;

    currentLevel = levelIndex;
    gameState = STATE_PLAYING;
    playerHealth = playerMaxHealthForCurrentLevel();
    ultHits = 0;
    ultReady = false;
    playerX = 0.0f;
    playerY = -2.65f;
    asteroidTimer = 30;

    clearGameObjects();
    resetStars();
    spawnEnemiesForLevel();
}

void startCutscene(GameState cutsceneState)
{
    cutsceneInit(cutsceneLine, cutsceneVisibleChars, cutsceneTimer);
}

/* =========================================================
   DRAWING
   ========================================================= */
void drawStars()
{
    Color c = levels[currentLevel].starColor;
    glColor3f(c.r, c.g, c.b);
    for(int i = 0; i < MAX_STARS; i++) {
        glPointSize(stars[i].size);
        glBegin(GL_POINTS);
            glVertex2f(stars[i].x, stars[i].y);
        glEnd();
    }
}

void drawBackgroundBand(float yTop, float yBottom, Color color)
{
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_QUADS);
        glVertex2f(-5.0f, yTop);
        glVertex2f( 5.0f, yTop);
        glVertex2f( 5.0f, yBottom);
        glVertex2f(-5.0f, yBottom);
    glEnd();
}

void drawMatureScenarioBackground(int theme)
{
    float time = (float)glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    float drift = sinf(time * 0.22f);
    float pulse = 0.5f + 0.5f * sinf(time * 1.15f);

    Color baseTop = {0.035f, 0.040f, 0.145f};
    Color baseMid = {0.022f, 0.028f, 0.105f};
    Color baseLow = {0.015f, 0.020f, 0.070f};
    Color accent = {0.24f, 0.48f, 0.82f};
    Color planet = {0.34f, 0.34f, 0.62f};

    if(theme == 1 || theme == 6) {
        baseTop = {0.060f, 0.030f, 0.075f};
        baseMid = {0.038f, 0.020f, 0.055f};
        baseLow = {0.018f, 0.014f, 0.038f};
        accent = {0.70f, 0.26f, 0.30f};
        planet = {0.42f, 0.24f, 0.36f};
    } else if(theme == 2 || theme == 5) {
        baseTop = {0.032f, 0.040f, 0.135f};
        baseMid = {0.020f, 0.038f, 0.108f};
        baseLow = {0.012f, 0.026f, 0.070f};
        accent = {0.28f, 0.62f, 0.76f};
        planet = {0.30f, 0.38f, 0.62f};
    } else if(theme == 3) {
        baseTop = {0.040f, 0.030f, 0.120f};
        baseMid = {0.025f, 0.018f, 0.082f};
        baseLow = {0.014f, 0.012f, 0.050f};
        accent = {0.58f, 0.32f, 0.64f};
        planet = {0.36f, 0.30f, 0.58f};
    }

    drawBackgroundBand(3.5f, 1.15f, baseTop);
    drawBackgroundBand(1.15f, -1.55f, baseMid);
    drawBackgroundBand(-1.55f, -3.5f, baseLow);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Broad low-opacity space currents, inspired by the sample's mature layered waves.
    glColor4f(0.22f, 0.24f, 0.46f, 0.28f);
    glBegin(GL_POLYGON);
        glVertex2f(-5.0f, 2.80f);
        glVertex2f(-2.0f, 3.02f + drift * 0.05f);
        glVertex2f( 1.6f, 2.92f);
        glVertex2f( 5.0f, 2.70f + drift * 0.04f);
        glVertex2f( 5.0f, 2.18f);
        glVertex2f( 1.4f, 2.28f - drift * 0.05f);
        glVertex2f(-1.8f, 2.26f);
        glVertex2f(-5.0f, 2.08f);
    glEnd();

    glColor4f(0.12f, 0.15f, 0.34f, 0.38f);
    glBegin(GL_POLYGON);
        glVertex2f(-5.0f, 1.35f);
        glVertex2f(-2.8f, 1.50f - drift * 0.04f);
        glVertex2f(-0.4f, 1.40f);
        glVertex2f( 2.3f, 1.58f + drift * 0.06f);
        glVertex2f( 5.0f, 1.28f);
        glVertex2f( 5.0f, 0.58f);
        glVertex2f( 1.8f, 0.76f);
        glVertex2f(-1.6f, 0.62f + drift * 0.04f);
        glVertex2f(-5.0f, 0.72f);
    glEnd();

    glColor4f(0.18f, 0.20f, 0.46f, 0.26f);
    glBegin(GL_POLYGON);
        glVertex2f(-5.0f, -0.72f);
        glVertex2f(-2.1f, -0.48f);
        glVertex2f( 0.6f, -0.64f - drift * 0.04f);
        glVertex2f( 3.5f, -0.42f);
        glVertex2f( 5.0f, -0.58f);
        glVertex2f( 5.0f, -1.16f);
        glVertex2f( 2.4f, -1.04f + drift * 0.04f);
        glVertex2f(-1.2f, -1.18f);
        glVertex2f(-5.0f, -1.04f);
    glEnd();

    // Large soft foreground planet arc.
    glColor4f(planet.r + 0.14f, planet.g + 0.14f, planet.b + 0.18f, 0.36f);
    drawCustomEllipse(-0.35f, -3.70f, 3.35f, 0.92f, 64);
    glColor4f(0.08f, 0.09f, 0.18f, 0.42f);
    drawCustomEllipse(-2.72f, -2.72f, 0.56f, 0.20f, 28);
    drawCustomEllipse(0.72f, -3.04f, 0.46f, 0.15f, 28);
    drawCustomEllipse(2.52f, -3.00f, 0.58f, 0.18f, 28);

    // Distant planets, intentionally quiet so ships and bullets stay readable.
    glPushMatrix();
        glTranslatef(3.18f + drift * 0.08f, 2.02f, 0.0f);
        glRotatef(-12.0f, 0.0f, 0.0f, 1.0f);
        glColor4f(planet.r, planet.g, planet.b, 0.44f);
        drawCustomCircle(0.0f, 0.0f, 0.68f, 48);
        glColor4f(planet.r + 0.12f, planet.g + 0.10f, planet.b + 0.16f, 0.32f);
        drawCustomEllipse(0.0f, 0.0f, 1.15f, 0.18f, 48);
        glColor4f(0.04f, 0.05f, 0.12f, 0.30f);
        drawCustomEllipse(-0.12f, 0.20f, 0.46f, 0.22f, 32);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-3.65f - drift * 0.06f, 1.35f, 0.0f);
        glRotatef(-22.0f, 0.0f, 0.0f, 1.0f);
        glColor4f(planet.r * 0.82f, planet.g * 0.82f, planet.b * 0.92f, 0.30f);
        drawCustomCircle(0.0f, 0.0f, 0.48f, 38);
        glColor4f(0.05f, 0.06f, 0.14f, 0.30f);
        drawCustomEllipse(0.0f, 0.05f, 0.70f, 0.12f, 34);
    glPopMatrix();

    glColor4f(planet.r * 0.85f, planet.g * 0.85f, planet.b, 0.34f);
    drawCustomCircle(3.95f, -0.46f + drift * 0.05f, 0.36f, 36);

    // Subtle stars and motion glints.
    glColor4f(accent.r, accent.g, accent.b, 0.68f);
    drawEnergyDiamond(-4.62f, 2.70f, 0.05f, 0.09f);
    drawEnergyDiamond(-3.45f, 2.18f, 0.08f, 0.13f);
    drawEnergyDiamond(-1.22f, 1.18f, 0.05f, 0.08f);
    drawEnergyDiamond( 0.35f, 2.58f, 0.05f, 0.08f);
    drawEnergyDiamond( 2.35f, 2.70f, 0.05f, 0.08f);
    drawEnergyDiamond( 4.32f, 0.25f, 0.08f, 0.13f);

    glColor4f(accent.r, accent.g, accent.b, 0.16f + pulse * 0.08f);
    drawCustomEllipse(0.2f, 0.34f + drift * 0.03f, 1.55f, 0.14f, 38);

    glDisable(GL_BLEND);
}

void drawBossSectorBackground()
{
    float time = (float)glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    float drift = sinf(time * 0.25f);
    float pulse = 0.5f + 0.5f * sinf(time * 1.35f);

    drawBackgroundBand(3.5f, 1.05f, {0.028f, 0.026f, 0.115f});
    drawBackgroundBand(1.05f, -1.45f, {0.018f, 0.020f, 0.085f});
    drawBackgroundBand(-1.45f, -3.5f, {0.010f, 0.014f, 0.052f});

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Quiet purple-blue boss arena: mature, wide, and not too bright.
    glColor4f(0.20f, 0.18f, 0.44f, 0.28f);
    glBegin(GL_POLYGON);
        glVertex2f(-5.0f, 2.62f);
        glVertex2f(-2.15f, 2.82f + drift * 0.06f);
        glVertex2f( 1.35f, 2.72f);
        glVertex2f( 5.0f, 2.48f);
        glVertex2f( 5.0f, 1.88f);
        glVertex2f( 1.92f, 2.03f - drift * 0.05f);
        glVertex2f(-1.75f, 1.98f);
        glVertex2f(-5.0f, 2.18f);
    glEnd();

    glColor4f(0.10f, 0.30f, 0.44f, 0.22f);
    drawCustomEllipse(-1.10f + drift * 0.14f, 0.88f, 3.05f, 0.32f, 54);
    glColor4f(0.36f, 0.16f, 0.46f, 0.20f);
    drawCustomEllipse(1.20f - drift * 0.10f, 0.42f, 2.25f, 0.24f, 46);

    // Distant eclipsed planet behind the boss.
    glColor4f(0.34f, 0.34f, 0.60f, 0.32f);
    drawCustomCircle(2.95f, 1.55f + drift * 0.04f, 0.78f, 56);
    glColor4f(0.08f, 0.09f, 0.20f, 0.34f);
    drawCustomCircle(2.72f, 1.66f + drift * 0.04f, 0.62f, 52);
    glColor4f(0.42f, 0.40f, 0.76f, 0.20f);
    drawCustomEllipse(2.95f, 1.55f + drift * 0.04f, 1.35f, 0.18f, 48);

    // Boss-energy grid, deliberately subtle.
    glColor4f(0.42f, 0.88f, 1.0f, 0.20f + pulse * 0.10f);
    glLineWidth(1.4f + pulse * 0.7f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(-4.55f, -1.55f);
        glVertex2f(-2.55f, -1.34f + drift * 0.05f);
        glVertex2f(-0.45f, -1.50f);
        glVertex2f( 1.50f, -1.30f - drift * 0.05f);
        glVertex2f( 4.55f, -1.48f);
    glEnd();

    glColor4f(0.70f, 0.42f, 0.95f, 0.45f);
    drawEnergyDiamond(-3.65f, 2.20f, 0.06f, 0.11f);
    drawEnergyDiamond(-0.92f, 1.74f, 0.05f, 0.09f);
    drawEnergyDiamond( 4.12f, 0.62f, 0.07f, 0.12f);

    glColor4f(0.035f, 0.075f, 0.130f, 0.78f);
    glBegin(GL_POLYGON);
        glVertex2f(-5.0f, -3.5f);
        glVertex2f( 5.0f, -3.5f);
        glVertex2f( 5.0f, -2.26f);
        glVertex2f( 3.76f, -2.02f);
        glVertex2f( 2.18f, -2.32f);
        glVertex2f( 0.48f, -2.08f);
        glVertex2f(-1.15f, -2.34f);
        glVertex2f(-2.82f, -2.06f);
        glVertex2f(-5.0f, -2.38f);
    glEnd();

    glLineWidth(1.0f);
    glDisable(GL_BLEND);
}

void drawModernSpaceBackground(int theme)
{
    if(theme == 3) {
        drawBossSectorBackground();
        return;
    }

    if(theme == 4) {
        drawNightmareSectorBackground();
        return;
    }

    if(theme == 8) {
        drawReverseBossArenaBackground();
        return;
    }

    if(theme != 7) {
        drawMatureScenarioBackground(theme);
        return;
    }

    float time = (float)glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    float slowDrift = sinf(time * 0.45f);
    float pulse = 0.5f + 0.5f * sinf(time * 2.2f);
    float fastPulse = 0.5f + 0.5f * sinf(time * 5.5f);

    Color top;
    Color mid;
    Color low;
    Color nebulaA;
    Color nebulaB;
    Color accent;
    Color horizonA;
    Color horizonB;

    if(theme == 1) {
        top = {0.055f, 0.014f, 0.055f};
        mid = {0.105f, 0.025f, 0.055f};
        low = {0.028f, 0.012f, 0.030f};
        nebulaA = {0.42f, 0.08f, 0.16f};
        nebulaB = {0.92f, 0.30f, 0.18f};
        accent = {1.00f, 0.50f, 0.22f};
        horizonA = {0.115f, 0.035f, 0.040f};
        horizonB = {0.040f, 0.014f, 0.025f};
    } else if(theme == 2) {
        top = {0.020f, 0.018f, 0.082f};
        mid = {0.042f, 0.025f, 0.120f};
        low = {0.010f, 0.020f, 0.055f};
        nebulaA = {0.15f, 0.18f, 0.50f};
        nebulaB = {0.08f, 0.52f, 0.62f};
        accent = {0.92f, 0.22f, 0.95f};
        horizonA = {0.040f, 0.105f, 0.160f};
        horizonB = {0.012f, 0.040f, 0.075f};
    } else if(theme == 3) {
        drawDestroyedPlanet();
        glColor3f(0.10f + pulse * 0.08f, 0.85f, 1.0f);
        glLineWidth(2.0f + fastPulse * 2.0f);
        glBegin(GL_LINE_STRIP);
            glVertex2f(-4.70f, 2.72f + slowDrift * 0.12f);
            glVertex2f(-2.60f, 2.24f - slowDrift * 0.08f);
            glVertex2f(-0.68f, 2.52f + slowDrift * 0.10f);
            glVertex2f( 1.30f, 2.02f - slowDrift * 0.12f);
            glVertex2f( 4.62f, 2.35f + slowDrift * 0.08f);
        glEnd();

        glColor3f(1.0f, 0.16f + pulse * 0.28f, 0.60f + pulse * 0.22f);
        drawEnergyDiamond(-3.10f + slowDrift * 0.12f, 0.38f, 0.16f + pulse * 0.04f, 0.30f + pulse * 0.08f);
        drawEnergyDiamond( 3.12f - slowDrift * 0.10f, 1.08f, 0.13f + pulse * 0.03f, 0.25f + pulse * 0.06f);
        drawCustomEllipse(0.80f, -0.98f + slowDrift * 0.08f, 2.10f + pulse * 0.16f, 0.10f, 40);
        glLineWidth(1.0f);
        return;
    } else if(theme == 5) {
        top = {0.012f, 0.045f, 0.040f};
        mid = {0.018f, 0.088f, 0.075f};
        low = {0.008f, 0.040f, 0.042f};
        nebulaA = {0.08f, 0.42f, 0.28f};
        nebulaB = {0.16f, 0.58f, 0.62f};
        accent = {0.58f, 1.00f, 0.62f};
        horizonA = {0.030f, 0.120f, 0.095f};
        horizonB = {0.010f, 0.048f, 0.052f};
    } else if(theme == 6) {
        top = {0.050f, 0.006f, 0.015f};
        mid = {0.095f, 0.012f, 0.022f};
        low = {0.030f, 0.006f, 0.014f};
        nebulaA = {0.32f, 0.020f, 0.030f};
        nebulaB = {0.86f, 0.080f, 0.050f};
        accent = {1.00f, 0.20f, 0.10f};
        horizonA = {0.120f, 0.026f, 0.020f};
        horizonB = {0.045f, 0.010f, 0.016f};
    } else if(theme == 7) {
        top = {0.010f, 0.016f, 0.050f};
        mid = {0.025f, 0.052f, 0.110f};
        low = {0.020f, 0.012f, 0.045f};
        nebulaA = {0.05f, 0.42f, 0.52f};
        nebulaB = {0.48f, 0.12f, 0.34f};
        accent = {0.95f, 0.62f, 0.22f};
        horizonA = {0.035f, 0.095f, 0.145f};
        horizonB = {0.012f, 0.028f, 0.060f};
    } else {
        top = {0.006f, 0.016f, 0.050f};
        mid = {0.020f, 0.040f, 0.100f};
        low = {0.012f, 0.020f, 0.052f};
        nebulaA = {0.05f, 0.26f, 0.36f};
        nebulaB = {0.14f, 0.20f, 0.52f};
        accent = {0.24f, 0.88f, 1.00f};
        horizonA = {0.030f, 0.085f, 0.135f};
        horizonB = {0.010f, 0.032f, 0.068f};
    }

    drawBackgroundBand(3.5f, 1.1f, top);
    drawBackgroundBand(1.1f, -1.35f, mid);
    drawBackgroundBand(-1.35f, -3.5f, low);

    glColor3f(nebulaA.r, nebulaA.g, nebulaA.b);
    drawCustomEllipse(-1.45f + slowDrift * 0.18f, 1.30f + sinf(time * 0.32f) * 0.08f, 3.15f, 0.45f + pulse * 0.06f, 54);
    glColor3f(nebulaB.r, nebulaB.g, nebulaB.b);
    drawCustomEllipse(1.35f - slowDrift * 0.16f, 0.72f + cosf(time * 0.40f) * 0.06f, 2.55f, 0.30f + pulse * 0.04f, 48);
    glColor3f(accent.r * (0.34f + pulse * 0.20f), accent.g * (0.34f + pulse * 0.20f), accent.b * (0.34f + pulse * 0.20f));
    drawCustomEllipse(-2.95f + cosf(time * 0.55f) * 0.10f, -0.10f, 1.20f + pulse * 0.10f, 0.22f, 36);

    glPushMatrix();
        glTranslatef(-3.95f + slowDrift * 0.07f, 1.65f + cosf(time * 0.62f) * 0.04f, 0.0f);
        glRotatef(-15.0f + sinf(time * 0.38f) * 2.0f, 0.0f, 0.0f, 1.0f);
        glColor3f(accent.r * (0.58f + pulse * 0.22f), accent.g * (0.58f + pulse * 0.22f), accent.b * (0.58f + pulse * 0.22f));
        glLineWidth(2.2f + pulse * 1.8f);
        glBegin(GL_LINE_STRIP);
            glVertex2f(-1.25f, -0.16f);
            glVertex2f(-0.55f, -0.04f);
            glVertex2f( 0.55f,  0.04f);
            glVertex2f( 1.25f,  0.16f);
        glEnd();
        glColor3f(top.r + 0.10f, top.g + 0.10f, top.b + 0.14f);
        drawCustomCircle(0.0f, 0.0f, 0.52f, 36);
        glColor3f(mid.r + 0.16f, mid.g + 0.16f, mid.b + 0.18f);
        glBegin(GL_POLYGON);
            for(int i = -90; i <= 90; i += 6) {
                float t = (float)i * 3.14159f / 180.0f;
                glVertex2f(cosf(t) * 0.52f, sinf(t) * 0.52f);
            }
        glEnd();
    glPopMatrix();

    glColor3f(accent.r, accent.g * (0.80f + pulse * 0.20f), accent.b);
    drawEnergyDiamond(2.92f + sinf(time * 0.75f) * 0.08f, 1.70f, 0.12f + pulse * 0.03f, 0.22f + pulse * 0.05f);
    drawEnergyDiamond(0.48f, 2.42f + cosf(time * 0.68f) * 0.07f, 0.09f + pulse * 0.02f, 0.15f + pulse * 0.04f);
    drawEnergyDiamond(-1.15f - slowDrift * 0.08f, 0.34f, 0.08f + pulse * 0.02f, 0.14f + pulse * 0.03f);

    glColor3f(accent.r * (0.60f + pulse * 0.28f), accent.g * (0.60f + pulse * 0.28f), accent.b * (0.60f + pulse * 0.28f));
    glBegin(GL_TRIANGLES);
        glVertex2f(1.42f, 2.58f + slowDrift * 0.08f); glVertex2f(1.78f, 1.96f); glVertex2f(1.06f, 1.98f);
        glVertex2f(3.70f, 0.98f - slowDrift * 0.06f); glVertex2f(4.02f, 0.46f); glVertex2f(3.38f, 0.50f);
        glVertex2f(-2.02f, 2.42f + slowDrift * 0.05f); glVertex2f(-1.58f, 1.92f); glVertex2f(-2.38f, 1.86f);
    glEnd();

    glColor3f(horizonA.r, horizonA.g, horizonA.b);
    glBegin(GL_POLYGON);
        glVertex2f(-5.0f, -3.5f);
        glVertex2f( 5.0f, -3.5f);
        glVertex2f( 5.0f, -2.05f);
        glVertex2f( 4.18f, -1.78f);
        glVertex2f( 3.00f, -2.18f);
        glVertex2f( 1.82f, -1.76f);
        glVertex2f( 0.45f, -2.05f);
        glVertex2f(-0.85f, -1.62f);
        glVertex2f(-2.18f, -2.14f);
        glVertex2f(-3.55f, -1.80f);
        glVertex2f(-5.0f, -2.22f);
    glEnd();

    glColor3f(horizonB.r, horizonB.g, horizonB.b);
    glBegin(GL_POLYGON);
        glVertex2f(-5.0f, -3.5f);
        glVertex2f( 5.0f, -3.5f);
        glVertex2f( 5.0f, -2.75f);
        glVertex2f( 3.55f, -2.46f);
        glVertex2f( 2.10f, -2.76f);
        glVertex2f( 0.20f, -2.45f);
        glVertex2f(-1.68f, -2.82f);
        glVertex2f(-3.05f, -2.50f);
        glVertex2f(-5.0f, -2.82f);
    glEnd();

    glColor3f(accent.r * (0.50f + pulse * 0.34f), accent.g * (0.50f + pulse * 0.34f), accent.b * (0.50f + pulse * 0.34f));
    glLineWidth(1.4f + pulse * 1.0f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(-4.55f, -2.18f);
        glVertex2f(-3.12f, -2.38f + slowDrift * 0.04f);
        glVertex2f(-1.45f, -2.14f);
        glVertex2f( 0.30f, -2.36f - slowDrift * 0.04f);
        glVertex2f( 1.86f, -2.09f);
        glVertex2f( 3.35f, -2.36f);
        glVertex2f( 4.58f, -2.20f);
    glEnd();

    if(theme == 1 || theme == 6) {
        glColor3f(1.0f, 0.22f + pulse * 0.30f, 0.10f);
        glLineWidth(1.8f + fastPulse * 1.4f);
        glBegin(GL_LINES);
            glVertex2f(-4.80f + fmodf(time * 1.25f, 9.5f), 2.90f);
            glVertex2f(-4.35f + fmodf(time * 1.25f, 9.5f), 2.70f);
            glVertex2f(-5.10f + fmodf(time * 1.75f, 10.2f), 0.92f);
            glVertex2f(-4.55f + fmodf(time * 1.75f, 10.2f), 0.72f);
        glEnd();
    }

    if(theme == 2 || theme == 5) {
        glColor3f(0.32f + pulse * 0.30f, 1.0f, 0.88f);
        drawCustomEllipse(3.35f, -0.72f + slowDrift * 0.08f, 0.45f + pulse * 0.10f, 0.12f, 28);
        glLineWidth(1.4f);
        glBegin(GL_LINE_STRIP);
            glVertex2f(2.74f, -0.75f);
            glVertex2f(3.18f, -0.60f + pulse * 0.06f);
            glVertex2f(3.78f, -0.72f);
        glEnd();
    }

    glLineWidth(1.0f);
}

void drawMenuLevelPanel(float x, float y, int levelNumber, const char *title, const char *subtitle, bool unlocked, Color accent)
{
    float time = (float)glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    float pulse = 0.5f + 0.5f * sinf(time * 2.4f + levelNumber);
    float w = 2.05f;
    float h = 1.08f;

    if(unlocked) {
        glColor3f(0.035f, 0.070f, 0.125f);
    } else {
        glColor3f(0.025f, 0.030f, 0.050f);
    }
    glBegin(GL_QUADS);
        glVertex2f(x - w / 2.0f, y - h / 2.0f);
        glVertex2f(x + w / 2.0f, y - h / 2.0f);
        glVertex2f(x + w / 2.0f, y + h / 2.0f);
        glVertex2f(x - w / 2.0f, y + h / 2.0f);
    glEnd();

    if(unlocked) {
        glColor3f(accent.r * (0.62f + pulse * 0.38f), accent.g * (0.62f + pulse * 0.38f), accent.b * (0.62f + pulse * 0.38f));
    } else {
        glColor3f(0.22f, 0.25f, 0.30f);
    }
    glLineWidth(unlocked ? 2.2f : 1.2f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x - w / 2.0f, y - h / 2.0f);
        glVertex2f(x + w / 2.0f, y - h / 2.0f);
        glVertex2f(x + w / 2.0f, y + h / 2.0f);
        glVertex2f(x - w / 2.0f, y + h / 2.0f);
    glEnd();

    if(unlocked) {
        glColor3f(accent.r * 0.45f, accent.g * 0.45f, accent.b * 0.45f);
        drawCustomEllipse(x, y - h / 2.0f + 0.04f, 0.70f + pulse * 0.18f, 0.04f, 24);
        glColor3f(accent.r, accent.g, accent.b);
        drawEnergyDiamond(x - 0.78f, y + 0.25f, 0.08f, 0.13f);
    }

    glColor3f(unlocked ? 0.92f : 0.42f, unlocked ? 0.98f : 0.45f, unlocked ? 1.0f : 0.50f);
    char numberText[18];
    snprintf(numberText, sizeof(numberText), "%d", levelNumber);
    drawText(x - 0.86f, y + 0.20f, numberText);
    drawText(x - 0.45f, y + 0.20f, title);

    if(unlocked) {
        glColor3f(0.70f, 0.84f, 0.92f);
        drawText(x - 0.45f, y - 0.12f, subtitle);

        char keyText[24];
        snprintf(keyText, sizeof(keyText), "Press %d", levelNumber);
        glColor3f(accent.r, accent.g, accent.b);
        drawText(x - 0.45f, y - 0.42f, keyText);
    } else {
        glColor3f(0.54f, 0.60f, 0.66f);
        drawText(x - 0.45f, y - 0.12f, "LOCKED");
        drawText(x - 0.45f, y - 0.42f, subtitle);
    }

    glLineWidth(1.0f);
}

void drawMenu()
{
    glClearColor(0.006f, 0.016f, 0.050f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawModernSpaceBackground(7);
    drawStars();

    float time = (float)glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    float pulse = 0.5f + 0.5f * sinf(time * 2.0f);

    glColor3f(0.16f, 0.56f + pulse * 0.18f, 0.72f + pulse * 0.20f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(-4.15f, 0.60f);
        glVertex2f(-1.40f, 0.92f + pulse * 0.08f);
        glVertex2f( 1.40f, 0.50f - pulse * 0.06f);
        glVertex2f( 4.15f, 0.80f);
    glEnd();

    glColor3f(0.08f, 0.16f, 0.28f);
    glBegin(GL_QUADS);
        glVertex2f(-5.0f, 2.80f);
        glVertex2f( 5.0f, 2.80f);
        glVertex2f( 5.0f, 1.55f);
        glVertex2f(-5.0f, 1.55f);
    glEnd();

    glColor3f(0.85f, 0.95f, 1.0f);
    drawText(-1.75f, 2.35f, "SPACE SECTOR COMMAND");
    glColor3f(0.62f, 0.80f, 0.92f);
    drawText(-2.15f, 1.92f, "Select a mission route");

    drawMenuLevelPanel(-3.45f, 0.55f, 1, "Patrol", "Cold entry sector", true, {0.26f, 0.88f, 1.00f});
    drawMenuLevelPanel(-1.15f, 0.90f, 2, "Raid", "Clear level 1", unlockedLevelCount >= 2, {1.00f, 0.36f, 0.22f});
    drawMenuLevelPanel( 1.15f, 0.55f, 3, "Boss", "Clear level 2", unlockedLevelCount >= 3, {0.82f, 0.36f, 1.00f});
    drawMenuLevelPanel( 3.45f, 0.90f, 4, "Nightmare", "Unlocked challenge", true, {1.00f, 0.16f, 0.58f});
    drawMenuLevelPanel( 1.15f,-0.90f, 5, "Reverse", "Command the boss", unlockedLevelCount >= 5, {0.90f, 0.24f, 0.88f});

    glColor3f(0.78f, 0.90f, 1.0f);
    drawText(-3.35f, -2.10f, "WASD / Arrows move");
    drawText(-0.78f, -2.10f, "Space shoots");
    drawText(1.18f, -2.10f, "U uses ult");
    drawText(2.85f, -2.10f, "M returns");

    glutSwapBuffers();
}

void drawBullets()
{
    /* Player bullets are slim cyan lasers. Ult shots are larger gold beams. */
    for(int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if(!playerBullets[i].active) continue;

        if(playerBullets[i].type == 4) {
            glColor3f(1.0f, 0.90f, 0.20f);
            glBegin(GL_QUADS);
                glVertex2f(playerBullets[i].x - 0.12f, playerBullets[i].y);
                glVertex2f(playerBullets[i].x + 0.12f, playerBullets[i].y);
                glVertex2f(playerBullets[i].x + 0.08f, playerBullets[i].y + 0.70f);
                glVertex2f(playerBullets[i].x - 0.08f, playerBullets[i].y + 0.70f);
            glEnd();
        } else if(playerBullets[i].type == 7) {
            glColor3f(1.0f, 0.08f, 0.36f);
            glBegin(GL_QUADS);
                glVertex2f(playerBullets[i].x - 0.055f, playerBullets[i].y);
                glVertex2f(playerBullets[i].x + 0.055f, playerBullets[i].y);
                glVertex2f(playerBullets[i].x + 0.035f, playerBullets[i].y + 0.42f);
                glVertex2f(playerBullets[i].x - 0.035f, playerBullets[i].y + 0.42f);
            glEnd();

            glColor3f(0.94f, 0.42f, 0.92f);
            glBegin(GL_LINE_LOOP);
                glVertex2f(playerBullets[i].x - 0.070f, playerBullets[i].y - 0.02f);
                glVertex2f(playerBullets[i].x + 0.070f, playerBullets[i].y - 0.02f);
                glVertex2f(playerBullets[i].x + 0.045f, playerBullets[i].y + 0.44f);
                glVertex2f(playerBullets[i].x - 0.045f, playerBullets[i].y + 0.44f);
            glEnd();
        } else {
            glColor3f(0.25f, 0.95f, 1.0f);
            glBegin(GL_QUADS);
                glVertex2f(playerBullets[i].x - 0.035f, playerBullets[i].y);
                glVertex2f(playerBullets[i].x + 0.035f, playerBullets[i].y);
                glVertex2f(playerBullets[i].x + 0.035f, playerBullets[i].y + 0.28f);
                glVertex2f(playerBullets[i].x - 0.035f, playerBullets[i].y + 0.28f);
            glEnd();
        }
    }

    /*
       Enemy bullet types:
       1 = interceptor: small red shot
       2 = bomber: larger orange diamond, slower and stronger
       3 = boss: yellow/purple energy bolt
     */
    for(int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if(!enemyBullets[i].active) continue;

        if(enemyBullets[i].type == 2) {
            if(enemyBullets[i].type == 2) {
                if(currentLevel == 3) {
                    // Homing Missile in Nightmare Level
                    glPushMatrix();
                    glTranslatef(enemyBullets[i].x, enemyBullets[i].y, 0.0f);

                    // Calculate rotation angle so the missile points towards its velocity
                    float angle = atan2(enemyBullets[i].speedY, enemyBullets[i].speedX) * 180.0f / 3.14159f;
                    glRotatef(angle + 90.0f, 0.0f, 0.0f, 1.0f);

                    // Missile Body
                    glColor3f(0.65f, 0.65f, 0.70f);
                    glBegin(GL_QUADS);
                        glVertex2f(-0.04f, 0.15f); glVertex2f(0.04f, 0.15f);
                        glVertex2f(0.04f, -0.15f); glVertex2f(-0.04f, -0.15f);
                    glEnd();
                    // Red Nose Cone
                    glColor3f(1.0f, 0.15f, 0.15f);
                    glBegin(GL_TRIANGLES);
                        glVertex2f(-0.04f, -0.15f); glVertex2f(0.04f, -0.15f); glVertex2f(0.0f, -0.28f);
                    glEnd();
                    // Stabilizer Fins
                    glColor3f(0.25f, 0.25f, 0.25f);
                    glBegin(GL_TRIANGLES);
                        glVertex2f(-0.04f, 0.10f); glVertex2f(-0.12f, 0.18f); glVertex2f(-0.04f, 0.02f);
                        glVertex2f(0.04f, 0.10f); glVertex2f(0.12f, 0.18f); glVertex2f(0.04f, 0.02f);
                    glEnd();
                    // Engine Exhaust Flare
                    glColor3f(1.0f, 0.6f, 0.1f);
                    glBegin(GL_TRIANGLES);
                        glVertex2f(-0.03f, 0.15f); glVertex2f(0.03f, 0.15f); glVertex2f(0.0f, 0.28f);
                    glEnd();

                    glPopMatrix();
                } else {
                    // Standard Bomber Bullet
                    glColor3f(1.0f, 0.58f, 0.08f);
                    glBegin(GL_POLYGON);
                        glVertex2f(enemyBullets[i].x,          enemyBullets[i].y - 0.34f);
                        glVertex2f(enemyBullets[i].x + 0.12f, enemyBullets[i].y - 0.17f);
                        glVertex2f(enemyBullets[i].x,          enemyBullets[i].y);
                        glVertex2f(enemyBullets[i].x - 0.12f, enemyBullets[i].y - 0.17f);
                    glEnd();
                }
            }
        } else if(enemyBullets[i].type == 3) {
            glColor3f(1.0f, 0.95f, 0.25f);
            glBegin(GL_QUADS);
                glVertex2f(enemyBullets[i].x - 0.08f, enemyBullets[i].y);
                glVertex2f(enemyBullets[i].x + 0.08f, enemyBullets[i].y);
                glVertex2f(enemyBullets[i].x + 0.04f, enemyBullets[i].y - 0.40f);
                glVertex2f(enemyBullets[i].x - 0.04f, enemyBullets[i].y - 0.40f);
            glEnd();

            glColor3f(0.72f, 0.18f, 1.0f);
            glBegin(GL_LINE_LOOP);
                glVertex2f(enemyBullets[i].x - 0.10f, enemyBullets[i].y + 0.03f);
                glVertex2f(enemyBullets[i].x + 0.10f, enemyBullets[i].y + 0.03f);
                glVertex2f(enemyBullets[i].x + 0.05f, enemyBullets[i].y - 0.43f);
                glVertex2f(enemyBullets[i].x - 0.05f, enemyBullets[i].y - 0.43f);
            glEnd();
        } else if(enemyBullets[i].type == 5) {
            // Neon Magenta Long Beam
            glColor3f(1.0f, 0.0f, 0.4f);
            glBegin(GL_QUADS);
                glVertex2f(enemyBullets[i].x - 0.025f, enemyBullets[i].y);
                glVertex2f(enemyBullets[i].x + 0.025f, enemyBullets[i].y);
                glVertex2f(enemyBullets[i].x + 0.025f, enemyBullets[i].y - 0.65f);
                glVertex2f(enemyBullets[i].x - 0.025f, enemyBullets[i].y - 0.65f);
            glEnd();
        } else if(enemyBullets[i].type == 7) {
            glColor3f(0.25f, 0.95f, 1.0f);
            glBegin(GL_QUADS);
                glVertex2f(enemyBullets[i].x - 0.035f, enemyBullets[i].y);
                glVertex2f(enemyBullets[i].x + 0.035f, enemyBullets[i].y);
                glVertex2f(enemyBullets[i].x + 0.035f, enemyBullets[i].y - 0.34f);
                glVertex2f(enemyBullets[i].x - 0.035f, enemyBullets[i].y - 0.34f);
            glEnd();
        } else if(enemyBullets[i].type == 6) {
            // Boss Spiral Orb (Cyan Core, Magenta Outline)
            glColor3f(0.0f, 0.8f, 1.0f);
            glBegin(GL_POLYGON);
                glVertex2f(enemyBullets[i].x, enemyBullets[i].y + 0.04f);
                glVertex2f(enemyBullets[i].x + 0.04f, enemyBullets[i].y);
                glVertex2f(enemyBullets[i].x, enemyBullets[i].y - 0.04f);
                glVertex2f(enemyBullets[i].x - 0.04f, enemyBullets[i].y);
            glEnd();

            glColor3f(1.0f, 0.0f, 0.8f);
            glBegin(GL_LINE_LOOP);
                glVertex2f(enemyBullets[i].x, enemyBullets[i].y + 0.06f);
                glVertex2f(enemyBullets[i].x + 0.06f, enemyBullets[i].y);
                glVertex2f(enemyBullets[i].x, enemyBullets[i].y - 0.06f);
                glVertex2f(enemyBullets[i].x - 0.06f, enemyBullets[i].y);
            glEnd();
        } else {
            glColor3f(1.0f, 0.25f, 0.15f);
            glBegin(GL_QUADS);
                glVertex2f(enemyBullets[i].x - 0.045f, enemyBullets[i].y);
                glVertex2f(enemyBullets[i].x + 0.045f, enemyBullets[i].y);
                glVertex2f(enemyBullets[i].x + 0.045f, enemyBullets[i].y - 0.25f);
                glVertex2f(enemyBullets[i].x - 0.045f, enemyBullets[i].y - 0.25f);
            glEnd();
        }
    }
}

void drawPlayer()
{
    glPushMatrix();
        glTranslatef(playerX, playerY, 0.0f);
        glScalef(0.28f, 0.28f, 1.0f);
        if(currentLevel == 4) {
            drawReverseBoss2D();
        } else {
            drawSpaceship2D();
        }
    glPopMatrix();
}

void drawEnemyHealthBar(Enemy enemy)
{
    /* The boss uses a big health bar in the HUD instead. */
    if(enemy.type == 2) return;

    float percent = (float)enemy.health / (float)enemy.maxHealth;
    drawBar(enemy.x - 0.42f, enemy.y + 0.55f, 0.84f, 0.08f, percent, {1.0f, 0.20f, 0.18f});
}

void drawEnemies()
{
    for(int i = 0; i < MAX_ENEMIES; i++) {
        if(!enemies[i].active) continue;

        glPushMatrix();
            glTranslatef(enemies[i].x, enemies[i].y, 0.0f);
            glRotatef(180.0f, 0.0f, 0.0f, 1.0f); // Turn enemy ships to face the player.

            if(enemies[i].type == 2) {
                if(currentLevel == 3) {
                    glScalef(0.40f, 0.40f, 1.0f);
                    drawVoidSeraph2D(); // Nightmare boss
                } else if(currentLevel == 4) {
                    glScalef(0.28f, 0.28f, 1.0f);
                    drawSpaceship2D(); // Hero ship enemies in reverse level
                } else {
                    glScalef(0.42f, 0.42f, 1.0f);
                    drawBoss2D(); // Regular boss
                }
            } else if(currentLevel == 4) {
                glScalef(0.26f, 0.26f, 1.0f);
                drawSpaceship2D(); // All hero units appear as player-style ships
            } else if(enemies[i].type == 3) {
                glScalef(0.30f, 0.30f, 1.0f);
                drawLaserEnemy2D();
            } else if(enemies[i].type == 1) {
                glScalef(0.32f, 0.32f, 1.0f);
                if(currentLevel == 3) {
                    drawNightmareMissileShip2D();
                } else {
                    drawEnemyBomber2D();
                }
            } else {
                glScalef(0.34f, 0.34f, 1.0f);
                drawEnemyShip2D();
            }
        glPopMatrix();

        drawEnemyHealthBar(enemies[i]);
    }
}

void drawAsteroids()
{
    for(int i = 0; i < MAX_ASTEROIDS; i++) {
        if(!asteroids[i].active) continue;

        glPushMatrix();
            glTranslatef(asteroids[i].x, asteroids[i].y, 0.0f);
            glRotatef(asteroids[i].angle, 0.0f, 0.0f, 1.0f);
            glScalef(asteroids[i].scale, asteroids[i].scale, 1.0f);
            drawAsteroid2D();
        glPopMatrix();
    }
}

void drawHud()
{
    char text[80];
    snprintf(text, sizeof(text), "Level %d", currentLevel + 1);

    glColor3f(0.85f, 0.95f, 1.0f);
    drawText(-4.8f, 3.15f, text);

    /* Player health stays on the upper-left. */
    if(currentLevel == 4)
        drawText(-4.8f, 2.82f, "Boss Hull");
    else
        drawText(-4.8f, 2.82f, "Player HP");
    drawBar(-3.45f, 2.86f, 2.00f, 0.18f, (float)playerHealth / playerMaxHealthForCurrentLevel(), {0.15f, 1.0f, 0.35f});

    /* Ult meter: fills with successful hits, then press U. */
    if(ultReady)
        drawText(-4.8f, 2.45f, currentLevel == 4 ? "Barrage Ready" : "Ult Ready");
    else
        drawText(-4.8f, 2.45f, currentLevel == 4 ? "Barrage" : "Ult");
    drawBar(-3.45f, 2.49f, 2.00f, 0.15f, (float)ultHits / ULT_HITS_REQUIRED, {1.0f, 0.86f, 0.20f});

    if(levels[currentLevel].bossLevel && enemies[0].active && enemies[0].type == 2) {
        float percent = (float)enemies[0].health / (float)enemies[0].maxHealth;

        /* Boss health is centered and lower so it does not blend with player HP. */
        glColor3f(1.0f, 0.72f, 0.20f);
        drawText(-0.70f, 2.62f, "BOSS HP");
        drawBar(-2.65f, 2.28f, 5.30f, 0.25f, percent, {1.0f, 0.18f, 0.10f});
    }
}

void drawEndScreen()
{
    if(gameState == STATE_LEVEL_CLEAR) {
        glColor3f(0.72f, 1.0f, 0.78f);
        drawText(-1.20f, 0.20f, "LEVEL COMPLETE");
        drawText(-1.85f, -0.20f, "Next level unlocked");
    } else if(gameState == STATE_GAME_OVER) {
        glColor3f(1.0f, 0.35f, 0.30f);
        drawText(-0.85f, 0.20f, "GAME OVER");
    }

    glColor3f(0.85f, 0.95f, 1.0f);
    drawText(-1.55f, -0.70f, "Press R to retry");
    drawText(-1.55f, -1.10f, "Press M for menu");
}

/* =========================================================
   SHOOTING AND COLLISIONS
   ========================================================= */
bool spawnPlayerBullet(float x, float y, float speedX, float speedY, int damage, int type)
{
    for(int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if(!playerBullets[i].active) {
            playerBullets[i].active = true;
            playerBullets[i].x = x;
            playerBullets[i].y = y;
            playerBullets[i].speed = speedY;
            playerBullets[i].speedX = speedX;
            playerBullets[i].speedY = speedY;
            playerBullets[i].damage = damage;
            playerBullets[i].type = type;
            return true;
        }
    }

    return false;
}

void shootPlayerBullet()
{
    bool fired = false;

    if(currentLevel == 4) {
        fired |= spawnPlayerBullet(playerX, playerY + 0.42f, 0.0f, REVERSE_BOSS_BULLET_SPEED, 2, 7);
        fired |= spawnPlayerBullet(playerX - 0.24f, playerY + 0.18f, -0.018f, REVERSE_BOSS_BULLET_SPEED * 0.92f, 2, 7);
        fired |= spawnPlayerBullet(playerX + 0.24f, playerY + 0.18f,  0.018f, REVERSE_BOSS_BULLET_SPEED * 0.92f, 2, 7);
    } else {
        fired = spawnPlayerBullet(playerX, playerY + 0.35f, 0.0f, 0.17f, 1, 0);
    }

    if(fired) {
        playLaserSound();
        audioCooldown = (currentLevel == 4) ? 10 : 15;
    }
}

void shootUltBullet()
{
    if(!ultReady) return;

    bool fired = false;

    if(currentLevel == 4) {
        for(int b = -3; b <= 3; b++) {
            fired |= spawnPlayerBullet(playerX + b * 0.12f, playerY + 0.42f, b * 0.020f, ULT_BULLET_SPEED + 0.035f, 5, 4);
        }
    } else {
        fired = spawnPlayerBullet(playerX, playerY + 0.35f, 0.0f, ULT_BULLET_SPEED, ULT_DAMAGE, 4);
    }

    if(fired) {
        /* Spend the ult after firing. */
        ultHits = 0;
        ultReady = false;
    }
}
void shootBossSpiralBullet(float x, float y, float speedX, float speedY, int damage) {
    for(int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if(!enemyBullets[i].active) {
            enemyBullets[i].active = true;
            enemyBullets[i].x = x;
            enemyBullets[i].y = y;
            enemyBullets[i].speedX = speedX;
            enemyBullets[i].speedY = speedY;
            enemyBullets[i].damage = damage;
            enemyBullets[i].type = 6;

            if(audioCooldown <= 0) {
                playEnemyLaserSound();
                audioCooldown = 4;
            }
            return;
        }
    }
}

void shootEnemyVectorBullet(float x, float y, float speedX, float speedY, int damage, int type) {
    for(int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if(!enemyBullets[i].active) {
            enemyBullets[i].active = true;
            enemyBullets[i].x = x;
            enemyBullets[i].y = y;
            enemyBullets[i].speedX = speedX;
            enemyBullets[i].speedY = speedY;
            enemyBullets[i].damage = damage;
            enemyBullets[i].type = type;

            if(audioCooldown <= 0) {
                playEnemyLaserSound();
                audioCooldown = 8;
            }
            return;
        }
    }
}

void shootEnemyBullet(float x, float y, float speed, int damage, int type)
{
    /* Reuse the first inactive enemy bullet slot. */
    for(int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if(!enemyBullets[i].active) {
            enemyBullets[i].active = true;
            enemyBullets[i].x = x;
            enemyBullets[i].y = y;
            enemyBullets[i].speed = speed;
            enemyBullets[i].damage = damage;
            enemyBullets[i].type = type;

            if(type == 2 && currentLevel == 3) {
                float dx = playerX - x;
                float dy = playerY - y;
                float distance = sqrtf(dx * dx + dy * dy);

                if(distance != 0) {
                    enemyBullets[i].speedX = (dx / distance) * speed;
                    enemyBullets[i].speedY = (dy / distance) * speed;
                } else {
                    enemyBullets[i].speedX = 0.0f;
                    enemyBullets[i].speedY = -speed;
                }
            } else {
                enemyBullets[i].speedX = 0.0f;
                enemyBullets[i].speedY = -speed;
            }

            if(audioCooldown <= 0) {
                playEnemyLaserSound();
                audioCooldown = 10;
            }
            return;
        }
    }
}

bool pointInBox(float px, float py, float cx, float cy, float halfW, float halfH)
{
    return fabsf(px - cx) < halfW && fabsf(py - cy) < halfH;
}

bool playerHit(float x, float y)
{
    float halfSize = (currentLevel == 4) ? 0.48f : 0.38f;
    return pointInBox(x, y, playerX, playerY, halfSize, halfSize);
}

bool bulletHitEnemy(Bullet bullet, Enemy enemy)
{
    float hitW = (enemy.type == 2) ? 1.55f : 0.62f;
    float hitH = (enemy.type == 2) ? 1.10f : 0.60f;
    return pointInBox(bullet.x, bullet.y, enemy.x, enemy.y, hitW, hitH);
}

void damagePlayer(int amount)
{
    if(gameState == STATE_GAME_OVER) return;

    playerHealth -= amount;
    if(playerHealth <= 0) {
        playerHealth = 0;
        gameState = STATE_GAME_OVER;

        playPlayerDeathSound();
        loseTimer = 75;
    }
}

void checkPlayerBullets()
{
    for(int b = 0; b < MAX_PLAYER_BULLETS; b++) {
        if(!playerBullets[b].active) continue;

        for(int e = 0; e < MAX_ENEMIES; e++) {
            if(!enemies[e].active) continue;

            if(bulletHitEnemy(playerBullets[b], enemies[e])) {
                playerBullets[b].active = false;
                enemies[e].health -= playerBullets[b].damage;

                /* Each successful hit charges the ult. */
                if(!ultReady) {
                    ultHits++;
                    if(ultHits >= ULT_HITS_REQUIRED) {
                        ultHits = ULT_HITS_REQUIRED;
                        ultReady = true;
                    }
                }

                if(enemies[e].health <= 0){
                    playEnemyDeathSound();
                    enemies[e].active = false;
                }
                break;
            }
        }
    }
}

void checkEnemyBullets()
{
    for(int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if(!enemyBullets[i].active) continue;

        if(playerHit(enemyBullets[i].x, enemyBullets[i].y)) {
            enemyBullets[i].active = false;
            damagePlayer(enemyBullets[i].damage);
        }
    }
}

void checkAsteroidHits()
{
    for(int i = 0; i < MAX_ASTEROIDS; i++) {
        if(!asteroids[i].active) continue;

        if(pointInBox(asteroids[i].x, asteroids[i].y, playerX, playerY, 0.38f + asteroids[i].scale, 0.38f + asteroids[i].scale)) {
            asteroids[i].active = false;
            damagePlayer(18);
        }
    }
}

bool levelCleared()
{
    for(int i = 0; i < MAX_ENEMIES; i++) {
        if(enemies[i].active)
            return false;
    }
    return true;
}

/* =========================================================
   UPDATE
   ========================================================= */
void updatePlayer()
{
    if(keyDown['a'] || keyDown['A'] || specialDown[GLUT_KEY_LEFT])  playerX -= PLAYER_SPEED;
    if(keyDown['d'] || keyDown['D'] || specialDown[GLUT_KEY_RIGHT]) playerX += PLAYER_SPEED;
    if(keyDown['w'] || keyDown['W'] || specialDown[GLUT_KEY_UP])    playerY += PLAYER_SPEED;
    if(keyDown['s'] || keyDown['S'] || specialDown[GLUT_KEY_DOWN])  playerY -= PLAYER_SPEED;

    playerX = clampFloat(playerX, -4.55f, 4.55f);
    playerY = clampFloat(playerY, -3.00f, 2.20f);
}

void updateStars()
{
    for(int i = 0; i < MAX_STARS; i++) {
        stars[i].y -= stars[i].speed;
        if(stars[i].y < -WORLD_H / 2.0f) {
            stars[i].y = WORLD_H / 2.0f;
            stars[i].x = randomRange(-WORLD_W / 2.0f, WORLD_W / 2.0f);
        }
    }
}

void updatePlayerBullets()
{
    for(int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if(!playerBullets[i].active) continue;

        playerBullets[i].x += playerBullets[i].speedX;
        playerBullets[i].y += playerBullets[i].speedY;
        if(playerBullets[i].y > WORLD_H / 2.0f || playerBullets[i].x < -WORLD_W / 2.0f || playerBullets[i].x > WORLD_W / 2.0f)
            playerBullets[i].active = false;
    }
}

void updateEnemyBullets()
{
    for(int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if(!enemyBullets[i].active) continue;

        enemyBullets[i].x += enemyBullets[i].speedX;
        enemyBullets[i].y += enemyBullets[i].speedY;

        if(enemyBullets[i].y < -WORLD_H / 2.0f || enemyBullets[i].y > WORLD_H / 2.0f ||
           enemyBullets[i].x < -WORLD_W / 2.0f || enemyBullets[i].x > WORLD_W / 2.0f) {
            enemyBullets[i].active = false;
        }
    }
}

void updateEnemies()
{
    LevelSettings level = levels[currentLevel];
    float time = (float)glutGet(GLUT_ELAPSED_TIME) * 0.002f;

    for(int i = 0; i < MAX_ENEMIES; i++) {
        if(!enemies[i].active) continue;

        /* Wobble left/right so enemies do not stand completely still. */
        float wobble = sinf(time + i * 1.7f);
        enemies[i].x += wobble * enemies[i].speed;
        enemies[i].x = clampFloat(enemies[i].x, -4.25f, 4.25f);

        if(enemies[i].type == 2) {
            /* Boss stays near the top and moves slightly. */
            enemies[i].y = 1.70f + sinf(time * 0.7f) * 0.12f;
        } else {
            /* Regular enemies slowly drift downward. */
            enemies[i].y -= enemies[i].speed * 0.22f;
            if(enemies[i].y < -3.2f) {
                enemies[i].x = randomRange(-4.0f, 4.0f);
                enemies[i].y = randomRange(2.0f, 3.2f);
            }
        }

        enemies[i].shootTimer--;
        if(enemies[i].shootTimer <= 0) {
            if(currentLevel == 4) {
                float dx = playerX - enemies[i].x;
                float dy = playerY - enemies[i].y;
                float distance = sqrtf(dx * dx + dy * dy);
                float shotSpeed = 0.096f + enemies[i].type * 0.010f;
                float vx = 0.0f;
                float vy = -shotSpeed;

                if(distance != 0.0f) {
                    vx = (dx / distance) * shotSpeed;
                    vy = (dy / distance) * shotSpeed;
                }

                shootEnemyVectorBullet(enemies[i].x, enemies[i].y - 0.34f, vx, vy, 7 + enemies[i].type, 7);

                if(enemies[i].type == 3) {
                    shootEnemyBullet(enemies[i].x - 0.18f, enemies[i].y - 0.28f, 0.13f, 9, 7);
                    shootEnemyBullet(enemies[i].x + 0.18f, enemies[i].y - 0.28f, 0.13f, 9, 7);
                    enemies[i].shootTimer = level.enemyShootDelay + 36;
                } else {
                    enemies[i].shootTimer = level.enemyShootDelay + (int)randomRange(0, 30);
                }
            } else if(enemies[i].type == 2) {
                if(currentLevel == 3) {
                    // Static variable keeps track of the angle between frames
                    static float spiralAngle = 0.0f;
                    float radians = spiralAngle * 3.14159f / 180.0f;
                    float speed = 0.035f;

                    // Fire 4 bullets at once in a cross pattern that spins
                    for(int b = 0; b < 4; b++) {
                        float offsetRad = radians + (b * 3.14159f / 2.0f);
                        shootBossSpiralBullet(enemies[i].x, enemies[i].y, cosf(offsetRad) * speed, sinf(offsetRad) * speed, 12);
                    }

                    spiralAngle += 18.0f; // Increase this to make the spiral spin faster!
                    if(spiralAngle >= 360.0f) spiralAngle -= 360.0f;

                    enemies[i].shootTimer = 3; // Fires extremely rapidly!
                } else {
                    shootEnemyBullet(enemies[i].x, enemies[i].y - 0.55f, BOSS_BULLET_SPEED, BOSS_CENTER_DAMAGE, 3);
                    shootEnemyBullet(enemies[i].x - 0.55f, enemies[i].y - 0.35f, BOSS_BULLET_SPEED, BOSS_SIDE_DAMAGE, 3);
                    shootEnemyBullet(enemies[i].x + 0.55f, enemies[i].y - 0.35f, BOSS_BULLET_SPEED, BOSS_SIDE_DAMAGE, 3);
                    enemies[i].shootTimer = level.bossShootDelay;
                }
            }else if(enemies[i].type == 3) {
                /* Laser Enemy: Extremely fast, high damage, type 5 bullet */
                shootEnemyBullet(enemies[i].x, enemies[i].y - 0.45f, 0.16f, 15, 5);
                enemies[i].shootTimer = level.enemyShootDelay + 50;
            } else if(enemies[i].type == 1) {
                /* Bomber shot: slower, fires less often, but hurts more. */
                shootEnemyBullet(enemies[i].x, enemies[i].y - 0.35f, BOMBER_BULLET_SPEED, BOMBER_BULLET_DAMAGE, 2);
                enemies[i].shootTimer = level.enemyShootDelay + BOMBER_EXTRA_DELAY + (int)randomRange(0, 35);
            } else {
                /* Interceptor shot: faster, weaker, fires more often. */
                shootEnemyBullet(enemies[i].x, enemies[i].y - 0.35f, INTERCEPTOR_BULLET_SPEED, INTERCEPTOR_BULLET_DAMAGE, 1);
                enemies[i].shootTimer = level.enemyShootDelay + INTERCEPTOR_EXTRA_DELAY + (int)randomRange(0, 35);
            }
        }
    }
}

void updateAsteroids()
{
    asteroidTimer--;
    if(asteroidTimer <= 0) {
        spawnAsteroid();
        asteroidTimer = levels[currentLevel].asteroidDelay + (int)randomRange(0, 35);
    }

    for(int i = 0; i < MAX_ASTEROIDS; i++) {
        if(!asteroids[i].active) continue;

        asteroids[i].y -= asteroids[i].speed;
        asteroids[i].x += asteroids[i].speedX;
        asteroids[i].angle += asteroids[i].spin;
        if(asteroids[i].y < -4.0f || asteroids[i].x < -6.0f || asteroids[i].x > 6.0f)
            asteroids[i].active = false;
    }
}



void updateGame()
{
    if(audioCooldown > 0) audioCooldown--;

    if(loseTimer > 0) {
        loseTimer--;
        if(loseTimer == 0) {
            playLoseSound();
        }
    }

    updateStars();
    if(gameState == STATE_INTRO_CUTSCENE || gameState == STATE_END_CUTSCENE)
    cutsceneUpdate(gameState, cutsceneLine, cutsceneVisibleChars, cutsceneTimer);

    if(gameState == STATE_MENU || gameState == STATE_INTRO_CUTSCENE || gameState == STATE_END_CUTSCENE)
        return;

    if(gameState != STATE_PLAYING)
        return;

    updatePlayer();
    updatePlayerBullets();
    updateEnemyBullets();
    updateEnemies();
    updateAsteroids();

    checkPlayerBullets();
    checkEnemyBullets();
    checkAsteroidHits();

    if(gameState == STATE_PLAYING && levelCleared()) {

        playWinSound();
        audioCooldown = 60;

        if(currentLevel == 2) {
            unlockedLevelCount = 3;
            startCutscene(STATE_END_CUTSCENE);
        } else {
            if(unlockedLevelCount < currentLevel + 2)
                unlockedLevelCount = currentLevel + 2;
            gameState = STATE_LEVEL_CLEAR;
        }
    }
}

/* =========================================================
   GLUT CALLBACKS
   ========================================================= */
void onDisplay()
{
    if(gameState == STATE_INTRO_CUTSCENE || gameState == STATE_END_CUTSCENE) {
        drawCutscene(gameState, cutsceneLine, cutsceneVisibleChars);
        return;
    }

    if(gameState == STATE_MENU) {
        drawMenu();
        return;
    }


    Color bg = levels[currentLevel].background;
    glClearColor(bg.r, bg.g, bg.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    int backgroundTheme = currentLevel;
    if(currentLevel == 3)
        backgroundTheme = 4;
    if(currentLevel == 4)
        backgroundTheme = 8;
    if(gameState == STATE_LEVEL_CLEAR)
        backgroundTheme = 5;
    if(gameState == STATE_GAME_OVER)
        backgroundTheme = 6;

    drawModernSpaceBackground(backgroundTheme);
    drawStars();
    drawAsteroids();
    drawBullets();
    drawPlayer();
    drawEnemies();
    drawHud();

    if(gameState == STATE_LEVEL_CLEAR || gameState == STATE_GAME_OVER)
        drawEndScreen();

    glutSwapBuffers();
}

void onResize(int w, int h)
{
    if(h == 0) h = 1;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = (float)w / (float)h;
    if(aspect >= WORLD_W / WORLD_H) {
        float visibleW = WORLD_H * aspect;
        glOrtho(-visibleW / 2.0f, visibleW / 2.0f, -WORLD_H / 2.0f, WORLD_H / 2.0f, -1.0f, 1.0f);
    } else {
        float visibleH = WORLD_W / aspect;
        glOrtho(-WORLD_W / 2.0f, WORLD_W / 2.0f, -visibleH / 2.0f, visibleH / 2.0f, -1.0f, 1.0f);
    }

    glMatrixMode(GL_MODELVIEW);
}

void onTimer(int value)
{
    updateGame();
    glutPostRedisplay();
    glutTimerFunc(16, onTimer, 0);
}

void onKeyDown(unsigned char key, int x, int y)
{
    keyDown[key] = true;

    if(key == 27 || key == 'q' || key == 'Q')
        exit(0);

    if(gameState == STATE_INTRO_CUTSCENE || gameState == STATE_END_CUTSCENE) {
        if(key == ' ' || key == 13)
            if(cutsceneAdvance(gameState, cutsceneLine, cutsceneVisibleChars, cutsceneTimer))
                gameState = STATE_MENU;
        return;
    }

    if(key == 'm' || key == 'M') {
        gameState = STATE_MENU;
        clearGameObjects();
        return;
    }

    if(key == '1') startLevel(0);
    if(key == '2') startLevel(1);
    if(key == '3') startLevel(2);
    if(key == '4') startLevel(3);
    if(key == '5') startLevel(4);

    if((key == 'r' || key == 'R') && gameState != STATE_MENU)
        startLevel(currentLevel);

    if(key == ' ' && gameState == STATE_PLAYING)
        shootPlayerBullet();

    if((key == 'u' || key == 'U') && gameState == STATE_PLAYING)
        shootUltBullet();
}

void onKeyUp(unsigned char key, int x, int y)
{
    keyDown[key] = false;
}

void onSpecialDown(int key, int x, int y)
{
    if(key >= 0 && key < 256)
        specialDown[key] = true;
}

void onSpecialUp(int key, int x, int y)
{
    if(key >= 0 && key < 256)
        specialDown[key] = false;
}

/* ================= MAIN ================= */
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(900, 600);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Space Shooter");

    glutDisplayFunc(onDisplay);
    glutReshapeFunc(onResize);
    glutKeyboardFunc(onKeyDown);
    glutKeyboardUpFunc(onKeyUp);
    glutSpecialFunc(onSpecialDown);
    glutSpecialUpFunc(onSpecialUp);
    glutTimerFunc(16, onTimer, 0);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    srand(7);
    resetStars();

    playStartSound();

    glutMainLoop();
    return 0;
}
