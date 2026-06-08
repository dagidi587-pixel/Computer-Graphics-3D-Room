
#define GL_SILENCE_DEPRECATION
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>   // needed for sin/cos in moon phase and flicker

// -----------------------------------------------------------------------------
//  GLOBAL FLAGS & CAMERA
// -----------------------------------------------------------------------------
GLboolean redFlag = true;
GLboolean switchOne = false, switchTwo = false, switchLamp = false;
GLboolean amb1 = true, diff1 = true, spec1 = true;
GLboolean amb2 = true, diff2 = true, spec2 = true;
GLboolean amb3 = true, diff3 = true, spec3 = true;

double windowHeight = 800, windowWidth = 600;
double eyeX = 7.0, eyeY = 2.0, eyeZ = 15.0;
double refX = 0, refY = 0, refZ = 0;
double theta = 180.0, y = 1.36, z = 7.97888;

// -----------------------------------------------------------------------------
//  NIGHT MODE
// -----------------------------------------------------------------------------
GLboolean nightMode = false;   // toggled with 'y'

// -----------------------------------------------------------------------------
//  WALL / ROOM COLOR  (8 presets, cycled with 'c')
// -----------------------------------------------------------------------------
static int colorIndex = 1;   // 1-8

// Each preset: { R, G, B }
static GLfloat wallPresets[9][3] = {
    {0,0,0},            // 0 - unused (1-based index)
    {1.00f, 0.80f, 0.70f},  // 1 Warm Ivory
    {0.60f, 0.72f, 1.00f},  // 2 Cool Blue
    {0.72f, 0.93f, 0.72f},  // 3 Pastel Green
    {0.85f, 0.72f, 1.00f},  // 4 Soft Purple
    {1.00f, 0.70f, 0.45f},  // 5 Sunset Orange
    {1.00f, 0.85f, 0.90f},  // 6 Blush Pink
    {0.78f, 0.88f, 0.88f},  // 7 Seafoam Teal
    {0.95f, 0.95f, 0.85f},  // 8 Linen White
};

GLfloat roomColorR, roomColorG, roomColorB;   // set from preset in main()

// -----------------------------------------------------------------------------
//  FLOOR COLOR  (4 presets, cycled with 'f')
// -----------------------------------------------------------------------------
static int floorIndex = 1;   // 1-4

// Each preset: { diffR, diffG, diffB, ambR, ambG, ambB }
static GLfloat floorPresets[5][6] = {
    {0,0,0, 0,0,0},                                     // 0 unused
    {0.55f, 0.27f, 0.07f,  0.28f, 0.14f, 0.04f},       // 1 Wood Brown
    {0.25f, 0.12f, 0.05f,  0.13f, 0.06f, 0.03f},       // 2 Dark Brown
    {0.92f, 0.92f, 0.90f,  0.46f, 0.46f, 0.45f},       // 3 Marble White
    {0.55f, 0.55f, 0.55f,  0.28f, 0.28f, 0.28f},       // 4 Slate Gray
};

// -----------------------------------------------------------------------------
//  MOON  (animation + phases)
// -----------------------------------------------------------------------------
GLfloat moonAngle = 0.0f;   // rotation angle (mode 0) or horizontal offset (mode 1)
GLfloat moonPosX = 8.0f;   // Z-slide position for moon (window spans Z 8.9-9.8)
static int moonAnimMode = 0;  // 0 = rotate on axis, 1 = slide across window  ('k' toggles)
static int moonPhase = 0;  // 0 = full, 1 = half, 2 = crescent              ('p' cycles)

// -----------------------------------------------------------------------------
//  LAMP FLICKER
// -----------------------------------------------------------------------------
static GLfloat lampFlicker = 1.0f;   // multiplier applied to lamp emission
static int     flickerTick = 0;      // counter to control flicker speed


// -----------------------------------------------------------------------------
//  GEOMETRY DATA
// -----------------------------------------------------------------------------
static GLfloat v_cube[8][3] =
{
    {0.0f, 0.0f, 0.0f}, //0
    {0.0f, 0.0f, 3.0f}, //1
    {3.0f, 0.0f, 3.0f}, //2
    {3.0f, 0.0f, 0.0f}, //3
    {0.0f, 3.0f, 0.0f}, //4
    {0.0f, 3.0f, 3.0f}, //5
    {3.0f, 3.0f, 3.0f}, //6
    {3.0f, 3.0f, 0.0f}  //7
};

static GLubyte quadIndices[6][4] =
{
    {0, 1, 2, 3}, //bottom
    {4, 5, 6, 7}, //top
    {5, 1, 2, 6}, //front
    {0, 4, 7, 3}, //back  (clockwise)
    {2, 3, 7, 6}, //right
    {1, 5, 4, 0}  //left  (clockwise)
};

static GLfloat v_trapezoid[8][3] =
{
    {0.0f, 0.0f, 0.0f}, //0
    {0.0f, 0.0f, 3.0f}, //1
    {3.0f, 0.0f, 3.0f}, //2
    {3.0f, 0.0f, 0.0f}, //3
    {0.5f, 3.0f, 0.5f}, //4
    {0.5f, 3.0f, 2.5f}, //5
    {2.5f, 3.0f, 2.5f}, //6
    {2.5f, 3.0f, 0.5f}  //7
};

static GLubyte TquadIndices[6][4] =
{
    {0, 1, 2, 3}, //bottom
    {4, 5, 6, 7}, //top
    {5, 1, 2, 6}, //front
    {0, 4, 7, 3}, //back
    {2, 3, 7, 6}, //right
    {1, 5, 4, 0}  //left
};

static GLfloat v_pyramid[5][3] =
{
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 2.0f},
    {2.0f, 0.0f, 2.0f},
    {2.0f, 0.0f, 0.0f},
    {1.0f, 4.0f, 1.0f}
};

static GLubyte p_Indices[4][3] =
{
    {4, 1, 2},
    {4, 2, 3},
    {4, 3, 0},
    {4, 0, 1}
};

static GLubyte PquadIndices[1][4] =
{
    {0, 3, 2, 1}
};

// -----------------------------------------------------------------------------
//  NORMAL HELPER
// -----------------------------------------------------------------------------
static void getNormal3p(
    GLfloat x1, GLfloat y1, GLfloat z1,
    GLfloat x2, GLfloat y2, GLfloat z2,
    GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux = x2 - x1, Uy = y2 - y1, Uz = z2 - z1;
    GLfloat Vx = x3 - x1, Vy = y3 - y1, Vz = z3 - z1;
    glNormal3f(Uy * Vz - Uz * Vy,
        Uz * Vx - Ux * Vz,
        Ux * Vy - Uy * Vx);
}


// -----------------------------------------------------------------------------
//  PRIMITIVE DRAW HELPERS
// -----------------------------------------------------------------------------

void drawCube()
{
    glBegin(GL_QUADS);
    for (GLint i = 0; i < 6; i++)
    {
        getNormal3p(
            v_cube[quadIndices[i][0]][0], v_cube[quadIndices[i][0]][1], v_cube[quadIndices[i][0]][2],
            v_cube[quadIndices[i][1]][0], v_cube[quadIndices[i][1]][1], v_cube[quadIndices[i][1]][2],
            v_cube[quadIndices[i][2]][0], v_cube[quadIndices[i][2]][1], v_cube[quadIndices[i][2]][2]);
        glVertex3fv(&v_cube[quadIndices[i][0]][0]);
        glVertex3fv(&v_cube[quadIndices[i][1]][0]);
        glVertex3fv(&v_cube[quadIndices[i][2]][0]);
        glVertex3fv(&v_cube[quadIndices[i][3]][0]);
    }
    glEnd();
}

// shine defaults raised for shinier surfaces; specular kept white
void drawCube1(GLfloat difX, GLfloat difY, GLfloat difZ,
    GLfloat ambX = 0, GLfloat ambY = 0, GLfloat ambZ = 0,
    GLfloat shine = 50)
{
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0f };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0f };
    //  specular intensity scaled by shine so very shiny objects sparkle more
    GLfloat specScale = (shine > 80) ? 1.0f : 0.4f;
    GLfloat mat_specular[] = { specScale, specScale, specScale, 1.0f };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    glBegin(GL_QUADS);
    for (GLint i = 0; i < 6; i++)
    {
        getNormal3p(
            v_cube[quadIndices[i][0]][0], v_cube[quadIndices[i][0]][1], v_cube[quadIndices[i][0]][2],
            v_cube[quadIndices[i][1]][0], v_cube[quadIndices[i][1]][1], v_cube[quadIndices[i][1]][2],
            v_cube[quadIndices[i][2]][0], v_cube[quadIndices[i][2]][1], v_cube[quadIndices[i][2]][2]);
        glVertex3fv(&v_cube[quadIndices[i][0]][0]);
        glVertex3fv(&v_cube[quadIndices[i][1]][0]);
        glVertex3fv(&v_cube[quadIndices[i][2]][0]);
        glVertex3fv(&v_cube[quadIndices[i][3]][0]);
    }
    glEnd();
}

void drawTrapezoid(GLfloat difX, GLfloat difY, GLfloat difZ,
    GLfloat ambX, GLfloat ambY, GLfloat ambZ,
    GLfloat shine = 50)
{
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0f };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    //  lamp shade emits warm light when lamp is on, with flicker applied
    GLfloat mat_emission[] = { difX * lampFlicker, difY * lampFlicker, difZ * lampFlicker, 1.0f };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    if (switchLamp == true) {
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    }
    else {
        glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    }

    glBegin(GL_QUADS);
    for (GLint i = 0; i < 6; i++)
    {
        getNormal3p(
            v_trapezoid[TquadIndices[i][0]][0], v_trapezoid[TquadIndices[i][0]][1], v_trapezoid[TquadIndices[i][0]][2],
            v_trapezoid[TquadIndices[i][1]][0], v_trapezoid[TquadIndices[i][1]][1], v_trapezoid[TquadIndices[i][1]][2],
            v_trapezoid[TquadIndices[i][2]][0], v_trapezoid[TquadIndices[i][2]][1], v_trapezoid[TquadIndices[i][2]][2]);
        glVertex3fv(&v_trapezoid[TquadIndices[i][0]][0]);
        glVertex3fv(&v_trapezoid[TquadIndices[i][1]][0]);
        glVertex3fv(&v_trapezoid[TquadIndices[i][2]][0]);
        glVertex3fv(&v_trapezoid[TquadIndices[i][3]][0]);
    }
    glEnd();
}

void drawpyramid(GLfloat difX, GLfloat difY, GLfloat difZ,
    GLfloat ambX, GLfloat ambY, GLfloat ambZ,
    GLfloat shine)
{
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0f };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_TRIANGLES);
    for (GLint i = 0; i < 4; i++)
    {
        getNormal3p(
            v_pyramid[p_Indices[i][0]][0], v_pyramid[p_Indices[i][0]][1], v_pyramid[p_Indices[i][0]][2],
            v_pyramid[p_Indices[i][1]][0], v_pyramid[p_Indices[i][1]][1], v_pyramid[p_Indices[i][1]][2],
            v_pyramid[p_Indices[i][2]][0], v_pyramid[p_Indices[i][2]][1], v_pyramid[p_Indices[i][2]][2]);
        glVertex3fv(&v_pyramid[p_Indices[i][0]][0]);
        glVertex3fv(&v_pyramid[p_Indices[i][1]][0]);
        glVertex3fv(&v_pyramid[p_Indices[i][2]][0]);
    }
    glEnd();

    glBegin(GL_QUADS);
    for (GLint i = 0; i < 1; i++)
    {
        getNormal3p(
            v_pyramid[PquadIndices[i][0]][0], v_pyramid[PquadIndices[i][0]][1], v_pyramid[PquadIndices[i][0]][2],
            v_pyramid[PquadIndices[i][1]][0], v_pyramid[PquadIndices[i][1]][1], v_pyramid[PquadIndices[i][1]][2],
            v_pyramid[PquadIndices[i][2]][0], v_pyramid[PquadIndices[i][2]][1], v_pyramid[PquadIndices[i][2]][2]);
        glVertex3fv(&v_pyramid[PquadIndices[i][0]][0]);
        glVertex3fv(&v_pyramid[PquadIndices[i][1]][0]);
        glVertex3fv(&v_pyramid[PquadIndices[i][2]][0]);
        glVertex3fv(&v_pyramid[PquadIndices[i][3]][0]);
    }
    glEnd();
}

void polygon(GLfloat difX, GLfloat difY, GLfloat difZ,
    GLfloat ambX, GLfloat ambY, GLfloat ambZ,
    GLfloat shine)
{
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0f };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_POLYGON);
    glVertex2f(0, 0); glVertex2f(6, 0);
    glVertex2f(5.8f, 1); glVertex2f(5.2f, 2);
    glVertex2f(5, 2.2f); glVertex2f(4, 2.8f);
    glVertex2f(3, 3);    glVertex2f(2, 2.8f);
    glVertex2f(1, 2.2f); glVertex2f(0.8f, 2);
    glVertex2f(0.2f, 1);
    glEnd();
}

void polygonLine(GLfloat difX, GLfloat difY, GLfloat difZ,
    GLfloat ambX, GLfloat ambY, GLfloat ambZ,
    GLfloat shine)
{
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0f };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glBegin(GL_LINE_STRIP);
    glVertex2f(6, 0);    glVertex2f(5.8f, 1);
    glVertex2f(5.2f, 2); glVertex2f(5, 2.2f);
    glVertex2f(4, 2.8f); glVertex2f(3, 3);
    glVertex2f(2, 2.8f); glVertex2f(1, 2.2f);
    glVertex2f(0.8f, 2); glVertex2f(0.2f, 1);
    glVertex2f(0, 0);
    glEnd();
}

// shine defaults to 90; specular highlight for shiny objects (lamp, handles)
void drawSphere(GLfloat difX, GLfloat difY, GLfloat difZ,
    GLfloat ambX, GLfloat ambY, GLfloat ambZ,
    GLfloat shine = 90)
{
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0f };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { shine };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glutSolidSphere(3.0, 20, 16);
}


// -----------------------------------------------------------------------------
//  SCENE OBJECTS
// -----------------------------------------------------------------------------

void cupboard()
{
    // richer wood tone with higher specular shine for polished finish
    // Cupboard body - dark walnut wood
    glPushMatrix();
    glTranslatef(4, 0, 4.4f);
    glScalef(0.5f, 1, 0.5f);
    drawCube1(0.36f, 0.18f, 0.09f, 0.18f, 0.09f, 0.045f, 80);
    glPopMatrix();

    // Vertical strip lines - slightly darker shade for depth
    glPushMatrix();
    glTranslatef(4, 1, 5.9f);
    glScalef(0.5f, 0.01f, 0.0001f);
    drawCube1(0.22f, 0.11f, 0.05f, 0.11f, 0.055f, 0.025f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4, 0.5f, 5.9f);
    glScalef(0.5f, 0.01f, 0.0001f);
    drawCube1(0.22f, 0.11f, 0.05f, 0.11f, 0.055f, 0.025f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4, 0, 5.9f);
    glScalef(0.5f, 0.01f, 0.0001f);
    drawCube1(0.22f, 0.11f, 0.05f, 0.11f, 0.055f, 0.025f);
    glPopMatrix();

    // Horizontal strip lines
    glPushMatrix();
    glTranslatef(5.5f, 0, 5.9f);
    glScalef(0.01f, 1, 0.0001f);
    drawCube1(0.22f, 0.11f, 0.05f, 0.11f, 0.055f, 0.025f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.75f, 1, 5.9f);
    glScalef(0.01f, 0.67f, 0.0001f);
    drawCube1(0.22f, 0.11f, 0.05f, 0.11f, 0.055f, 0.025f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4, 0, 5.9f);
    glScalef(0.01f, 1, 0.0001f);
    drawCube1(0.22f, 0.11f, 0.05f, 0.11f, 0.055f, 0.025f);
    glPopMatrix();

    // metallic gold handles with high shine
    glPushMatrix();
    glTranslatef(5, 1.4f, 5.9f);
    glScalef(0.02f, 0.18f, 0.01f);
    drawCube1(0.83f, 0.68f, 0.21f, 0.42f, 0.34f, 0.10f, 120);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.02f, 1.9f, 5.91f);
    glScalef(0.02f, 0.02f, 0.01f);
    drawSphere(0.83f, 0.68f, 0.21f, 0.42f, 0.34f, 0.10f, 120);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.5f, 1.4f, 5.9f);
    glScalef(0.02f, 0.18f, 0.01f);
    drawCube1(0.83f, 0.68f, 0.21f, 0.42f, 0.34f, 0.10f, 120);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.52f, 1.9f, 5.91f);
    glScalef(0.02f, 0.02f, 0.01f);
    drawSphere(0.83f, 0.68f, 0.21f, 0.42f, 0.34f, 0.10f, 120);
    glPopMatrix();

    // Drawer handles - gold metallic
    glPushMatrix();
    glTranslatef(4.5f, 0.7f, 5.9f);
    glScalef(0.16f, 0.02f, 0.01f);
    drawCube1(0.83f, 0.68f, 0.21f, 0.42f, 0.34f, 0.10f, 120);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.5f, 0.25f, 5.9f);
    glScalef(0.16f, 0.02f, 0.01f);
    drawCube1(0.83f, 0.68f, 0.21f, 0.42f, 0.34f, 0.10f, 120);
    glPopMatrix();
}

void room()
{
    // -- Carpet --------------------------------------------------------------
    //  richer burgundy carpet
    glPushMatrix();
    glTranslatef(3, -0.2f, 7);
    glScalef(1.3f, 0.01f, 1.7f);
    drawCube1(0.50f, 0.08f, 0.08f, 0.25f, 0.04f, 0.04f);
    glPopMatrix();

    // -- Walls ----------------------------------------------------------------
    //  night mode dims walls to ~25% brightness; day uses full preset color
    GLfloat wR = nightMode ? roomColorR * 0.25f : roomColorR;
    GLfloat wG = nightMode ? roomColorG * 0.25f : roomColorG;
    GLfloat wB = nightMode ? roomColorB * 0.25f : roomColorB;
    // Ambient is half of diffuse for a subtle self-lit look
    GLfloat aR = wR * 0.5f, aG = wG * 0.5f, aB = wB * 0.5f;

    // Right wall
    glPushMatrix();
    glTranslatef(-1.5f, -1, 0.5f);
    glScalef(5, 2, 0.1f);
    drawCube1(wR, wG, wB, aR, aG, aB);
    glPopMatrix();

    // Left wall
    glPushMatrix();
    glTranslatef(-4.5f, -1, 0);
    glScalef(1, 2, 5);
    drawCube1(wR, wG, wB, aR, aG, aB);
    glPopMatrix();

    // Far right wall
    glPushMatrix();
    glTranslatef(8, -1, 0);
    glScalef(0.2f, 2, 5);
    drawCube1(wR, wG, wB, aR, aG, aB);
    glPopMatrix();

    // -- Ceiling --------------------------------------------------------------
    //  night ceiling is deep navy; day is warm off-white
    glPushMatrix();
    glTranslatef(-2, 5.1f, 0);
    glScalef(5, 0.1f, 7);
    if (nightMode) {
        drawCube1(0.05f, 0.05f, 0.15f, 0.03f, 0.03f, 0.08f);
    }
    else {
        drawCube1(1.0f, 0.95f, 0.88f, 0.50f, 0.48f, 0.44f);
    }
    glPopMatrix();

    // -- Floor ----------------------------------------------------------------
    // uses floorPresets array; dimmed in night mode
    GLfloat* fp = floorPresets[floorIndex];
    GLfloat fdR = nightMode ? fp[0] * 0.3f : fp[0];
    GLfloat fdG = nightMode ? fp[1] * 0.3f : fp[1];
    GLfloat fdB = nightMode ? fp[2] * 0.3f : fp[2];
    GLfloat faR = nightMode ? fp[3] * 0.3f : fp[3];
    GLfloat faG = nightMode ? fp[4] * 0.3f : fp[4];
    GLfloat faB = nightMode ? fp[5] * 0.3f : fp[5];

    glPushMatrix();
    glScalef(5, 0.1f, 7);
    glTranslatef(-1, -5, 0);
    drawCube1(fdR, fdG, fdB, faR, faG, faB, 30);
    glPopMatrix();
}

void bed()
{
    // dark mahogany headboard
    glPushMatrix();
    glScalef(0.1f, 0.5f, 0.9f);
    glTranslatef(-2, -0.5f, 6.2f);
    drawCube1(0.40f, 0.16f, 0.08f, 0.20f, 0.08f, 0.04f, 60);
    glPopMatrix();

    //  warm linen bed body
    glPushMatrix();
    glScalef(1, 0.2f, 0.9f);
    glTranslatef(0, -0.5f, 6.2f);
    drawCube1(0.87f, 0.78f, 0.65f, 0.44f, 0.39f, 0.33f);
    glPopMatrix();

    // soft terracotta pillows
    glPushMatrix();
    glTranslatef(0.5f, 0.5f, 6);
    glRotatef(20, 0, 0, 1);
    glScalef(0.1f, 0.15f, 0.28f);
    drawCube1(0.80f, 0.42f, 0.30f, 0.40f, 0.21f, 0.15f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.5f, 0.5f, 7.2f);
    glRotatef(22, 0, 0, 1);
    glScalef(0.1f, 0.15f, 0.28f);
    drawCube1(0.80f, 0.42f, 0.30f, 0.40f, 0.21f, 0.15f);
    glPopMatrix();

    //  deep teal blanket
    glPushMatrix();
    glTranslatef(1.4f, 0.45f, 5.5f);
    glScalef(0.5f, 0.05f, 0.95f);
    drawCube1(0.18f, 0.45f, 0.50f, 0.09f, 0.23f, 0.25f);
    glPopMatrix();

    // Blanket side fold
    glPushMatrix();
    glTranslatef(1.4f, -0.3f, 8.16f);
    glScalef(0.5f, 0.25f, 0.05f);
    drawCube1(0.18f, 0.45f, 0.50f, 0.09f, 0.23f, 0.25f);
    glPopMatrix();
}

void bedsideDrawer()
{
    //  medium oak drawer body
    glPushMatrix();
    glTranslatef(0.5f, -0.1f, 8.7f);
    glScalef(0.12f, 0.2f, 0.23f);
    drawCube1(0.60f, 0.35f, 0.15f, 0.30f, 0.18f, 0.08f, 60);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.88f, 0, 8.8f);
    glScalef(0.0001f, 0.11f, 0.18f);
    drawCube1(0.65f, 0.40f, 0.18f, 0.33f, 0.20f, 0.09f);
    glPopMatrix();

    //  shiny brass knob
    glPushMatrix();
    glTranslatef(0.9f, 0.15f, 9.05f);
    glScalef(0.01f, 0.02f, 0.02f);
    drawSphere(0.83f, 0.68f, 0.21f, 0.42f, 0.34f, 0.10f, 128);
    glPopMatrix();
}

void lamp()
{
    //  dark navy lamp base
    glPushMatrix();
    glTranslatef(0.6f, 0.5f, 8.95f);
    glScalef(0.07f, 0.02f, 0.07f);
    drawCube1(0.10f, 0.10f, 0.35f, 0.05f, 0.05f, 0.18f, 40);
    glPopMatrix();

    //  brushed silver/chrome stand with high specular
    glPushMatrix();
    glTranslatef(0.7f, 0.35f, 9.05f);
    glScalef(0.01f, 0.2f, 0.01f);
    drawCube1(0.75f, 0.75f, 0.78f, 0.38f, 0.38f, 0.39f, 128);
    glPopMatrix();

    //  warm amber lamp shade - emits warm glow when on (flicker applied in drawTrapezoid)
    glPushMatrix();
    glTranslatef(0.6f, 0.9f, 8.9f);
    glScalef(0.08f, 0.09f, 0.08f);
    drawTrapezoid(1.00f, 0.75f, 0.20f, 0.50f, 0.38f, 0.10f);
    glPopMatrix();
}

void LinkinParkPoster()
{
    // Poster black background
    glPushMatrix();
    glTranslatef(-1, 1.4f, 4.6f);
    glScalef(0.0001f, 0.65f, 0.8f);
    drawCube1(0, 0, 0, 0, 0, 0, 10);
    glPopMatrix();

    // Logo elements (white)
    glPushMatrix();
    glTranslatef(-0.9f, 2.1f, 5.5f);
    glScalef(0.0001f, 0.02f, 0.25f);
    drawCube1(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.9f, 2.1f, 6.2f);
    glRotatef(-14, 1, 0, 0);
    glScalef(0.0001f, 0.28f, 0.02f);
    drawCube1(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.9f, 1.8f, 6);
    glRotatef(-14, 1, 0, 0);
    glScalef(0.0001f, 0.29f, 0.02f);
    drawCube1(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.9f, 2.1f, 5.5f);
    glRotatef(23, 1, 0, 0);
    glScalef(0.0001f, 0.25f, 0.02f);
    drawCube1(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 10);
    glPopMatrix();
}

void wardrobe()
{
    //warm chestnut wood body with polished finish
    glPushMatrix();
    glTranslatef(0, 0, 4);
    glScalef(0.12f, 0.6f, 0.4f);
    drawCube1(0.45f, 0.22f, 0.07f, 0.23f, 0.11f, 0.04f, 70);
    glPopMatrix();

    // Drawer faces - slightly lighter shade for contrast
    glPushMatrix();
    glTranslatef(0.36f, 1.4f, 4.05f);
    glScalef(0.0001f, 0.11f, 0.38f);
    drawCube1(0.55f, 0.28f, 0.10f, 0.28f, 0.14f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.36f, 1, 4.05f);
    glScalef(0.0001f, 0.11f, 0.38f);
    drawCube1(0.55f, 0.28f, 0.10f, 0.28f, 0.14f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.36f, 0.6f, 4.05f);
    glScalef(0.0001f, 0.11f, 0.38f);
    drawCube1(0.55f, 0.28f, 0.10f, 0.28f, 0.14f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.36f, 0.2f, 4.05f);
    glScalef(0.0001f, 0.11f, 0.38f);
    drawCube1(0.55f, 0.28f, 0.10f, 0.28f, 0.14f, 0.05f);
    glPopMatrix();

    //  gold metallic drawer handles
    glPushMatrix();
    glTranslatef(0.37f, 1.5f, 4.3f);
    glScalef(0.01f, 0.03f, 0.2f);
    drawCube1(0.83f, 0.68f, 0.21f, 0.42f, 0.34f, 0.10f, 120);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.37f, 1.1f, 4.3f);
    glScalef(0.01f, 0.03f, 0.2f);
    drawCube1(0.83f, 0.68f, 0.21f, 0.42f, 0.34f, 0.10f, 120);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.37f, 0.7f, 4.3f);
    glScalef(0.01f, 0.03f, 0.2f);
    drawCube1(0.83f, 0.68f, 0.21f, 0.42f, 0.34f, 0.10f, 120);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.37f, 0.3f, 4.3f);
    glScalef(0.01f, 0.03f, 0.2f);
    drawCube1(0.83f, 0.68f, 0.21f, 0.42f, 0.34f, 0.10f, 120);
    glPopMatrix();
}


void dressingTable()
{
    //  honey maple wood body
    glPushMatrix();
    glTranslatef(5.9f, 0, 4.6f);
    glScalef(0.2f, 0.2f, 0.2f);
    drawCube1(0.72f, 0.45f, 0.15f, 0.36f, 0.23f, 0.08f, 70);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(7, 0, 4.6f);
    glScalef(0.2f, 0.2f, 0.2f);
    drawCube1(0.72f, 0.45f, 0.15f, 0.36f, 0.23f, 0.08f, 70);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.9f, 0.6f, 4.6f);
    glScalef(0.57f, 0.1f, 0.2f);
    drawCube1(0.72f, 0.45f, 0.15f, 0.36f, 0.23f, 0.08f, 70);
    glPopMatrix();

    // Drawer stripes
    glPushMatrix();
    glTranslatef(5.9f, 0.6f, 5.2f);
    glScalef(0.57f, 0.01f, 0.0001f);
    drawCube1(0.30f, 0.15f, 0.05f, 0.15f, 0.08f, 0.03f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.9f, 0.9f, 5.2f);
    glScalef(0.57f, 0.01f, 0.0001f);
    drawCube1(0.30f, 0.15f, 0.05f, 0.15f, 0.08f, 0.03f);
    glPopMatrix();

    //  gold handles
    glPushMatrix();
    glTranslatef(6.5f, 0.75f, 5.2f);
    glScalef(0.16f, 0.02f, 0.0001f);
    drawCube1(0.83f, 0.68f, 0.21f, 0.42f, 0.34f, 0.10f, 120);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(6.4f, 0.1f, 5.2f);
    glScalef(0.02f, 0.13f, 0.0001f);
    drawCube1(0.83f, 0.68f, 0.21f, 0.42f, 0.34f, 0.10f, 120);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(7.1f, 0.1f, 5.2f);
    glScalef(0.02f, 0.13f, 0.0001f);
    drawCube1(0.83f, 0.68f, 0.21f, 0.42f, 0.34f, 0.10f, 120);
    glPopMatrix();

    //  mirrors with high specular (light blue tint, very shiny)
    glPushMatrix();
    glTranslatef(6.2f, 0.9f, 4.7f);
    glScalef(0.36f, 0.5f, 0.0001f);
    drawCube1(0.75f, 0.90f, 0.95f, 0.38f, 0.45f, 0.48f, 128);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.92f, 0.9f, 4.7f);
    glScalef(0.1f, 0.48f, 0.0001f);
    drawCube1(0.75f, 0.90f, 0.95f, 0.38f, 0.45f, 0.48f, 128);
    glPopMatrix();

    // Mirror frame stripes
    glPushMatrix();
    glTranslatef(5.92f, 0.9f, 4.71f);
    glScalef(0.019f, 0.48f, 0.0001f);
    drawCube1(0.30f, 0.15f, 0.05f, 0.15f, 0.08f, 0.03f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(6.17f, 0.9f, 4.71f);
    glScalef(0.019f, 0.48f, 0.0001f);
    drawCube1(0.30f, 0.15f, 0.05f, 0.15f, 0.08f, 0.03f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.92f, 0.9f, 4.71f);
    glScalef(0.55f, 0.019f, 0.0001f);
    drawCube1(0.30f, 0.15f, 0.05f, 0.15f, 0.08f, 0.03f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.92f, 2.3f, 4.71f);
    glScalef(0.1f, 0.019f, 0.0001f);
    drawCube1(0.30f, 0.15f, 0.05f, 0.15f, 0.08f, 0.03f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(7.25f, 0.9f, 4.7f);
    glScalef(0.1f, 0.48f, 0.0001f);
    drawCube1(0.75f, 0.90f, 0.95f, 0.38f, 0.45f, 0.48f, 128);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(7.25f, 2.3f, 4.71f);
    glScalef(0.1f, 0.019f, 0.0001f);
    drawCube1(0.30f, 0.15f, 0.05f, 0.15f, 0.08f, 0.03f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(7.25f, 0.9f, 4.71f);
    glScalef(0.019f, 0.48f, 0.0001f);
    drawCube1(0.30f, 0.15f, 0.05f, 0.15f, 0.08f, 0.03f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(7.5f, 0.9f, 4.71f);
    glScalef(0.019f, 0.48f, 0.0001f);
    drawCube1(0.30f, 0.15f, 0.05f, 0.15f, 0.08f, 0.03f);
    glPopMatrix();

    // Mirror top polygon
    glPushMatrix();
    glTranslatef(6.2f, 2.4f, 4.7f);
    glScalef(0.18f, 0.18f, 2);
    polygon(0.75f, 0.90f, 0.95f, 0.38f, 0.45f, 0.48f, 128);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(6.2f, 2.4f, 4.71f);
    glScalef(0.18f, 0.18f, 1);
    polygonLine(0.30f, 0.15f, 0.05f, 0.15f, 0.08f, 0.03f, 50);
    glPopMatrix();
}

void wallshelf()
{
    // each shelf level has a distinct warm wood tone for visual variety
    // Shelf 1 (top) - light pine
    glPushMatrix();
    glTranslatef(1.5f, 2.7f, 3);
    glScalef(0.4f, 0.03f, 0.2f);
    drawCube1(0.80f, 0.60f, 0.30f, 0.40f, 0.30f, 0.15f);
    glPopMatrix();

    // Shelf 2 - medium oak
    glPushMatrix();
    glTranslatef(1, 2.3f, 3);
    glScalef(0.4f, 0.03f, 0.2f);
    drawCube1(0.65f, 0.42f, 0.18f, 0.33f, 0.21f, 0.09f);
    glPopMatrix();

    // Shelf 3 - warm walnut
    glPushMatrix();
    glTranslatef(0.5f, 1.9f, 3);
    glScalef(0.4f, 0.03f, 0.2f);
    drawCube1(0.50f, 0.28f, 0.10f, 0.25f, 0.14f, 0.05f);
    glPopMatrix();

    // Shelf 4 - dark mahogany
    glPushMatrix();
    glTranslatef(1, 1.5f, 3);
    glScalef(0.4f, 0.03f, 0.2f);
    drawCube1(0.38f, 0.18f, 0.07f, 0.19f, 0.09f, 0.04f);
    glPopMatrix();

    // Shelf 5 (bottom) - ebony
    glPushMatrix();
    glTranslatef(1.5f, 1.1f, 3);
    glScalef(0.4f, 0.03f, 0.2f);
    drawCube1(0.22f, 0.10f, 0.04f, 0.11f, 0.05f, 0.02f);
    glPopMatrix();

    // Showpieces - kept colorful, slightly improved shading
    glPushMatrix();
    glTranslatef(1.5f, 1.2f, 3);
    glScalef(0.04f, 0.06f, 0.2f);
    drawCube1(0.698f, 0.133f, 0.133f, 0.349f, 0.067f, 0.067f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2, 1.2f, 3);
    glScalef(0.04f, 0.06f, 0.2f);
    drawCube1(0.729f, 0.333f, 0.827f, 0.365f, 0.167f, 0.414f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.5f, 1.2f, 3);
    glScalef(0.04f, 0.06f, 0.2f);
    drawCube1(0.098f, 0.098f, 0.439f, 0.049f, 0.049f, 0.220f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.51f, 1.35f, 3);
    glScalef(0.01f, 0.05f, 0.2f);
    drawCube1(0.529f, 0.808f, 0.980f, 0.265f, 0.404f, 0.490f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.5f, 2.71f, 3);
    glScalef(0.05f, 0.16f, 0.01f);
    drawCube1(0.502f, 0.502f, 0.000f, 0.251f, 0.251f, 0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.8f, 2.71f, 3);
    glScalef(0.16f, 0.1f, 0.01f);
    drawCube1(0, 0, 0.9f, 0, 0, 0.45f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.3f, 2.4f, 3);
    glScalef(0.16f, 0.08f, 0.01f);
    drawCube1(0.416f, 0.353f, 0.804f, 0.208f, 0.177f, 0.402f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4f, 1.9f, 3);
    glScalef(0.05f, 0.16f, 0.01f);
    drawCube1(0.863f, 0.078f, 0.235f, 0.432f, 0.039f, 0.118f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.7f, 1.9f, 3);
    glScalef(0.05f, 0.12f, 0.01f);
    drawCube1(0.780f, 0.082f, 0.522f, 0.390f, 0.041f, 0.261f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1, 1.9f, 3);
    glScalef(0.05f, 0.09f, 0.01f);
    drawCube1(0.6f, 0.196f, 0.8f, 0.3f, 0.098f, 0.4f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.8f, 1.5f, 3);
    glScalef(0.2f, 0.1f, 0.2f);
    drawpyramid(0.282f, 0.239f, 0.545f, 0.141f, 0.120f, 0.273f, 50);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.4f, 1.5f, 3);
    glScalef(0.15f, 0.1f, 0.2f);
    drawpyramid(0.251f, 0.878f, 0.816f, 0.126f, 0.439f, 0.408f, 50);
    glPopMatrix();
}

void Clock()
{
    // Clock body - dark walnut
    glPushMatrix();
    glTranslatef(-0.9f, 1.8f, 7.87f);
    glScalef(0.08f, 0.25f, 0.1f);
    drawCube1(0.40f, 0.20f, 0.06f, 0.20f, 0.10f, 0.03f, 50);
    glPopMatrix();

    // Clock face - cream white
    glPushMatrix();
    glTranslatef(-0.83f, 1.9f, 7.9f);
    glScalef(0.06f, 0.2f, 0.08f);
    drawCube1(0.98f, 0.95f, 0.88f, 0.49f, 0.48f, 0.44f);
    glPopMatrix();

    // Clock hands - black
    glPushMatrix();
    glTranslatef(-0.65f, 2.18f, 8.01f);
    glRotatef(45, 1, 0, 0);
    glScalef(0.0001f, 0.01f, 0.04f);
    drawCube1(0, 0, 0, 0, 0, 0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.65f, 2.18f, 8.01f);
    glRotatef(90, 1, 0, 0);
    glScalef(0.0001f, 0.012f, 0.08f);
    drawCube1(0, 0, 0, 0, 0, 0);
    glPopMatrix();

    // Pendulum stick
    glPushMatrix();
    glTranslatef(-0.7f, 2, 8.1f);
    glRotatef((GLfloat)theta, 1, 0, 0);
    glScalef(0.0001f, 0.2f, 0.03f);
    drawCube1(0.30f, 0.15f, 0.05f, 0.15f, 0.08f, 0.03f);
    glPopMatrix();

    // Pendulum bob - brass sphere
    glPushMatrix();
    glTranslatef(-0.72f, 1.42f, (GLfloat)z);
    glScalef(0.035f, 0.035f, 0.035f);
    drawSphere(0.83f, 0.68f, 0.21f, 0.42f, 0.34f, 0.10f, 100);
    glPopMatrix();

    // Clock top pyramid
    glPushMatrix();
    glTranslatef(-0.9f, 2.5f, 7.81f);
    glScalef(0.16f, 0.1f, 0.2f);
    drawpyramid(0.40f, 0.20f, 0.06f, 0.20f, 0.10f, 0.03f, 50);
    glPopMatrix();
}


// -----------------------------------------------------------------------------
//  MOON  (NEW - phases + animation modes)
// -----------------------------------------------------------------------------

// draw a solid sphere with moon material
static void drawMoonSphere(GLfloat radius)
{
    GLfloat mat_ambient[] = { 0.85f, 0.85f, 0.70f, 1.0f };
    GLfloat mat_diffuse[] = { 1.00f, 1.00f, 0.85f, 1.0f };
    GLfloat mat_specular[] = { 0.60f, 0.60f, 0.50f, 1.0f };
    GLfloat mat_shininess[] = { 30.0f };

    //  moon emits a soft glow so it is visible even without direct light
    GLfloat mat_emission[] = { 0.40f, 0.40f, 0.30f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    glutSolidSphere(radius, 40, 40);

    // Reset emission so other objects are not affected
    GLfloat no_emit[] = { 0,0,0,1 };
    glMaterialfv(GL_FRONT, GL_EMISSION, no_emit);
}

//  draw a dark "bite" sphere to simulate crescent/half-moon via subtraction.
// Drawn in the background sky color so it visually masks the moon sphere.
static void drawMoonOccluder(GLfloat radius, GLfloat offsetX)
{
    glDisable(GL_LIGHTING);

    // Match the night sky background color so the occluder blends in
    glColor3f(0.02f, 0.02f, 0.06f);
    glPushMatrix();
    glTranslatef(offsetX, 0.0f, 0.0f);
    glutSolidSphere(radius, 40, 40);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawMoon()
{
    // Only draw during night mode
    if (!nightMode) return;

    // -------------------------------------------------------------------------
    // MOON POSITION: placed OUTSIDE the bedroom, beyond the left wall.
    //
    // Scene layout (X axis):
    //   Left wall inner face  ~  X = -1.5
    //   Left wall outer face  ~  X = -4.5  (wall thickness = 1 unit * scale 1)
    //   Window is cut into the left wall at X = -0.9 (inner face side)
    //
    // To appear OUTSIDE the window the moon must be at X < -4.5 so it sits
    // in the "sky" beyond the exterior of the building.
    // We place it at X = -7.0 (well outside), Y = 3.5 (above window centre),
    // Z = 9.35 (centred on the window opening which spans Z 8.9 -> 9.8).
    //
    // Slide animation moves the moon along Z so it drifts across the window.
    // Rotate animation spins it on its own Y axis in place.
    // -------------------------------------------------------------------------

    glPushMatrix();

    if (moonAnimMode == 0) {
        // Mode 0: moon sits centred outside the window and rotates on its axis
        glTranslatef(-7.0f, 3.5f, 9.35f);
    }
    else {
        // Mode 1: moon slides along Z (drifts across the window view)
        // moonPosX is repurposed here as the Z-slide offset
        glTranslatef(-7.0f, 3.5f, moonPosX);
    }

    // -- Phase ----------------------------------------------------------------
    switch (moonPhase) {
    case 0:
        // Full moon - rotates on its own axis
        glRotatef(moonAngle, 0, 1, 0);
        drawMoonSphere(1.2f);   // larger radius so it is clearly visible
        break;

    case 1:
        // Half moon - sphere + occluder (occluder must NOT rotate)
        glPushMatrix();
        glRotatef(moonAngle, 0, 1, 0);
        drawMoonSphere(1.2f);
        glPopMatrix();
        drawMoonOccluder(1.25f, 0.62f);
        break;

    case 2:
        // Crescent - sphere + larger-offset occluder
        glPushMatrix();
        glRotatef(moonAngle, 0, 1, 0);
        drawMoonSphere(1.2f);
        glPopMatrix();
        drawMoonOccluder(1.25f, 0.45f);
        break;
    }

    glPopMatrix();
}

// -----------------------------------------------------------------------------
//  WINDOW  (updated to call new drawMoon)
// -----------------------------------------------------------------------------
void window()
{
    // Window pane - sky blue by day, deep night blue at night
    glPushMatrix();
    glTranslatef(-0.9f, 1, 8.9f);
    glScalef(0.0001f, 0.6f, 0.3f);
    if (nightMode) {
        drawCube1(0.05f, 0.05f, 0.18f, 0.03f, 0.03f, 0.09f);
    }
    else {
        drawCube1(0.53f, 0.81f, 0.98f, 0.27f, 0.41f, 0.49f);
    }
    glPopMatrix();

    // Frame pieces
    glPushMatrix();
    glTranslatef(-0.9f, 1, 8.9f);
    glScalef(0.04f, 0.6f, 0.0001f);
    drawCube1(0.8f, 0.65f, 0.45f, 0.4f, 0.33f, 0.23f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.9f, 1, 9.8f);
    glScalef(0.04f, 0.6f, 0.0001f);
    drawCube1(0.8f, 0.65f, 0.45f, 0.4f, 0.33f, 0.23f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.7f, 2.7f, 8.9f);
    glScalef(0.0001f, 0.05f, 0.4f);
    drawCube1(0.75f, 0.62f, 0.42f, 0.38f, 0.31f, 0.21f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.8f, 1.02f, 8.9f);
    glScalef(0.0001f, 0.02f, 0.34f);
    drawCube1(0.75f, 0.62f, 0.42f, 0.38f, 0.31f, 0.21f);
    glPopMatrix();

    // Window bars
    glPushMatrix();
    glTranslatef(-0.87f, 2.1f, 8.9f);
    glScalef(0.0001f, 0.02f, 0.3f);
    drawCube1(0.1f, 0.1f, 0.1f, 0.05f, 0.05f, 0.05f, 5);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.87f, 1.6f, 8.9f);
    glScalef(0.0001f, 0.02f, 0.3f);
    drawCube1(0.1f, 0.1f, 0.1f, 0.05f, 0.05f, 0.05f, 5);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.87f, 1, 9.3f);
    glScalef(0.0001f, 0.6f, 0.02f);
    drawCube1(0.1f, 0.1f, 0.1f, 0.05f, 0.05f, 0.05f, 5);
    glPopMatrix();
    // drawMoon() is called from display() in world space, NOT here,
    // so the moon is correctly positioned OUTSIDE the building.
}

void sphericalObject()
{
    // Table top - warm amber
    glPushMatrix();
    glTranslatef(5, 0.2f, 10);
    glScalef(0.1f, 0.02f, 0.1f);
    drawSphere(0.65f, 0.35f, 0.10f, 0.33f, 0.18f, 0.05f, 40);
    glPopMatrix();

    // Table leg - dark brown
    glPushMatrix();
    glTranslatef(4.98f, -0.1f, 10);
    glScalef(0.02f, 0.1f, 0.02f);
    drawCube1(0.30f, 0.15f, 0.05f, 0.15f, 0.08f, 0.03f);
    glPopMatrix();

    // Base
    glPushMatrix();
    glTranslatef(5, -0.1f, 10);
    glScalef(0.05f, 0.01f, 0.05f);
    drawSphere(0.65f, 0.35f, 0.10f, 0.33f, 0.18f, 0.05f, 40);
    glPopMatrix();
}

// -----------------------------------------------------------------------------
//  LIGHT BULB VISUALS
// -----------------------------------------------------------------------------

void lightBulb1()
{
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_diffuse[] = { 1.000f, 0.843f, 0.000f, 1.0f };
    GLfloat high_shininess[] = { 100.0f };
    //  warm emission when on; use a local pointer to avoid ternary-array issue
    GLfloat mat_emission[] = { 1.0f, 0.95f, 0.7f, 1.0f };
    GLfloat* emitPtr = switchOne ? mat_emission : no_mat;

    glPushMatrix();
    glTranslatef(5, 5, 8);
    glScalef(0.2f, 0.2f, 0.2f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, emitPtr);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();
}

void lightBulb2()
{
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_diffuse[] = { 1.000f, 0.843f, 0.000f, 1.0f };
    GLfloat high_shininess[] = { 100.0f };
    GLfloat mat_emission[] = { 1.0f, 0.95f, 0.7f, 1.0f };
    GLfloat* emitPtr = switchTwo ? mat_emission : no_mat;

    glPushMatrix();
    glTranslatef(0, 5, 8);
    glScalef(0.2f, 0.2f, 0.2f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, emitPtr);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();
}

//  lamp bulb - warm amber glow with flicker
void lightBulb3()
{
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_diffuse[] = { 1.000f, 0.843f, 0.000f, 1.0f };
    GLfloat high_shininess[] = { 100.0f };

    // Flicker applied to emission intensity
    GLfloat mat_emission[] = { 1.0f * lampFlicker, 0.75f * lampFlicker, 0.2f * lampFlicker, 1.0f };
    GLfloat* emitPtr = switchLamp ? mat_emission : no_mat;

    glPushMatrix();
    glTranslatef(0.7f, 1.5f, 9.0f);
    glScalef(0.2f, 0.2f, 0.2f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, emitPtr);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();
}


// -----------------------------------------------------------------------------
//  LIGHTING SETUP
// -----------------------------------------------------------------------------

void lightOne()
{
    glPushMatrix();
    GLfloat no_light[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    //  night mode dims ceiling lights to 30% to simulate darkness
    GLfloat dimFactor = nightMode ? 0.30f : 1.0f;
    GLfloat light_ambient[] = { 0.5f * dimFactor, 0.5f * dimFactor, 0.45f * dimFactor, 1.0f };
    GLfloat light_diffuse[] = { 1.0f * dimFactor, 0.95f * dimFactor, 0.85f * dimFactor, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_position[] = { 5.0f, 5.0f, 8.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, (amb1 ? light_ambient : no_light));
    glLightfv(GL_LIGHT0, GL_DIFFUSE, (diff1 ? light_diffuse : no_light));
    glLightfv(GL_LIGHT0, GL_SPECULAR, (spec1 ? light_specular : no_light));
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glPopMatrix();
}

void lightTwo()
{
    glPushMatrix();
    GLfloat no_light[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat dimFactor = nightMode ? 0.30f : 1.0f;
    GLfloat light_ambient[] = { 0.5f * dimFactor, 0.5f * dimFactor, 0.45f * dimFactor, 1.0f };
    GLfloat light_diffuse[] = { 1.0f * dimFactor, 0.95f * dimFactor, 0.85f * dimFactor, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_position[] = { 0.0f, 5.0f, 8.0f, 1.0f };

    glLightfv(GL_LIGHT1, GL_AMBIENT, (amb2 ? light_ambient : no_light));
    glLightfv(GL_LIGHT1, GL_DIFFUSE, (diff2 ? light_diffuse : no_light));
    glLightfv(GL_LIGHT1, GL_SPECULAR, (spec2 ? light_specular : no_light));
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glPopMatrix();
}

void lampLight()
{
    glPushMatrix();
    GLfloat no_light[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    //  lamp is brighter at night (nightMode boosts it) and flickers
    GLfloat nightBoost = nightMode ? 1.5f : 1.0f;
    GLfloat light_ambient[] = { 0.4f * nightBoost * lampFlicker, 0.35f * nightBoost * lampFlicker, 0.20f * nightBoost * lampFlicker, 1.0f };
    GLfloat light_diffuse[] = { 1.0f * nightBoost * lampFlicker, 0.80f * nightBoost * lampFlicker, 0.40f * nightBoost * lampFlicker, 1.0f };
    GLfloat light_specular[] = { 1.0f, 0.9f, 0.6f, 1.0f };
    GLfloat light_position[] = { 0.7f, 1.5f, 9.0f, 1.0f };

    glLightfv(GL_LIGHT2, GL_AMBIENT, (amb3 ? light_ambient : no_light));
    glLightfv(GL_LIGHT2, GL_DIFFUSE, (diff3 ? light_diffuse : no_light));
    glLightfv(GL_LIGHT2, GL_SPECULAR, (spec3 ? light_specular : no_light));
    glLightfv(GL_LIGHT2, GL_POSITION, light_position);

    GLfloat spot_direction[] = { 0.3f, -1, -0.8f };
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 35.0f);
    glPopMatrix();
}

// -----------------------------------------------------------------------------
//  DISPLAY
// -----------------------------------------------------------------------------

void display(void)
{
    // night mode sets a deep navy background; day uses a light sky tone
    if (nightMode) {
        glClearColor(0.02f, 0.02f, 0.06f, 1.0f);
    }
    else {
        glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, refX, refY, refZ, 0, 1, 0);

    glEnable(GL_LIGHTING);
    lightOne();
    lightTwo();
    lampLight();

    room();
    bed();
    bedsideDrawer();
    lamp();
    LinkinParkPoster();
    wallshelf();
    wardrobe();
    cupboard();
    dressingTable();
    Clock();
    window();
    drawMoon();         // moon drawn in world space OUTSIDE the building
    sphericalObject();
    lightBulb1();
    lightBulb2();
    lightBulb3();   //  lamp bulb now always drawn (emission only when on)

    glDisable(GL_LIGHTING);

    glFlush();
    glutSwapBuffers();
}

// -----------------------------------------------------------------------------
//  KEYBOARD
// -----------------------------------------------------------------------------

void myKeyboardFunc(unsigned char key, int x, int y)
{
    switch (key)
    {
        // -- Wall color cycling (8 presets) -----
    case 'c':
        colorIndex = (colorIndex % 8) + 1;   // cycles 1-8
        roomColorR = wallPresets[colorIndex][0];
        roomColorG = wallPresets[colorIndex][1];
        roomColorB = wallPresets[colorIndex][2];
        break;

        // -- Floor color cycling (4 presets) -----------
        // 'f' cycles through wood brown, dark brown, marble white, slate gray
    case 'f':
        floorIndex = (floorIndex % 4) + 1;
        break;

        // -- Night mode toggle -----------
    case 'y':
        nightMode = !nightMode;
        break;

        // -- Moon animation mode toggle ------------
       
    case 'k':
        moonAnimMode = (moonAnimMode + 1) % 2;
        moonPosX = 8.0f;   // reset to left edge of window Z range when switching
        break;

        // -- Moon phase cycling ------------
        // 'p' cycles full -> half -> crescent -> full
    case 'p':
        moonPhase = (moonPhase + 1) % 3;
        break;

        // -- Camera eye point -------------
    case 'w': eyeY += 1.0; break;
    case 's': eyeY -= 1.0; break;
    case 'a': eyeX -= 1.0; break;
    case 'd': eyeX += 1.0; break;
    case 'o': eyeZ += 1;   break;
    case 'i': eyeZ -= 1;   break;
    case 'q':
        eyeX = 7.0; eyeY = 2.0; eyeZ = 15.0;
        refX = 0.0; refY = 0.0; refZ = 0.0;
        break;

        // -- Camera reference point ----------------
        // 'k' is used for moon animation toggle above, so refZ forward uses 'u'
    case 'j': refY += 1.0; break;
    case 'n': refY -= 1.0; break;
    case 'b': refX -= 1.0; break;
    case 'm': refX += 1.0; break;
    case 'u': refZ += 1;   break;   // move ref point away (was 'k', reassigned to avoid conflict)
    case 'l': refZ -= 1;   break;

        // -- Light source 1 -------
    case '1':
        if (!switchOne) { switchOne = true;  amb1 = true;  diff1 = true;  spec1 = true;  glEnable(GL_LIGHT0); }
        else { switchOne = false; amb1 = false; diff1 = false; spec1 = false; glDisable(GL_LIGHT0); }
        break;
    case '4': amb1 = !amb1;  break;
    case '5': diff1 = !diff1; break;
    case '6': spec1 = !spec1; break;

        // -- Light source 2 ---------
    case '2':
        if (!switchTwo) { switchTwo = true;  amb2 = true;  diff2 = true;  spec2 = true;  glEnable(GL_LIGHT1); }
        else { switchTwo = false; amb2 = false; diff2 = false; spec2 = false; glDisable(GL_LIGHT1); }
        break;
    case '7': amb2 = !amb2;  break;
    case '8': diff2 = !diff2; break;
    case '9': spec2 = !spec2; break;

        // -- Lamp light ----------
    case '3':
        if (!switchLamp) { switchLamp = true;  amb3 = true;  diff3 = true;  spec3 = true;  glEnable(GL_LIGHT2); }
        else { switchLamp = false; amb3 = false; diff3 = false; spec3 = false; glDisable(GL_LIGHT2); }
        break;
    case 'e': amb3 = !amb3;  break;
    case 'r': diff3 = !diff3; break;
    case 't': spec3 = !spec3; break;

    case 27: exit(1); break;  
    }

    glutPostRedisplay();
}

// -----------------------------------------------------------------------------
//  ANIMATION  (idle callback)
// -----------------------------------------------------------------------------

void animate()
{
    // -- Moon animation ------------
    if (nightMode) {
        if (moonAnimMode == 0) {
            // Rotate on axis (moon stays centred outside window)
            moonAngle += 0.3f;
            if (moonAngle > 360.0f) moonAngle = 0.0f;
        }
        else {
            // Slide moon along Z so it drifts across the window opening.
            // Window spans Z = 8.9 to 9.8; give a little margin either side.
            moonPosX += 0.004f;
            if (moonPosX > 10.5f) moonPosX = 8.0f;  // wrap: re-enter from left
        }
    }

    // -- Lamp flicker (night mode only, lamp must be on) ----
    // subtle random-ish flicker using a sine wave with fast frequency
    if (nightMode && switchLamp) {
        flickerTick++;

        // Combine two sine waves for a more organic flicker
        lampFlicker = 0.88f
            + 0.07f * (GLfloat)sin(flickerTick * 0.31f)
            + 0.05f * (GLfloat)sin(flickerTick * 0.73f);
    }
    else {
        lampFlicker = 1.0f;   // steady when not flickering
    }

    // -- Clock pendulum --------
    if (redFlag == true)
    {
        theta += 2;
        z -= 0.02;
        if (theta >= 196 && theta <= 210) y = 1.44;
        else if (theta >= 180 && theta <= 194) y = 1.42;
        else if (theta >= 164 && theta <= 178) y = 1.42;
        if (theta == 210) redFlag = false;
    }
    else
    {
        theta -= 2;
        z += 0.02;
        if (theta >= 196 && theta <= 210) y = 1.44;
        else if (theta >= 180 && theta <= 194) y = 1.42;
        else if (theta >= 164 && theta <= 178) y = 1.42;
        if (theta == 150) redFlag = true;
    }

    glutPostRedisplay();
}

// -----------------------------------------------------------------------------
//  RESHAPE
// -----------------------------------------------------------------------------

void fullScreen(int w, int h)
{
    if (h == 0) h = 1;
    float ratio = (GLfloat)w / (GLfloat)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(60, ratio, 1, 500);
    glMatrixMode(GL_MODELVIEW);
}

// ----------------
//  MAIN
// ----------------

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    // Initialise wall color from preset 1
    roomColorR = wallPresets[colorIndex][0];
    roomColorG = wallPresets[colorIndex][1];
    roomColorB = wallPresets[colorIndex][2];

    std::cout << "=== BEDROOM SCENE CONTROLS ===" << std::endl;
    std::cout << std::endl;
    std::cout << "CAMERA (Eye point):" << std::endl;
    std::cout << "  w/s  - up/down    a/d - left/right" << std::endl;
    std::cout << "  i/o  - zoom in/out    q - reset" << std::endl;
    std::cout << std::endl;
    std::cout << "CAMERA (Reference point):" << std::endl;
    std::cout << "  j/n  - up/down    b/m - left/right    l/u - near/far" << std::endl;
    std::cout << std::endl;
    std::cout << "COLORS:" << std::endl;
    std::cout << "  c - cycle wall color  (8 presets: warm, blue, green, purple," << std::endl;
    std::cout << "                         orange, pink, teal, linen)" << std::endl;
    std::cout << "  f - cycle floor color (4 presets: wood, dark wood, marble, gray)" << std::endl;
    std::cout << std::endl;
    std::cout << "NIGHT MODE & MOON:" << std::endl;
    std::cout << "  y - toggle night mode" << std::endl;
    std::cout << "  k - toggle moon animation (rotate / slide)" << std::endl;
    std::cout << "  p - cycle moon phase (full / half / crescent)" << std::endl;
    std::cout << std::endl;
    std::cout << "LIGHTING:" << std::endl;
    std::cout << "  1 - toggle ceiling light 1    4/5/6 - amb/diff/spec of light 1" << std::endl;
    std::cout << "  2 - toggle ceiling light 2    7/8/9 - amb/diff/spec of light 2" << std::endl;
    std::cout << "  3 - toggle lamp               e/r/t - amb/diff/spec of lamp" << std::endl;
    std::cout << std::endl;
    std::cout << "  ESC - quit" << std::endl;

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize((int)windowHeight, (int)windowWidth);
    glutCreateWindow("Bedroom Scene - Enhanced");

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glutReshapeFunc(fullScreen);
    glutDisplayFunc(display);
    glutKeyboardFunc(myKeyboardFunc);
    glutIdleFunc(animate);
    glutMainLoop();

    return 0;
}
