#include <GL/glut.h>

static GLfloat light0_position[] = {0.0, 2.0, 2.0, 0.0};
GLfloat mat_amb_diff_white[]   = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_amb_diff_green[]   = {0.0, 1.0, 0.0, 1.0};

void init()
{
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
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
    glVertex3f(0, 0, -12);
    glVertex3f(0, 0, 7);
  glEnd();
}

void display(){	
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();         

/*$$$
  [cameraX, f, 3.0, -10.0, 10.0, c],
  [cameraY, f, 4.0, -10.0, 10.0, c],
  [cameraZ, f, 10.0,-15.0, 15.0, c],
  [aimX, f, 0.0, -5.0, 5.0, n],
  [armY, f, 0.0, -5.0, 5.0, c],
  [armZ, f, 0.0, -5.0, 5.0, c],
  [upVecX, f, 0.0, -1.0, 1.0, n],
  [upVecY, f, 1.0, -1.0, 1.0, c],
  [upVecZ, f, 0.0, -1.0, 1.0, c]
  $$$*/
  gluLookAt(JPOT$1, JPOT$2, JPOT$3, JPOT$4, JPOT$5, JPOT$6, JPOT$7, JPOT$8,JPOT$9);

  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  drawaxes();
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_green);
  glutSolidTeapot(1.0);
   
  glFlush();
}

void reshape (int w, int h)
{
  glViewport(0,0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
  glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE |GLUT_RGB|GLUT_DEPTH);
  glutInitWindowSize(350, 350);
  glutInitWindowPosition(0,0);
  glutCreateWindow("gluLookAt Example");
  init(); 
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMainLoop();
  return 0;
}
	

/* 
 *  Extended by Binh Phu Le from the original progam by Mahesh Kumar
 *  parts of this program might have been adopted from or inspired by 
 *  sample programs in 
 *  "OpenGL Programming Guide", 
 *  Copyright (c) 1993-1997, Silicon Graphics, Inc.
 *  "Interactive Computer Graphics"  
 *  Copyright (c) 1997, Addison-Wesley,
 *  and/or those written by Dr. Sumanta Guha guha@cs.uwm.edu
 *  and/or those written by Dr. Ichiro Suzuki suzuki@cs.uwm.edu
 */







