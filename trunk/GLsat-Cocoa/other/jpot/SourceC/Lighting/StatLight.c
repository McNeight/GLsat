#include <GL/glut.h>

static GLfloat light0_position[] = {0.0, 5.0, 6.0, 0.0};
GLfloat mat_amb_diff_white[]   = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_amb_diff_green[]   = {0.0, 1.0, 0.0, 1.0};

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   glEnable(GL_DEPTH_TEST);
}

void drawaxes()
{
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_white);
  glLineWidth(2.0);
  glBegin(GL_LINES);
    glVertex3f(-7,0,0);
    glVertex3f(7,0,0);
    glVertex3f(0,-7,0);
    glVertex3f(0,7,0);
    glVertex3f(0, 0, -7);
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

void display()
{ 
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);   
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLoadIdentity();
  /*$$$swap-flip View Right$$$*/
  gluLookAt(2, 4, 15, 0 , 0 , 0, 0, 1, 0);
  /*$$$swap-end$$$*/
  /*$$$swap-flop View Left$$$*/
  gluLookAt(-2, 4, -15, 0, 0, 0, 0, 1, 0);
  /*$$$swap-end$$$*/
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  drawaxes();
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_green);
  glutSolidTeapot(1.0);
  glFlush();
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
  glutMainLoop();
  return 0;
}
	
/* 
 *  by Binh Phu Le and by Mahesh Kumar
 *  parts of this program might have been adopted from or inspired by 
 *  sample programs in 
 *  "OpenGL Programming Guide", 
 *  Copyright (c) 1993-1997, Silicon Graphics, Inc.
 *  "Interactive Computer Graphics"  
 *  Copyright (c) 1997, Addison-Wesley,
 *  and/or those written by Dr. Sumanta Guha guha@cs.uwm.edu
 *  and/or those written by Dr. Ichiro Suzuki suzuki@cs.uwm.edu
 */










