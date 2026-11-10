
#include "camera.h"
#include <GL/glut.h>
#include <cmath>

// Initialise la caméra à 15 unités du centre
Camera::Camera()
    : x(0.0f), y(0.0f), z(15.0f),
      yaw(0.0f), pitch(0.0f),
      movementSpeed(0.5f)
{
}

// Applique la position et l'orientation de la caméra
void Camera::applyView()
{
    glRotatef(-pitch, 1.0f, 0.0f, 0.0f);
    glRotatef(-yaw, 0.0f, 1.0f, 0.0f);

    glTranslatef(-x, -y, -z);
}

// Déplace la caméra vers l'avant ou l'arrière
void Camera::moveForward(float direction)
{
    float angle = yaw * 3.14159265f / 180.0f;

    x += std::sin(angle) * movementSpeed * direction;
    z -= std::cos(angle) * movementSpeed * direction;
}

// Déplace la caméra vers la droite ou la gauche
void Camera::moveRight(float direction)
{
    float angle = yaw * 3.14159265f / 180.0f;

    x += std::cos(angle) * movementSpeed * direction;
    z += std::sin(angle) * movementSpeed * direction;
}

// Déplace la caméra verticalement
void Camera::moveUp(float direction)
{
    y += movementSpeed * direction;
}

// Oriente la caméra horizontalement
void Camera::rotateYaw(float angle)
{
    yaw += angle;
}

// Oriente la caméra verticalement
void Camera::rotatePitch(float angle)
{
    pitch += angle;

    // Limite l'inclinaison verticale
    if (pitch > 89.0f)
        pitch = 89.0f;

    if (pitch < -89.0f)
        pitch = -89.0f;
}


