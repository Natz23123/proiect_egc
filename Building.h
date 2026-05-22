#ifndef BUILDING_H
#define BUILDING_H

#include <GL/freeglut.h>
#include <cstdlib>

class Building {
private:
    float x, z;
    float width, depth;
    int numFloors;
    float floorHeight;
    GLuint roofTexture;

    struct FloorData {
        GLuint faceTextures[4]; // 0: Față, 1: Spate, 2: Stânga, 3: Dreapta
    };

    FloorData* floors;

public:
    Building(float x, float z, float w, float d, int floorsCount, float fHeight,
        GLuint wallTextures[], int wallPoolSize, GLuint entranceTexture, GLuint roofTex) {
        this->x = x;
        this->z = z;
        this->width = w;
        this->depth = d;
        this->numFloors = floorsCount;
        this->floorHeight = fHeight;
        this->roofTexture = roofTex;

        this->floors = new FloorData[numFloors];

        if (wallPoolSize > 0) {
            for (int i = 0; i < numFloors; i++) {
                for (int face = 0; face < 4; face++) {
                    int randomWallIdx = rand() % wallPoolSize;
                    floors[i].faceTextures[face] = wallTextures[randomWallIdx];
                }
            }
        }

        if (numFloors > 0) {
            int randomFaceForEntrance = rand() % 4;
            floors[0].faceTextures[randomFaceForEntrance] = entranceTexture;
        }
    }

    ~Building() {
        if (floors != nullptr) {
            delete[] floors;
        }
    }

    void draw() const {
        float xMin = x - width / 2.0f;
        float xMax = x + width / 2.0f;
        float zMin = z - depth / 2.0f;
        float zMax = z + depth / 2.0f;

        glDisable(GL_CULL_FACE);
        glEnable(GL_LIGHTING); // ACTIVĂM iluminarea pentru clădiri!
        glEnable(GL_TEXTURE_2D);
        GLfloat matAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Reflectă maxim lumina ambientală
        GLfloat matDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Reflectă maxim lumina difuză
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);

        glColor3f(1.0f, 1.0f, 1.0f);

        for (int i = 0; i < numFloors; i++) {
            float yMin = i * floorHeight;
            float yMax = (i + 1) * floorHeight;

            // Fața din FAȚĂ (Z Max) -> Normala privește spre +Z
            glBindTexture(GL_TEXTURE_2D, floors[i].faceTextures[0]);
            glBegin(GL_QUADS);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(xMin, yMin, zMax);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(xMax, yMin, zMax);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(xMax, yMax, zMax);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(xMin, yMax, zMax);
            glEnd();

            // Fața din SPATE (Z Min) -> Normala privește spre -Z
            glBindTexture(GL_TEXTURE_2D, floors[i].faceTextures[1]);
            glBegin(GL_QUADS);
            glNormal3f(0.0f, 0.0f, -1.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(xMax, yMin, zMin);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(xMin, yMin, zMin);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(xMin, yMax, zMin);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(xMax, yMax, zMin);
            glEnd();

            // Fața din STÂNGA (X Min) -> Normala privește spre -X
            glBindTexture(GL_TEXTURE_2D, floors[i].faceTextures[2]);
            glBegin(GL_QUADS);
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(xMin, yMin, zMin);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(xMin, yMin, zMax);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(xMin, yMax, zMax);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(xMin, yMax, zMin);
            glEnd();

            // Fața din DREAPTA (X Max) -> Normala privește spre +X
            glBindTexture(GL_TEXTURE_2D, floors[i].faceTextures[3]);
            glBegin(GL_QUADS);
            glNormal3f(1.0f, 0.0f, 0.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(xMax, yMin, zMax);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(xMax, yMin, zMin);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(xMax, yMax, zMin);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(xMax, yMax, zMax);
            glEnd();
        }

        // --- ACOPERIȘ --- -> Normala privește în sus (+Y)
        float topY = numFloors * floorHeight;
        glBindTexture(GL_TEXTURE_2D, roofTexture);
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(xMin, topY, zMin);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(xMax, topY, zMin);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(xMax, topY, zMax);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(xMin, topY, zMax);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
};

#endif // BUILDING_H