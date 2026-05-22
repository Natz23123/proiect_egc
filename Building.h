#ifndef BUILDING_H
#define BUILDING_H

#include <GL/freeglut.h>
#include <cstdlib> // Pentru rand()

class Building {
private:
    float x, z;              // Poziția pe sol a centrului clădirii
    float width, depth;      // Dimensiunile la sol
    int numFloors;           // Numărul de etaje
    float floorHeight;       // Înălțimea unui etaj (ex: 3.5f)
    GLuint roofTexture;      // Textura pentru acoperiș

    // Structura care ține cele 4 fețe ale unui etaj
    struct FloorData {
        GLuint faceTextures[4];
    };

    FloorData* floors; // Pointer către array-ul de etaje alocat dinamic

public:
    // Constructorul primește acum un array de GLuint și mărimea lui (numărul de poze disponibile)
    Building(float x, float z, float w, float d, int floorsCount, float fHeight,
        GLuint availableTextures[], int texturePoolSize, GLuint roofTex) {
        this->x = x;
        this->z = z;
        this->width = w;
        this->depth = d;
        this->numFloors = floorsCount;
        this->floorHeight = fHeight;
        this->roofTexture = roofTex;

        this->floors = new FloorData[numFloors];

        if (texturePoolSize > 0) {
            for (int i = 0; i < numFloors; i++) {
                for (int face = 0; face < 4; face++) {
                    int randomIndex = rand() % texturePoolSize;
                    floors[i].faceTextures[face] = availableTextures[randomIndex];
                }
            }
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
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);

        // --- RANDARE ETAJE ---
        for (int i = 0; i < numFloors; i++) {
            float yMin = i * floorHeight;
            float yMax = (i + 1) * floorHeight;

            // Fața din FAȚĂ (Z Max)
            glBindTexture(GL_TEXTURE_2D, floors[i].faceTextures[0]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(xMin, yMin, zMax);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(xMax, yMin, zMax);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(xMax, yMax, zMax);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(xMin, yMax, zMax);
            glEnd();

            // Fața din SPATE (Z Min)
            glBindTexture(GL_TEXTURE_2D, floors[i].faceTextures[1]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(xMax, yMin, zMin);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(xMin, yMin, zMin);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(xMin, yMax, zMin);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(xMax, yMax, zMin);
            glEnd();

            // Fața din STÂNGA (X Min)
            glBindTexture(GL_TEXTURE_2D, floors[i].faceTextures[2]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(xMin, yMin, zMin);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(xMin, yMin, zMax);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(xMin, yMax, zMax);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(xMin, yMax, zMin);
            glEnd();

            // Fața din DREAPTA (X Max)
            glBindTexture(GL_TEXTURE_2D, floors[i].faceTextures[3]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(xMax, yMin, zMax);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(xMax, yMin, zMin);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(xMax, yMax, zMin);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(xMax, yMax, zMax);
            glEnd();
        }

        // --- RANDARE ACOPERIȘ ---
        float topY = numFloors * floorHeight;
        glBindTexture(GL_TEXTURE_2D, roofTexture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(xMin, topY, zMin);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(xMax, topY, zMin);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(xMax, topY, zMax);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(xMin, topY, zMax);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
};

#endif // BUILDING_H