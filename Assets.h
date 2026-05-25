#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glut.h>
#include "utils.h"
#include <cstdlib>
#include <vector>
#include "Street.h"
#include "Building.h"
#include "Cigarette.h"

void drawBalcony(float base, float backLimit, float frontLimit, float leftLimit, float rightLimit, GLuint betonID, GLuint floorID, GLuint ceilID) {
    float wallH = 2.5f;
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    // Podea
    glBindTexture(GL_TEXTURE_2D, floorID); glColor3f(0.35f, 0.32f, 0.25f);
    glBegin(GL_QUADS); glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(leftLimit, base, backLimit);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(rightLimit, base, backLimit);
    glTexCoord2f(2.0f, 2.0f); glVertex3f(rightLimit, base, frontLimit);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(leftLimit, base, frontLimit); glEnd();

    // Tavan
    glBindTexture(GL_TEXTURE_2D, ceilID); glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS); glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(leftLimit, base + wallH, backLimit);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(rightLimit, base + wallH, backLimit);
    glTexCoord2f(2.0f, 2.0f); glVertex3f(rightLimit, base + wallH, frontLimit);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(leftLimit, base + wallH, frontLimit); glEnd();

    // Pereți Beton
    glBindTexture(GL_TEXTURE_2D, betonID);
    // Perete Stânga
    glBegin(GL_QUADS); glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(leftLimit, base, backLimit);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(leftLimit, base + wallH, backLimit);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(leftLimit, base + wallH, frontLimit);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(leftLimit, base, frontLimit); glEnd();

    // Perete Dreapta
    glBegin(GL_QUADS); glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(rightLimit, base, backLimit);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(rightLimit, base + wallH, backLimit);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(rightLimit, base + wallH, frontLimit);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(rightLimit, base, frontLimit); glEnd();

    // Perete Spate
    glBegin(GL_QUADS); glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(leftLimit, base, frontLimit);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(rightLimit, base, frontLimit);
    glTexCoord2f(2.0f, 1.0f); glVertex3f(rightLimit, base + wallH, frontLimit);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(leftLimit, base + wallH, frontLimit); glEnd();

    // Balustradă Margine
    glBegin(GL_QUADS); glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(leftLimit, base, backLimit);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(rightLimit, base, backLimit);
    glTexCoord2f(2.0f, 1.0f); glVertex3f(rightLimit, base + 1.25f, backLimit);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(leftLimit, base + 1.25f, backLimit); glEnd();

    glDisable(GL_TEXTURE_2D);
}

// Subdivizat în rețea (Grid) pentru a respinge lumina stâlpilor
void drawGround(GLuint groundID) {
    if (groundID == 0) return;
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, groundID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glColor3f(0.35f, 0.32f, 0.25f);

    float step = 5.0f; // Densitatea caroiajului de pământ
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    for (float x = -250.0f; x < 250.0f; x += step) {
        for (float z = -250.0f; z < 250.0f; z += step) {
            glTexCoord2f(x / 10.0f, z / 10.0f);         glVertex3f(x, 0.0f, z);
            glTexCoord2f((x + step) / 10.0f, z / 10.0f);  glVertex3f(x + step, 0.0f, z);
            glTexCoord2f((x + step) / 10.0f, (z + step) / 10.0f); glVertex3f(x + step, 0.0f, z + step);
            glTexCoord2f(x / 10.0f, (z + step) / 10.0f);  glVertex3f(x, 0.0f, z + step);
        }
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void drawSky(GLuint skyTexture) {
    if (skyTexture == 0) return;
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    int sky_subdiv = 30;
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, skyTexture);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    for (int i = 0; i < sky_subdiv; i++) {
        for (int j = 0; j < sky_subdiv; j++) {
            int offs[][2] = { {0, 0}, {1, 0}, {1, 1}, {0, 1} };
            for (int k = 0; k < 4; k++) {
                float u = (float)(i + offs[k][0]) / (float)sky_subdiv;
                float v = (float)(j + offs[k][1]) / (float)sky_subdiv;
                float theta = u * 2.0f * M_PI;
                float phi = (v * 0.75f - 0.2f) * M_PI;
                float radius = 500.0f;

                glVertex3f(radius * cos(phi) * sin(theta), radius * sin(phi), radius * cos(phi) * cos(theta));
            }
        }
    }
    glEnd();
    glPopAttrib();
}

void drawCity(const std::vector<Street*>& listaStrazi, const std::vector<Building*>& listaCartier) {
    for (auto* strada : listaStrazi) if (strada) strada->draw();
    for (auto* cladire : listaCartier) if (cladire) cladire->draw();
}

void drawStreetLamp(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glDisable(GL_TEXTURE_2D);

    // Pilon stâlp
    glColor3f(0.25f, 0.25f, 0.27f);
    glPushMatrix(); glTranslatef(0.0f, 4.0f, 0.0f); glScalef(0.3f, 8.0f, 0.3f); glutSolidCube(1.0f); glPopMatrix();

    // Braț stâlp
    glPushMatrix(); glTranslatef(0.5f, 8.0f, 0.0f); glScalef(1.3f, 0.2f, 0.3f); glutSolidCube(1.0f); glPopMatrix();

    // Casetă bec
    glPushMatrix(); glTranslatef(1.0f, 7.8f, 0.0f); glScalef(0.5f, 0.2f, 0.4f); glutSolidCube(1.0f); glPopMatrix();

    // Geometrie Bec Incandescent
    glDisable(GL_LIGHTING); glColor3f(1.0f, 0.9f, 0.6f);
    glPushMatrix(); glTranslatef(1.0f, 7.65f, 0.0f); glScalef(0.2f, 0.1f, 0.2f); glutSolidCube(1.0f); glPopMatrix();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawBrutalistTable(float posX, float posZ, float baseHeight, GLuint textureID) {
    glPushMatrix();
    glTranslatef(posX, baseHeight, posZ);

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Gri ciment neutru
    glColor3f(0.6f, 0.6f, 0.6f);

    // Dimensiuni masă
    float tableH = 0.9f;       // Înălțimea mesei
    float blatSize = 0.8f;     // Lățimea blatului (pătrat de 0.8x0.8)
    float blatThickness = 0.1f;// Grosimea blatului
    float legSize = 0.25f;     // Lățimea piciorului central masiv

    // ==========================================
    // 1. PICIORUL CENTRAL (Format din 4 fețe alpine)
    // ==========================================
    float legMin = -legSize / 2.0f;
    float legMax = legSize / 2.0f;
    float legTop = tableH - blatThickness;

    glBegin(GL_QUADS);
    // Față
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(legMin, 0.0f, legMax);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(legMax, 0.0f, legMax);
    glTexCoord2f(1.0f, 2.0f); glVertex3f(legMax, legTop, legMax);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(legMin, legTop, legMax);
    // Spate
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(legMax, 0.0f, legMin);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(legMin, 0.0f, legMin);
    glTexCoord2f(1.0f, 2.0f); glVertex3f(legMin, legTop, legMin);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(legMax, legTop, legMin);
    // Stânga
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(legMin, 0.0f, legMin);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(legMin, 0.0f, legMax);
    glTexCoord2f(1.0f, 2.0f); glVertex3f(legMin, legTop, legMax);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(legMin, legTop, legMin);
    // Dreapta
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(legMax, 0.0f, legMax);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(legMax, 0.0f, legMin);
    glTexCoord2f(1.0f, 2.0f); glVertex3f(legMax, legTop, legMin);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(legMax, legTop, legMax);
    glEnd();

    // ==========================================
    // 2. BLATUL SUBDIVIZAT (Pentru reflexia luminii)
    // ==========================================
    float bMin = -blatSize / 2.0f;
    float bMax = blatSize / 2.0f;
    float yBottom = tableH - blatThickness;
    float yTop = tableH;
    float step = 0.1f; // Pătrățele mici de 10cm pentru umbre fine

    // Fața de sus a blatului (Grid de pătrățele)
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    for (float x = bMin; x < bMax; x += step) {
        for (float z = bMin; z < bMax; z += step) {
            glTexCoord2f(x * 2.0f, z * 2.0f);         glVertex3f(x, yTop, z);
            glTexCoord2f((x + step) * 2.0f, z * 2.0f);  glVertex3f(x + step, yTop, z);
            glTexCoord2f((x + step) * 2.0f, (z + step) * 2.0f); glVertex3f(x + step, yTop, z + step);
            glTexCoord2f(x * 2.0f, (z + step) * 2.0f);  glVertex3f(x, yTop, z + step);
        }
    }
    glEnd();

    // Marginile laterale ale blatului (Grosimea)
    glBegin(GL_QUADS);
    // Blat Față
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(bMin, yBottom, bMax);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(bMax, yBottom, bMax);
    glTexCoord2f(2.0f, 0.5f); glVertex3f(bMax, yTop, bMax);
    glTexCoord2f(0.0f, 0.5f); glVertex3f(bMin, yTop, bMax);
    // Blat Spate
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(bMax, yBottom, bMin);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(bMin, yBottom, bMin);
    glTexCoord2f(2.0f, 0.5f); glVertex3f(bMin, yTop, bMin);
    glTexCoord2f(0.0f, 0.5f); glVertex3f(bMax, yTop, bMin);
    // Blat Stânga
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(bMin, yBottom, bMin);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(bMin, yBottom, bMax);
    glTexCoord2f(2.0f, 0.5f); glVertex3f(bMin, yTop, bMax);
    glTexCoord2f(0.0f, 0.5f); glVertex3f(bMin, yTop, bMin);
    // Blat Dreapta
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(bMax, yBottom, bMax);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(bMax, yBottom, bMin);
    glTexCoord2f(2.0f, 0.5f); glVertex3f(bMax, yTop, bMin);
    glTexCoord2f(0.0f, 0.5f); glVertex3f(bMax, yTop, bMax);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawBrutalistChair(float posX, float posZ, float rotY, float baseHeight, GLuint betonTexID, GLuint woodTexID) {
    glPushMatrix();
    glTranslatef(posX, baseHeight, posZ);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f); // Rotație ca să îl poți orienta spre masă

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    // Dimensiuni scaun
    float w = 0.5f;          // Lățime scaun
    float d = 0.5f;          // Adâncime scaun
    float seatH = 0.45f;     // Înălțime șezut
    float totalH = 0.9f;     // Înălțime spătar
    float t = 0.08f;         // Grosime laterale beton / scânduri

    // ==========================================
    // 1. LATERALELE MASIVE DIN BETON (Stânga & Dreapta)
    // ==========================================
    glBindTexture(GL_TEXTURE_2D, betonTexID);
    glColor3f(0.6f, 0.6f, 0.6f); // Gri ciment

    float sidesX[] = { -w / 2.0f, w / 2.0f - t };

    for (int i = 0; i < 2; i++) {
        float sx = sidesX[i];

        glBegin(GL_QUADS);
        // Față exterioară/interioară (Schițăm profilul lateral de scaun)
        // Baza laterală
        glNormal3f((i == 0) ? -1.0f : 1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(sx, 0.0f, -d / 2.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(sx, 0.0f, d / 2.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(sx, seatH, d / 2.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(sx, seatH, -d / 2.0f);

        // Spătarul lateral (urcă mai sus în spate)
        glTexCoord2f(0.0f, 0.0f); glVertex3f(sx, seatH, -d / 2.0f);
        glTexCoord2f(0.5f, 0.0f); glVertex3f(sx, seatH, -d / 2.0f + t * 2);
        glTexCoord2f(0.5f, 1.5f); glVertex3f(sx, totalH, -d / 2.0f + t * 2);
        glTexCoord2f(0.0f, 1.5f); glVertex3f(sx, totalH, -d / 2.0f);
        glEnd();

        // Grosimea profilului de beton (randăm fețele mici care unesc profilele pe X)
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f); // Partea de sus a sprijinului pentru mâini/șezut
        glVertex3f(sx, seatH, -d / 2.0f + t * 2);
        glVertex3f(sx + t, seatH, -d / 2.0f + t * 2);
        glVertex3f(sx + t, seatH, d / 2.0f);
        glVertex3f(sx, seatH, d / 2.0f);
        glEnd();
    }

    // ==========================================
    // 2. ȘEZUTUL ȘI SPĂTARUL DIN LEMN SUBDIVIZAT
    // ==========================================
    glBindTexture(GL_TEXTURE_2D, woodTexID);
    glColor3f(0.55f, 0.38f, 0.23f); // Maro lemn brutalist închis

    float innerWMin = -w / 2.0f + t;
    float innerWMax = w / 2.0f - t;
    float step = 0.05f; // Subdiviziune fină a scândurilor pentru lumină curată

    // --- ȘEZUT (Grid pe Z) ---
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    // Pornim din spatele șezutului până în față
    for (float z = -d / 2.0f + t * 2; z < d / 2.0f - 0.02f; z += step) {
        float nextZ = (z + step > d / 2.0f) ? (d / 2.0f) : (z + step);

        glTexCoord2f(0.0f, z * 4.0f);       glVertex3f(innerWMin, seatH - 0.02f, z);
        glTexCoord2f(1.0f, z * 4.0f);       glVertex3f(innerWMax, seatH - 0.02f, z);
        glTexCoord2f(1.0f, nextZ * 4.0f);   glVertex3f(innerWMax, seatH - 0.02f, nextZ);
        glTexCoord2f(0.0f, nextZ * 4.0f);   glVertex3f(innerWMin, seatH - 0.02f, nextZ);
    }
    glEnd();

    // --- SPĂTAR (Grid pe Y) ---
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f); // Privește spre față
    float backZ = -d / 2.0f + t + 0.02f;
    // Construim scândurile spătarului urcând pe Y
    for (float y = seatH + 0.05f; y < totalH - 0.05f; y += step) {
        float nextY = (y + step > totalH - 0.05f) ? (totalH - 0.05f) : (y + step);

        glTexCoord2f(0.0f, y * 4.0f);       glVertex3f(innerWMin, y, backZ);
        glTexCoord2f(1.0f, y * 4.0f);       glVertex3f(innerWMax, y, backZ);
        glTexCoord2f(1.0f, nextY * 4.0f);   glVertex3f(innerWMax, nextY, backZ);
        glTexCoord2f(0.0f, nextY * 4.0f);   glVertex3f(innerWMin, nextY, backZ);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


void drawCatBillboard(float x, float y, float z, GLuint catTexID) {
<<<<<<< HEAD
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(x, y, z);

    // Billboarding corect după cameră
=======
    if (catTexID == 0) return;

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, catTexID);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Activăm Blending-ul standard pentru PNG transparent
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Oprim scrierea în Z-buffer doar pentru acest pătrat transparent 
    // ca să poți vedea clădirile prin zonele unde pisica e invizibilă
    glDepthMask(GL_FALSE);

    glPushMatrix();
    glTranslatef(x, y, z);

>>>>>>> de6534426a6e11e9730ced9c2ef6c6fa506f4b42
    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    float camRightX = modelview[0], camRightY = modelview[4], camRightZ = modelview[8];
    float camUpX = modelview[1], camUpY = modelview[5], camUpZ = modelview[9];

    float catSizeX = 0.35f;
    float catSizeY = 0.45f;

<<<<<<< HEAD
    if (catTexID == 0) {
        // === MOD DEBUGER === 
        // Dacă textura nu s-a încărcat, desenăm un pătrat roșu solid ca să verificăm poziția
        glDisable(GL_TEXTURE_2D);
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex3f((-camRightX * catSizeX), (-camUpY * catSizeY), (-camRightZ * catSizeX));
        glVertex3f((camRightX * catSizeX), (-camUpY * catSizeY), (camRightZ * catSizeX));
        glVertex3f((camRightX * catSizeX), (camUpY * catSizeY), (camRightZ * catSizeX));
        glVertex3f((-camRightX * catSizeX), (camUpY * catSizeY), (-camRightZ * catSizeX));
        glEnd();
    }
    else {
        // === MOD NORMAL CU TEXTURĂ ===
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, catTexID);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.1f);
        glColor3f(1.0f, 1.0f, 1.0f); // Resetăm culoarea la alb ca să nu se înnegrească poza

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f((-camRightX * catSizeX), (-camUpY * catSizeY), (-camRightZ * catSizeX));
        glTexCoord2f(1.0f, 0.0f); glVertex3f((camRightX * catSizeX), (-camUpY * catSizeY), (camRightZ * catSizeX));
        glTexCoord2f(1.0f, 1.0f); glVertex3f((camRightX * catSizeX), (camUpY * catSizeY), (camRightZ * catSizeX));
        glTexCoord2f(0.0f, 1.0f); glVertex3f((-camRightX * catSizeX), (camUpY * catSizeY), (-camRightZ * catSizeX));
        glEnd();
    }

    glPopMatrix();
=======
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f((-camRightX * catSizeX), (-camUpY * catSizeY), (-camRightZ * catSizeX));
    glTexCoord2f(1.0f, 0.0f); glVertex3f((camRightX * catSizeX), (-camUpY * catSizeY), (camRightZ * catSizeX));
    glTexCoord2f(1.0f, 1.0f); glVertex3f((camRightX * catSizeX), (camUpY * catSizeY), (camRightZ * catSizeX));
    glTexCoord2f(0.0f, 1.0f); glVertex3f((-camRightX * catSizeX), (camUpY * catSizeY), (-camRightZ * catSizeX));
    glEnd();

    glPopMatrix();

    glDepthMask(GL_TRUE); // Repornim Z-buffer-ul pentru cadrele următoare
>>>>>>> de6534426a6e11e9730ced9c2ef6c6fa506f4b42
    glPopAttrib();
}