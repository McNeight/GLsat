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
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_POLYGON);
    /*$$$[X1, f, 0.25, 0.0, 1.0, c],[Y1, f, 0.25, 0.0, 1.0, c] $$$*/
    glVertex2f(JPOT$1, JPOT$2);
    /*$$$[X2, f, 0.75, 0.0, 1.0, n],[Y2, f, 0.25, 0.0, 1.0, c] $$$*/
    glVertex2f(JPOT$3, JPOT$4);
    /*$$$[X3, f, 0.75, 0.0, 1.0, n],[Y3, f, 0.75, 0.0, 1.0, c] $$$*/
    glVertex2f(JPOT$5, JPOT$6);
    /*$$$[X4, f, 0.25, 0.0, 1.0, n],[Y4, f, 0.75, 0.0, 1.0, c] $$$*/
    glVertex2f(JPOT$7, JPOT$8);
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
  glutCreateWindow("Drawing Polygon");
  init();
  glutDisplayFunc(display);
  glutIdleFunc(idlefunc);
  glutMainLoop();
  return 0;
}


	
/* 
 *  by Binh Phu Le
 *  parts of this program might have been adopted from or inspired by 
 *  sample programs in 
 *  "OpenGL Programming Guide", 
 *  Copyright (c) 1993-1997, Silicon Graphics, Inc.
 *  "Interactive Computer Graphics"  
 *  Copyright (c) 1997, Addison-Wesley,
 *  and/or those written by Dr. Sumanta Guha guha@cs.uwm.edu
 *  and/or those written by Dr. Ichiro Suzuki suzuki@cs.uwm.edu
 */
