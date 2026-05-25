#include "utils.h"
#include <cstdlib>
#include "PLayer.h"

struct SmokeParticle {
    float x, y, z;
    float vx, vy, vz;
    float alpha;
    float size;
    float life;
    float growth;
};

std::vector<SmokeParticle> smokeSystem;

void updateSmokeSystem(float emitX, float emitY, float emitZ, bool heavySmoke) {
    // 1. Generăm particule noi la vârful țigării
    // Dacă e heavySmoke (când tragi din ea), generăm mai multe particule pe cadru
    int particlesToSpawn = heavySmoke ? 3 : 1;

    // Pentru fum normal, punem o mică șansă random ca să nu fie prea dens
    if (heavySmoke || (rand() % 3 == 0)) {
        for (int i = 0; i < particlesToSpawn; i++) {
            SmokeParticle p;
            p.x = emitX;
            p.y = emitY;
            p.z = emitZ;

            // Viteza: urcă în sus pe Y (vy) și se împrăștie ușor și haotic pe X și Z
            p.vx = ((float)(rand() % 100) / 100.0f - 0.5f) * 0.003f;
            p.vy = ((float)(rand() % 100) / 100.0f) * 0.015f + 0.004f; // Urcare constantă
            p.vz = ((float)(rand() % 100) / 100.0f - 0.5f) * 0.003f;

            p.life = 1.0f; // Începe cu viață maximă
            p.alpha = heavySmoke ? 0.6f : 0.35f; // Mai opac dacă tragi din țigară
            p.size = 0.01f; // Începe foarte mică, exact de la mărimea jarului
            p.growth = 0.002f + ((float)(rand() % 100) / 100.0f) * 0.0005f; // Rata de mărire randomizată

            smokeSystem.push_back(p);
        }
    }

    // 2. Actualizăm starea particulelor existente
    for (size_t i = 0; i < smokeSystem.size(); ) {
        // Aplicăm viteza pe poziție
        smokeSystem[i].x += smokeSystem[i].vx;
        smokeSystem[i].y += smokeSystem[i].vy;
        smokeSystem[i].z += smokeSystem[i].vz;

        // Fumul se dilată/mărește pe măsură ce se ridică în aer
        smokeSystem[i].size += smokeSystem[i].growth;

        // Scădem viața particulei (viteza cu care moare fumul)
        smokeSystem[i].life -= 0.03f;

        // Transparența scade odată cu viața (fade out fluid)
        smokeSystem[i].alpha = smokeSystem[i].life * (heavySmoke ? 0.6f : 0.35f);

        // Dacă particula a dispărut complet, o eliminăm din memorie
        if (smokeSystem[i].life <= 0.0f || smokeSystem[i].alpha <= 0.0f) {
            smokeSystem.erase(smokeSystem.begin() + i);
        }
        else {
            i++; // Trecem la următoarea
        }
    }
}

void drawSmoothParticle(float size) {
    // Desenăm o particulă formată din cercuri concentrice din ce în ce mai transparente.
    // Acest truc simulează o textură "soft" de fum fără să avem nevoie de un fișier imagine extern!
    int segments = 12;
    int rings = 5; // Straturi de transparență

    for (int r = rings; r > 0; r--) {
        float radius = size * ((float)r / (float)rings);

        // Cu cât cercul e mai la exterior, cu atât e mai transparent
        float ringAlphaMultiplier = (float)(rings - r + 1) / (float)rings;

        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0.0f, 0.0f, 0.0f); // Centrul cercului

        for (int i = 0; i <= segments; i++) {
            float angle = i * 2.0f * M_PI / segments;
            glVertex3f(cos(angle) * radius, sin(angle) * radius, 0.0f);
        }
        glEnd();
    }
}

void renderSmoke(float lookX, float lookY, float lookZ) {
    if (smokeSystem.empty()) return;

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);   // Fumul nu e afectat direct de sursele de lumină 3D
    glDisable(GL_TEXTURE_2D);  // Folosim culori și transparențe directe

    // Activăm Blending-ul pentru transparențe fluide
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Dezactivăm scrierea în Depth Buffer.
    // Asta împiedică particulele din spate să fie tăiate/ascunse urât de cele din față.
    glDepthMask(GL_FALSE);

    // Calculăm unghiurile de rotație pentru ca particulele de fum (care sunt plane 2D)
    // să fie orientate mereu cu fața spre cameră (Tehnica numită Billboarding)
    // În OpenGL clasic, putem face asta rapid anulând rotația matricii de vizualizare.
    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    // Extriem vectorii Right (dreapta) și Up (sus) ai camerei direct din matricea curentă
    float camRightX = modelview[0], camRightY = modelview[4], camRightZ = modelview[8];
    float camUpX = modelview[1], camUpY = modelview[5], camUpZ = modelview[9];

    // Randerăm fiecare particulă activă
    for (const auto& p : smokeSystem) {
        // Setăm culoarea fumului: un gri-albăstrui palid + transparența ei dinamică
        glColor4f(0.85f, 0.87f, 0.9f, p.alpha);

        glPushMatrix();
        // Mutăm originea la coordonata particulei în spațiul 3D
        glTranslatef(p.x, p.y, p.z);

        // Desenăm pătratul (sau cercul soft) aliniat perfect cu orientarea camerei
        glBegin(GL_QUADS);
        // Jos Stânga
        glVertex3f((-camRightX - camUpX) * p.size, (-camRightY - camUpY) * p.size, (-camRightZ - camUpZ) * p.size);
        // Jos Dreapta
        glVertex3f((camRightX - camUpX) * p.size, (camRightY - camUpY) * p.size, (camRightZ - camUpZ) * p.size);
        // Sus Dreapta
        glVertex3f((camRightX + camUpX) * p.size, (camRightY + camUpY) * p.size, (camRightZ + camUpZ) * p.size);
        // Sus Stânga
        glVertex3f((-camRightX + camUpX) * p.size, (-camRightY + camUpY) * p.size, (-camRightZ + camUpZ) * p.size);
        glEnd();

        glPopMatrix();
    }

    glPopAttrib(); // Restaurăm toate stările OpenGL
}

// Funcție ajutătoare pentru a plasa lumina jarului la coordonatele calculate
void setupAmberLight(float x, float y, float z) {
    // Poziția luminii în spațiul 3D (al patrulea parametru 1.0f înseamnă lumină punctiformă, pozițională)
    GLfloat lightPos[] = { x, y, z, 1.0f };

    // Culorile luminii: roșu aprins cu tentă de galben (portocaliu)
    GLfloat lightAmbient[] = { 0.2f, 0.05f, 0.0f, 1.0f };
    GLfloat lightDiffuse[] = { 1.0f, 0.3f, 0.0f, 1.0f };
    GLfloat lightSpecular[] = { 1.0f, 0.5f, 0.0f, 1.0f };

    glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);

    // Atenuare: vrem ca lumina să scadă FOARTE repede cu distanța (efect local)
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 2.0f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 5.0f);

    glEnable(GL_LIGHT1); // Activăm sursa de lumină pentru jar
}

void drawCigaretteModel(float x, float y, float z, float rotY, bool isOnBalustrade, bool heavySmoke) {
    glPushMatrix();

    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    float length = 0.19f;
    float totalRot = rotY;
    if (isOnBalustrade) {
        glRotatef(-rotY, 0.0f, 1.0f, 0.0f);
        glRotatef(135.0f, 0.0f, 1.0f, 0.0f);
        totalRot = 135;
    }

    float rad = totalRot * M_PI / 180.0f;
    float varfX = x + sin(rad) * length;
    float varfY = y;
    float varfZ = z + cos(rad) * length;

    updateSmokeSystem(varfX, varfY, varfZ, heavySmoke);

    // --- ADAUGARE PUNCT LUMINOS (Când țigara e pe balustradă) ---
    setupAmberLight(varfX, varfY, varfZ);

    extern float timeOfDay;

    // ... (restul codului tău de poziționare, fum și setupAmberLight) ...

    glDisable(GL_LIGHTING); // O păstrăm dezactivată ca să nu devină un „dreptunghi bej”
    float halfW = 0.007f;

    // Calculăm un factor de umbră pentru corpul țigării (foiță și filtru)
    // Ziua = 1.0 (culori maxime), Noaptea = 0.25 (culori stinse, adaptate la întuneric)
    float cigDarkness = 0.25f + 0.75f * timeOfDay;

    // 1. Filtrul portocaliu (influențat de întunericul nopții)
    drawSimpleBlock(-halfW, halfW, -halfW, halfW, 0.0f, 0.05f,
        0.85f * cigDarkness, 0.45f * cigDarkness, 0.2f * cigDarkness);

    // 2. Foița albă (influențată puternic de noapte - devine un gri-închis realist)
    drawSimpleBlock(-halfW, halfW, -halfW, halfW, 0.05f, 0.18f,
        0.95f * cigDarkness, 0.95f * cigDarkness, 0.95f * cigDarkness);

    // 3. Jarul aprins! (ATENȚIE: Jarul EMITE lumină, deci NU trebuie să se stingă noaptea! 
    // Din contră, noaptea trebuie să fie la fel de roșu și intens!)
    drawSimpleBlock(-halfW, halfW, -halfW, halfW, 0.18f, 0.19f, 1.0f, 0.2f, 0.0f);

    glEnable(GL_LIGHTING);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawCigaretteModel(const Player& player, bool heavySmoke) {
    glPushMatrix();

<<<<<<< HEAD
    glTranslatef(player.x, player.y, player.z);

=======
    // Translatăm la poziția camerei jucătorului
    glTranslatef(player.x, player.y, player.z);

    // Rotațiile după orientarea camerei (lookX, lookY, lookZ)
>>>>>>> de6534426a6e11e9730ced9c2ef6c6fa506f4b42
    float angleY = atan2(player.lookX, player.lookZ) * 180.0f / M_PI;
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);

    float pitchLen = sqrt(player.lookX * player.lookX + player.lookZ * player.lookZ);
    float angleX = -atan2(player.lookY, pitchLen) * 180.0f / M_PI;
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);

<<<<<<< HEAD
    float localX = -0.16f;
    float localY = -0.08f;
    float localZ = 0.20f;

    glTranslatef(localX, localY, localZ);

=======
    // --- LOGICĂ DE TRANZIIE: MÂNĂ VS GURĂ ---
    float localX, localY, localZ;

    if (heavySmoke) {
        // Când fumează: o aducem la gură (aproape de centru, puțin mai jos de ochi și mai aproape pe Z)
        localX = -0.02f;  // Aproape pe centru (0.0f e fix în mijlocul ecranului)
        localY = -0.05f;  // Puțin mai sus decât atunci când stă în mână
        localZ = 0.12f;   // Mai aproape de cameră (Z mai mic înseamnă mai aproape de ochi în HUD-ul tău)
    }
    else {
        // Când NU fumează: stă normal în mâna stângă jos
        localX = -0.16f;
        localY = -0.08f;
        localZ = 0.20f;
    }

    // Aplicăm poziționarea calculată
    glTranslatef(localX, localY, localZ);

    // --- RECALCULARE CORECTĂ PENTRU VÂRFUL ȚIGĂRII (FUM + LUMINĂ JAR) ---
>>>>>>> de6534426a6e11e9730ced9c2ef6c6fa506f4b42
    float lX = player.lookX, lY = player.lookY, lZ = player.lookZ;
    float len = sqrt(lX * lX + lY * lY + lZ * lZ);
    if (len > 0.0f) { lX /= len; lY /= len; lZ /= len; }

    float rX = lZ, rY = 0.0f, rZ = -lX;
    float rLen = sqrt(rX * rX + rZ * rZ);
    if (rLen > 0.0f) { rX /= rLen; rZ /= rLen; }

<<<<<<< HEAD
=======
    // Folosim dinamic localZ și localX calculate mai sus pentru ca jarul și fumul să urmărească țigara la gură
>>>>>>> de6534426a6e11e9730ced9c2ef6c6fa506f4b42
    float varfX = player.x + lX * (localZ + 0.19f) + rX * localX;
    float varfY = player.y + lY * (localZ + 0.19f) + localY;
    float varfZ = player.z + lZ * (localZ + 0.19f) + rZ * localX;

<<<<<<< HEAD
    updateSmokeSystem(varfX, varfY, varfZ, heavySmoke);

    // --- ADAUGARE PUNCT LUMINOS (Când țigara e în mâna jucătorului) ---
=======
    // Actualizăm fumul intens și lumina dinamică a jarului la noile coordonate
    updateSmokeSystem(varfX, varfY, varfZ, heavySmoke);
>>>>>>> de6534426a6e11e9730ced9c2ef6c6fa506f4b42
    setupAmberLight(varfX, varfY, varfZ);

    extern float timeOfDay;

<<<<<<< HEAD
    // ... (restul codului tău de poziționare, fum și setupAmberLight) ...

    glDisable(GL_LIGHTING); // O păstrăm dezactivată ca să nu devină un „dreptunghi bej”
    float halfW = 0.007f;

    // Calculăm un factor de umbră pentru corpul țigării (foiță și filtru)
    // Ziua = 1.0 (culori maxime), Noaptea = 0.25 (culori stinse, adaptate la întuneric)
    float cigDarkness = 0.25f + 0.75f * timeOfDay;

    // 1. Filtrul portocaliu (influențat de întunericul nopții)
    drawSimpleBlock(-halfW, halfW, -halfW, halfW, 0.0f, 0.05f,
        0.85f * cigDarkness, 0.45f * cigDarkness, 0.2f * cigDarkness);

    // 2. Foița albă (influențată puternic de noapte - devine un gri-închis realist)
    drawSimpleBlock(-halfW, halfW, -halfW, halfW, 0.05f, 0.18f,
        0.95f * cigDarkness, 0.95f * cigDarkness, 0.95f * cigDarkness);

    // 3. Jarul aprins! (ATENȚIE: Jarul EMITE lumină, deci NU trebuie să se stingă noaptea! 
    // Din contră, noaptea trebuie să fie la fel de roșu și intens!)
    drawSimpleBlock(-halfW, halfW, -halfW, halfW, 0.18f, 0.19f, 1.0f, 0.2f, 0.0f);
=======
    glDisable(GL_LIGHTING);
    float halfW = 0.007f;

    // Calculăm factorul de umbră ambientala în funcție de zi/noapte
    float cigDarkness = 0.25f + 0.75f * timeOfDay;

    // 1. Filtrul portocaliu
    drawSimpleBlock(-halfW, halfW, -halfW, halfW, 0.0f, 0.05f,
        0.85f * cigDarkness, 0.45f * cigDarkness, 0.2f * cigDarkness);

    // 2. Foița albă
    drawSimpleBlock(-halfW, halfW, -halfW, halfW, 0.05f, 0.18f,
        0.95f * cigDarkness, 0.95f * cigDarkness, 0.95f * cigDarkness);

    // 3. Jarul aprins (rămâne intens indiferent de noapte/zi, mărim intensitatea când e heavySmoke)
    if (heavySmoke) {
        // Când tragi din ea, jarul strălucește mai puternic spre galben/alb incandescent
        drawSimpleBlock(-halfW, halfW, -halfW, halfW, 0.18f, 0.19f, 1.0f, 0.6f, 0.1f);
    }
    else {
        drawSimpleBlock(-halfW, halfW, -halfW, halfW, 0.18f, 0.19f, 1.0f, 0.2f, 0.0f);
    }
>>>>>>> de6534426a6e11e9730ced9c2ef6c6fa506f4b42

    glEnable(GL_LIGHTING);
    glPopMatrix();
}