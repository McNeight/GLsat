#include <GL/glut.h>
#include <math.h>
#include "mesa_sgi.h"


GLfloat mat[3] = {0.1745, 0.01175, 0.01175};	
GLfloat fogColor[4] = {0.5, 0.5, 0.5, 1.0};
GLfloat position[] = { 0.5, 0.5, 3.0, 0.0 };

void output(GLfloat x, GLfloat y, char *text)
{
  char *p;
  glPushMatrix();
  glTranslatef(x, y, 0);
  for(p = text; *p; p++)
    glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
  glPopMatrix();
}

static void init(void)
{
  glEnable(GL_DEPTH_TEST);
  glLightfv(GL_LIGHT0, GL_POSITION, position);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glMaterialfv (GL_FRONT, GL_AMBIENT, mat);
  mat[0] = 0.61424; mat[1] = 0.04136; mat[2] = 0.04136;	
  glMaterialfv (GL_FRONT, GL_DIFFUSE, mat);
  mat[0] = 0.727811; mat[1] = 0.626959; mat[2] = 0.626959;
  glMaterialfv (GL_FRONT, GL_SPECULAR, mat);
  glMaterialf (GL_FRONT, GL_SHININESS, 0.6*128.0);
}

static void renderSphere (GLfloat x, GLfloat y, GLfloat z)
{
  glPushMatrix();
  glTranslatef (x, y, z);
  glutSolidSphere(0.4, 16, 16);
  glPopMatrix();
}

void drawaxes()
{
  /* Next draw the Coordinate Axes */
  glLineWidth(2.0);
  glBegin(GL_LINES);
    glVertex3f(-7,0,0);
    glVertex3f(7,0,0);
  glEnd();
  glBegin(GL_LINES);
    glVertex3f(0,-7,0);
    glVertex3f(0,7,0);
  glEnd();
  glBegin(GL_LINES);   
    glVertex3f(0, 0, -12);
    glVertex3f(0, 0, 7);
  glEnd();
}

/* display() draws 5 spheres at different z positions.
 */
void display(void)
{
  glClearColor(0.5, 0.5, 0.5, 1.0);  /* fog color */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_FOG);

  /*$$$swap-flip GL_LINEAR$$$*/
  glFogi(GL_FOG_MODE, GL_LINEAR);
  /*$$$swap-end$$$*/
  /*$$$swap-flop GL_EXP $$$*/
  glFogi(GL_FOG_MODE, GL_EXP);
  /*$$$swap-end$$$*/
  
  glFogfv(GL_FOG_COLOR, fogColor);
  /*$$$[Density, f, 0.35, 0.0, 1.0, c]$$$*/
  glFogf(GL_FOG_DENSITY, JPOT$1);
  glHint(GL_FOG_HINT, GL_DONT_CARE);
  /*$$$[Start, f, 1.0, 0.0, 1.0, c]$$$*/
  glFogf(GL_FOG_START, JPOT$2);
  /*$$$[Finish, f, 0.5, 0.0, 1.0, c]$$$*/
  glFogf(GL_FOG_END, JPOT$3);
     
  drawaxes();
  renderSphere (-2., -0.5, -1.0);
  renderSphere (-1., -0.5, -2.0);
  renderSphere (0., -0.5, -3.0);
  renderSphere (1., -0.5, -4.0);
  renderSphere (2., -0.5, -5.0);
  glFlush();

  glColor3f(1.0, 1.0, 0.0);
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, 2500, 0, 2500);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  output(50, 1400, "-Z");
  output(1350, 2300, "+Y");
  output(1350, 300, "-Y");  
  
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPopAttrib();
  glMatrixMode(GL_MODELVIEW);

}

void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h)
     glOrtho (-2.5, 2.5, -2.5*(GLfloat)h/(GLfloat)w,
        2.5*(GLfloat)h/(GLfloat)w, -10.0, 10.0);
  else
     glOrtho (-2.5*(GLfloat)w/(GLfloat)h,
        2.5*(GLfloat)w/(GLfloat)h, -2.5, 2.5, -10.0, 10.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity ();
  
}

void idlefunc(void)
{
}


int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(350, 350);
  glutInitWindowPosition(0,0);
  glutCreateWindow("Fog - Parameters");
  init();
  glutReshapeFunc (reshape);
  glutIdleFunc(idlefunc);
  glutDisplayFunc (display);
  glutMainLoop();
  return 0;
}

/*
 * Copyright (c) 1993-1997, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED 
 * Permission to use, copy, modify, and distribute this software for 
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that 
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * US Government Users Restricted Rights 
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */













