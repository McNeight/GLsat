#include <GL/glut.h>
#include <stdlib.h>

GLfloat scatt=1.5, slatt= 0.5, sqatt=0.2;
GLfloat scut=10.0;
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 50 };
GLfloat mat_amb_diff_white[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_emission[] = {0.0, 0.0, 0.0, 1.0};
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
GLfloat light1_ambient[] = {0.2, 0.2, 0.2, 1.0};
GLfloat light1_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light1_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light1_position[] = {-2.0, 0.0, 2.0, 1.0};
GLfloat spot_direction[] = { 2.0, 0.0,-2.0};

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);  
   glEnable(GL_DEPTH_TEST);
}

void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30, (GLfloat) w / (GLfloat) h, 1.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_white);
   glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

   glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
   glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
   glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
   /*$$$[Spot Const Att, f, 1.5, 0.0, 5.0, c]$$$*/
   glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, JPOT$1);
   /*$$$[Spot Linear Att, f, 0.5, 0.0, 5.0, c]$$$*/
   glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, JPOT$2);
   /*$$$[Spot Quad Att, f, 0.2, 0.0, 5.0, n]$$$*/
   glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, JPOT$3);
   /*$$$[Spot CutOff, f, 10.0, 0.0, 20.0, c]$$$*/
   glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, JPOT$4);
   /*$$$
     [Direction X, f, 2.0, -5.0, 5.0, n],
     [Direction Y, f, 0.0, -5.0, 5.0, c],
     [Direction Z, f,-2.0, -5.0, 5.0, c]
     $$$*/
   spot_direction[0] = JPOT$5; spot_direction[1] = JPOT$6; spot_direction[2] = JPOT$7;
   glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
   glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);
   glEnable(GL_DEPTH_TEST);
   glutSolidTeapot(1.0);
   glFlush ();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (300, 300); 
   glutInitWindowPosition (0, 0);
   glutCreateWindow ("A Second (Spot) Light Source");
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutMainLoop();
   return 0;
}

/* 
 *  By Binh Phu Le
 *  parts of this program might have been adopted from or inspired by 
 *  sample programs in 
 *  "OpenGL Programming Guide", 
 *  Copyright (c) 1993-1997, Silicon Graphics, Inc.
 *  "Interactive Computer Graphics"  
 *  Copyright (c) 1997, Addison-Wesley,
 *  and/or those written by Dr. Sumanta Guha guha@cs.uwm.edu
 *  and/or those written by Dr. Ichiro Suzuki suzuki@cs.uwm.edu
 */

