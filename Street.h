#ifndef STREET_H
#define STREET_H

#include <GL/glut.h>

class Street {
private:
    float startX, startZ;
    float length;
    float width;
    bool axisX;
    GLuint textureID;

public:
    Street(float startX, float startZ, float length, float width, bool axisX, GLuint texID) {
        this->startX = startX;
        this->startZ = startZ;
        this->length = length;
        this->width = width;
        this->axisX = axisX;
        this->textureID = texID;
    }

    void draw() const {
        if (textureID == 0) return;

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor3f(1.0f, 1.0f, 1.0f);

        float y = 0.01f;
        float step = 2.0f; // Subdiviziune mică pentru a capta conul Spotlight

        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);

        if (axisX) {
            for (float x = startX; x < startX + length; x += step) {
                float nextX = (x + step > startX + length) ? (startX + length) : (x + step);
                float texS1 = (x - startX) / width;
                float texS2 = (nextX - startX) / width;

                glTexCoord2f(texS1, 0.0f);  glVertex3f(x, y, startZ - width / 2.0f);
                glTexCoord2f(texS2, 0.0f);  glVertex3f(nextX, y, startZ - width / 2.0f);
                glTexCoord2f(texS2, 1.0f);  glVertex3f(nextX, y, startZ + width / 2.0f);
                glTexCoord2f(texS1, 1.0f);  glVertex3f(x, y, startZ + width / 2.0f);
            }
        }
        else {
            for (float z = startZ; z < startZ + length; z += step) {
                float nextZ = (z + step > startZ + length) ? (startZ + length) : (z + step);
                float texT1 = (z - startZ) / width;
                float texT2 = (nextZ - startZ) / width;

                glTexCoord2f(0.0f, texT1);  glVertex3f(startX - width / 2.0f, y, z);
                glTexCoord2f(1.0f, texT1);  glVertex3f(startX + width / 2.0f, y, z);
                glTexCoord2f(1.0f, texT2);  glVertex3f(startX + width / 2.0f, y, nextZ);
                glTexCoord2f(0.0f, texT2);  glVertex3f(startX - width / 2.0f, y, nextZ);
            }
        }
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
};

#endif // STREET_H