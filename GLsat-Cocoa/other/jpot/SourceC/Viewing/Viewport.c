#include<GL/glut.h>

GLint width, height;
  
void draw_cube(int x, int y, int w, int h)
{
  glMatrixMode (GL_MODELVIEW);	/*  back to modelview matrix	*/
  glLoadIdentity ();	/*  clear the matrix	*/
  glViewport((GLint) x,(GLint) y, (GLint) w, (GLint) h);
  glTranslatef (0.0, 0.0, -5.0);	/*  viewing transformation	*/
  glutWireCube(1.0);	/*  draw the cube	*/
  glFlush();    
}

void display (void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  /*$$$swap-flip One Viewport $$$*/
  glColor3f (1.0, 0.0, 0.0);
  draw_cube(0,0, width, height);
  /*$$$swap-end$$$*/
  /*$$$swap-flop Two Viewports$$$*/
  glColor3f (1.0, 0.0, 0.0);
  draw_cube(0, 0, width/2.0, height);
  glColor3f (0.0, 1.0, 0.0);
  draw_cube(width/2.0, 0, width/2.0, height);
  /*$$$swap-end$$$*/
}
	  
void init (void) {
    glShadeModel (GL_FLAT);
}

void reshape(int w, int h)
{
  glMatrixMode (GL_PROJECTION);	/*  prepare for and then  */ 
  glLoadIdentity ();	/*  define the projection  */
  gluPerspective(60.0, (GLfloat) w / (GLfloat) h, 1.5, 20.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  width = w;
  height = h;
}

void idlefunc(void)
{
}
int main(int argc, char** argv)
{
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(350, 350);
    glutCreateWindow ("Viewport Transformation");
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
 *  and/or those written by Mahesh Kumar kumar@cs.uwm.edu
 *  and/or those written by Dr. Sumanta Guha guha@cs.uwm.edu
 *  and/or those written by Dr. Ichiro Suzuki suzuki@cs.uwm.edu
 */
