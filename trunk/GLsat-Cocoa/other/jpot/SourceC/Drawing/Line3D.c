#include <GL/glut.h>
#include <stdlib.h>

static GLfloat light0_position[] = {0.0, 2.0, 2.0, 0.0};
GLfloat mat_specular[]         = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_amb_diff_white[]   = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_amb_diff_cyan[]    = {0.0, 1.0, 1.0, 1.0}; 
GLfloat mat_amb_diff_grey[]    = {0.5, 0.5, 0.5, 1.0}; 

void init(void) 
{
  glClearColor (0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);   
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}

void drawaxes()
{
  glLineWidth(2.0);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_grey);
  glBegin(GL_LINES);
    glVertex3f(-7,0,0);
    glVertex3f(7,0,0);
    glVertex3f(0,-7,0);
    glVertex3f(0,7,0);
    glVertex3f(0, 0, -12);
    glVertex3f(0, 0, 7);
  glEnd();
}

void reshape (int w, int h) 
{
  glViewport(0,0, (GLsizei) w, (GLsizei) h); 
  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity(); 
  gluPerspective(45.0, (GLfloat) w / (GLfloat) h, 2.0, 20.0); 
  glMatrixMode(GL_MODELVIEW); 
}

void display(void)
{
   int i;

   glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glLoadIdentity(); 
   gluLookAt(5, 3, 15, 0 , 0 , 0, 0, 1, 0);
   glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
   drawaxes();
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_white);
   
   glBegin(GL_LINES);
     glVertex3f(-3.0,-3.0, 3.0);
     glVertex3f(-3.0, 3.0,-3.0);
     glVertex3f( 3.0, 3.0, 3.0);
     glVertex3f( 3.0,-3.0,-3.0); 
     //glVertex3f(-.0, 3.0, 2.0); 
     //glVertex3f( 2.0, 3.0,-2.0);
   glEnd();

   glFlush ();
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
