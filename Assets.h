#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glut.h>
#include "utils.h"
#include <cstdlib>
#include "Cigarette.h"
#include "Street.h"
#include "Building.h"

void drawBalcony(float base, float backLimit, float frontLimit, float leftLimit, float rightLimit, GLuint betonID, GLuint floorID, GLuint ceilID) {
    float wallH = 2.5f;

    glEnable(GL_LIGHTING); // Ne asigurăm că iluminarea e pornită pentru balcon
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, floorID);
    glColor3f(0.35f, 0.32f, 0.25f);

    // 1. FLOOR (Podeaua) -> Normala în sus (+Y)
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(leftLimit, base, backLimit);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(rightLimit, base, backLimit);
    glTexCoord2f(2.0f, 2.0f); glVertex3f(rightLimit, base, frontLimit);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(leftLimit, base, frontLimit);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, ceilID);
    glColor3f(0.4f, 0.4f, 0.4f);

    // 2. Ceiling (Acoperișul) -> Normala în jos (-Y)
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(leftLimit, base + wallH, backLimit);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(rightLimit, base + wallH, backLimit);
    glTexCoord2f(2.0f, 2.0f); glVertex3f(rightLimit, base + wallH, frontLimit);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(leftLimit, base + wallH, frontLimit);
    glEnd();

    // Pentru pereți, folosim textura de beton
    glBindTexture(GL_TEXTURE_2D, betonID);

    // 3. LEFT WALL -> Normala spre interiorul balconului (+X)
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(leftLimit, base, backLimit);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(leftLimit, base + wallH, backLimit);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(leftLimit, base + wallH, frontLimit);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(leftLimit, base, frontLimit);
    glEnd();

    // 4. RIGHT WALL -> Normala spre interiorul balconului (-X)
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(rightLimit, base, backLimit);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(rightLimit, base + wallH, backLimit);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(rightLimit, base + wallH, frontLimit);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(rightLimit, base, frontLimit);
    glEnd();

    // 5. BACK WALL (Peretele blocului) -> Normala spre stradă (-Z)
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(leftLimit, base, frontLimit);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(rightLimit, base, frontLimit);
    glTexCoord2f(2.0f, 1.0f); glVertex3f(rightLimit, base + wallH, frontLimit);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(leftLimit, base + wallH, frontLimit);
    glEnd();

    // 6. EDGE (Balustrada) -> Normala spre cameră (+Z)
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(leftLimit, base, backLimit);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(rightLimit, base, backLimit);
    glTexCoord2f(2.0f, 1.0f); glVertex3f(rightLimit, base + 1.25f, backLimit);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(leftLimit, base + 1.25f, backLimit);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawGround() {
    glEnable(GL_LIGHTING);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // Solul privește în sus
    glColor3f(0.05f, 0.05f, 0.1f);
    glVertex3f(-50.0f, 0.0f, -50.0f);
    glVertex3f(50.0f, 0.0f, -50.0f);
    glVertex3f(50.0f, 0.0f, 50.0f);
    glVertex3f(-50.0f, 0.0f, 50.0f);
    glEnd();
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

                float vertexX = radius * cos(phi) * sin(theta);
                float vertexY = radius * sin(phi);
                float vertexZ = radius * cos(phi) * cos(theta);

                glTexCoord2f(u, v);
                glVertex3f(vertexX, vertexY, vertexZ);
            }
        }
    }
    glEnd();

    glPopAttrib();
}


void drawCity() {

}