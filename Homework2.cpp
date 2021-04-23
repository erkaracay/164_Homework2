/*********
CTIS164 - Template Source Program
----------
STUDENT : Suphi Erkin Karaçay
SECTION : 002
HOMEWORK: #2
----------
PROBLEMS:
o Color changing on intersections works, but in a really weird way that I could not resolve.
----------
ADDITIONAL FEATURES:
o Press F4 to start drawing a big line strip, press F4 to stop/continue
o Pressing F1, F2, F3, F4 while the mode is active deactivates it, pressing any one of them again lets you
continue in chosen mode
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT  749

#define TIMER_PERIOD  1000 // Period for the timer.
#define TIMER_ON         0 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

//Modes
#define F0	 0
#define F1   1
#define F2   2
#define F3   3
#define F4	 4

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight;  // current Window width and height

typedef struct {
    float X, Y;
    bool pS;
}point_t;

typedef struct {
    point_t start, end;
}line_t;

int mode = F1;             //Modes of the program
int Sx, Sy;                //Start x, Start y
int Ex, Ey;                //End x, End y
bool start = false, end = false;
int r = 1, q = 1;

line_t SE;
point_t points[41], points2[31];

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r) {
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r) {
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font) {
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

void drawLineS(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void drawLineStrips(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINE_STRIP);
    for (int q = 0; q < 15; q++) {
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    }
    glEnd();
}

void drawGrid() {
    glColor3f(0.25, 0, 0.55);
    for (int q = -winWidth / 2 / 50 * 50; q <= winWidth / 2; q += 50)
        drawLineS(q, -winHeight / 2, q, winHeight / 2);

    glColor3f(0, 0, 0);
    for (int q = -winHeight / 2 / 50 * 50; q <= winHeight / 2; q += 50)
        drawLineS(-winWidth / 2, q, winWidth / 2, q);
}

bool calcLines(float x3, float y3, float x4, float y4) {
    float denom = ((x4 - x3) * (SE.end.Y - SE.start.Y)) - ((SE.end.X - SE.start.X) * (y4 - y3)),
        numer1 = ((x4 - x3) * (y3 - SE.start.Y)) - ((x3 - SE.start.X) * (y4 - y3)),
        t1 = numer1 / denom;

    float numer2 = ((SE.end.X - SE.start.X) * (y3 - SE.start.Y)) - ((x3 - SE.start.X) * (SE.end.Y - SE.start.X)),
        t2 = numer2 / denom;

    if ((t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1))
        return true;  //Intersecting
    else
        return false; //Not Intersecting
}

void drawBars() {
    glColor3ub(102, 0, 51);
    glRectf(-winWidth / 2, winHeight / 2, winWidth / 2, winHeight / 2 - 25);   //Top Bar
    glRectf(-winWidth / 2, -winHeight / 2, winWidth / 2, -winHeight / 2 + 25); //Bottom Bar

    glColor3f(1, 1, 1);
    //Top Bar Text
    vprint(-winWidth / 2 + 5, winHeight / 2 - 15, GLUT_BITMAP_9_BY_15, "Suphi Erkin Karacay - 22003597");
    vprint(-120, winHeight / 2 - 15, GLUT_BITMAP_9_BY_15, "HW #2 - 2020-2021 Spring");
    //Bottom Bar Text
    vprint(-winWidth / 2 + 20, -winHeight / 2 + 8, GLUT_BITMAP_9_BY_15, "F1: Set Start Position");
    vprint(-winWidth / 2 + 250, -winHeight / 2 + 8, GLUT_BITMAP_9_BY_15, "F2: Set End Position");
    vprint(-winWidth / 2 + 460, -winHeight / 2 + 8, GLUT_BITMAP_9_BY_15, "F3: Add Lines");
    vprint(-winWidth / 2 + 610, -winHeight / 2 + 8, GLUT_BITMAP_9_BY_15, "F4: Add Line Strips");


    glColor4f(1, 1, 0, 0.5);
    switch (mode) {
    case F1: circle(-winWidth / 2 + 27, -winHeight / 2 + 12, 12);
        break;
    case F2: circle(-winWidth / 2 + 258, -winHeight / 2 + 12, 12);
        break;
    case F3: circle(-winWidth / 2 + 469, -winHeight / 2 + 12, 12);
        break;
    case F4: circle(-winWidth / 2 + 619, -winHeight / 2 + 12, 12);
        break;
    }

}

void drawStartEnd() {
    glColor3ub(40, 40, 40);
    glLineWidth(5);
    if (start && end)
        drawLineS(SE.start.X, SE.start.Y, SE.end.X, SE.end.Y);

    if (start) {
        glColor3f(.45, .05, .45);
        circle(SE.start.X, SE.start.Y, 35);
        glLineWidth(2);
        glColor3f(0, 0, 0);
        vprint2(SE.start.X - 18, SE.start.Y - 17, 0.39, "S");
        glLineWidth(3);
        circle_wire(SE.start.X, SE.start.Y, 35);
    }
    if (end) {
        glColor3f(.55, .05, .25);
        circle(SE.end.X, SE.end.Y, 35);
        glColor3f(0, 0, 0);
        glLineWidth(2);
        vprint2(SE.end.X - 18, SE.end.Y - 17, 0.39, "E");
        glLineWidth(3);
        circle_wire(SE.end.X, SE.end.Y, 35);
    }
    glLineWidth(1);
    glutPostRedisplay();
}

void addLines() {
    for (int r = 1; r <= 41; r++) {
        if (!calcLines(points[r - 1].X, points[r - 1].Y, points[r].X, points[r].Y))
            glColor4ub(0, 193, 96, 128);
        else
            glColor4ub(255, 0, 0, 128);
        glLineWidth(4);
        if (points[r].pS && r > 1) {
            if (r % 2 != 0 && r > 1)
                drawLineS(points[r - 1].X, points[r - 1].Y, points[r].X, points[r].Y);
            glColor3f(0, 0, 0);
            if (r > 1)
                circle(points[r].X, points[r].Y, 6);
        }
    }
    glLineWidth(1);
    glutPostRedisplay();
}

void addlineStrips() {
    bool yes = true;
    for (int q = 1; q <= 31; q++) {
        glColor4ub(153, 35, 105, 128);
        glLineWidth(5);
        if (points2[q].pS) {
            if (q > 2)
                drawLineStrips(points2[q - 1].X, points2[q - 1].Y, points2[q].X, points2[q].Y);
            glColor3f(0, 0, 0);
            if (q > 1)
                circle(points2[q].X, points2[q].Y, 6);
            if (q == 31)
                yes = false;
        }
    }
    if (!yes) {
        glColor4ub(153, 35, 105, 255);
        drawLineS(points2[2].X, points2[2].Y, points2[31].X, points2[31].Y);
    }
    glLineWidth(1);
    glutPostRedisplay();
}

//
// To display onto window using OpenGL commands
//
void display() {
    //
    // clear window to black
    //
    glClearColor(124 / 256., 124 / 256., 124 / 256., 1);
    glClear(GL_COLOR_BUFFER_BIT);

    drawGrid();
    drawBars();
    drawStartEnd();
    addLines();
    addlineStrips();

    glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y) {
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y) {
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y) {
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    }

    switch (key) {
    case GLUT_KEY_F1: if (!(mode == F1)) mode = F1;
                    else mode = F0; break;
    case GLUT_KEY_F2: if (!(mode == F2)) mode = F2;
                    else mode = F0; break;
    case GLUT_KEY_F3: if (!(mode == F3)) mode = F3;
                    else mode = F0; break;
    case GLUT_KEY_F4: if (!(mode == F4)) mode = F4;
                    else mode = F0; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y) {
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}


void onClick(int button, int stat, int x, int y) {
    // Write your codes here.
    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && mode == F1) {
        SE.start.X = x - winWidth / 2;
        SE.start.Y = winHeight / 2 - y;
        start = true;
    }

    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && mode == F2) {
        SE.end.X = x - winWidth / 2;
        SE.end.Y = winHeight / 2 - y;
        end = true;
    }

    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && mode == F3) {
        points[r].X = x - winWidth / 2;
        points[r].Y = winHeight / 2 - y;
        points[r].pS = true;

        r++;
    }

    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && mode == F4) {
        points2[q].X = x - winWidth / 2;
        points2[q].Y = winHeight / 2 - y;
        points2[q].pS = true;

        q++;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h) {
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("L.S. Intersections on Grid");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}
