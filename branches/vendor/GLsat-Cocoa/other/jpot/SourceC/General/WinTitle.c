#include <stdlib.h>
#include <GL/glut.h>

char * title;
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
  title = (argv[1]);
  
  glutInitWindowSize(400, 400);
  glutInitWindowPosition(0,0); 
  glutCreateWindow(title);
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










