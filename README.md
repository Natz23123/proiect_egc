# Brutalism Balcony (Scena urbană cu particule și efecte de iluminare)

Un proiect grafic 3D interactiv dezvoltat în **C++** și **OpenGL 1.1** (pipeline cu funcții fixe), care simulează o scenă urbană cu estetică brutalistă și o atmosferă cinematică *noir*. Jucătorul explorează mediul dintr-o perspectivă de tip First-Person, limitată la perimetrul unui balcon suspendat, interacționând cu elemente dinamice, sisteme de particule și condiții meteorologice în timp real.

Proiect realizat în cadrul disciplinei **Elemente de Grafică pe Calculator (EGC)**.

---

## 🚀 Caracteristici Principale

* **Mediu Urban Generat Procedural:** Cartier de blocuri cu un număr variabil de etaje și texturi randomizate per-fațadă pentru a asigura diversitate vizuală la fiecare rulare.
* **Sisteme de Particule (Fizică pe CPU):** * **Ploaie:** Sistem dinamic cu până la 1500 de picături active simultan, care include efect de vânt, motion blur (prin gradient de transparență) și logica de coliziune cu tavanul balconului și solul.
  * **Fum de Țigară:** Sistem de particule semi-transparente generat dinamic, ce se extinde și se disipă organic, orientat constant spre cameră folosind tehnica de *Billboarding*.
* **Iluminare Hibridă și Ciclu Zi/Noapte:**
  * Cer dinamic (Skybox) cu tranziție fluidă zi/noapte (modificarea luminii globale ambientale).
  * Surse de lumină de tip *Spotlight* pentru stâlpii stradali (con de lumină calculat fin pe suprafețe subdivizate).
  * Sursă de lumină punctuală cu atenuare pătratică masivă pentru a simula jarul țigării.
* **Tehnici Avansate de Randare OpenGL Clasic:**
  * *Alpha Blending & Alpha Testing* pentru transparențe fine și decupaje perfecte (ex. billboard-ul cu pisica "Pufarina").
  * Randare ierarhică prin manipularea stivei de matrice (`glPushMatrix` / `glPopMatrix`) pentru mobilierul brutalist.
* **Cameră First-Person Restrictivă:** Sistem de navigație cu coliziuni implementate matematic, prevenind ieșirea utilizatorului în afara perimetrului balconului.

---

## 🛠️ Tehnologii Utilizate

* **Limbaj de programare:** C++
* **Grafică:** OpenGL 1.1 (Immediate Mode - `glBegin` / `glEnd`)
* **Utilitare Ferestre & Evenimente:** FreeGLUT (gestionează bucla principală, contextul grafic și input-ul)
* **Procesare Imagini:** `stb_image.h` (librărie public-domain utilizată pentru încărcarea texturilor `.BMP`, `.PNG` cu canal Alpha și `.PPM`).
* **Mediu de dezvoltare (IDE):** Microsoft Visual Studio (arhitectură x86 / x64)

---

## 🎮 Control și Interacțiune

Aplicația folosește o combinație clasică de tastatură și mouse pentru explorare și interacțiune:

* **W, A, S, D** - Deplasarea camerei (limitată automat de pereții și balustrada balconului).
* **Mouse** - Orientarea camerei (360 de grade orizontal, unghi vertical restricționat pentru a preveni răsturnarea).
* **E** - Acțiune contextuală (ridică/pune pe balustradă țigara).
* **F** - Declanșează animația de fumat (aduce țigara la gură, intensifică lumina jarului și generează un nor dens de fum).

---

## ⚙️ Instalare și Configurare (Visual Studio)

Pentru a compila și rula proiectul pe o mașină locală, asigură-te că ai configurate corect dependențele **FreeGLUT** și structura directoarelor:

### 1. Structura Proiectului
Proiectul necesită organizarea dependențelor și a folderului de resurse (`assets`) după cum urmează:
```text
ProiectEGC/
│
├── Dependencies/
│   ├── include/
│   │   ├── GL/ (conține freeglut.h, etc.)
│   │   └── stb_image.h
│   └── lib/ (conține freeglut.lib)
│
├── x64/Debug/ (sau Release/)
│   ├── proiect_nou.exe
│   ├── freeglut.dll
│   └── assets/ (conține toate texturile: .bmp, .png, .ppm)
│
└── Source/ (fișierele .cpp și .h)
```
### 2. Setări Visual Studio
1. Crează un proiect tip **Empty Project** (C++).
2. Adaugă folderul de dependințe:
   * `Properties -> C/C++ -> General -> Additional Include Directories`: `$(ProjectDir)Dependencies\include`
   * `Properties -> Linker -> General -> Additional Library Directories`: `$(ProjectDir)Dependencies\lib`
3. Configurează link-editarea:
   * `Properties -> Linker -> Input -> Additional Dependencies`: Adaugă `opengl32.lib`, `glu32.lib`, `freeglut.lib`.
4. Asigură-te că fișierul `freeglut.dll` și folderul `assets/` sunt copiate manual în folderul unde este generat executabilul (ex. `x64/Debug/`).

---

## 🏗️ Arhitectura Codului

Aplicația este modularizată utilizând principiile programării orientate pe obiect (OOP) pentru o separare clară a logicii:

* `main.cpp`: Orchestratorul sistemului. Inițializează scena, stările OpenGL și gestionează bucla GLUT (`display`, `timer`, `keyboard`, `mouse`).
* `Player.h`: Logica camerei First-Person, calculul vectorilor de direcție și limitele de coliziune.
* `Building.h` & `Street.h`: Generarea procedurală a peisajului urban îndepărtat (blocuri și străzi).
* `Assets.h`: Modele statice hardcodate (balconul, masa, scaunul, skybox-ul, billboard-ul cu pisica).
* `Cigarette.h`: Mecanica țigării, logica sursei locale de lumină (`GL_LIGHT1`) și simularea matematică pentru sistemul de particule de fum.
* `Rain.h`: Managementul vectorului de picături, logica de cădere, efectul de vânt și coliziunea ploii.
* `utils.h`: Încapsulează funcțiile de încărcare a resurselor folosind `stb_image.h` (`loadTexture`, `loadPNGTexture`, `loadSkyTexture`).

---

## 👤 Autor

* **Vatajita Teodor-Florin**
* Grupa 4LF342, TI II
* Universitatea *Transilvania* din Brașov
* Facultatea de Inginerie Electrică și Știința Calculatoarelor