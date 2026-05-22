#include <GL/freeglut.h>
#include "Player.h"
#include "Assets.h"
#include <ctype.h>

// --- VARIABILE GLOBALE ---
float floorHeight = 9.0f; // Etajul 4 (4 etaje * 3m)
float eyeLevel = 1.7f;
int lastCigaretteToggleTime = 0; // Memorează când s-a apăsat ultima dată tasta E
float backLimit = -0.5f, frontLimit = 3.0f, leftLimit = -3.5f, rightLimit = 1.0f; // Marimea Balconului
Player player(0.0f, floorHeight + eyeLevel, 1.0f); // Pornim de pe balcon

int screenW = 1160, screenH = 600;
bool keys[256];

void initKeys() {
    for (int i = 0; i < 256; i++) keys[i] = false;
}

GLuint texBeton, texFloor, texCeiling, texClouds;

void init() {
    glClearColor(0.55f, 0.7f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    initKeys();
    texBeton = loadTexture("wall.bmp");
    texCeiling = loadTexture("ceiling.bmp");
	texFloor = loadTexture("floor.bmp");
    texClouds = loadSkyTexture("clouds.ppm");
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    screenW = w; screenH = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int v) {
    float oldX = player.x, oldZ = player.z;
    float mSpeed = player.speed; // Viteza fixă pentru timer

    if (keys['w']) player.moveForward(mSpeed);
    if (keys['s']) player.moveForward(-mSpeed);
    if (keys['a']) player.moveRight(-mSpeed);
    if (keys['d']) player.moveRight(mSpeed);
    if (keys['e']) {
        int currentTime = glutGet(GLUT_ELAPSED_TIME); // Timpul curent în milisecunde

        // Verificăm dacă au trecut cel puțin 300 ms de la ultima apăsare
        if (currentTime - lastCigaretteToggleTime > 300) {

            if (!player.hasCigarette) {
                // --- LOGICA DE LUAT ȚIGARA ---
                float cigX = 0.0f;
                float cigZ = backLimit + 0.06f;

                float dx = player.x - cigX;
                float dz = player.z - cigZ;
                float distance = sqrt(dx * dx + dz * dz);

                if (distance < 1.5f) {
                    player.hasCigarette = true;
                    lastCigaretteToggleTime = currentTime; // Salvăm timpul curent
                }
            }
            else {
                // --- LOGICA DE LĂSAT ȚIGARA JOS ---
                player.hasCigarette = false;
                lastCigaretteToggleTime = currentTime; // Salvăm timpul curent
            }
        }

        keys['e'] = false; // Dezactivăm tasta pentru cadrul curent
    }
	// if (keys['shift']) player.y += 0.05; 

	// Coliziune simplă cu limitele balconului
    if (player.x < leftLimit+0.2 || player.x > rightLimit-0.2) player.x = oldX;
    if (player.z < backLimit+0.2 || player.z > frontLimit-0.2) player.z = oldZ;

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    gluLookAt(player.x, player.y, player.z,
        player.x + player.lookX, player.y + player.lookY, player.z + player.lookZ,
        0.0f, 1.0f, 0.0f);
    
    glPushMatrix();
	    glTranslatef(player.x, player.y, player.z);
	    drawSky(texClouds);
    glPopMatrix();

	drawGround();

    GLfloat ambientColor[] = { 1.0f, 0.9f, 0.7f, 0.6f }; // Un gri-albăstrui de noapte
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    drawBalcony(floorHeight,backLimit,frontLimit,leftLimit,rightLimit,texBeton, texFloor, texCeiling);

    if (!player.hasCigarette) {
        float cigaretteX = 0.0f;
        float cigaretteY = floorHeight + 1.26f;
        float cigaretteZ = backLimit+0.06;
        drawCigaretteModel(cigaretteX, cigaretteY, cigaretteZ, 180.0f, true, false);
    }
    else {
        drawCigaretteModel(player, false);
    }
    

    renderSmoke(player.lookX, player.lookY, player.lookZ);

    glutSwapBuffers();
}

void keyboardDown(unsigned char key, int x, int y) {
    keys[tolower(key)] = true;
    if (key == 27) exit(0);
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[tolower(key)] = false;
}

void mouseMotion(int x, int y) {
    int centerX = screenW / 2;
    int centerY = screenH / 2;
    if (x != centerX || y != centerY) {
        player.handleMouse(x, y, centerX, centerY);
        glutWarpPointer(centerX, centerY);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(screenW, screenH);
    glutCreateWindow("Brutalism Balcony");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);


    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);


    glutPassiveMotionFunc(mouseMotion);

    glutTimerFunc(16, timer, 0);

    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(screenW / 2, screenH / 2);

    glutMainLoop();
    return 0;
}