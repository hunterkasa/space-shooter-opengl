#ifndef CUSTOM_LEVEL_H
#define CUSTOM_LEVEL_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>
#include <cstdlib>


inline void drawDestroyedPlanet() {
    static float planetAngle = 0.0f;
    planetAngle += 0.20f;
    if(planetAngle >= 360.0f) planetAngle -= 360.0f;

    glPushMatrix();

    glTranslatef(2.5f, 1.5f, 0.0f);
    glRotatef(planetAngle, 0.0f, 0.0f, 1.0f);

    // Shattered floating debris
    glColor3f(0.12f, 0.04f, 0.04f); 
    glBegin(GL_TRIANGLES);
        glVertex2f(-1.8f, 1.7f); glVertex2f(-1.4f, 1.5f); glVertex2f(-1.6f, 2.0f);
        glVertex2f(-2.3f, 0.8f); glVertex2f(-2.0f, 0.6f); glVertex2f(-1.9f, 1.0f);
        glVertex2f(-1.1f, 2.3f); glVertex2f(-0.8f, 2.0f); glVertex2f(-0.6f, 2.4f);
    glEnd();

    // Base
    glColor3f(0.15f, 0.05f, 0.05f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i += 5) {
        float theta = i * 3.14159f / 180.0f;
        float radius = 2.5f;

        if(i > 110 && i < 200) {
            radius -= 0.8f * sinf((i - 110) * 3.14159f / 90.0f)
                    + 0.15f * sinf((i - 110) * 3.14159f / 15.0f);
        }

        glVertex2f(radius * cosf(theta), radius * sinf(theta));
    }
    glEnd();

    // Surface
    glColor3f(0.10f, 0.03f, 0.03f); 


    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i+=20) {
        float t = i * 3.14159f / 180.0f;
        glVertex2f(0.8f + 0.3f*cosf(t), -0.6f + 0.15f*sinf(t));
    }
    glEnd();


    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i+=20) {
        float t = i * 3.14159f / 180.0f;
        glVertex2f(-0.2f + 0.4f*cosf(t), -1.5f + 0.2f*sinf(t));
    }
    glEnd();


    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i+=20) {
        float t = i * 3.14159f / 180.0f;
        glVertex2f(1.5f + 0.25f*cosf(t), 0.7f + 0.35f*sinf(t));
    }
    glEnd();

    // Core
    glColor3f(1.0f, 0.4f, 0.0f);
    glBegin(GL_POLYGON);
    for(int i = 125; i <= 185; i += 10) {
        float theta = i * 3.14159f / 180.0f;
        float coreRadius = 1.6f + 0.1f * sinf(i * 3.14f / 10.0f);
        glVertex2f(coreRadius * cosf(theta), coreRadius * sinf(theta));
    }
    glVertex2f(-0.6f, 0.6f); 
    glEnd();

    // Lava
    glColor3f(1.0f, 0.35f, 0.0f); 
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(-1.0f, 0.9f);
        glVertex2f(-0.5f, 0.5f);
        glVertex2f(-0.2f, -0.3f);
        glVertex2f(0.4f, -0.8f);
        glVertex2f(1.2f, -1.2f);
    glEnd();

    glColor3f(1.0f, 0.15f, 0.0f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(-0.5f, 0.5f);
        glVertex2f(0.1f, 0.8f);
        glVertex2f(0.8f, 0.6f);
        glVertex2f(1.5f, 0.9f);
    glEnd();

    glLineWidth(1.5f);
    glColor3f(1.0f, 0.6f, 0.0f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(-0.2f, -0.3f);
        glVertex2f(-0.6f, -0.7f);
        glVertex2f(-0.5f, -1.4f);
    glEnd();

    glBegin(GL_LINE_STRIP);
        glVertex2f(0.4f, -0.8f);
        glVertex2f(0.9f, -0.5f);
        glVertex2f(1.6f, -0.7f);
    glEnd();

    glLineWidth(1.0f);
    glPopMatrix();
}


#define NIGHTMARE_NUM_WANDER 3
#define NIGHTMARE_WORLD_W 10.0f
#define NIGHTMARE_WORLD_H 7.0f

inline float nightmareRand01() {
    return (float)rand() / (float)RAND_MAX;
}

inline void nightmareSpawnPlanet(int i,float *px, float *py, float *vx, float *vy,float *radius, float *colR, float *colG, float *colB, float *spinSpeed, bool fromOffscreen) {

    radius[i] = 0.30f + nightmareRand01() * 0.45f;
    float r = radius[i];

    float downSpeed = 0.014f + nightmareRand01() * 0.012f;
    float sideSpeed = (nightmareRand01() * 2.0f - 1.0f) * 0.010f; 
    vx[i] = sideSpeed;
    vy[i] = -downSpeed;

    spinSpeed[i] = 0.15f + nightmareRand01() * 0.35f;
    if(nightmareRand01() < 0.5f) spinSpeed[i] = -spinSpeed[i];

    int palette = rand() % 5;
    switch(palette) {
        case 0: colR[i]=0.35f; colG[i]=0.22f; colB[i]=0.48f; break;
        case 1: colR[i]=0.20f; colG[i]=0.42f; colB[i]=0.50f; break; 
        case 2: colR[i]=0.32f; colG[i]=0.40f; colB[i]=0.26f; break; 
        case 3: colR[i]=0.45f; colG[i]=0.40f; colB[i]=0.30f; break; 
        default: colR[i]=0.30f; colG[i]=0.32f; colB[i]=0.45f; break; 
    }

    px[i] = -NIGHTMARE_WORLD_W * 0.5f + nightmareRand01() * NIGHTMARE_WORLD_W;

    if(fromOffscreen) {
        py[i] = NIGHTMARE_WORLD_H * 0.5f + r + 0.3f;
    } else {
        py[i] = nightmareRand01() * NIGHTMARE_WORLD_H * 0.5f;
    }
}

inline void drawWanderingPlanets() {
    static bool initialized = false;
    static float px[NIGHTMARE_NUM_WANDER], py[NIGHTMARE_NUM_WANDER];
    static float vx[NIGHTMARE_NUM_WANDER], vy[NIGHTMARE_NUM_WANDER];
    static float radius[NIGHTMARE_NUM_WANDER];
    static float colR[NIGHTMARE_NUM_WANDER], colG[NIGHTMARE_NUM_WANDER], colB[NIGHTMARE_NUM_WANDER];
    static float spinAngle[NIGHTMARE_NUM_WANDER];
    static float spinSpeed[NIGHTMARE_NUM_WANDER];

    if(!initialized) {
        for(int i = 0; i < NIGHTMARE_NUM_WANDER; i++) {
            spinAngle[i] = nightmareRand01() * 360.0f;
            nightmareSpawnPlanet(i, px, py, vx, vy, radius, colR, colG, colB, spinSpeed, false);
        }
        initialized = true;
    }

    for(int i = 0; i < NIGHTMARE_NUM_WANDER; i++) {
        px[i] += vx[i];
        py[i] += vy[i];
        spinAngle[i] += spinSpeed[i];
        if(spinAngle[i] >= 360.0f) spinAngle[i] -= 360.0f;
        if(spinAngle[i] < 0.0f) spinAngle[i] += 360.0f;

        float halfW = NIGHTMARE_WORLD_W * 0.5f + radius[i] + 0.2f;
        float halfH = NIGHTMARE_WORLD_H * 0.5f + radius[i] + 0.2f;

        if (px[i] > halfW) px[i] = -halfW;
        else if (px[i] < -halfW) px[i] = halfW;

        if (py[i] > halfH) py[i] = -halfH;
        else if (py[i] < -halfH) py[i] = halfH;

        // Draw the planet
        glPushMatrix();
        glTranslatef(px[i], py[i], 0.0f);
        glRotatef(spinAngle[i], 0.0f, 0.0f, 1.0f);

        float r = radius[i];

        // main body
        glColor3f(colR[i], colG[i], colB[i]);
        glBegin(GL_POLYGON);
        for(int a = 0; a < 360; a += 12) {
            float t = a * 3.14159f / 180.0f;
            glVertex2f(r * cosf(t), r * sinf(t));
        }
        glEnd();

        // Shadow half 
        glColor3f(colR[i] * 0.35f, colG[i] * 0.35f, colB[i] * 0.40f);
        glBegin(GL_POLYGON);
        for(int a = 100; a <= 260; a += 12) {
            float t = a * 3.14159f / 180.0f;
            glVertex2f(r * cosf(t), r * sinf(t));
        }
        glEnd();

        glColor3f(colR[i] * 0.65f, colG[i] * 0.65f, colB[i] * 0.65f);
        glBegin(GL_POLYGON);
        for(int a = 0; a < 360; a += 30) {
            float t = a * 3.14159f / 180.0f;
            glVertex2f(r * 0.30f + r * 0.18f * cosf(t),
                       r * 0.15f + r * 0.12f * sinf(t));
        }
        glEnd();
        glBegin(GL_POLYGON);
        for(int a = 0; a < 360; a += 30) {
            float t = a * 3.14159f / 180.0f;
            glVertex2f(-r * 0.20f + r * 0.10f * cosf(t),
                       -r * 0.35f + r * 0.10f * sinf(t));
        }
        glEnd();

        glPopMatrix();
    }
}

inline void drawNightmareSectorBackground() {
    drawWanderingPlanets();
    drawDestroyedPlanet();
    glLineWidth(1.0f);
}

inline void drawLaserEnemy2D() {
    // Dark 
    glColor3f(0.15f, 0.15f, 0.20f);
    glBegin(GL_POLYGON);
        glVertex2f(0.0f, 0.8f);
        glVertex2f(0.4f, 0.1f);
        glVertex2f(0.4f, -0.6f);
        glVertex2f(0.0f, -0.4f);
        glVertex2f(-0.4f, -0.6f);
        glVertex2f(-0.4f, 0.1f);
    glEnd();

    // Laser
    glColor3f(0.4f, 0.4f, 0.45f);
    glBegin(GL_QUADS);
        glVertex2f(0.2f, 0.1f); glVertex2f(0.3f, 0.1f); glVertex2f(0.3f, 1.2f); glVertex2f(0.2f, 1.2f);
        glVertex2f(-0.2f, 0.1f); glVertex2f(-0.3f, 0.1f); glVertex2f(-0.3f, 1.2f); glVertex2f(-0.2f, 1.2f);
    glEnd();

    // Core
    glColor3f(1.0f, 0.0f, 0.4f);
    glBegin(GL_POLYGON);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.15f, 0.1f);
        glVertex2f(0.0f, -0.2f);
        glVertex2f(-0.15f, 0.1f);
    glEnd();

    // Neon outline 
    glColor3f(1.0f, 0.0f, 0.4f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.0f, 0.8f);
        glVertex2f(0.4f, 0.1f);
        glVertex2f(0.4f, -0.6f);
        glVertex2f(0.0f, -0.4f);
        glVertex2f(-0.4f, -0.6f);
        glVertex2f(-0.4f, 0.1f);
    glEnd();
}

inline void drawVoidSeraph2D() {
    // base
    glColor3f(0.05f, 0.05f, 0.08f);
    glBegin(GL_POLYGON);
        glVertex2f(0.0f, 1.2f);
        glVertex2f(0.5f, 0.3f);
        glVertex2f(0.3f, -0.8f);
        glVertex2f(0.0f, -1.2f);
        glVertex2f(-0.3f, -0.8f);
        glVertex2f(-0.5f, 0.3f);
    glEnd();

    glColor3f(0.15f, 0.15f, 0.18f);
    glBegin(GL_POLYGON);
        glVertex2f(0.0f, 0.9f);
        glVertex2f(0.3f, 0.3f);
        glVertex2f(0.2f, -0.5f);
        glVertex2f(0.0f, -0.8f);
        glVertex2f(-0.2f, -0.5f);
        glVertex2f(-0.3f, 0.3f);
    glEnd();

    // Core
    glColor3f(0.0f, 0.8f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.15f, 0.2f);
        glVertex2f(0.0f, -0.1f);
        glVertex2f(-0.15f, 0.2f);
    glEnd();

    glColor3f(1.0f, 0.0f, 0.8f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.15f, 0.2f);
        glVertex2f(0.0f, -0.1f);
        glVertex2f(-0.15f, 0.2f);
    glEnd();

    glPushMatrix();
    glTranslatef(0.8f, 0.0f, 0.0f); 

    glColor3f(0.05f, 0.05f, 0.08f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.1f, 0.6f);
        glVertex2f(0.8f, 1.5f);
        glVertex2f(0.5f, 0.0f);
        glVertex2f(0.1f, -1.0f);
        glVertex2f(-0.2f, -0.2f);
    glEnd();

    glColor3f(1.0f, 0.0f, 0.8f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.6f, 1.2f);
        glVertex2f(0.4f, 0.1f);
        glVertex2f(0.1f, -0.8f);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.8f, 0.0f, 0.0f);

    glColor3f(0.05f, 0.05f, 0.08f);
    glBegin(GL_POLYGON);
        glVertex2f(0.1f, 0.6f);
        glVertex2f(-0.8f, 1.5f);
        glVertex2f(-0.5f, 0.0f);
        glVertex2f(-0.1f, -1.0f);
        glVertex2f(0.2f, -0.2f);
    glEnd();

    glColor3f(1.0f, 0.0f, 0.8f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(-0.6f, 1.2f);
        glVertex2f(-0.4f, 0.1f);
        glVertex2f(-0.1f, -0.8f);
    glEnd();
    glPopMatrix();

    glLineWidth(1.0f);
}

inline void drawNightmareMissileShip2D() {
    glColor3f(0.20f, 0.05f, 0.08f);
    glBegin(GL_POLYGON);
        glVertex2f(0.0f, -0.8f);
        glVertex2f(0.3f, -0.4f);
        glVertex2f(0.3f, 0.5f);
        glVertex2f(0.0f, 0.8f);
        glVertex2f(-0.3f, 0.5f);
        glVertex2f(-0.3f, -0.4f);
    glEnd();

    glColor3f(0.12f, 0.12f, 0.15f);
    glBegin(GL_QUADS);
        glVertex2f(0.3f, 0.1f);
        glVertex2f(0.7f, -0.2f);
        glVertex2f(0.7f, 0.6f);
        glVertex2f(0.3f, 0.5f);
        glVertex2f(-0.3f, 0.1f);
        glVertex2f(-0.7f, -0.2f);
        glVertex2f(-0.7f, 0.6f);
        glVertex2f(-0.3f, 0.5f);
    glEnd();

    glColor3f(1.0f, 0.15f, 0.1f);
    glBegin(GL_POLYGON);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.15f, 0.2f);
        glVertex2f(0.0f, 0.4f);
        glVertex2f(-0.15f, 0.2f);
    glEnd();

    glColor3f(1.0f, 0.55f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(0.4f, 0.15f); glVertex2f(0.6f, 0.0f); glVertex2f(0.6f, 0.45f); glVertex2f(0.4f, 0.4f);
        glVertex2f(-0.4f, 0.15f); glVertex2f(-0.6f, 0.0f); glVertex2f(-0.6f, 0.45f); glVertex2f(-0.4f, 0.4f);
    glEnd();

    glColor3f(1.0f, 0.3f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.15f, 0.8f); glVertex2f(0.15f, 0.8f); glVertex2f(0.0f, 1.1f);
    glEnd();
}

#endif
