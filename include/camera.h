#ifndef CAMERA_H
#define CAMERA_H

class Camera
{
private:
    float x, y, z;
    float yaw;
    float pitch;
    float movementSpeed;

public:
    Camera();

    void applyView();
    void reset();   // Remet la caméra à sa position de départ

    void moveForward(float direction);
    void moveRight(float direction);
    void moveUp(float direction);

    void rotateYaw(float angle);
    void rotatePitch(float angle);
};

#endif



