#include <GL/glut.h>
#include <stdlib.h>

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void display(void)
{

   glClear (GL_COLOR_BUFFER_BIT);
   /*$$$
     [Polygon Mode, s, GL_LINE, GL_POINT 6912 GL_LINE 6913 GL_FILL 6914, c]
     $$$*/
   glPolygonMode(GL_FRONT_AND_BACK, JPOT$1);
   glColor3f (1.0, 1.0, 1.0);

   /*$$$
     [X0, f, 5.0, 0.0, 30.0, n], [Y0, f, 5.0, 0.0, 30.0, c],
     [X1, f,10.0, 0.0, 30.0, n], [Y1, f,20.0, 0.0, 30.0, c],
     [X2, f,20.0, 0.0, 30.0, n], [Y2, f,25.0, 0.0, 30.0, c],
     [X3, f,25.0, 0.0, 30.0, n], [Y3, f, 5.0, 0.0, 30.0, c],
     $$$*/
   glBegin(GL_POLYGON);
     glVertex2f(JPOT$2, JPOT$3);
     glVertex2f(JPOT$4, JPOT$5);
     glVertex2f(JPOT$6, JPOT$7);
     glVertex2f(JPOT$8, JPOT$9);
   glEnd();

   glFlush ();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluOrtho2D (0.0, (GLdouble) w/10, 0.0, (GLdouble) h/10);
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (300, 300); 
   glutInitWindowPosition (0, 0);
   glutCreateWindow (argv[0]);
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
