#include <GL/glut.h>

static GLfloat light0_position[] = {0.0, 2.0, 2.0, 0.0};
GLfloat mat_specular[]         = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_shininess[]        = {50.0};
GLfloat mat_amb_diff_white[]   = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_amb_diff_green[]   = {0.0, 1.0, 0.0, 1.0};
GLfloat mat_amb_diff_cyan[]    = {0.0, 1.0, 1.0, 1.0}; 

void init()
{ 
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);   
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
} 

void drawaxes()
{
  glLineWidth(2.0);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_white);
  glBegin(GL_LINES);
    glVertex3f(-7,0,0);
    glVertex3f(7,0,0);
    glVertex3f(0,-7,0);
    glVertex3f(0,7,0);
    glVertex3f(0, 0, -7);
    glVertex3f(0, 0, 7);
  glEnd();
}

void drawlocalaxes()
{
  glLineWidth(2.0);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_cyan);
  glBegin(GL_LINES);
    glVertex3f(-3,0,0);
    glVertex3f(3,0,0);
    glVertex3f(0,-3,0);
    glVertex3f(0,3,0);
    glVertex3f(0, 0, -3);
    glVertex3f(0, 0, 3);
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
  /* Clear the matrix */
  glLoadIdentity();     
  gluLookAt(5, 5, 10, 0 , 0 , 0, 0, 1, 0);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  drawaxes();
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_green);

  /*$$$[X1, f, 0.0, -5.0, 5.0, c], [Y1, f, 0.0, -5.0, 5.0, c], 
   [Z1, f, 0.0, -5.0, 5.0, c]$$$*/
  glTranslatef(JPOT$1, JPOT$2, JPOT$3);
  /*$$$[Angle, f, 0.0, -180.0, 180.0, n], [X2, f, 0.0, -5.0, 5.0, n], 
   [Y2, f, 0.0, -5.0, 5.0, c], [Z2, f, 0.0, -5.0, 5.0, c]$$$*/
  glRotatef(JPOT$4, JPOT$5, JPOT$6, JPOT$7);
  glutSolidTeapot(1.0);
  drawlocalaxes();

  glFlush();
}

int main(int argc, char** argv)
{	
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE |GLUT_RGB|GLUT_DEPTH);
  glutInitWindowSize(350, 350);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Combining Translation & Rotation");
  init(); 
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMainLoop();
  return 0;
}
	
/* 
 *  By Binh Phu Le and Mahesh Kumar
 *  parts of this program might have been adopted from or inspired by 
 *  sample programs in 
 *  "OpenGL Programming Guide", 
 *  Copyright (c) 1993-1997, Silicon Graphics, Inc.
 *  "Interactive Computer Graphics"  
 *  Copyright (c) 1997, Addison-Wesley,
 *  and/or those written by Dr. Sumanta Guha guha@cs.uwm.edu
 *  and/or those written by Dr. Ichiro Suzuki suzuki@cs.uwm.edu
 */
