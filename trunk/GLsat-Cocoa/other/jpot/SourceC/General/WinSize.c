#include <stdlib.h>
#include <GL/glut.h>

GLint wide=200, high=200;

void display(void)
{
  /* clear all pixels */
  glClear(GL_COLOR_BUFFER_BIT);
}

void init(void)
{
  /* select clearing (background) color   */
  glClearColor(0.0, 0.0, 0.0, 0.0);
  
  /* initialize viewing values */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

/*
 *  Declare initial window size, position, and display mode
 */

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
  wide = atoi (argv[1]);
  high = atoi (argv[2]);
  
  glutInitWindowSize(wide, high);
  glutInitWindowPosition(0,0); 
  glutCreateWindow("Window Size");
  init();
  
  glutDisplayFunc(display);
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









