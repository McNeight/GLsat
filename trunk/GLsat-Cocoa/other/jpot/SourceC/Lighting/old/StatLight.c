#include <GL/glut.h>
#include "mesa_sgi.h"

static GLfloat light0_position[] = {0.0, 5.0, 6.0, 0.0};
GLfloat mat_specular[]         = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_shininess[]        = {50.0};
GLfloat mat_amb_diff_white[]   = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_amb_diff_grey[]    = {0.5, 0.5, 0.5, 1.0};
GLfloat mat_amb_diff_red[]     = {1.0, 0.0, 0.0, 1.0};
GLfloat mat_amb_diff_green[]   = {0.0, 1.0, 0.0, 1.0};
GLfloat mat_amb_diff_blue[]    = {0.0, 0.0, 1.0, 1.0};
GLfloat mat_amb_diff_yellow[]  = {1.0, 1.0, 0.0, 1.0}; 
GLfloat mat_amb_diff_magenta[] = {1.0, 0.0, 1.0, 1.0}; 
GLfloat mat_amb_diff_cyan[]    = {0.0, 1.0, 1.0, 1.0}; 

void init()
{
  glClearColor(0.0, 0.0, 0.0, 0.0);
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
    glVertex3f(0, 0, -7);
    glVertex3f(0, 0, 7);
  glEnd();
}

void drawcube() {
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_red);
  glBegin(GL_POLYGON);
  /* back */
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glVertex3f(1.0, 1.0, -1.0);
  glEnd();
	
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_green);
  glBegin(GL_POLYGON);
  /* left */
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glVertex3f(-1.0, 1.0, -1.0);
  glEnd();

  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_blue);
  glBegin(GL_POLYGON);
  /* right */
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glVertex3f(1.0, 1.0, 1.0);
  glEnd();
			
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_magenta);
  glBegin(GL_POLYGON);
  /* bottom */
    glNormal3f(0.0, -1.0, 0.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(-1.0, -1.0, 1.0);
  glEnd();
  
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_yellow);
  glBegin(GL_POLYGON);
  /* front */
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(1.0, 1.0, 1.0);
    glVertex3f(-1.0, 1.0, 1.0);
  glEnd();	
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_cyan);
  glBegin(GL_POLYGON);
  /* top */
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(1.0, 1.0, -1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glVertex3f(1.0, 1.0, 1.0);
  glEnd();
  /*drawaxes();*/

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
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);   
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLoadIdentity();
  /*$$$swap-flip View Front Side$$$*/
  gluLookAt(-2, 4, -15, 0, 0, 0, 0, 1, 0);
  /*$$$swap-end$$$*/
  /*$$$swap-flop View Back Side$$$*/
  gluLookAt(2, 4, 15, 0 , 0 , 0, 0, 1, 0);
  /*$$$swap-end$$$*/
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  drawaxes();
  drawcube();
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
  glutInitDisplayMode(GLUT_SINGLE |GLUT_RGB|GLUT_DEPTH);
  glutInitWindowSize(350, 350);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Stationary Lighting");
  init(); 
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
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










