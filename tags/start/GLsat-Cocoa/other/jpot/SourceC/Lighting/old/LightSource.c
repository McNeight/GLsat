#include <GL/glut.h>
#include "mesa_sgi.h"

GLfloat material_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat material_shininess[] = { 50 };
GLfloat material_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat material_ambient[] = {1.0, 1.0, 1.0, 1.0};
GLfloat material_emission[] = {0.0, 0.0, 0.0, 1.0};
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0};
GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0};

void init(void)
{
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);   
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
} 

void drawaxes()
{
  /* Next draw the Coordinate Axes */
  glLineWidth(2.0);
  glBegin(GL_LINES);
    glVertex3f(-7,0,0);
    glVertex3f(7,0,0);
  glEnd();
  glBegin(GL_LINES);
    glVertex3f(0,-7,0);
    glVertex3f(0,7,0);
  glEnd();
  glBegin(GL_LINES);   
    glVertex3f(0, 0, -12);
    glVertex3f(0, 0, 7);
  glEnd();
}

void display()
{ 
  if (ALIASMODE == 0) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);  
   }
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);   
  glLoadIdentity();          /* clear the matrix */
  gluLookAt(2, 4, 5, 0 , 0 , 0, 0, 1, 0);
  glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);  
  glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
  glMaterialfv(GL_FRONT, GL_EMISSION, material_emission);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  /*$$$
    [X, f, 1.0, -10.0, 10.0, c], 
    [Y, f, 1.0, -10.0, 10.0, c],
    [Z, f, 1.0, -10.0, 10.0, c],
    [W, f, 1.0, 0.0, 1.0, c]
    $$$*/
  light_position[0] = JPOT$1;
  light_position[1] = JPOT$2;
  light_position[2] = JPOT$3;
  light_position[3] = JPOT$4;
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  drawaxes();
  glTranslatef(0, 0, -3);
  glutSolidSphere(1.0, 30, 30);;
  glFlush();
}

void reshape (int w, int h)
{
  glViewport(0,0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -1.0, 1.0, 2.0, 40.0);
  glMatrixMode(GL_MODELVIEW);
}

void idlefunc(void)
{
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (350, 350); 
   glutInitWindowPosition (0, 0);
   glutCreateWindow ("Material Ambience");
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
