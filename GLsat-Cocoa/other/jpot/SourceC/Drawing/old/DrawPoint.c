#include <GL/glut.h>

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void display(void)
{

   glClear (GL_COLOR_BUFFER_BIT);

   /* select white for all points  */
   glColor3f (1.0, 1.0, 1.0);
   /* set the point's size for the first point*/
   /*$$$[First Point Size, f, 1.0, 0.0, 10.0, c] $$$*/
   glPointSize(JPOT$1);
   /* draw the first point */
   glBegin(GL_POINTS);
   /*$$$[X1, f, 10.0, 0.0, 100.0, c], [Y1,f, 10.0, 0.0, 100.0,c]$$$*/
     glVertex2f(JPOT$2, JPOT$3);
   glEnd();
   /* set the point's size for the second point*/
   /*$$$[Second Point Size, f, 1.0, 0.0, 10.0, n] $$$*/
   glPointSize(JPOT$4);
   /* draw the second point */
   glBegin(GL_POINTS);
   /*$$$[X2, f, 30.0, 0.0, 100.0, c], [Y2,f, 30.0, 0.0, 100.0,c]$$$*/
     glVertex2f(JPOT$5, JPOT$6);
   glEnd();    
   glFlush ();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluOrtho2D (0.0, (GLdouble) w, 0.0, (GLdouble) h);
}

void idlefunc(void)
{
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (300, 300); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
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

