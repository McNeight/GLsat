#include <GL/glut.h>

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
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_SMOOTH);  
}


void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(45.0, (GLfloat) w / (GLfloat) h, 2.0, 20.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(3.0, 4.0, 12.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0);
}


void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0, 0.0, 0.0);
  glLineWidth(4.0);
  glBegin(GL_LINES);
    glVertex3f(-5.0, 0.0, 0.0);
    glVertex3f(4.5, 0.0, 0.0);
  glEnd();

  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_LINES);
    glVertex3f(0.0, 4.5, 0.0);
    glVertex3f(0.0, -5.0,0.0);
  glEnd();

  glColor3f(1.0, 1.0, 0.0);
  glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, -7.0);
    glVertex3f(0.0, 0.0, 5.0);
  glEnd();
    
  glLineWidth(1.0);
  glColor3f(1.0, 0.0, 0.0);
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, 2500, 0, 2500);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  output(50, 1400, "-X");
  output(2000, 1250, "+X");
  glColor3f(0.0, 1.0, 0.0);
  output(1350, 2300, "+Y");
  output(1350, 300, "-Y");  
  glColor3f(1.0, 1.0, 0.0);
  output(1500, 1450, "-Z");
  output(400, 750, "+Z");
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPopAttrib();
  glMatrixMode(GL_MODELVIEW);
	
  glFlush();    
}


int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize(400, 400); 
   glutInitWindowPosition(0, 0);
   glutCreateWindow("Coordinate System");
   init();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape); 
   glutMainLoop();
   return 0;
}


/* 
 *  By Mahesh Kumar parts of this program might have been adopted 
 *  from or inspired by sample programs in 
 *  "OpenGL Programming Guide", 
 *  Copyright (c) 1993-1997, Silicon Graphics, Inc.
 *  "Interactive Computer Graphics"  
 *  Copyright (c) 1997, Addison-Wesley,
 *  and/or those written by Dr. Sumanta Guha guha@cs.uwm.edu
 *  and/or those written by Dr. Ichiro Suzuki suzuki@cs.uwm.edu
 */
