#define ALIASMODE 1
#include <GL/glut.h>

void init(void) 
{  
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  /* initialize viewing values */
  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

void display(void)
{
  if (ALIASMODE == 0) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);  
   }
  /* clear all pixels */
  glClear(GL_COLOR_BUFFER_BIT);

  /* draw red  polygon (triangle)*/ 
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_TRIANGLES);
  /*$$$ [x1, f, 0.25, 0, 1, c], [y1, f, 0.25, 0, 1, c] $$$*/
    glVertex2f(JPOT$1, JPOT$2);
 /*$$$ [x2, f, 0.75, 0, 1, n], [y2, f, 0.25, 0, 1, c] $$$*/
    glVertex2f(JPOT$3, JPOT$4);
 /*$$$ [x3, f, 0.75, 0, 1, n], [y3, f, 0.75, 0, 1, c] $$$*/
    glVertex2f(JPOT$5, JPOT$6);
  glEnd();   
  /* dont wait
   * start processing buffered OpenGL routines
   */
  glFlush();
}

void idlefunc(void)
{
}


int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
  glutInitWindowSize(400, 400);
  glutInitWindowPosition(0,0);
  glutCreateWindow("Drawing Objects");
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







