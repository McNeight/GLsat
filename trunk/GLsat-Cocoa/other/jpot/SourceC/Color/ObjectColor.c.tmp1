#include <GL/glut.h>

void init()
{  
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

}

void display(void)
{  
  /* clear all pixels */
   glClear(GL_COLOR_BUFFER_BIT);   
  /* draw polygon (rectangle) with corners at 
   *  (0.25, 0.25, 0.0) and (0.75, 0.75, 0.0)
   */
/*$$$[Red, f, 1.0, 0.0, 1.0, c],[Green, f, 1.0, 0.0, 1.0, c],
  [Blue, f, 1.0, 0.0, 1.0, c] $$$*/
  glColor3f(JPOT$1, JPOT$2, JPOT$3);

  glBegin(GL_POLYGON);
    glVertex3f(0.25, 0.25, 0.0);
    glVertex3f(0.75, 0.25, 0.0);
    glVertex3f(0.75, 0.75, 0.0);
    glVertex3f(0.25, 0.75, 0.0);
  glEnd();
  glFlush();
}

void idlefunc(void)
{
}

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
  glutInitWindowSize(450, 450);
  glutInitWindowPosition(0,0);
  glutCreateWindow("Foreground - Object Color");
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
