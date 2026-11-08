
#include <GL/freeglut.h>

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.2f, 0.8f, 0.4f);

    glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
    glEnd();

    glFlush();
}

int main(int argc, char** argv) {

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("My First OpenGL Project");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glutDisplayFunc(display);

    glutMainLoop();

    return 0;
}
