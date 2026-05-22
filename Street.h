#ifndef STREET_H
#define STREET_H

#include <GL/glut.h>

class Street {
private:
    float startX, startZ; // Punctul de unde începe strada
    float length;         // Cât de lungă este strada
    float width;          // Lățimea străzii (ex: 6.0f pentru două benzi)
    bool axisX;           // true dacă strada merge pe axa X, false dacă merge pe axa Z
    GLuint textureID;     // ID-ul texturii de drum drept repetabil

public:
    // Constructorul segmentului de drum
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
        glColor3f(1.0f, 1.0f, 1.0f); // Resetăm culoarea la alb pentru textură

        // Calculăm de câte ori trebuie să se repete textura pe lungime
        // astfel încât marcajele (liniile) să nu se lungească nenatural.
        float tileRepeat = length / width;

        // yOffset foarte mic (Z-fighting buffer) ca drumul să stea fix deasupra solului
        float y = 0.01f;

        glBegin(GL_QUADS);
        if (axisX) {
            // --- STRADĂ PE AXA X ---
            // Repetăm textura pe axa S (orizontalul imaginii devine lungimea drumului)
            glTexCoord2f(0.0f, 0.0f);       glVertex3f(startX, y, startZ - width / 2.0f);
            glTexCoord2f(tileRepeat, 0.0f);  glVertex3f(startX + length, y, startZ - width / 2.0f);
            glTexCoord2f(tileRepeat, 1.0f);  glVertex3f(startX + length, y, startZ + width / 2.0f);
            glTexCoord2f(0.0f, 1.0f);       glVertex3f(startX, y, startZ + width / 2.0f);
        }
        else {
            // --- STRADĂ PE AXA Z ---
            // Repetăm textura pe axa T (verticalul imaginii devine lungimea drumului)
            glTexCoord2f(0.0f, 0.0f);       glVertex3f(startX - width / 2.0f, y, startZ);
            glTexCoord2f(1.0f, 0.0f);       glVertex3f(startX + width / 2.0f, y, startZ);
            glTexCoord2f(1.0f, tileRepeat);  glVertex3f(startX + width / 2.0f, y, startZ + length);
            glTexCoord2f(0.0f, tileRepeat);  glVertex3f(startX - width / 2.0f, y, startZ + length);
        }
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
};

#endif // STREET_H