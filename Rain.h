#ifndef RAIN_H
#define RAIN_H

#include <GL/glut.h>
#include <vector>
#include <cstdlib>

struct RainDrop {
    float x, y, z;
    float vy;     // Viteza de cădere (pe axa Y)
    float length; // Lungimea dârei de apă
};

std::vector<RainDrop> rainSystem;

// Funcția generează ploaie doar într-o rază din jurul jucătorului
void updateRainSystem(float playerX, float playerY, float playerZ, float floorH, float bBack, float bFront, float bLeft, float bRight) {
    int maxDrops = 1500; // Numărul de picături simultane (mărește pentru o furtună mai deasă)

    // 1. Generăm picături noi dacă nu am atins limita
    if (rainSystem.size() < maxDrops) {
        int dropsToSpawn = 15; // Generăm în batch-uri per cadru
        for (int i = 0; i < dropsToSpawn; i++) {
            RainDrop r;
            // Le generăm random într-un pătrat de 30x30 metri în jurul jucătorului
            r.x = playerX + ((float)(rand() % 3000) / 100.0f - 15.0f);
            r.z = playerZ + ((float)(rand() % 3000) / 100.0f - 15.0f);

            // Le generăm la înălțime mare deasupra capului
            r.y = playerY + 10.0f + ((float)(rand() % 500) / 100.0f);

            // Viteza de cădere randomizată ca să nu cadă toate perfect la fel
            r.vy = -0.4f - ((float)(rand() % 20) / 100.0f);

            // Lungimea dârei vizuale (motion blur fals)
            r.length = 0.4f + ((float)(rand() % 30) / 100.0f);

            rainSystem.push_back(r);
        }
    }

    // 2. Actualizăm poziția picăturilor și gestionăm coliziunile
    for (size_t i = 0; i < rainSystem.size(); ) {
        rainSystem[i].y += rainSystem[i].vy;

        // Efect subtil de vânt (picăturile se duc ușor pe diagonală)
        rainSystem[i].x += 0.02f;
        rainSystem[i].z -= 0.01f;

        bool isInsideBalconyX = (rainSystem[i].x > bLeft && rainSystem[i].x < bRight);
        bool isInsideBalconyZ = (rainSystem[i].z > bBack && rainSystem[i].z < bFront);

        // Dacă picătura e deasupra balconului și a atins nivelul tavanului (floorH + 2.5f)
        bool hitBalconyRoof = (isInsideBalconyX && isInsideBalconyZ && rainSystem[i].y < floorH + 2.5f);

        // Dacă picătura a atins pământul orașului (Y < 0.0f)
        bool hitGround = (rainSystem[i].y < 0.0f);

        if (hitBalconyRoof || hitGround) {
            // Ștergem picătura care s-a lovit de ceva
            rainSystem.erase(rainSystem.begin() + i);
        }
        else {
            i++;
        }
    }
}

void renderRain() {
    if (rainSystem.empty()) return;

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // Activăm transparența pentru efectul de apă
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Nu scriem în buffer-ul de adâncime ca să nu avem tăieturi urâte
    glDepthMask(GL_FALSE);

    // Setăm grosimea liniei
    glLineWidth(1.0f);

    glBegin(GL_LINES);
    for (const auto& r : rainSystem) {
        // Culoarea ploii (gri-albăstrui palid, semi-transparent)
        // O poți înmulți cu timeOfDay dacă vrei să fie mai întunecată noaptea
        glColor4f(0.5f, 0.6f, 0.7f, 0.4f);

        // Punctul de jos al picăturii
        glVertex3f(r.x, r.y, r.z);

        // Punctul de sus al picăturii (mai în spate pe traseul vântului pentru unghi)
        glColor4f(0.5f, 0.6f, 0.7f, 0.0f); // Face *fade out* spre capătul de sus
        glVertex3f(r.x - 0.02f, r.y + r.length, r.z + 0.01f);
    }
    glEnd();

    glDepthMask(GL_TRUE);
    glPopAttrib();
}

#endif // RAIN_H