#define ALIASMODE 1
#include <GL/glut.h>

void init(void) 
{  
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0, 0.0, 0.0);

  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();
  /*$$$[left, f, 0, -1, 1, c], [right, f, 1, -1, 1, c],
  [bottom, f, 0, -1, 1, c], [top, f, 1, -1, 1, c]$$$*/
  gluOrtho2D(JPOT$1, JPOT$2, JPOT$3, JPOT$4);
  glBegin(GL_POLYGON);
    glVertex2f(0.25, 0.25);
    glVertex2f(0.75, 0.25);
    glVertex2f(0.75, 0.75);
  glEnd();
  glFlush();
}

void idlefunc(void)
{
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(400, 400); 
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Absolute / Relative Measure");
  init();
  glutDisplayFunc(display); 
  glutIdleFunc(idlefunc);
  glutMainLoop();
  return 0;
}

/* 
 *  Extended by Binh Phu Le from the original program by Mahesh Kumar
 *  parts of this program might have been adopted from or inspired by 
 *  sample programs in 
 *  "OpenGL Programming Guide", 
 *  Copyright (c) 1993-1997, Silicon Graphics, Inc.
 *  "Interactive Computer Graphics"  
 *  Copyright (c) 1997, Addison-Wesley,
 *  and/or those written by Dr. Sumanta Guha guha@cs.uwm.edu
 *  and/or those written by Dr. Ichiro Suzuki suzuki@cs.uwm.edu
 */

