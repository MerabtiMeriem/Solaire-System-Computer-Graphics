
#include "solarSystem.h"
#include "camera.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

// Objet caméra de notre simulation
Camera camera;

// Angle de rotation du Soleil
float sunRotation = 0.0f;

// Orbital angles of the eight planets.
float planetAngles[8] = { 0.0f };

// Orbital speeds of the planets.
float planetSpeeds[8] = {
    4.0f,  // Mercury
    2.5f,  // Venus
    2.0f,  // Earth
    1.5f,  // Mars
    0.8f,  // Jupiter
    0.6f,  // Saturn
    0.4f,  // Uranus
    0.3f   // Neptune
};

// Initialise les paramètres OpenGL
void initOpenGL()
{
    // Définit le fond noir
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Active la gestion de profondeur
    glEnable(GL_DEPTH_TEST);

    // Active le remplissage des surfaces
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// Configure la projection en perspective
void reshape(int width, int height)
{
    // Évite une division par zéro
    if (height == 0)
        height = 1;

    // Définit la zone d'affichage
    glViewport(0, 0, width, height);

    // Configure la projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Calcule le rapport largeur/hauteur
    float aspect = static_cast<float>(width) / height;

    // Paramètres de perspective
    float fov = 90.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;

    float top = nearPlane * std::tan(fov * 3.14159265f / 360.0f);
    float right = top * aspect;

    // Crée la projection en perspective
    glFrustum(-right, right, -top, top, nearPlane, farPlane);

    // Retourne au mode modèle/vue
    glMatrixMode(GL_MODELVIEW);
}

// Draw the orbital paths of the eight planets.
void drawOrbits()
{
    const float PI = 3.14159265f;

    // Orbital radii corresponding to planet positions.
    float orbitRadii[] = {
        3.0f, 4.0f, 5.5f, 7.0f,
        10.0f, 13.0f, 15.5f, 17.5f
    };

    glColor3f(0.35f, 0.35f, 0.35f);

    // Draw one circular orbit for each planet.
    for (float radius : orbitRadii)
    {
        glBegin(GL_LINE_LOOP);

        for (int i = 0; i < 100; i++)
        {
            float angle = 2.0f * PI * i / 100.0f;

            float x = radius * cos(angle);
            float y = radius * sin(angle);

            glVertex3f(x, y, 0.0f);
        }

        glEnd();
    }
}

// Dessine le Soleil au centre de la scène
void drawSun()
{
    glPushMatrix();

    // Rotation autour de l'axe vertical
    glRotatef(sunRotation, 0.0f, 1.0f, 0.0f);

    // Couleur jaune-orange
    glColor3f(1.0f, 0.65f, 0.05f);

    // Dessine une sphère de rayon 2
    glutSolidSphere(2.0, 50, 50);

    glPopMatrix();
}



// Draw the eight planets and calculate their orbital positions.
void drawPlanets()
{
    // Planet sizes.
    float sizes[8] = {
        0.25f, 0.4f, 0.45f, 0.35f,
        1.0f, 0.85f, 0.65f, 0.6f
    };

    // Planet orbital distances.
    float distances[8] = {
        3.0f, 4.0f, 5.5f, 7.0f,
        10.0f, 13.0f, 15.5f, 17.5f
    };

    // Planet colors.
    float colors[8][3] = {
        {0.6f, 0.6f, 0.6f},
        {0.9f, 0.6f, 0.2f},
        {0.1f, 0.3f, 0.9f},
        {0.9f, 0.2f, 0.1f},
        {0.8f, 0.6f, 0.4f},
        {0.8f, 0.7f, 0.5f},
        {0.3f, 0.8f, 0.9f},
        {0.1f, 0.2f, 0.8f}
    };

    // Draw each planet at its orbital position.
    for (int i = 0; i < 8; i++)
    {
        glPushMatrix();

        // Rotate around the Sun.
        glRotatef(planetAngles[i], 0.0f, 0.0f, 1.0f);

        // Move the planet away from the Sun.
        glTranslatef(distances[i], 0.0f, 0.0f);

        // Set the planet color.
        glColor3f(colors[i][0], colors[i][1], colors[i][2]);

        // Draw the planet.
        glutSolidSphere(sizes[i], 30, 30);

        glPopMatrix();
    }
}


// Affiche tous les éléments de la scène
void display()
{
    // Efface l'image précédente
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Initialise la matrice de vue
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Applique la caméra
    camera.applyView();

    //Dessine des orbits
    drawOrbits();

    // Dessine le Soleil
    drawSun();

    // Dessine une planette
    drawPlanets();

    // Affiche la nouvelle image
    glutSwapBuffers();
}

// Met à jour l'animation
void update(int value)
{
    // Rotate the Sun.
    sunRotation += 0.2f;

    if (sunRotation >= 360.0f)
        sunRotation -= 360.0f;

    // Update the orbital angle of each planet.
    for (int i = 0; i < 8; i++)
    {
        planetAngles[i] += planetSpeeds[i];

        if (planetAngles[i] >= 360.0f)
            planetAngles[i] -= 360.0f;
    }

    // Request a new frame.
    glutPostRedisplay();

    // Schedule the next update.
    glutTimerFunc(16, update, 0);
}

// Gère les touches du clavier
void keyboard(unsigned char key, int mouseX, int mouseY)
{
    switch (key)
    {
        case 'z':
        case 'Z':
            camera.moveForward(1.0f);
            break;

        case 's':
        case 'S':
            camera.moveForward(-1.0f);
            break;

        case 'q':
        case 'Q':
            camera.moveRight(-1.0f);
            break;

        case 'd':
        case 'D':
            camera.moveRight(1.0f);
            break;

        case 'e':
        case 'E':
            camera.moveUp(1.0f);
            break;

        case 'a':
        case 'A':
            camera.moveUp(-1.0f);
            break;

        case 27:
            // Ferme le programme
            std::exit(EXIT_SUCCESS);
            break;
    }

    glutPostRedisplay();
}

// Gère les touches directionnelles
void specialKeyboard(int key, int mouseX, int mouseY)
{
    switch (key)
    {
        case GLUT_KEY_LEFT:
            camera.rotateYaw(-3.0f);
            break;

        case GLUT_KEY_RIGHT:
            camera.rotateYaw(3.0f);
            break;

        case GLUT_KEY_UP:
            camera.rotatePitch(3.0f);
            break;

        case GLUT_KEY_DOWN:
            camera.rotatePitch(-3.0f);
            break;
    }

    glutPostRedisplay();
}

// Initialise GLUT et démarre la simulation
void mySolarSystem(int argc, char** argv)
{
    // Initialise GLUT
    glutInit(&argc, argv);

    // Active le double buffering et le buffer de profondeur
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Définit la taille de la fenêtre
    glutInitWindowSize(1200, 800);

    // Définit la position de la fenêtre
    glutInitWindowPosition(100, 50);

    // Crée la fenêtre
    glutCreateWindow("Solar System Simulation");

    // Initialise OpenGL
    initOpenGL();

    // Enregistre les fonctions de rendu et de redimensionnement
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    // Enregistre les fonctions clavier
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);

    // Démarre l'animation
    glutTimerFunc(16, update, 0);

    // Lance la boucle principale
    glutMainLoop();
}
