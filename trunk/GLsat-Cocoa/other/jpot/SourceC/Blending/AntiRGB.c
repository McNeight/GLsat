#include<GL/glut.h>

void init (void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel (GL_FLAT);
}

void display (void)
{   
  glClear(GL_COLOR_BUFFER_BIT);
  /*$$$swap-flip AntiAliased $$$*/
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable (GL_LINE_SMOOTH);
  glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
  /*$$$swap-end$$$*/
  /*$$$swap-flop Aliased $$$*/
  glDisable(GL_BLEND);
  glDisable(GL_LINE_SMOOTH);
  /*$$$swap-end$$$*/
  glColor3f (1.0, 0.0, 0.0);
  glutWireCube(1.0);
  glFlush();
}

void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode (GL_PROJECTION);	/*  prepare for and then  */ 
  glLoadIdentity ();	/*  define the projection  */
  gluPerspective(45.0, (GLfloat) w / (GLfloat) h, 2.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(2.0, 4.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void idlefunc(void)
{
}


int main(int argc, char** argv)
{
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(350, 350);
    glutCreateWindow ("AntiAliasing RGBA Mode");
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
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

