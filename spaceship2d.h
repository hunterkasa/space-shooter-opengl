#ifndef SPACESHIP2D_H
#define SPACESHIP2D_H
#include <initializer_list>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* =========================================================
   Helper: Draw filled polygon
   ========================================================= */
static void _poly(float r, float g, float b, int n, float v[][2])
{
    glColor3f(r,g,b);
    glBegin(GL_POLYGON);
    for(int i=0;i<n;i++)
        glVertex2f(v[i][0], v[i][1]);
    glEnd();
}

/* =========================================================
   Helper: Draw outline
   ========================================================= */
static void _outline(float r,float g,float b,int n,float v[][2])
{
    glColor3f(r,g,b);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<n;i++)
        glVertex2f(v[i][0], v[i][1]);
    glEnd();
}


/* =========================================================
   PLAYER SHIP — unchanged
   ========================================================= */
void drawSpaceship2D()
{
    float hullMain[3]  = {0.75f, 0.78f, 0.82f};
    float hullSec[3]   = {0.88f, 0.92f, 0.95f};
    float neon[3]      = {0.00f, 0.85f, 1.00f};
    float glass[3]     = {0.40f, 0.80f, 1.00f};
    float engineCore[3]= {1.00f, 1.00f, 1.00f};
    float engineGlow[3]= {0.10f, 0.60f, 1.00f};

    float body[][2] = {
        {-0.10f, -1.2f}, {0.10f, -1.2f},
        {0.25f, -0.4f},  {0.15f, 1.0f},
        {0.00f,  1.9f},
        {-0.15f, 1.0f},  {-0.25f, -0.4f}
    };
    _poly(hullMain[0], hullMain[1], hullMain[2], 7, body);

    float intake[][2] = {
        {-0.08f, 0.0f}, {0.08f, 0.0f},
        {0.12f, 0.8f}, {-0.12f, 0.8f}
    };
    _poly(hullSec[0], hullSec[1], hullSec[2], 4, intake);

    float cockpit[][2] = {
        {-0.08f, 0.4f}, {0.08f, 0.4f},
        {0.06f, 1.1f}, {0.00f, 1.4f}, {-0.06f, 1.1f}
    };
    _poly(glass[0], glass[1], glass[2], 5, cockpit);

    float wr[][2] = {
        {0.18f, 0.6f}, {1.2f, 0.25f}, {1.5f, 0.1f},
        {1.8f, -0.3f}, {0.25f, -0.4f}
    };
    _poly(hullMain[0], hullMain[1], hullMain[2], 5, wr);

    float wl[][2] = {
        {-0.18f, 0.6f}, {-1.2f, 0.25f}, {-1.5f, 0.1f},
        {-1.8f, -0.3f}, {-0.25f, -0.4f}
    };
    _poly(hullMain[0], hullMain[1], hullMain[2], 5, wl);

    float waR[][2] = {{0.5f, 0.4f}, {1.5f, 0.1f}, {1.5f, 0.2f}, {0.5f, 0.55f}};
    _poly(neon[0], neon[1], neon[2], 4, waR);
    float waL[][2] = {{-0.5f, 0.4f}, {-1.5f, 0.1f}, {-1.5f, 0.2f}, {-0.5f, 0.55f}};
    _poly(neon[0], neon[1], neon[2], 4, waL);

    float engines[][2] = {
        {-0.35f, -1.3f}, {-0.10f, -1.3f}, {-0.05f, -1.0f}, {-0.40f, -1.0f},
        {0.10f, -1.3f},  {0.35f, -1.3f},  {0.40f, -1.0f},  {0.05f, -1.0f}
    };
    _poly(hullSec[0], hullSec[1], hullSec[2], 4, &engines[0]);
    _poly(hullSec[0], hullSec[1], hullSec[2], 4, &engines[4]);

    for(float i=0; i<2; i++) {
        float offset = i * 0.45f - 0.22f;
        glColor3f(engineGlow[0], engineGlow[1], engineGlow[2]);
        glBegin(GL_QUADS);
            glVertex2f(offset-0.12f,-1.3f); glVertex2f(offset+0.12f,-1.3f);
            glVertex2f(offset+0.08f,-1.6f); glVertex2f(offset-0.08f,-1.6f);
        glEnd();
        glColor3f(engineCore[0], engineCore[1], engineCore[2]);
        glBegin(GL_QUADS);
            glVertex2f(offset-0.04f,-1.3f); glVertex2f(offset+0.04f,-1.3f);
            glVertex2f(offset+0.02f,-1.5f); glVertex2f(offset-0.02f,-1.5f);
        glEnd();
    }

    float gunR[][2] = {{1.2f,-0.2f},{1.35f,-0.2f},{1.35f,0.6f},{1.2f,0.6f}};
    _poly(0.3f, 0.3f, 0.3f, 4, gunR);
    float gunL[][2] = {{-1.2f,-0.2f},{-1.35f,-0.2f},{-1.35f,0.6f},{-1.2f,0.6f}};
    _poly(0.3f, 0.3f, 0.3f, 4, gunL);

    _outline(0.1f, 0.1f, 0.1f, 7, body);
    _outline(0.1f, 0.1f, 0.1f, 5, wr);
    _outline(0.1f, 0.1f, 0.1f, 5, wl);
    _outline(neon[0], neon[1], neon[2], 4, intake);
}


/* =========================================================
   ASTEROID — Jagged debris with ore veins
   ========================================================= */
void drawAsteroid2D() {
    float rock_main[3]  = {0.25f, 0.25f, 0.27f};
    float rock_light[3] = {0.45f, 0.45f, 0.48f};
    float crystal[3]    = {0.2f,  1.0f,  0.8f};
    float lava[3]       = {1.0f,  0.3f,  0.1f};

    float rock[][2] = {
        { 0.00f,  1.00f}, { 0.65f,  0.75f}, { 0.85f,  0.35f}, { 0.70f, -0.20f},
        { 0.40f, -0.85f}, { 0.00f, -1.00f}, {-0.60f, -0.80f}, {-0.80f, -0.20f},
        {-0.75f,  0.45f}, {-0.50f,  0.90f}
    };
    _poly(rock_main[0], rock_main[1], rock_main[2], 10, rock);

    float f1[][2] = {{ 0.40f, 0.50f}, { 0.65f, 0.75f}, { 0.50f, 0.30f}};
    _poly(rock_light[0], rock_light[1], rock_light[2], 3, f1);
    float f2[][2] = {{-0.40f,-0.50f}, {-0.60f,-0.80f}, {-0.30f,-0.60f}};
    _poly(rock_light[0], rock_light[1], rock_light[2], 3, f2);

    float cr[][2] = {{0.10f, 0.20f},{0.40f, 0.10f},{0.35f,-0.30f},{0.00f,-0.25f}};
    _poly(crystal[0], crystal[1], crystal[2], 4, cr);
    _outline(crystal[0], crystal[1], crystal[2], 4, cr);

    glColor3f(lava[0], lava[1], lava[2]);
    glBegin(GL_LINES);
        glVertex2f(-0.30f, 0.40f); glVertex2f( 0.20f,-0.10f);
        glVertex2f( 0.00f, 0.60f); glVertex2f(-0.50f, 0.00f);
        glVertex2f( 0.30f,-0.50f); glVertex2f(-0.20f,-0.70f);
    glEnd();

    _outline(0.1f, 0.1f, 0.1f, 10, rock);
}


/* =========================================================
   ENEMY INTERCEPTOR — "Scimitar-class"

   Crescent swept-back wings, needle spine, split tail.
   Palette: deep gunmetal + violet glow.
   Reads fast, predatory, fights in packs.
   ========================================================= */
void drawEnemyShip2D() {

    float bodyCol[3]  = {0.28f, 0.28f, 0.34f}; // Deep gunmetal
    float panelCol[3] = {0.40f, 0.40f, 0.50f}; // Raised panel
    float cockCol[3]  = {0.50f, 0.08f, 0.78f}; // Violet cockpit
    float glowCol[3]  = {0.68f, 0.18f, 1.00f}; // Violet engine glow
    float coreCol[3]  = {1.00f, 0.92f, 1.00f}; // White-hot core
    float trimCol[3]  = {0.80f, 0.45f, 1.00f}; // Soft purple trim

    /* Needle-nosed fuselage spine */
    float spine[][2] = {
        { 0.00f,  1.10f},  // Nose tip
        { 0.12f,  0.48f},
        { 0.15f, -0.55f},
        { 0.09f, -0.98f},
        {-0.09f, -0.98f},
        {-0.15f, -0.55f},
        {-0.12f,  0.48f}
    };
    _poly(bodyCol[0], bodyCol[1], bodyCol[2], 7, spine);

    /* Crescent right wing */
    float wingR[][2] = {
        { 0.12f,  0.48f},
        { 0.75f,  0.68f},  // Wingtip forward crest
        { 1.28f,  0.38f},  // Outer tip
        { 1.18f, -0.08f},
        { 0.88f, -0.52f},  // Trailing edge
        { 0.15f, -0.38f}
    };
    _poly(panelCol[0], panelCol[1], panelCol[2], 6, wingR);

    /* Crescent left wing */
    float wingL[][2] = {
        {-0.12f,  0.48f},
        {-0.75f,  0.68f},
        {-1.28f,  0.38f},
        {-1.18f, -0.08f},
        {-0.88f, -0.52f},
        {-0.15f, -0.38f}
    };
    _poly(panelCol[0], panelCol[1], panelCol[2], 6, wingL);

    /* Wing highlight stripe — thin trim line for depth */
    float strR[][2] = {{0.22f,0.44f},{0.88f,0.58f},{0.88f,0.66f},{0.22f,0.54f}};
    _poly(trimCol[0], trimCol[1], trimCol[2], 4, strR);
    float strL[][2] = {{-0.22f,0.44f},{-0.88f,0.58f},{-0.88f,0.66f},{-0.22f,0.54f}};
    _poly(trimCol[0], trimCol[1], trimCol[2], 4, strL);

    /* Violet cockpit gem */
    float cock[][2] = {
        {-0.08f,0.18f},{ 0.08f,0.18f},
        { 0.09f,0.68f},{ 0.00f,0.95f},{-0.09f,0.68f}
    };
    _poly(cockCol[0], cockCol[1], cockCol[2], 5, cock);
    _outline(trimCol[0], trimCol[1], trimCol[2], 5, cock);

    /* Split-tail fins */
    float finR[][2] = {{ 0.09f,-0.88f},{ 0.34f,-1.22f},{ 0.34f,-1.32f},{ 0.09f,-1.02f}};
    float finL[][2] = {{-0.09f,-0.88f},{-0.34f,-1.22f},{-0.34f,-1.32f},{-0.09f,-1.02f}};
    _poly(bodyCol[0], bodyCol[1], bodyCol[2], 4, finR);
    _poly(bodyCol[0], bodyCol[1], bodyCol[2], 4, finL);
    _outline(trimCol[0], trimCol[1], trimCol[2], 4, finR);
    _outline(trimCol[0], trimCol[1], trimCol[2], 4, finL);

    /* Twin violet engine exhausts */
    for(float sx : {-0.10f, 0.10f}) {
        glColor3f(glowCol[0], glowCol[1], glowCol[2]);
        glBegin(GL_TRIANGLES);
            glVertex2f(sx-0.06f,-0.98f);
            glVertex2f(sx+0.06f,-0.98f);
            glVertex2f(sx,      -1.42f);
        glEnd();
        glColor3f(coreCol[0], coreCol[1], coreCol[2]);
        glBegin(GL_TRIANGLES);
            glVertex2f(sx-0.025f,-0.98f);
            glVertex2f(sx+0.025f,-0.98f);
            glVertex2f(sx,       -1.22f);
        glEnd();
    }

    _outline(0.10f, 0.08f, 0.14f, 7, spine);
    _outline(0.14f, 0.08f, 0.22f, 6, wingR);
    _outline(0.14f, 0.08f, 0.22f, 6, wingL);
}


/* =========================================================
   ENEMY BOMBER — "Wraith-class assault platform"

   Wide armoured delta, ventral bomb bay, quad amber drives,
   red warning chevrons, thick dorsal ridge.
   Reads slow, relentless, terrifying firepower.
   ========================================================= */
void drawEnemyBomber2D() {

    float armorA[3]   = {0.27f, 0.29f, 0.26f}; // Dark olive-grey
    float armorB[3]   = {0.38f, 0.40f, 0.36f}; // Lighter panel
    float armorC[3]   = {0.18f, 0.20f, 0.18f}; // Deep shadow recess
    float warnRed[3]  = {0.88f, 0.16f, 0.16f}; // Danger red
    float amber[3]    = {1.00f, 0.55f, 0.12f}; // Drive glow
    float amberC[3]   = {1.00f, 0.94f, 0.68f}; // Drive core

    /* Broad 8-point delta main hull */
    float hull[][2] = {
        {-0.10f,  0.92f},
        { 0.10f,  0.92f},
        { 1.02f,  0.22f},
        { 1.12f, -0.68f},
        { 0.56f, -1.12f},
        {-0.56f, -1.12f},
        {-1.12f, -0.68f},
        {-1.02f,  0.22f}
    };
    _poly(armorA[0], armorA[1], armorA[2], 8, hull);

    /* Raised dorsal ridge / spine plate */
    float ridge[][2] = {
        {-0.22f,  0.88f},{ 0.22f,  0.88f},
        { 0.32f,  0.12f},{ 0.32f, -0.52f},
        {-0.32f, -0.52f},{-0.32f,  0.12f}
    };
    _poly(armorB[0], armorB[1], armorB[2], 6, ridge);

    /* Armored command bridge (dark recess box) */
    float bridge[][2] = {
        {-0.16f,-0.04f},{ 0.16f,-0.04f},
        { 0.14f, 0.58f},{-0.14f, 0.58f}
    };
    _poly(armorC[0], armorC[1], armorC[2], 4, bridge);

    /* Viewport slit — dim red glow */
    float vport[][2] = {{-0.10f,0.26f},{0.10f,0.26f},{0.10f,0.44f},{-0.10f,0.44f}};
    _poly(0.62f, 0.12f, 0.08f, 4, vport);

    /* Wing panel segment lines */
    glColor3f(armorC[0], armorC[1], armorC[2]);
    glBegin(GL_LINES);
        glVertex2f( 0.38f, 0.58f); glVertex2f( 0.96f, 0.12f);
        glVertex2f( 0.58f, 0.22f); glVertex2f( 1.02f,-0.20f);
        glVertex2f(-0.38f, 0.58f); glVertex2f(-0.96f, 0.12f);
        glVertex2f(-0.58f, 0.22f); glVertex2f(-1.02f,-0.20f);
    glEnd();

    /* Red warning chevrons on wing roots */
    float chevR[][2] = {{ 0.34f,-0.08f},{ 0.58f,-0.08f},{ 0.48f, 0.18f},{ 0.24f, 0.18f}};
    float chevL[][2] = {{-0.34f,-0.08f},{-0.58f,-0.08f},{-0.48f, 0.18f},{-0.24f, 0.18f}};
    _poly(warnRed[0], warnRed[1], warnRed[2], 4, chevR);
    _poly(warnRed[0], warnRed[1], warnRed[2], 4, chevL);

    /* Ventral bomb bay — dark recess */
    float bay[][2] = {
        {-0.36f,-0.56f},{ 0.36f,-0.56f},
        { 0.30f,-1.08f},{-0.30f,-1.08f}
    };
    _poly(armorC[0], armorC[1], armorC[2], 4, bay);

    /* Bomb bay door lines */
    glColor3f(armorA[0], armorA[1], armorA[2]);
    glBegin(GL_LINES);
        glVertex2f( 0.00f,-0.56f); glVertex2f( 0.00f,-1.08f);
        glVertex2f(-0.16f,-0.72f); glVertex2f( 0.16f,-0.72f);
        glVertex2f(-0.16f,-0.90f); glVertex2f( 0.16f,-0.90f);
    glEnd();

    /* Quad engine bank — 4 exhausts */
    float ePos[4] = {-0.66f, -0.30f, 0.30f, 0.66f};
    for(int i = 0; i < 4; i++) {
        float x = ePos[i];
        // Housing
        glColor3f(armorC[0], armorC[1], armorC[2]);
        glBegin(GL_QUADS);
            glVertex2f(x-0.09f,-1.12f); glVertex2f(x+0.09f,-1.12f);
            glVertex2f(x+0.09f,-1.24f); glVertex2f(x-0.09f,-1.24f);
        glEnd();
        // Amber glow plume
        glColor3f(amber[0], amber[1], amber[2]);
        glBegin(GL_QUADS);
            glVertex2f(x-0.07f,-1.24f); glVertex2f(x+0.07f,-1.24f);
            glVertex2f(x+0.04f,-1.58f); glVertex2f(x-0.04f,-1.58f);
        glEnd();
        // Hot core
        glColor3f(amberC[0], amberC[1], amberC[2]);
        glBegin(GL_QUADS);
            glVertex2f(x-0.03f,-1.24f); glVertex2f(x+0.03f,-1.24f);
            glVertex2f(x+0.02f,-1.42f); glVertex2f(x-0.02f,-1.42f);
        glEnd();
    }

    _outline(0.10f, 0.12f, 0.10f, 8, hull);
    _outline(0.10f, 0.12f, 0.10f, 6, ridge);
}


/* =========================================================
   BOSS — "Nemesis-class Dreadnought"

   Skull-faced command ship. Five visual tiers:
   outer wingspans → inner hull → skull face → weapon systems → reactor.
   Palette: void-black + deep crimson eyes + hellfire reactor + purple drives.
   Pure dread.
   ========================================================= */
void drawBoss2D() {

    float voidBlk[3]  = {0.12f, 0.10f, 0.12f}; // Near-black hull
    float darkPlt[3]  = {0.22f, 0.18f, 0.22f}; // Secondary plating
    float midPlt[3]   = {0.34f, 0.28f, 0.34f}; // Raised ridges
    float crimson[3]  = {0.85f, 0.04f, 0.04f}; // Pure menace red
    float hellfire[3] = {1.00f, 0.26f, 0.00f}; // Reactor / cannon glow
    float scorch[3]   = {1.00f, 0.86f, 0.50f}; // Core white-orange
    float sickGrn[3]  = {0.10f, 0.90f, 0.38f}; // Shield emitter green
    float voidPurp[3] = {0.54f, 0.00f, 0.90f}; // Drive exhaust purple

    /* ====== TIER 1: Massive outer wing spans ====== */
    float owR[][2] = {
        { 0.60f,  0.80f},
        { 1.82f,  1.12f},
        { 2.22f,  0.60f},
        { 2.12f, -0.28f},
        { 1.62f, -0.88f},
        { 0.80f, -0.70f}
    };
    _poly(voidBlk[0], voidBlk[1], voidBlk[2], 6, owR);

    float owL[][2] = {
        {-0.60f,  0.80f},
        {-1.82f,  1.12f},
        {-2.22f,  0.60f},
        {-2.12f, -0.28f},
        {-1.62f, -0.88f},
        {-0.80f, -0.70f}
    };
    _poly(voidBlk[0], voidBlk[1], voidBlk[2], 6, owL);

    /* Wing armor ridge overlay */
    float wrR[][2] = {{0.80f,0.76f},{1.60f,0.96f},{1.82f,0.56f},{1.58f,-0.38f},{0.92f,-0.50f},{0.76f,0.24f}};
    _poly(darkPlt[0], darkPlt[1], darkPlt[2], 6, wrR);
    float wrL[][2] = {{-0.80f,0.76f},{-1.60f,0.96f},{-1.82f,0.56f},{-1.58f,-0.38f},{-0.92f,-0.50f},{-0.76f,0.24f}};
    _poly(darkPlt[0], darkPlt[1], darkPlt[2], 6, wrL);

    /* Crimson wing etch lines */
    glColor3f(crimson[0], crimson[1], crimson[2]);
    glBegin(GL_LINES);
        glVertex2f( 1.00f, 0.62f); glVertex2f( 2.02f, 0.72f);
        glVertex2f( 1.22f, 0.12f); glVertex2f( 2.02f, 0.22f);
        glVertex2f(-1.00f, 0.62f); glVertex2f(-2.02f, 0.72f);
        glVertex2f(-1.22f, 0.12f); glVertex2f(-2.02f, 0.22f);
    glEnd();

    /* Wingtip heavy cannon barrels */
    float ctR[][2] = {{ 1.86f,0.92f},{ 2.12f,0.92f},{ 2.16f,1.62f},{ 1.82f,1.62f}};
    float ctL[][2] = {{-1.86f,0.92f},{-2.12f,0.92f},{-2.16f,1.62f},{-1.82f,1.62f}};
    _poly(darkPlt[0], darkPlt[1], darkPlt[2], 4, ctR);
    _poly(darkPlt[0], darkPlt[1], darkPlt[2], 4, ctL);
    /* Muzzle glow */
    glColor3f(crimson[0], crimson[1], crimson[2]);
    glBegin(GL_QUADS);
        glVertex2f( 1.84f,1.56f); glVertex2f( 2.12f,1.56f);
        glVertex2f( 2.14f,1.66f); glVertex2f( 1.82f,1.66f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2f(-1.84f,1.56f); glVertex2f(-2.12f,1.56f);
        glVertex2f(-2.14f,1.66f); glVertex2f(-1.82f,1.66f);
    glEnd();

    /* ====== TIER 2: Inner core hull body ====== */
    float core[][2] = {
        { 0.00f,  2.02f},
        { 0.55f,  1.52f},
        { 0.70f,  0.80f},
        { 0.75f, -0.58f},
        { 0.50f, -1.42f},
        { 0.00f, -1.72f},
        {-0.50f, -1.42f},
        {-0.75f, -0.58f},
        {-0.70f,  0.80f},
        {-0.55f,  1.52f}
    };
    _poly(darkPlt[0], darkPlt[1], darkPlt[2], 10, core);

    /* Raised center armor spine */
    float spine[][2] = {
        {-0.30f, 1.82f},{ 0.30f, 1.82f},
        { 0.38f, 0.62f},{ 0.38f,-0.80f},
        {-0.38f,-0.80f},{-0.38f, 0.62f}
    };
    _poly(midPlt[0], midPlt[1], midPlt[2], 6, spine);

    /* ====== TIER 3: THE SKULL FACE ====== */

    /* Brow ridge — heavy protruding ledge */
    float brow[][2] = {{-0.56f,0.68f},{0.56f,0.68f},{0.50f,0.52f},{-0.50f,0.52f}};
    _poly(voidBlk[0], voidBlk[1], voidBlk[2], 4, brow);
    _outline(crimson[0], crimson[1], crimson[2], 4, brow);

    /* Eye sockets — dark void recesses */
    float esR[][2] = {{ 0.11f,0.20f},{ 0.44f,0.20f},{ 0.46f,0.50f},{ 0.09f,0.50f}};
    float esL[][2] = {{-0.11f,0.20f},{-0.44f,0.20f},{-0.46f,0.50f},{-0.09f,0.50f}};
    _poly(voidBlk[0], voidBlk[1], voidBlk[2], 4, esR);
    _poly(voidBlk[0], voidBlk[1], voidBlk[2], 4, esL);

    /* Eye weapon glow — weapon lock-on ports */
    float eyR[][2] = {{ 0.14f,0.23f},{ 0.42f,0.23f},{ 0.44f,0.47f},{ 0.12f,0.47f}};
    float eyL[][2] = {{-0.14f,0.23f},{-0.42f,0.23f},{-0.44f,0.47f},{-0.12f,0.47f}};
    _poly(crimson[0], crimson[1], crimson[2], 4, eyR);
    _poly(crimson[0], crimson[1], crimson[2], 4, eyL);

    /* Eye bright cores */
    float ecR[][2] = {{ 0.20f,0.29f},{ 0.36f,0.29f},{ 0.36f,0.41f},{ 0.20f,0.41f}};
    float ecL[][2] = {{-0.20f,0.29f},{-0.36f,0.29f},{-0.36f,0.41f},{-0.20f,0.41f}};
    _poly(1.00f, 0.82f, 0.82f, 4, ecR);
    _poly(1.00f, 0.82f, 0.82f, 4, ecL);

    /* Nasal intake — angular snout */
    float snout[][2] = {{-0.18f,0.00f},{0.18f,0.00f},{0.14f,0.19f},{-0.14f,0.19f}};
    _poly(voidBlk[0], voidBlk[1], voidBlk[2], 4, snout);

    /* Jaw plate — lower face armour */
    float jaw[][2] = {
        {-0.50f,-0.06f},{ 0.50f,-0.06f},
        { 0.45f,-0.38f},{ 0.25f,-0.56f},
        {-0.25f,-0.56f},{-0.45f,-0.38f}
    };
    _poly(voidBlk[0], voidBlk[1], voidBlk[2], 6, jaw);

    /* Teeth — serrated weapon ports */
    for(float tx : {-0.36f, -0.18f, 0.00f, 0.18f, 0.36f}) {
        glColor3f(crimson[0], crimson[1], crimson[2]);
        glBegin(GL_TRIANGLES);
            glVertex2f(tx-0.07f,-0.38f);
            glVertex2f(tx+0.07f,-0.38f);
            glVertex2f(tx,      -0.55f);
        glEnd();
    }

    /* ====== TIER 4: Mid-body weapon systems ====== */

    /* Side heavy turret sponsons */
    float spR[][2] = {{ 0.62f,-0.04f},{ 0.96f,-0.04f},{ 0.92f,-0.46f},{ 0.58f,-0.46f}};
    float spL[][2] = {{-0.62f,-0.04f},{-0.96f,-0.04f},{-0.92f,-0.46f},{-0.58f,-0.46f}};
    _poly(midPlt[0], midPlt[1], midPlt[2], 4, spR);
    _poly(midPlt[0], midPlt[1], midPlt[2], 4, spL);

    /* Sponson cannon barrels */
    float sbR[][2] = {{ 0.68f,0.06f},{ 0.84f,0.06f},{ 0.86f,0.58f},{ 0.66f,0.58f}};
    float sbL[][2] = {{-0.68f,0.06f},{-0.84f,0.06f},{-0.86f,0.58f},{-0.66f,0.58f}};
    _poly(darkPlt[0], darkPlt[1], darkPlt[2], 4, sbR);
    _poly(darkPlt[0], darkPlt[1], darkPlt[2], 4, sbL);

    /* Cannon muzzle hellfire glow */
    glColor3f(hellfire[0], hellfire[1], hellfire[2]);
    glBegin(GL_QUADS);
        glVertex2f( 0.67f,0.54f); glVertex2f( 0.85f,0.54f);
        glVertex2f( 0.85f,0.62f); glVertex2f( 0.67f,0.62f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2f(-0.67f,0.54f); glVertex2f(-0.85f,0.54f);
        glVertex2f(-0.85f,0.62f); glVertex2f(-0.67f,0.62f);
    glEnd();

    /* Green shield emitter domes */
    float emR[][2] = {{ 0.48f,-0.62f},{ 0.70f,-0.62f},{ 0.70f,-0.80f},{ 0.48f,-0.80f}};
    float emL[][2] = {{-0.48f,-0.62f},{-0.70f,-0.62f},{-0.70f,-0.80f},{-0.48f,-0.80f}};
    _poly(sickGrn[0], sickGrn[1], sickGrn[2], 4, emR);
    _poly(sickGrn[0], sickGrn[1], sickGrn[2], 4, emL);
    _outline(sickGrn[0], sickGrn[1], sickGrn[2], 4, emR);
    _outline(sickGrn[0], sickGrn[1], sickGrn[2], 4, emL);

    /* ====== TIER 5: Tri-drive reactor stern ====== */
    float stern[][2] = {
        {-0.48f,-0.84f},{ 0.48f,-0.84f},
        { 0.50f,-1.46f},{-0.50f,-1.46f}
    };
    _poly(voidBlk[0], voidBlk[1], voidBlk[2], 4, stern);

    /* Three main drive exhausts */
    float driveX[3] = {-0.32f, 0.00f, 0.32f};
    for(int i = 0; i < 3; i++) {
        float dx = driveX[i];
        glColor3f(midPlt[0], midPlt[1], midPlt[2]);
        glBegin(GL_QUADS);
            glVertex2f(dx-0.12f,-1.02f); glVertex2f(dx+0.12f,-1.02f);
            glVertex2f(dx+0.12f,-1.44f); glVertex2f(dx-0.12f,-1.44f);
        glEnd();
        glColor3f(voidPurp[0], voidPurp[1], voidPurp[2]);
        glBegin(GL_QUADS);
            glVertex2f(dx-0.09f,-1.44f); glVertex2f(dx+0.09f,-1.44f);
            glVertex2f(dx+0.06f,-1.82f); glVertex2f(dx-0.06f,-1.82f);
        glEnd();
        glColor3f(scorch[0], scorch[1], scorch[2]);
        glBegin(GL_QUADS);
            glVertex2f(dx-0.04f,-1.44f); glVertex2f(dx+0.04f,-1.44f);
            glVertex2f(dx+0.02f,-1.66f); glVertex2f(dx-0.02f,-1.66f);
        glEnd();
    }

    /* Central hellfire reactor between drives */
    float reactor[][2] = {
        {-0.18f,-0.86f},{ 0.18f,-0.86f},
        { 0.14f,-1.44f},{-0.14f,-1.44f}
    };
    _poly(hellfire[0], hellfire[1], hellfire[2], 4, reactor);
    float rCore[][2] = {
        {-0.08f,-0.94f},{ 0.08f,-0.94f},
        { 0.06f,-1.30f},{-0.06f,-1.30f}
    };
    _poly(scorch[0], scorch[1], scorch[2], 4, rCore);

    /* ====== Command spire + antenna ====== */
    float spire[][2] = {{-0.12f,1.48f},{0.12f,1.48f},{0.08f,1.97f},{-0.08f,1.97f}};
    _poly(midPlt[0], midPlt[1], midPlt[2], 4, spire);

    glColor3f(crimson[0], crimson[1], crimson[2]);
    glBegin(GL_LINES);
        glVertex2f(0.00f, 1.97f); glVertex2f(0.00f, 2.32f);
    glEnd();
    /* Red beacon tip */

    /* Final outlines — crimson hull edge, purple wings */
    _outline(0.50f, 0.02f, 0.02f, 10, core);
    _outline(0.20f, 0.04f, 0.22f,  6, owR);
    _outline(0.20f, 0.04f, 0.22f,  6, owL);
    _outline(crimson[0], crimson[1], crimson[2], 6, jaw);
}

#endif
