#include <GL/glut.h>
#include <stdlib.h>

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluOrtho2D (0.0, (GLdouble) w/10, 0.0, (GLdouble) h/10);
}

void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT);
   glColor3f (1.0, 1.0, 1.0);
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

   /*$$$
     [Triangle Mode, s, GL_TRIANGLES, GL_TRIANGLES 4 GL_TRIANGLE_STRIP 5 GL_TRIANGLE_FAN 6, c]
     $$$*/
   glBegin(JPOT$1);
   /*$$$
     [X0, f, 5.0, 0.0, 30.0, n], [Y0, f,25.0, 0.0, 30.0, c],
     [X1, f,10.0, 0.0, 30.0, n], [Y1, f,25.0, 0.0, 30.0, c],
     [X2, f,15.0, 0.0, 30.0, n], [Y2, f,10.0, 0.0, 30.0, c],
     [X3, f,25.0, 0.0, 30.0, n], [Y3, f, 5.0, 0.0, 30.0, c],
     [X4, f,25.0, 0.0, 30.0, n], [Y4, f,20.0, 0.0, 30.0, c],
     $$$*/
     glVertex2f(JPOT$2, JPOT$3);
     glVertex2f(JPOT$4, JPOT$5);
     glVertex2f(JPOT$6, JPOT$7);
     glVertex2f(JPOT$8, JPOT$9);
     glVertex2f(JPOT$10, JPOT$11);
   glEnd();

   glFlush ();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (300, 300); 
   glutInitWindowPosition (0, 0);
   glutCreateWindow ("Triangle Mode");
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
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
