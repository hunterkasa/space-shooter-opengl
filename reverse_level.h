#ifndef REVERSE_LEVEL_H
#define REVERSE_LEVEL_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>

inline void drawReverseEllipse(float cx, float cy, float rx, float ry, int segments) {
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for(int i = 0; i <= segments; i++) {
            float theta = 2.0f * 3.14159f * (float)i / (float)segments;
            glVertex2f(cx + cosf(theta) * rx, cy + sinf(theta) * ry);
        }
    glEnd();
}

// Reverse Boss art is meant to read as the former hero now turned sinister: the previous villain as the main character.
inline void drawReverseBoss2D() {
    float hullDark[3] = {0.08f, 0.08f, 0.12f};
    float hullLight[3] = {0.24f, 0.18f, 0.30f};
    float glowRed[3] = {1.00f, 0.02f, 0.28f};
    float glowPink[3] = {0.94f, 0.40f, 0.84f};
    float coldWhite[3] = {0.92f, 0.96f, 1.00f};
    float sharpTeal[3] = {0.20f, 0.92f, 0.92f};

    float coreHull[][2] = {
        {0.00f, 1.20f}, {1.10f, 0.75f}, {1.55f, -0.10f}, {1.25f, -0.90f},
        {0.52f, -1.40f}, {-0.52f, -1.40f}, {-1.25f, -0.90f}, {-1.55f, -0.10f},
        {-1.10f, 0.75f}
    };
    glColor3f(hullDark[0], hullDark[1], hullDark[2]);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 9; i++) glVertex2f(coreHull[i][0], coreHull[i][1]);
    glEnd();

    float innerPlate[][2] = {
        {0.00f, 0.90f}, {0.75f, 0.55f}, {0.95f, -0.05f}, {0.80f, -0.70f},
        {0.25f, -1.05f}, {-0.25f, -1.05f}, {-0.80f, -0.70f}, {-0.95f, -0.05f}, {-0.75f, 0.55f}
    };
    glColor3f(hullLight[0], hullLight[1], hullLight[2]);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 9; i++) glVertex2f(innerPlate[i][0], innerPlate[i][1]);
    glEnd();

    float rimA[][2] = {{-0.70f, 0.80f}, {0.70f, 0.80f}, {0.92f, 0.18f}, {0.52f, -0.40f}, {-0.52f, -0.40f}, {-0.92f, 0.18f}};
    glColor3f(glowPink[0], glowPink[1], glowPink[2]);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 6; i++) glVertex2f(rimA[i][0], rimA[i][1]);
    glEnd();

    glColor3f(glowRed[0], glowRed[1], glowRed[2]);
    glBegin(GL_QUADS);
        glVertex2f(-0.20f, 0.82f); glVertex2f(0.20f, 0.82f);
        glVertex2f(0.26f, 0.36f); glVertex2f(-0.26f, 0.36f);
    glEnd();

    glColor3f(coldWhite[0], coldWhite[1], coldWhite[2]);
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.12f, 0.82f); glVertex2f(0.12f, 0.82f); glVertex2f(0.00f, 1.08f);
    glEnd();

    glColor3f(sharpTeal[0], sharpTeal[1], sharpTeal[2]);
    glBegin(GL_QUADS);
        glVertex2f(-0.58f, -0.35f); glVertex2f(-0.48f, -0.35f);
        glVertex2f(-0.48f, -0.85f); glVertex2f(-0.58f, -0.85f);
        glVertex2f(0.48f, -0.35f); glVertex2f(0.58f, -0.35f);
        glVertex2f(0.58f, -0.85f); glVertex2f(0.48f, -0.85f);
    glEnd();

    glColor3f(0.16f, 0.16f, 0.20f);
    glBegin(GL_LINE_LOOP);
        for(int i = 0; i < 9; i++) glVertex2f(coreHull[i][0], coreHull[i][1]);
    glEnd();
}

inline void drawReverseBossArenaBackground() {
    float time = (float)glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    float pulsate = 0.4f + 0.2f * sinf(time * 2.1f);

    glColor3f(0.006f, 0.008f, 0.028f);
    glBegin(GL_QUADS);
        glVertex2f(-5.0f, 3.5f);
        glVertex2f( 5.0f, 3.5f);
        glVertex2f( 5.0f, -3.5f);
        glVertex2f(-5.0f, -3.5f);
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.64f, 0.18f, 0.74f, 0.12f + pulsate * 0.04f);
    drawReverseEllipse(0.0f, 1.45f, 3.20f, 0.72f, 64);
    drawReverseEllipse(0.0f, 0.22f, 4.10f, 0.90f, 64);
    drawReverseEllipse(0.0f, -1.20f, 3.65f, 0.60f, 64);

    glColor4f(0.28f, 0.12f, 0.40f, 0.28f);
    drawReverseEllipse(-3.0f, 2.15f, 0.58f, 0.34f, 36);
    drawReverseEllipse( 3.2f, 1.70f, 0.46f, 0.28f, 36);
    drawReverseEllipse( 2.00f,-2.10f, 0.84f, 0.50f, 42);

    glColor4f(0.70f, 0.28f, 0.96f, 0.18f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(-4.8f, -0.90f);
        glVertex2f(-2.3f, -1.22f + sinf(time * 0.9f) * 0.10f);
        glVertex2f( 0.0f, -1.05f);
        glVertex2f( 2.4f, -1.22f - sinf(time * 0.8f) * 0.10f);
        glVertex2f( 4.8f, -0.88f);
    glEnd();

    glColor4f(0.95f, 0.44f, 0.92f, 0.36f);
    drawReverseEllipse(-1.6f, 0.80f, 0.48f, 0.18f, 28);
    drawReverseEllipse(1.4f, 0.45f, 0.36f, 0.16f, 28);
    drawReverseEllipse(0.0f, -0.45f, 0.66f, 0.22f, 32);

    glColor4f(1.0f, 0.56f, 0.92f, 0.26f);
    drawReverseEllipse(-2.45f, 2.50f, 0.18f, 0.12f, 22);
    drawReverseEllipse( 2.55f, 2.25f, 0.16f, 0.10f, 22);
    drawReverseEllipse( 0.95f,-2.55f, 0.12f, 0.08f, 20);

    glDisable(GL_BLEND);
}

#endif // REVERSE_LEVEL_H
