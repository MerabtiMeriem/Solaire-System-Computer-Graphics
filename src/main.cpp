
#include <GL/glut.h>

// Window dimensions
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 750;

// Init openGL
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // background black

    glEnable(GL_DEPTH_TEST); // add the depth to the sceen,an object hide other object behind him
}

// display the window
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Camera position
    gluLookAt(
        0.0, 5.0, 15.0,
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0
    );

    // draw the sun
    glPushMatrix();

        glColor3f(1.0f, 0.6f, 0.0f);

        glutSolidSphere(2.0, 50, 50);

    glPopMatrix();

    glutSwapBuffers();
}

// Window resizing
void reshape(int width, int height) {
    if (height == 0) height = 1;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(
        45.0,
        (double)width / height,
        0.1,
        100.0
    );

    glMatrixMode(GL_MODELVIEW);
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Solar System");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();

    return 0;
}

