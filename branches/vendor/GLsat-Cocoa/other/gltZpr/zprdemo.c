#include <GL/glut.h>

#include "zpr.h"

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix ();

#if 0
	glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
			glColor3f(0,0,1);
			glVertex3f(0,0,0);
			glVertex3f(0,0,1);
			glColor3f(0,1,0);
			glVertex3f(0,0,0);
			glVertex3f(0,1,0);
			glColor3f(1,0,0);
			glVertex3f(0,0,0);
			glVertex3f(1,0,0);
		glEnd();
#else

		glColor3f(0.3,0.3,0.3);
		glutSolidSphere(0.7, 20, 20);

		glColor3f(0,0,1);
		glutSolidCone(0.6, 4.0, 20, 20);

		glPushMatrix ();
			glColor3f(1,0,0);
			glRotatef(90,0,1,0);
			glutSolidCone(0.6, 4.0, 20, 20);
		glPopMatrix();

		glPushMatrix ();
			glColor3f(0,1,0);
			glRotatef(-90,1,0,0);
			glutSolidCone(0.6, 4.0, 20, 20);
		glPopMatrix();
#endif

    glPopMatrix ();

    glutSwapBuffers();
}

GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat high_shininess[] = { 100.0 };

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400,400);
    glutCreateWindow("Mouse ZPR");

	/* Callbacks */

    glutDisplayFunc(display);
	glScalef(0.25,0.25,0.25);
	zprInit();

	/* OpenGL Initialisation */

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

    glutMainLoop();

    return 0;         
}

