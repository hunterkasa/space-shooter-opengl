#pragma once

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>

/* =========================================================
   CUTSCENE DATA TYPES
   ========================================================= */

struct DialogueLine {
    const char *speaker;
    const char *text;
    int portrait;   // 0 = pilot, 1 = raider/enemy, 2 = boss/warlord
};

/* =========================================================
   CUTSCENE SCRIPT EDITOR
   Add, remove, or reorder lines here to change the story.
   portrait values:
     0  ->  left side  (Pilot  / player character)
     1  ->  right side (Raider / enemy)
     2  ->  right side (Warlord / boss, larger sprite)
   ========================================================= */

DialogueLine introScene[] = {
    {"Pilot",  "This sector has gone quiet. Too quiet for a normal patrol.", 0},
    {"Raider", "Turn back, little ship. This space belongs to us now.",      1},
    {"Pilot",  "Then I will clear a path through all three sectors.",        0}
};

DialogueLine endingScene[] = {
    {"Boss",  "Impossible... my fleet was meant to be endless.", 2},
    {"Pilot", "Not endless. Just in the way.",                   0},
    {"Pilot", "The route home is open again.",                   0}
};

/* =========================================================
   CUTSCENE STATE  (extern linkage – define once, used everywhere)
   ========================================================= */
extern int  cutsceneLine;
extern int  cutsceneVisibleChars;
extern int  cutsceneTimer;

/* GameState values this header needs to know about */
#ifndef GAMESTATE_CUTSCENE_DEFINED
#define GAMESTATE_CUTSCENE_DEFINED
/* These must match the enum in your main file exactly. */
static const int CS_INTRO = 0;   // STATE_INTRO_CUTSCENE
static const int CS_END   = 5;   // STATE_END_CUTSCENE
#endif

/* =========================================================
   HELPER FORWARD DECLARATIONS (implemented in main or util)
   ========================================================= */
void drawText(float x, float y, const char *text);
void drawTextPartial(float x, float y, const char *text, int visibleChars);
int  textLength(const char *text);
void drawModernSpaceBackground(int theme);
void drawStars();
inline static void drawCustomCircle(float cx, float cy, float r, int segs)
{
    glBegin(GL_POLYGON);
    for(int i = 0; i < segs; ++i) {
        float t = (float)i * 2.0f * 3.14159f / (float)segs;
        glVertex2f(cx + r * cosf(t), cy + r * sinf(t));
    }
    glEnd();
}

inline static void drawCustomEllipse(float cx, float cy, float rx, float ry, int segs)
{
    glBegin(GL_POLYGON);
    for(int i = 0; i < segs; ++i) {
        float t = (float)i * 2.0f * 3.14159f / (float)segs;
        glVertex2f(cx + rx * cosf(t), cy + ry * sinf(t));
    }
    glEnd();
}

inline static void drawEnergyDiamond(float cx, float cy, float hw, float hh)
{
    glBegin(GL_POLYGON);
        glVertex2f(cx,        cy + hh);
        glVertex2f(cx + hw,   cy);
        glVertex2f(cx,        cy - hh);
        glVertex2f(cx - hw,   cy);
    glEnd();
}

/* =========================================================
   PORTRAIT SPRITES
   Each function draws one character centered at (0,0).
   Call them inside a glPushMatrix / glPopMatrix with the
   desired glTranslatef + glScalef already applied.
   ========================================================= */

/* ── Pilot (left side) ─────────────────────────────────── */
inline static void drawPilotPortrait(float cx, float cy)
{
    /* Helmet glow ring */
    glColor3f(0.18f, 0.55f, 0.90f);
    glLineWidth(2.8f);
    glBegin(GL_LINE_LOOP);
        for(int a = 0; a < 36; a++) {
            float t = a * 3.14159f * 2.0f / 36.0f;
            glVertex2f(cx + cosf(t)*0.42f, cy + 0.62f + sinf(t)*0.42f);
        }
    glEnd();
    glLineWidth(1.0f);

    /* Helmet fill */
    glColor3f(0.08f, 0.14f, 0.28f);
    glBegin(GL_POLYGON);
        for(int a = 0; a < 36; a++) {
            float t = a * 3.14159f * 2.0f / 36.0f;
            glVertex2f(cx + cosf(t)*0.38f, cy + 0.62f + sinf(t)*0.38f);
        }
    glEnd();

    /* Visor */
    glColor3f(0.20f, 0.76f, 0.96f);
    glBegin(GL_POLYGON);
        glVertex2f(cx - 0.22f, cy + 0.72f);
        glVertex2f(cx + 0.22f, cy + 0.72f);
        glVertex2f(cx + 0.18f, cy + 0.44f);
        glVertex2f(cx - 0.18f, cy + 0.44f);
    glEnd();

    /* Visor glint */
    glColor3f(0.85f, 0.98f, 1.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(cx - 0.14f, cy + 0.70f);
        glVertex2f(cx - 0.04f, cy + 0.66f);
        glVertex2f(cx - 0.14f, cy + 0.60f);
    glEnd();

    /* Helmet outline */
    glColor3f(0.28f, 0.72f, 1.00f);
    glLineWidth(1.8f);
    glBegin(GL_LINE_LOOP);
        for(int a = 0; a < 36; a++) {
            float t = a * 3.14159f * 2.0f / 36.0f;
            glVertex2f(cx + cosf(t)*0.38f, cy + 0.62f + sinf(t)*0.38f);
        }
    glEnd();
    glLineWidth(1.0f);

    /* Neck / collar */
    glColor3f(0.12f, 0.20f, 0.40f);
    glBegin(GL_QUADS);
        glVertex2f(cx - 0.14f, cy + 0.24f);
        glVertex2f(cx + 0.14f, cy + 0.24f);
        glVertex2f(cx + 0.10f, cy + 0.14f);
        glVertex2f(cx - 0.10f, cy + 0.14f);
    glEnd();

    /* Suit body */
    glColor3f(0.10f, 0.18f, 0.38f);
    glBegin(GL_POLYGON);
        glVertex2f(cx - 0.42f, cy + 0.14f);
        glVertex2f(cx + 0.42f, cy + 0.14f);
        glVertex2f(cx + 0.50f, cy - 0.22f);
        glVertex2f(cx + 0.44f, cy - 0.62f);
        glVertex2f(cx - 0.44f, cy - 0.62f);
        glVertex2f(cx - 0.50f, cy - 0.22f);
    glEnd();

    /* Chest panel */
    glColor3f(0.18f, 0.70f, 0.96f);
    glBegin(GL_QUADS);
        glVertex2f(cx - 0.20f, cy + 0.02f);
        glVertex2f(cx + 0.20f, cy + 0.02f);
        glVertex2f(cx + 0.16f, cy - 0.18f);
        glVertex2f(cx - 0.16f, cy - 0.18f);
    glEnd();

    /* Chest dot */
    glColor3f(0.85f, 0.98f, 1.0f);
    glBegin(GL_POLYGON);
        for(int a = 0; a < 12; a++) {
            float t = a * 3.14159f * 2.0f / 12.0f;
            glVertex2f(cx + cosf(t)*0.05f, cy - 0.08f + sinf(t)*0.05f);
        }
    glEnd();

    /* Left shoulder pad */
    glColor3f(0.14f, 0.26f, 0.55f);
    glBegin(GL_POLYGON);
        glVertex2f(cx - 0.42f, cy + 0.14f);
        glVertex2f(cx - 0.65f, cy + 0.08f);
        glVertex2f(cx - 0.68f, cy - 0.10f);
        glVertex2f(cx - 0.50f, cy - 0.18f);
        glVertex2f(cx - 0.42f, cy - 0.08f);
    glEnd();

    /* Right shoulder pad */
    glColor3f(0.14f, 0.26f, 0.55f);
    glBegin(GL_POLYGON);
        glVertex2f(cx + 0.42f, cy + 0.14f);
        glVertex2f(cx + 0.65f, cy + 0.08f);
        glVertex2f(cx + 0.68f, cy - 0.10f);
        glVertex2f(cx + 0.50f, cy - 0.18f);
        glVertex2f(cx + 0.42f, cy - 0.08f);
    glEnd();


    /* Suit outline */
    glColor3f(0.24f, 0.58f, 0.88f);
    glLineWidth(1.6f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(cx - 0.42f, cy + 0.14f);
        glVertex2f(cx + 0.42f, cy + 0.14f);
        glVertex2f(cx + 0.50f, cy - 0.22f);
        glVertex2f(cx + 0.44f, cy - 0.62f);
        glVertex2f(cx - 0.44f, cy - 0.62f);
        glVertex2f(cx - 0.50f, cy - 0.22f);
    glEnd();
    glLineWidth(1.0f);

    /* Name label */
    glColor3f(0.28f, 0.72f, 1.00f);
    drawText(cx - 0.22f, cy - 0.82f, "PILOT");
}

/* ── Warlord / Boss (right side) ───────────────────────── */
inline static void drawWarlordPortrait(float cx, float cy)
{
    /* Ominous aura */
    glColor3f(0.28f, 0.04f, 0.08f);
    glBegin(GL_POLYGON);
        for(int a = 0; a < 36; a++) {
            float t = a * 3.14159f * 2.0f / 36.0f;
            glVertex2f(cx + cosf(t)*0.55f, cy + 0.58f + sinf(t)*0.55f);
        }
    glEnd();

    /* Crown spikes */
    glColor3f(0.60f, 0.08f, 0.12f);
    glBegin(GL_TRIANGLES);
        glVertex2f(cx - 0.10f, cy + 0.96f);
        glVertex2f(cx + 0.10f, cy + 0.96f);
        glVertex2f(cx,          cy + 1.32f);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(cx - 0.32f, cy + 0.88f);
        glVertex2f(cx - 0.16f, cy + 0.90f);
        glVertex2f(cx - 0.26f, cy + 1.16f);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(cx + 0.16f, cy + 0.90f);
        glVertex2f(cx + 0.32f, cy + 0.88f);
        glVertex2f(cx + 0.26f, cy + 1.16f);
    glEnd();

    /* War-helm */
    glColor3f(0.16f, 0.06f, 0.10f);
    glBegin(GL_POLYGON);
        glVertex2f(cx - 0.38f, cy + 0.94f);
        glVertex2f(cx + 0.38f, cy + 0.94f);
        glVertex2f(cx + 0.44f, cy + 0.48f);
        glVertex2f(cx + 0.38f, cy + 0.22f);
        glVertex2f(cx - 0.38f, cy + 0.22f);
        glVertex2f(cx - 0.44f, cy + 0.48f);
    glEnd();

    /* Lower face */
    glColor3f(0.10f, 0.08f, 0.12f);
    glBegin(GL_POLYGON);
        glVertex2f(cx - 0.32f, cy + 0.24f);
        glVertex2f(cx + 0.32f, cy + 0.24f);
        glVertex2f(cx + 0.28f, cy - 0.04f);
        glVertex2f(cx - 0.28f, cy - 0.04f);
    glEnd();

    /* Eyes */
    glColor3f(1.00f, 0.12f, 0.08f);
    glBegin(GL_POLYGON);
        glVertex2f(cx - 0.24f, cy + 0.60f);
        glVertex2f(cx - 0.08f, cy + 0.62f);
        glVertex2f(cx - 0.06f, cy + 0.50f);
        glVertex2f(cx - 0.22f, cy + 0.48f);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2f(cx + 0.08f, cy + 0.62f);
        glVertex2f(cx + 0.24f, cy + 0.60f);
        glVertex2f(cx + 0.22f, cy + 0.48f);
        glVertex2f(cx + 0.06f, cy + 0.50f);
    glEnd();

    /* Eye glow dots */
    glColor3f(1.0f, 0.80f, 0.60f);
    glBegin(GL_POLYGON);
        for(int a = 0; a < 8; a++) {
            float t = a * 3.14159f * 2.0f / 8.0f;
            glVertex2f(cx - 0.15f + cosf(t)*0.04f, cy + 0.55f + sinf(t)*0.04f);
        }
    glEnd();
    glBegin(GL_POLYGON);
        for(int a = 0; a < 8; a++) {
            float t = a * 3.14159f * 2.0f / 8.0f;
            glVertex2f(cx + 0.15f + cosf(t)*0.04f, cy + 0.55f + sinf(t)*0.04f);
        }
    glEnd();

    /* Nose-guard center bar */
    glColor3f(0.38f, 0.06f, 0.10f);
    glBegin(GL_QUADS);
        glVertex2f(cx - 0.06f, cy + 0.90f);
        glVertex2f(cx + 0.06f, cy + 0.90f);
        glVertex2f(cx + 0.04f, cy + 0.22f);
        glVertex2f(cx - 0.04f, cy + 0.22f);
    glEnd();

    /* Helm outline */
    glColor3f(0.72f, 0.12f, 0.18f);
    glLineWidth(1.8f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(cx - 0.38f, cy + 0.94f);
        glVertex2f(cx + 0.38f, cy + 0.94f);
        glVertex2f(cx + 0.44f, cy + 0.48f);
        glVertex2f(cx + 0.38f, cy + 0.22f);
        glVertex2f(cx - 0.38f, cy + 0.22f);
        glVertex2f(cx - 0.44f, cy + 0.48f);
    glEnd();
    glLineWidth(1.0f);

    /* Collar */
    glColor3f(0.22f, 0.06f, 0.10f);
    glBegin(GL_POLYGON);
        glVertex2f(cx - 0.18f, cy + 0.22f);
        glVertex2f(cx + 0.18f, cy + 0.22f);
        glVertex2f(cx + 0.14f, cy + 0.06f);
        glVertex2f(cx - 0.14f, cy + 0.06f);
    glEnd();

    /* Armored torso */
    glColor3f(0.14f, 0.05f, 0.10f);
    glBegin(GL_POLYGON);
        glVertex2f(cx - 0.52f, cy + 0.06f);
        glVertex2f(cx + 0.52f, cy + 0.06f);
        glVertex2f(cx + 0.60f, cy - 0.28f);
        glVertex2f(cx + 0.52f, cy - 0.62f);
        glVertex2f(cx - 0.52f, cy - 0.62f);
        glVertex2f(cx - 0.60f, cy - 0.28f);
    glEnd();

    /* Chest energy core diamond */
    glColor3f(0.80f, 0.10f, 0.18f);
    glBegin(GL_POLYGON);
        glVertex2f(cx,          cy - 0.04f);
        glVertex2f(cx + 0.18f, cy - 0.18f);
        glVertex2f(cx,          cy - 0.32f);
        glVertex2f(cx - 0.18f, cy - 0.18f);
    glEnd();
    glColor3f(1.0f, 0.60f, 0.50f);
    glBegin(GL_POLYGON);
        glVertex2f(cx,          cy - 0.10f);
        glVertex2f(cx + 0.08f, cy - 0.18f);
        glVertex2f(cx,          cy - 0.26f);
        glVertex2f(cx - 0.08f, cy - 0.18f);
    glEnd();

    /* Left pauldron + spike */
    glColor3f(0.32f, 0.06f, 0.10f);
    glBegin(GL_POLYGON);
        glVertex2f(cx - 0.52f, cy + 0.06f);
        glVertex2f(cx - 0.76f, cy + 0.04f);
        glVertex2f(cx - 0.80f, cy - 0.14f);
        glVertex2f(cx - 0.60f, cy - 0.26f);
        glVertex2f(cx - 0.52f, cy - 0.14f);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(cx - 0.76f, cy + 0.04f);
        glVertex2f(cx - 0.82f, cy + 0.08f);
        glVertex2f(cx - 0.90f, cy - 0.04f);
    glEnd();

    /* Right pauldron + spike */
    glColor3f(0.32f, 0.06f, 0.10f);
    glBegin(GL_POLYGON);
        glVertex2f(cx + 0.52f, cy + 0.06f);
        glVertex2f(cx + 0.76f, cy + 0.04f);
        glVertex2f(cx + 0.80f, cy - 0.14f);
        glVertex2f(cx + 0.60f, cy - 0.26f);
        glVertex2f(cx + 0.52f, cy - 0.14f);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(cx + 0.76f, cy + 0.04f);
        glVertex2f(cx + 0.82f, cy + 0.08f);
        glVertex2f(cx + 0.90f, cy - 0.04f);
    glEnd();


    /* Torso outline */
    glColor3f(0.62f, 0.08f, 0.14f);
    glLineWidth(1.6f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(cx - 0.52f, cy + 0.06f);
        glVertex2f(cx + 0.52f, cy + 0.06f);
        glVertex2f(cx + 0.60f, cy - 0.28f);
        glVertex2f(cx + 0.52f, cy - 0.62f);
        glVertex2f(cx - 0.52f, cy - 0.62f);
        glVertex2f(cx - 0.60f, cy - 0.28f);
    glEnd();
    glLineWidth(1.0f);

    /* Name label */
    glColor3f(0.90f, 0.28f, 0.32f);
    drawText(cx - 0.42f, cy - 0.82f, "WARLORD");
}

/* =========================================================
   INTERNAL HELPERS
   ========================================================= */
static DialogueLine *activeCutsceneLines(int gs)
{
    if(gs == CS_END)
        return endingScene;
    return introScene;
}

static int activeCutsceneCount(int gs)
{
    if(gs == CS_END)
        return (int)(sizeof(endingScene) / sizeof(endingScene[0]));
    return (int)(sizeof(introScene) / sizeof(introScene[0]));
}

/* =========================================================
   PUBLIC API
   ========================================================= */

/* Call once when entering a cutscene state. */
inline void cutsceneInit(int &line, int &visChars, int &timer)
{
    line     = 0;
    visChars = 0;
    timer    = 0;
}

/* Call every frame from updateGame(). */
inline void cutsceneUpdate(int gs,
                           int &line, int &visChars, int &timer)
{
    (void)timer;
    DialogueLine dl = activeCutsceneLines(gs)[line];
    visChars = textLength(dl.text);
}

/*
   Call on Space / Enter press.
   Returns true when the cutscene is fully finished
   (caller should then change gameState).
*/
inline bool cutsceneAdvance(int gs,
                            int &line, int &visChars, int &timer)
{
    line++;
    visChars = 0;
    timer    = 0;

    if(line >= activeCutsceneCount(gs))
        return true;   // cutscene done

    return false;
}

/* =========================================================
   DRAW  –  call from onDisplay() when in a cutscene state.
   'isEnding'  true  -> ending cutscene (blue theme)
               false -> intro cutscene  (default theme)
   ========================================================= */
inline void drawCutscene(int gs,
                         int line, int visChars)
{
    bool isEnding = (gs == CS_END);

    glClearColor(0.010f, 0.016f, 0.050f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawModernSpaceBackground(isEnding ? 2 : 0);
    drawStars();

    DialogueLine dl = activeCutsceneLines(gs)[line];

    /* ── Cinematic dark floor ── */
    glColor3f(0.012f, 0.025f, 0.055f);
    glBegin(GL_QUADS);
        glVertex2f(-5.0f, -3.5f);
        glVertex2f( 5.0f, -3.5f);
        glVertex2f( 5.0f, -0.90f);
        glVertex2f(-5.0f, -0.90f);
    glEnd();

    /* Portraits: always full color, no tint/dim, drawn at 2x scale. */
    glPushMatrix();
        glTranslatef(-3.50f, -0.30f, 0.0f);
        glScalef(1.75f, 1.75f, 1.0f);
        drawPilotPortrait(0.0f, 0.0f);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(3.50f, -0.30f, 0.0f);
        glScalef(1.75f, 1.75f, 1.0f);
        drawWarlordPortrait(0.0f, 0.0f);
    glPopMatrix();

    /* ── Dialogue box ── */
    glColor3f(0.025f, 0.048f, 0.095f);
    glBegin(GL_QUADS);
        glVertex2f(-4.80f, -3.25f);
        glVertex2f( 4.80f, -3.25f);
        glVertex2f( 4.80f, -1.00f);
        glVertex2f(-4.80f, -1.00f);
    glEnd();

    /* Border colour: red for enemies, blue for player */
    if(dl.portrait == 1 || dl.portrait == 2)
        glColor3f(0.82f, 0.14f, 0.20f);
    else
        glColor3f(0.20f, 0.82f, 1.0f);

    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-4.80f, -3.25f);
        glVertex2f( 4.80f, -3.25f);
        glVertex2f( 4.80f, -1.00f);
        glVertex2f(-4.80f, -1.00f);
    glEnd();
    glLineWidth(1.0f);

    /* Speaker tag */
    glColor3f(0.92f, 0.98f, 1.0f);
    drawText(-4.30f, -1.20f, dl.speaker);

    /* Dialogue text (static, full line) */
    glColor3f(0.78f, 0.90f, 1.0f);
    drawText(-4.30f, -1.85f, dl.text);

    /* Continue prompt */
    glColor3f(0.48f, 0.78f, 1.0f);
    drawText(2.60f, -3.05f, "[ Space: Next ]");

    glutSwapBuffers();
}
