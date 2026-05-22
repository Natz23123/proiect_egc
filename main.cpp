#include <GL/freeglut.h>
#include "Player.h"
#include "Assets.h"
#include "Building.h"
#include "Street.h"
#include <ctype.h>

// --- VARIABILE GLOBALE ---
float averageHeight = 3.5f;
float floorHeight = averageHeight * 4; // Etajul 4 (4 etaje * 3m)
float eyeLevel = 1.7f;
int lastCigaretteToggleTime = 0; // Memorează când s-a apăsat ultima dată tasta E
float backLimit = -0.5f, frontLimit = 3.0f, leftLimit = -3.5f, rightLimit = 1.0f; // Marimea Balconului
Player player(0.0f, floorHeight + eyeLevel, 1.0f); // Pornim de pe balcon

float timeOfDay = 0.0f;
float timePass = 0.0015f;

Building* blocMuncitoresc;
Building* turnCiment;

int screenW = 1160, screenH = 600;
bool keys[256];

void initKeys() {
    for (int i = 0; i < 256; i++) keys[i] = false;
}

GLuint texBeton, texFloor, texCeiling, texClouds;
GLuint texWall1, texWall2, texWall3, texWall4;
GLuint texEntrance;
GLuint texRoof;

Street* stradaPrincipala;
Street* stradaSecundara;
GLuint texDrum;

void init() {
    glClearColor(0.55f, 0.7f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING); // !!! ACTIVARE CORECTĂ A ILUMINĂRII GLOBALE
    initKeys();

    texBeton = loadTexture("ceiling.bmp");
    texCeiling = loadTexture("ceiling.bmp");
    texFloor = loadTexture("floor.bmp");
    texClouds = loadSkyTexture("clouds.ppm");

    srand(time(NULL));

    texWall1 = loadTexture("face1.bmp");
    texWall2 = loadTexture("face2.bmp");
    texWall3 = loadTexture("face3.bmp");
    texWall4 = loadTexture("face4.bmp");
    texRoof = loadTexture("wall.bmp");

    GLuint wallPool[4] = { texWall1, texWall2, texWall3, texWall4 };
    texEntrance = loadTexture("intrare1.bmp");

    blocMuncitoresc = new Building(-11.0f, -15.0f, 12.0f, 12.0f, 10, averageHeight, wallPool, 4, texEntrance, texRoof);

    texDrum = loadTexture("asphalt.bmp");
    stradaPrincipala = new Street(0.0f, -40.0f, 80.0f, 6.0f, false, texDrum);
    stradaSecundara = new Street(0.0f, -50.0f, 50.0f, 6.0f, true, texDrum);
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

    // day-night cycle

    static float angle = 0.0f;
    angle += timePass;

    timeOfDay = (sin(angle) + 1.0f) / 2.0f;

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void display() {
    // === 1. SCHIMBĂM CULOAREA DE FUNDAL (ORIZONTUL) ===
    float skyR = 0.02f + 0.53f * timeOfDay;
    float skyG = 0.02f + 0.68f * timeOfDay;
    float skyB = 0.05f + 0.95f * timeOfDay;
    glClearColor(skyR, skyG, skyB, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(player.x, player.y, player.z,
        player.x + player.lookX, player.y + player.lookY, player.z + player.lookZ,
        0.0f, 1.0f, 0.0f);

    // === 2. DESENARE CER COORDONAT CU TIMPUL ===
    glPushMatrix();
    glTranslatef(player.x, player.y, player.z);

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D); // Ne asigurăm că texturile sunt pornite pentru cer

    // !!! FORȚĂM OPENGL SĂ COMBINE CULOAREA GLCOLOR CU TEXTURA NORILOR !!!
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Noaptea = 0.15 (semi-întuneric brutalist), Ziua = 1.0 (lumină maximă)
    float skyTone = 0.15f + 0.85f * timeOfDay;
    glColor3f(skyTone, skyTone, skyTone);

    drawSky(texClouds);

    glEnable(GL_LIGHTING);
    glPopMatrix();

    // === 3. COORDONARE LUMINĂ AMBIENTALĂ BLOCURI/STRADĂ ===
    float minAmbient = 0.15f;
    float maxAmbient = 0.90f;
    float currentAmbient = minAmbient + (maxAmbient - minAmbient) * timeOfDay;

    GLfloat ambientColor[] = { currentAmbient, currentAmbient, currentAmbient + (0.05f * timeOfDay), 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	drawGround();

    if (stradaPrincipala) stradaPrincipala->draw();
    if (stradaSecundara)  stradaSecundara->draw();

    if (blocMuncitoresc) 
        blocMuncitoresc->draw();
    if (turnCiment)
        turnCiment->draw();

    

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