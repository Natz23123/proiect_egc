#include <GL/freeglut.h>
#include <ctype.h>
#include <vector>
#include <math.h>
#include "Player.h"
#include "Assets.h"
#include "Building.h"
#include "Street.h"

// --- VARIABILE GLOBALE ---
float averageHeight = 3.5f;
float floorHeight = averageHeight * 4;
float eyeLevel = 1.7f;
int lastCigaretteToggleTime = 0;
float backLimit = -0.5f, frontLimit = 3.0f, leftLimit = -3.5f, rightLimit = 1.0f;
Player player(0.0f, floorHeight + eyeLevel, 1.0f);

float timeOfDay = 0.0f;
float timePass = 0.0015f;

std::vector<Building*> cartier;
std::vector<Street*> strazi;
int screenW = 1160, screenH = 600;
bool keys[256];

GLuint texBeton, texFloor, texCeiling, texClouds, texGround;
GLuint texWall1, texWall2, texWall3, texWall4, texEntrance, texRoof, texDrum;
GLuint wallPool[4];
GLuint catTexID;

void initKeys() {
    for (int i = 0; i < 256; i++) keys[i] = false;
}

void init_city() {
    cartier.clear(); strazi.clear();
    strazi.push_back(new Street(-25.0f, -200.0f, 200.0f, 12.0f, false, texDrum));
    strazi.push_back(new Street(-20.0f, -6.0f, 73.0f, 12.0f, true, texDrum));

    cartier.push_back(new Building(-5.0f, -22.0f, 14.0f, 14.0f, 10, averageHeight, wallPool, 4, texEntrance, texRoof));
    cartier.push_back(new Building(3.0f, -32.0f, 14.0f, 14.0f, 7, averageHeight, wallPool, 4, texEntrance, texRoof));
    cartier.push_back(new Building(12.0f, -23.0f, 14.0f, 14.0f, 12, averageHeight, wallPool, 4, texEntrance, texRoof));
    cartier.push_back(new Building(29.0f, -21.0f, 16.0f, 14.0f, 14, averageHeight, wallPool, 4, texEntrance, texRoof));
    cartier.push_back(new Building(50.0f, -22.5f, 16.0f, 14.0f, 10, averageHeight, wallPool, 4, texEntrance, texRoof));
    cartier.push_back(new Building(70.0f, -10.5f, 16.0f, 14.0f, 12, averageHeight, wallPool, 4, texEntrance, texRoof));
    cartier.push_back(new Building(-42.0f, -10.0f, 14.0f, 14.0f, 9, averageHeight, wallPool, 4, texEntrance, texRoof));
    cartier.push_back(new Building(-42.0f, -30.0f, 14.0f, 14.0f, 7, averageHeight, wallPool, 4, texEntrance, texRoof));
    cartier.push_back(new Building(-42.0f, -50.0f, 14.0f, 14.0f, 11, averageHeight, wallPool, 4, texEntrance, texRoof));
    cartier.push_back(new Building(-5.0f, -48.0f, 14.0f, 14.0f, 13, averageHeight, wallPool, 4, texEntrance, texRoof));
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    initKeys();

    // Încarcă texturi
    texBeton = loadTexture("ceiling.bmp"); texCeiling = loadTexture("ceiling.bmp");
    texFloor = loadTexture("floor.bmp");   texClouds = loadSkyTexture("clouds.ppm");
    texWall1 = loadTexture("face1.bmp");   texWall2 = loadTexture("face2.bmp");
    texWall3 = loadTexture("face3.bmp");   texWall4 = loadTexture("face4.bmp");
    texRoof = loadTexture("wall.bmp");     texEntrance = loadTexture("intrare1.bmp");
    texDrum = loadTexture("asphalt.bmp");  texGround = loadTexture("grass.bmp");
    catTexID = loadBMPWithChromaKey("pufarina.bmp");

    wallPool[0] = texWall1; wallPool[1] = texWall2; wallPool[2] = texWall3; wallPool[3] = texWall4;
    init_city();

    // CONFIGURARE SPOTLIGHTS STÂLPI
    GLfloat lampDiffuse[] = { 1.0f, 0.85f, 0.5f, 1.0f };
    GLfloat lampAmbient[] = { 0.05f, 0.05f, 0.02f, 1.0f };
    GLfloat spotDirection[] = { 0.0f, -1.0f, 0.0f };

    // Stâlp 1
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lampDiffuse);
    glLightfv(GL_LIGHT2, GL_AMBIENT, lampAmbient);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spotDirection);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 25.0f);
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 50.0f);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.05f);

    // Stâlp 2
    glLightfv(GL_LIGHT3, GL_DIFFUSE, lampDiffuse);
    glLightfv(GL_LIGHT3, GL_AMBIENT, lampAmbient);
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spotDirection);
    glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 25.0f);
    glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 50.0f);
    glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0.05f);

    glDisable(GL_LIGHT2); glDisable(GL_LIGHT3);
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    screenW = w; screenH = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int v) {
    float oldX = player.x, oldZ = player.z;
    float mSpeed = player.speed;

    if (keys['w']) player.moveForward(mSpeed);
    if (keys['s']) player.moveForward(-mSpeed);
    if (keys['a']) player.moveRight(-mSpeed);
    if (keys['d']) player.moveRight(mSpeed);

    if (keys['e']) {
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        if (currentTime - lastCigaretteToggleTime > 300) {
            if (!player.hasCigarette) {
                float cigX = 0.0f, cigZ = backLimit + 0.06f;
                if (sqrt((player.x - cigX) * (player.x - cigX) + (player.z - cigZ) * (player.z - cigZ)) < 0.6f) {
                    player.hasCigarette = true;
                    lastCigaretteToggleTime = currentTime;
                }
            }
            else {
                player.hasCigarette = false;
                lastCigaretteToggleTime = currentTime;
            }
        }
        keys['e'] = false;
    }

    if (player.x < leftLimit + 0.2f || player.x > rightLimit - 0.2f) player.x = oldX;
    if (player.z < backLimit + 0.2f || player.z > frontLimit - 0.2f) player.z = oldZ;

    static float angle = 0.0f;
    angle += timePass;
    timeOfDay = (sin(angle) + 1.0f) / 2.0f;

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void display() {
    // Cer dinamic
    float skyR = 0.02f + 0.53f * timeOfDay, skyG = 0.02f + 0.68f * timeOfDay, skyB = 0.05f + 0.95f * timeOfDay;
    glClearColor(skyR, skyG, skyB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(player.x, player.y, player.z, player.x + player.lookX, player.y + player.lookY, player.z + player.lookZ, 0.0f, 1.0f, 0.0f);

    // Render Skybox
    glPushMatrix();
    glTranslatef(player.x, player.y, player.z);
    glDisable(GL_LIGHTING); glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    float skyTone = 0.15f + 0.85f * timeOfDay; glColor3f(skyTone, skyTone, skyTone);
    drawSky(texClouds);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Lumina Ambientală Globală
    float currentAmbient = 0.15f + 0.75f * timeOfDay;
    GLfloat ambientColor[] = { currentAmbient, currentAmbient, currentAmbient + (0.05f * timeOfDay), 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    // Activare Sursă Spotlights (Sincronizate cu stâlpii fizici de la Z = -14.0f)
    GLfloat light2_Pos[] = { -18.0f, 7.7f, -14.0f, 1.0f };
    glLightfv(GL_LIGHT2, GL_POSITION, light2_Pos);

    GLfloat light3_Pos[] = { 20.0f, 7.7f, -14.0f, 1.0f };
    glLightfv(GL_LIGHT3, GL_POSITION, light3_Pos);

    glEnable(GL_LIGHT2); glEnable(GL_LIGHT3);

    // Desenare Obiecte Scenă
    drawGround(texGround);
    drawStreetLamp(-18.0f, -14.0f);
    drawStreetLamp(20.0f, -14.0f);
    drawCity(strazi, cartier);
    drawBalcony(floorHeight, backLimit, frontLimit, leftLimit, rightLimit, texBeton, texFloor, texCeiling);
    drawBrutalistTable(-3.0f, -0.1f, floorHeight, texBeton);
    drawBrutalistChair(-3.0f, 0.7f, 180.0f, floorHeight, texBeton, texFloor);
    drawCatBillboard(-3.0f, floorHeight + 0.8f, 0.7f, catTexID);

    // Logica Țigară & Fum
    if (!player.hasCigarette) {
        drawCigaretteModel(0.0f, floorHeight + 1.26f, backLimit + 0.06f, 180.0f, true, false);
    }
    else {
        drawCigaretteModel(player, false);
    }
    renderSmoke(player.lookX, player.lookY, player.lookZ);

    glutSwapBuffers();

    // Curățare lumini cadru curent
    glDisable(GL_LIGHT1); glDisable(GL_LIGHT2); glDisable(GL_LIGHT3);
}

void keyboardDown(unsigned char key, int x, int y) { keys[tolower(key)] = true; if (key == 27) exit(0); }
void keyboardUp(unsigned char key, int x, int y) { keys[tolower(key)] = false; }

void mouseMotion(int x, int y) {
    int centerX = screenW / 2, centerY = screenH / 2;
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