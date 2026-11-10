#include "solarSystem.h"
#include "camera.h"
#include <GL/glut.h>
#include <GL/glu.h>
#include <cmath>
#include <cstdlib>
#include <cstdio>

// Bibliothèque pour charger les images (jpg, png...)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


const float PI = 3.14159265f;

// ---------------------------------------------------------------
// Données de la simulation
// ---------------------------------------------------------------

Camera camera;

// Objet qui permet de dessiner des sphères AVEC coordonnées de texture
// (glutSolidSphere n'en a pas, donc impossible de la texturer)
GLUquadric* quad = nullptr;

// Identifiants OpenGL des textures
GLuint sunTexture = 0;
GLuint planetTextures[8] = { 0 };

// Animation
bool  paused = false;
float timeScale = 0.3f;        // vitesse globale (touches + et -)
float sunRotation = 0.0f;
float planetAngles[8] = { 0.0f };   // position sur l'orbite
float planetSpin[8]   = { 0.0f };   // rotation de la planète sur elle-même

// Vitesses orbitales (degrés par image)
float planetSpeeds[8] = { 4.0f, 2.5f, 2.0f, 1.5f, 0.8f, 0.6f, 0.4f, 0.3f };

// Vitesses de rotation propre
float spinSpeeds[8] = { 1.0f, -0.5f, 5.0f, 4.8f, 10.0f, 9.0f, -6.0f, 6.5f };

// Tailles, distances et couleurs de secours (si la texture est absente)
float sizes[8]     = { 0.25f, 0.4f, 0.45f, 0.35f, 1.0f, 0.85f, 0.65f, 0.6f };
float distances[8] = { 3.0f, 4.0f, 5.5f, 7.0f, 10.0f, 13.0f, 15.5f, 17.5f };
float colors[8][3] = {
    {0.6f, 0.6f, 0.6f}, {0.9f, 0.6f, 0.2f}, {0.1f, 0.3f, 0.9f}, {0.9f, 0.2f, 0.1f},
    {0.8f, 0.6f, 0.4f}, {0.8f, 0.7f, 0.5f}, {0.3f, 0.8f, 0.9f}, {0.1f, 0.2f, 0.8f}
};

// Étoiles du fond
const int NB_STARS = 1500;
float stars[NB_STARS][3];

// ---------------------------------------------------------------
// Textures
// ---------------------------------------------------------------

// Charge une image et renvoie son identifiant OpenGL (0 si échec)
GLuint loadTexture(const char* path)
{
    int width, height, channels;

    // L'image est stockée de haut en bas, OpenGL la veut de bas en haut
    stbi_set_flip_vertically_on_load(1);

    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    if (!data)
    {
        std::printf("Texture introuvable : %s\n", path);
        return 0;
    }

    GLenum format = GL_RGB;
    if (channels == 4) format = GL_RGBA;
    if (channels == 1) format = GL_LUMINANCE;

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Filtrage : rendu lisse même quand la planète est loin
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // Envoie l'image à la carte graphique + génère les mipmaps
    gluBuild2DMipmaps(GL_TEXTURE_2D, channels, width, height,
                      format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return id;
}

void loadAllTextures()
{
    const char* files[8] = {
        "assets/texture/mercury.jpg", "assets/texture/venus.jpg", "assets/texture/earth_daymap.jpg",
        "assets/texture/mars.jpg",    "assets/texture/jupiter.jpg", "assets/texture/saturn.jpg",
        "assets/texture/uranus.jpg",  "assets/texture/neptune.jpg"
    };

    sunTexture = loadTexture("assets/texture/2k_sun.jpg");

    for (int i = 0; i < 8; i++)
        planetTextures[i] = loadTexture(files[i]);
}

// ---------------------------------------------------------------
// Initialisation
// ---------------------------------------------------------------

void initStars()
{
    std::srand(42);   // même ciel à chaque lancement

    for (int i = 0; i < NB_STARS; i++)
    {
        // Point aléatoire sur une grande sphère de rayon 400
        float a = 2.0f * PI * (std::rand() / (float)RAND_MAX);
        float b = std::acos(2.0f * (std::rand() / (float)RAND_MAX) - 1.0f);

        stars[i][0] = 400.0f * std::sin(b) * std::cos(a);
        stars[i][1] = 400.0f * std::cos(b);
        stars[i][2] = 400.0f * std::sin(b) * std::sin(a);
    }
}

void initOpenGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Prépare l'objet "quadrique" avec les coordonnées de texture
    quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluQuadricNormals(quad, GLU_SMOOTH);   // utile pour la lumière plus tard

    loadAllTextures();
    initStars();
}

// Configure la projection en perspective
void reshape(int width, int height)
{
    if (height == 0)
        height = 1;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = static_cast<float>(width) / height;

    float fov = 60.0f;   // 90° déformait trop les bords, 60° est plus naturel
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;

    float top = nearPlane * std::tan(fov * PI / 360.0f);
    float right = top * aspect;

    glFrustum(-right, right, -top, top, nearPlane, farPlane);

    glMatrixMode(GL_MODELVIEW);
}

// ---------------------------------------------------------------
// Dessin
// ---------------------------------------------------------------

// Dessine une sphère texturée (ou colorée si pas de texture)
void drawTexturedSphere(float radius, GLuint texture, const float color[3])
{
    if (texture != 0)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        glColor3f(1.0f, 1.0f, 1.0f);   // blanc = ne modifie pas la texture
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
        glColor3fv(color);
    }

    glPushMatrix();
    // gluSphere a ses pôles sur l'axe Z : on le redresse pour qu'ils
    // soient sur l'axe Y (vertical)
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluSphere(quad, radius, 50, 50);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

// Fond d'étoiles
void drawStars()
{
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.9f, 0.9f, 0.9f);
    glPointSize(1.5f);

    glBegin(GL_POINTS);
    for (int i = 0; i < NB_STARS; i++)
        glVertex3fv(stars[i]);
    glEnd();
}

// Orbites : maintenant dans le plan horizontal XZ (y = 0)
void drawOrbits()
{
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.35f, 0.35f, 0.35f);

    for (int p = 0; p < 8; p++)
    {
        glBegin(GL_LINE_LOOP);

        for (int i = 0; i < 100; i++)
        {
            float angle = 2.0f * PI * i / 100.0f;

            glVertex3f(distances[p] * std::cos(angle),
                       0.0f,
                       distances[p] * std::sin(angle));
        }

        glEnd();
    }
}

void drawSun()
{
    glPushMatrix();

    glRotatef(sunRotation, 0.0f, 1.0f, 0.0f);

    float sunColor[3] = { 1.0f, 0.65f, 0.05f };
    drawTexturedSphere(2.0f, sunTexture, sunColor);

    glPopMatrix();
}

void drawPlanets()
{
    for (int i = 0; i < 8; i++)
    {
        glPushMatrix();

        // Tourne autour du Soleil (axe vertical Y)
        glRotatef(planetAngles[i], 0.0f, 1.0f, 0.0f);

        // S'éloigne du Soleil
        glTranslatef(distances[i], 0.0f, 0.0f);

        // Anneau de Saturne (dessiné avant la rotation propre)
        if (i == 5)
        {
            glDisable(GL_TEXTURE_2D);
            glColor3f(0.75f, 0.65f, 0.45f);

            glPushMatrix();
            glRotatef(27.0f, 0.0f, 0.0f, 1.0f);    // inclinaison
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);   // à plat
            gluDisk(quad, sizes[i] * 1.3f, sizes[i] * 2.2f, 60, 1);
            glPopMatrix();
        }

        // La planète tourne sur elle-même
        glRotatef(planetSpin[i], 0.0f, 1.0f, 0.0f);

        drawTexturedSphere(sizes[i], planetTextures[i], colors[i]);

        glPopMatrix();
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    camera.applyView();

    drawStars();
    drawOrbits();
    drawSun();
    drawPlanets();

    glutSwapBuffers();
}

// ---------------------------------------------------------------
// Animation
// ---------------------------------------------------------------

void update(int value)
{
    if (!paused)
    {
        sunRotation += 0.2f * timeScale;

        for (int i = 0; i < 8; i++)
        {
            planetAngles[i] += planetSpeeds[i] * timeScale;
            planetSpin[i]   += spinSpeeds[i] * timeScale;

            if (planetAngles[i] >= 360.0f) planetAngles[i] -= 360.0f;
            if (planetSpin[i] >= 360.0f)   planetSpin[i] -= 360.0f;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ---------------------------------------------------------------
// Clavier
// ---------------------------------------------------------------

void keyboard(unsigned char key, int mouseX, int mouseY)
{
    // Maintenir MAJ = déplacement 3 fois plus rapide
    float boost = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ? 3.0f : 1.0f;

    switch (key)
    {
        // Déplacement (clavier AZERTY)
        case 'z': case 'Z': camera.moveForward(1.0f * boost);  break;
        case 's': case 'S': camera.moveForward(-1.0f * boost); break;
        case 'q': case 'Q': camera.moveRight(-1.0f * boost);   break;
        case 'd': case 'D': camera.moveRight(1.0f * boost);    break;
        case 'e': case 'E': camera.moveUp(1.0f * boost);       break;
        case 'a': case 'A': camera.moveUp(-1.0f * boost);      break;

        // Remet la caméra au départ
        case 'r': case 'R': camera.reset(); break;

        // Pause / reprise
        case ' ': paused = !paused; break;

        // Vitesse de la simulation
        case '+': timeScale *= 1.25f; break;
        case '-': timeScale /= 1.25f; break;

        case 27:
            std::exit(EXIT_SUCCESS);
            break;
    }

    glutPostRedisplay();
}

void specialKeyboard(int key, int mouseX, int mouseY)
{
    switch (key)
    {
        case GLUT_KEY_LEFT:  camera.rotateYaw(-3.0f);   break;
        case GLUT_KEY_RIGHT: camera.rotateYaw(3.0f);    break;
        case GLUT_KEY_UP:    camera.rotatePitch(3.0f);  break;
        case GLUT_KEY_DOWN:  camera.rotatePitch(-3.0f); break;
    }

    glutPostRedisplay();
}

// ---------------------------------------------------------------
// Point d'entrée de la simulation
// ---------------------------------------------------------------

void mySolarSystem(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    glutInitWindowPosition(100, 50);
    glutCreateWindow("Solar System Simulation");

    // IMPORTANT : les textures se chargent après la création de la fenêtre
    initOpenGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
}