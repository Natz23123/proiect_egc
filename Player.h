#ifndef PLAYER_H
#define PLAYER_H

#include <GL/glut.h>
#include <math.h>

class Player {
public:
    float height = 1.7f;
    float x, y, z;
    float yaw, pitch;
    float lookX, lookY, lookZ;
    float speed, sensitivity;
    bool hasCigarette = false;

    Player(float startX = 0.0f, float startY = 1.7f, float startZ = 0.0f) {
        x = startX; y = startY; z = startZ;
        yaw = -1.57f; pitch = 0.0f;
        speed = 0.125f; sensitivity = 0.002f;
        updateVectors();
    }

    void updateVectors() {
        lookX = cos(yaw) * cos(pitch);
        lookY = sin(pitch);
        lookZ = sin(yaw) * cos(pitch);
    }

    void moveForward(float dir) {
        x += lookX * speed * dir;
        z += lookZ * speed * dir;
    }

    void moveRight(float dir) {
        float sideX = cos(yaw + 1.5708f);
        float sideZ = sin(yaw + 1.5708f);

        x += sideX * speed * dir;
        z += sideZ * speed * dir;
    }

    void handleMouse(int mouseX, int mouseY, int centerX, int centerY) {
        float xOffset = mouseX - centerX;
        float yOffset = centerY - mouseY;

        yaw += xOffset * sensitivity;
        pitch += yOffset * sensitivity;

        if (pitch > 1.5f) pitch = 1.5f;
        if (pitch < -1.5f) pitch = -1.5f;

        updateVectors();
    }
};

#endif