#pragma once
#define _USE_MATH_DEFINES // Pentru M_PI
#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>

// Funcția ta originală pentru texturi normale (fără transparență)
GLuint loadTexture(const char* filename) {
    GLuint texture;
    int width, height;
    unsigned char* data;

    FILE* file;
    fopen_s(&file, filename, "rb");
    if (file == NULL) return 0;

    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, file);

    width = *(int*)&info[18];
    height = *(int*)&info[22];

    int size = 3 * width * height;
    data = new unsigned char[size];
    fread(data, sizeof(unsigned char), size, file);
    fclose(file);

    for (int i = 0; i < size; i += 3) {
        unsigned char tmp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = tmp;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    delete[] data;
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