/****************************************************************************
    Xplanet 0.94 - render an image of a planet into an X window
    Copyright (C) 2002 Hari Nair <hari@alumni.caltech.edu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

/*
  glutfuncs.cc - OpenGL/Mesa routines to pop up a window with a globe the 
  user can manipulate
*/

#include <cstdio>
#include <cstdlib>
using namespace std;

#include "checkfuncs.h"

#ifdef HAVE_AQUA
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "auxfiles.h"
#include "xplanet.h"

#include "libprojection/libprojection.h"

static GLuint Sphere;

static GLfloat range = 100.;
static GLfloat radius = 1.;
static GLfloat fov = 1.2;          // field of view, in degrees

static GLsizei angle_theta=90, angle_phi=0;
static GLsizei glutwidth, glutheight;

static int ihelp = 0;
static int idir = -1;
static int ispeed = 2;

static void Reshape(GLsizei glutwidth, GLsizei glutheight);

void 
Idle()
{
    angle_phi = (angle_phi + idir*ispeed) % 360;
    glutPostRedisplay();
}

void 
keyboard(unsigned char key, int x, int y)
{
    switch (key) 
    {
    case 'h':
        ihelp = 1 - ihelp;
        break;
    case 'q':
        exit(EXIT_SUCCESS);
        break;
    case 'r':
        idir *= -1;
        break;
    case '+':
        ispeed++;
        break;
    case '-':
        ispeed--;
        if(ispeed < 0) ispeed = 0;
        break;
    }
}

void 
special(int key, int x, int y)
{
    switch (key) 
    {
    case GLUT_KEY_UP:
        angle_theta = (angle_theta + 5) % 360;
        break;
    case GLUT_KEY_DOWN:
        angle_theta = (angle_theta - 5) % 360;
        break;
    case GLUT_KEY_LEFT:
        angle_phi = (angle_phi - 5) % 360;
        break;
    case GLUT_KEY_RIGHT:
        angle_phi = (angle_phi + 5) % 360;
        break;
    case GLUT_KEY_HOME:
        range /= 1.5;
        Reshape(glutwidth, glutheight);
        break;
    case GLUT_KEY_END:
        range *= 1.5;
        Reshape(glutwidth, glutheight);
        break;
    default:
        break;
    }
}

static void 
printstring(int x, int y, void *font, char *string)
{
    glColor3f (1., 1., 1.);
    glRasterPos2i (x, y);
    for (int i = 0; string[i] != '\0'; i++)
        glutBitmapCharacter (font, string[i]);
}

static void 
help()
{
    int i = (int) (glutwidth * 0.2);
    int j = (int) (glutheight * 0.2);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (float) glutwidth, (float) glutheight, 0.0, 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity ();
    printstring(i, j, GLUT_BITMAP_HELVETICA_10,
                "Home/End: Move closer/farther");
    j += 15;
    printstring(i, j, GLUT_BITMAP_HELVETICA_10, "Arrow keys: rotate body");
    j += 15;
    printstring(i, j, GLUT_BITMAP_HELVETICA_10, 
                "+/-: Increase/decrease rotation speed");
    j += 15;
    printstring(i, j, GLUT_BITMAP_HELVETICA_10, "r: reverse rotation");
    j += 15;
    printstring(i, j, GLUT_BITMAP_HELVETICA_10, "h: Toggle this help screen");
    j += 15;
    printstring(i, j, GLUT_BITMAP_HELVETICA_10, "q: Quit");
    Reshape(glutwidth, glutheight);
}

void 
Reshape(GLsizei x, GLsizei y)
{
    glutwidth=x;
    glutheight=y;
    glViewport(0, 0, glutwidth, glutheight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, 1.0, 0.01, range);
    glMatrixMode(GL_MODELVIEW);
}

void 
display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glLoadIdentity();

    gluLookAt(0, 0., range,
              0., 0., 0.,
              0., 1., 0.);

    glRotatef((GLfloat) angle_theta, 1., 0., 0.);
    glRotatef((GLfloat) angle_phi, 0., 0., 1.);

    glCallList(Sphere);

    glPopMatrix();
    if (ihelp) help();
    glutSwapBuffers();
}

void 
Init(const ProjectionRectangular *image_rect)
{
    GLUquadricObj *Quadric;

    GLubyte *texImage = image_rect->rgb_data;

    glTexImage2D(GL_TEXTURE_2D, 0, 3, image_rect->getWidth(), 
                 image_rect->getHeight(), 0,
                 GL_RGB, GL_UNSIGNED_BYTE, texImage);
  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
    glEnable(GL_TEXTURE_2D);
  
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
  
    Quadric = gluNewQuadric();
    gluQuadricTexture(Quadric, GL_TRUE);
  
    Sphere = glGenLists(1);
    glNewList(Sphere, GL_COMPILE);
    gluSphere(Quadric, radius, 48, 48);
    glEndList();
}

void 
doAnimate(int argc, char **argv, const ProjectionRectangular *image_rect) 
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow(versionstring.c_str());
    Init(image_rect);
    glutReshapeFunc((void (*)(int, int))Reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(Idle);
    glutSpecialFunc(special);
    glutMainLoop();
}
