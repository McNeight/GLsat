#include <GL/glut.h>
#include <stdlib.h>

void output(GLfloat x, GLfloat y, char *text)
{
  char *p;
  glPushMatrix();
  glTranslatef(x, y, 0);
  for(p = text; *p; p++)
    glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
  glPopMatrix();
}

void init(void) 
{
   glClearColor(0.8, 0.8, 0.8, 0.0);
   glShadeModel(GL_SMOOTH);
}


void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(70.0, (GLfloat) w / (GLfloat) h, 2.0, 20.0);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(2.0, 3.0, 4.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  
  glColor3f(1.0, 0.0, 0.0);
  glTranslatef(-3, -1, 0);
  glLineWidth(4.0);
  glBegin(GL_LINE_LOOP);
    glVertex3f(-1.5, 1, -2);
    glVertex3f(-3.5, 2.5, -14);
    glVertex3f(-3.5, -2.5, -14);
    glVertex3f(-1.5, -1, -2);
  glEnd();
  glBegin(GL_LINE_LOOP);
    glVertex3f(-1.5, 1, -2);
    glVertex3f(-1.5, -1, -2);
    glVertex3f(1.5, -1, -2);
    glVertex3f(1.5, 1, -2);
  glEnd(); 
  glBegin(GL_LINE_LOOP);
    glVertex3f(1.5, 1, -2);
    glVertex3f(3.5, 2.5, -14);
    glVertex3f(3.5, -2.5, -14);
    glVertex3f(1.5, -1, -2);
  glEnd();
  glBegin(GL_LINE_LOOP);
    glVertex3f(-3.5, 2.5, -14);
    glVertex3f(-3.5, -2.5, -14);
    glVertex3f(3.5, -2.5, -14);
    glVertex3f(3.5, 2.5, -14);
  glEnd();
  
  glLineWidth(1.0);
  glColor3f(0.0, 0.0, 0.0);
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, 2500, 0, 2500);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  output(300, 1400, "Left");
  output(1200, 1350, "Right");
  output(1450, 2175, "Far Clip Plane");
  output(1450, 1150, "Near Clip Plane");  
  output(800, 1750, "Top");
  output(700, 1000, "Bottom");
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPopAttrib();
  glMatrixMode(GL_MODELVIEW);  
}


int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize(350, 350); 
   glutInitWindowPosition(0, 0);
   glutCreateWindow("Frustum - A Truncated pyramid");
   init();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape); 
   glutMainLoop();
   return 0;
}

/* 
 *  by Mahesh Kumar
 *  parts of this program might have been adopted from or inspired by 
 *  sample programs in 
 *  "OpenGL Programming Guide", 
 *  Copyright (c) 1993-1997, Silicon Graphics, Inc.
 *  "Interactive Computer Graphics"  
 *  Copyright (c) 1997, Addison-Wesley,
 *  and/or those written by Dr. Sumanta Guha guha@cs.uwm.edu
 *  and/or those written by Dr. Ichiro Suzuki suzuki@cs.uwm.edu
 */
