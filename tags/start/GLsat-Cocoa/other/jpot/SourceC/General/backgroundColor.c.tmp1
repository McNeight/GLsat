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
/*$$$[Red, f, 1.0, 0.0, 1.0, c],[Green, f, 1.0, 0.0, 1.0, c],
  [Blue, f, 1.0, 0.0, 1.0, c] $$$*/
   glClearColor(JPOT$1, JPOT$2, JPOT$3, 0.0);
   glClear(GL_COLOR_BUFFER_BIT);
   glFlush();
}

/*
 *  Declare initial window size, position, and display mode
 *  (single buffer and RGBA).  Call initialization routines.
 *  Register callback function to display graphics.
 *  Enter main loop and process events.
 */

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
  glutInitWindowSize(250, 250);
  glutInitWindowPosition(0,0);
  glutCreateWindow("Background Color");
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



