#pragma once
#define STB_IMAGE_IMPLEMENTATION
#define _USE_MATH_DEFINES // Pentru M_PI
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif
#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include "stb_image.h"

// Funcția ta originală pentru texturi normale (fără transparență)
GLuint loadTexture(const char* filename) {
    int width, height, channels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 3);

    if (data == NULL) return 0;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Acum va funcționa perfect, pentru că stb_image a eliminat padding-ul!
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return texture;
}

GLuint loadSkyTexture(const char* filename) {
    std::ifstream fp(filename, std::ios::binary);
    if (!fp.is_open()) {
        std::cerr << "Nu s-a putut deschide fisierul PPM: " << filename << "\n";
        return 0;
    }

    std::string header;
    if (!std::getline(fp, header) || header.rfind("P6", 0) != 0) {
        std::cerr << "Fisierul nu este un PPM valid de tip P6!\n";
        return 0;
    }

    // Ocolim comentariile
    char c = fp.peek();
    while (c == '#') {
        std::string comment;
        std::getline(fp, comment);
        c = fp.peek();
    }

    int width = 0, height = 0, max_val = 0;
    fp >> width >> height >> max_val;
    fp.get(); // Consumăm caracterul de whitespace de după max_val (\n)

    // Citim datele brute din fișier (harta alfa)
    int alpha_size = width * height * 3;
    std::vector<unsigned char> alpha_data(alpha_size);
    fp.read(reinterpret_cast<char*>(alpha_data.data()), alpha_size);

    // Generăm pixelii finali RGBA (4 bytes per pixel)
    std::vector<unsigned char> pixels(width * height * 4);

    for (int i = 0; i < width * height; i++) {
        // În codul original, când se dă doar harta alpha (0 pentru RGB), 
        // pixelii de culoare sunt setați nativ pe alb pur (0xffffffff)
        pixels[i * 4] = 255; // R
        pixels[i * 4 + 1] = 255; // G
        pixels[i * 4 + 2] = 255; // B

        // Luăm intensitatea culorii din PPM ca fiind valoarea de transparență
        pixels[i * 4 + 3] = alpha_data[i * 3];
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // !!! Filtrare liniară ca să nu fie norii pătrățoși !!!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}


void drawSimpleBlock(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    // Fața din față (Z max)
    glVertex3f(xMin, yMin, zMax); glVertex3f(xMax, yMin, zMax);
    glVertex3f(xMax, yMax, zMax); glVertex3f(xMin, yMax, zMax);
    // Fața din spate (Z min)
    glVertex3f(xMin, yMin, zMin); glVertex3f(xMin, yMax, zMin);
    glVertex3f(xMax, yMax, zMin); glVertex3f(xMax, yMin, zMin);
    // Fața de sus (Y max)
    glVertex3f(xMin, yMax, zMin); glVertex3f(xMin, yMax, zMax);
    glVertex3f(xMax, yMax, zMax); glVertex3f(xMax, yMax, zMin);
    // Fața de jos (Y min)
    glVertex3f(xMin, yMin, zMin); glVertex3f(xMax, yMin, zMin);
    glVertex3f(xMax, yMin, zMax); glVertex3f(xMin, yMin, zMax);
    // Fața din dreapta (X max)
    glVertex3f(xMax, yMin, zMin); glVertex3f(xMax, yMax, zMin);
    glVertex3f(xMax, yMax, zMax); glVertex3f(xMax, yMin, zMax);
    // Fața din stânga (X min)
    glVertex3f(xMin, yMin, zMin); glVertex3f(xMin, yMin, zMax);
    glVertex3f(xMin, yMax, zMax); glVertex3f(xMin, yMax, zMin);
    glEnd();
}

GLuint loadPNGTexture(const char* filename) {
    int width, height, channels;

    // =========================================================
    // LINIA MAGICĂ: Îi spune lui stb_image să întoarcă poza corect pentru OpenGL
    // =========================================================
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);

    if (data == NULL) {
        std::cerr << "Eroare la incarcarea imaginii PNG: " << filename << "\n";
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return texture;
}

// Asigură-te că funcția asta folosește stb_image (o avem deja din pasul cu pisica)
GLuint loadAnyTexture(const char* filename) {
    int width, height, channels;

    // Corectăm orientarea axei Y pentru OpenGL
    stbi_set_flip_vertically_on_load(true);

    // Forțăm citirea a 3 canale (RGB) pentru clădiri standard (fără transparență)
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 3);

    if (data == NULL) {
        std::cerr << "Eroare grava la incarcarea texturii: " << filename << "\n";
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Setăm filtre de calitate (previne artefactele și glitch-urile la distanță)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Încărcăm în OpenGL ca GL_RGB standard
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return texture;
}