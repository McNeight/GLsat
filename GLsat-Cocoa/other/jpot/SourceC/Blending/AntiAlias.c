#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

void init(void)
{
   glEnable (GL_BLEND);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
   glLineWidth (5.0);
   glPointSize(10.0);
   glClearColor(0.0, 0.0, 0.0, 0.0);
}

void reshape(int w, int h)
{
   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h) 
      gluOrtho2D (-1.0, 1.0, 
         -1.0*(GLfloat)h/(GLfloat)w, 1.0*(GLfloat)h/(GLfloat)w);
   else 
      gluOrtho2D (-1.0*(GLfloat)w/(GLfloat)h, 
         1.0*(GLfloat)w/(GLfloat)h, -1.0, 1.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   /*$$$swap-flip Enable Antialiasing$$$*/
   glEnable(GL_LINE_SMOOTH);
   glEnable(GL_POINT_SMOOTH);
   /*$$$swap-end$$$*/
   /*$$$swap-flop Disable Antialiasing$$$*/
   glDisable(GL_LINE_SMOOTH);
   glDisable(GL_POINT_SMOOTH);
   /*$$$swap-end$$$*/

   glColor3f (1.0, 1.0, 0.0);
   glBegin (GL_LINES);
      glVertex2f (0.5, 0.05);
      glVertex2f (-0.5, -0.05);
   glEnd ();

   glFlush();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (350, 350);
   glutInitWindowPosition(0, 0);
   glutCreateWindow (argv[0]);
   init();
   glutReshapeFunc (reshape);
   glutDisplayFunc (display);
   glutMainLoop();
   return 0;
}


/* 
 *  by Ichiro Suzuki
 *  parts of this program might have been adopted from or inspired by 
 *  sample programs in 
 *  "OpenGL Programming Guide", 
 *  Copyright (c) 1993-1997, Silicon Graphics, Inc.
 *  "Interactive Computer Graphics"  
 *  Copyright (c) 1997, Addison-Wesley,
 *  and/or those written by Dr. Sumanta Guha guha@cs.uwm.edu
 *  and/or those written by Dr. Ichiro Suzuki suzuki@cs.uwm.edu
 */
