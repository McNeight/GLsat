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
   glColor3f (1.0, 1.0, 1.0);

   /*$$$
     [X1, f, 5.0, 0.0, 30.0, c], [Y1, f, 5.0, 0.0, 30.0, c],
     [X2, f,10.0, 0.0, 30.0, c], [Y2, f,20.0, 0.0, 30.0, c],
     [X3, f,20.0, 0.0, 30.0, n], [Y3, f,25.0, 0.0, 30.0, c],
     [X4, f,25.0, 0.0, 30.0, c], [Y4, f, 5.0, 0.0, 30.0, c],
     $$$*/
   glBegin(GL_LINES);
     glVertex2f(JPOT$1, JPOT$2);
     glVertex2f(JPOT$3, JPOT$4);
     glVertex2f(JPOT$5, JPOT$6);
     glVertex2f(JPOT$7, JPOT$8); 
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
