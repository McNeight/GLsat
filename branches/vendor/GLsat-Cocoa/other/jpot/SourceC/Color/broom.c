/*
 *psychedelic_broom.c
 *
 */
#include <GL/glut.h>
#include <time.h>
#include<stdlib.h>

GLubyte clr0[3], clr1[3], clr2[3], clr3[3];

void init(void)
{
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);

}

void display(void)
{
  GLint i;
  glClear(GL_COLOR_BUFFER_BIT);
     
  /* Draw the two base triangles */
  glBegin(GL_TRIANGLES);
    glColor3f(0.52, 0.24, 0.75);
    glVertex3f(0.0, 0.0, 0.0);
    glColor3f(0.25, 0.42, 0.35);
    glVertex3f(7.5,16.0, 0.0);
    glColor3f(0.90, 0.68, 0.23);
    glVertex3f(16.0, 0.0, 0.0);
    glEnd();

  glBegin(GL_TRIANGLES);
    glColor3f(0.24, 0.52, 0.75);
    glVertex3f(2.0, 4.0, 0.0);
    glColor3f(0.43, 0.96, 0.35);
    glVertex3f(9.5,20.0, 0.0);
    glColor3f(0.16, 0.68, 0.75);
    glVertex3f(16.0, 0.0, 0.0);
  glEnd();
  /* Draw the vertical pole */
  glBegin(GL_POLYGON);
    glColor3f(0.3, 0.2, 0.6);
    glVertex3f(7.9, 16.75,0.0);
    glColor3f(0.7, 0.9, 0.4);
    glVertex3f(7.9, 55.0, 0.0);
    glColor3f(0.29, 0.48, 0.56);
    glVertex3f(9.9, 55.0, 0.0);
    glColor3f(0.54, 0.78, 0.89);
    glVertex3f(9.9, 18.0, 0.0);
  glEnd();

for (i=0;i<100; i++)
  {
    clr0[0] = rand()%256;
    clr0[1] = rand()%256;
    clr0[2] = rand()%256;
    clr1[0] = rand()%256;
    clr1[1] = rand()%256;
    clr1[2] = rand()%256;
    clr2[0] = rand()%256;
    clr2[1] = rand()%256;
    clr2[2] = rand()%256;
    clr3[0] = rand()%256;
    clr3[1] = rand()%256;
    clr3[2] = rand()%256;


  glBegin(GL_TRIANGLE_STRIP);
    glColor3ubv(clr0);   
    glVertex3f(9.9, 55.0, 0.0);/* vertex v0 */
    glColor3ubv(clr1);
    glVertex3f(9.9, 40.0, 0.0);/* vertex v1 */
    glColor3ubv(clr2);   
    glVertex3f(12.0, 55.0, 0.0);/* vertex v2 */
    glColor3ubv(clr3);   
    glVertex3f(12.0, 40.0, 0.0);/* vertex v3 */
  glEnd();  
  
  glFlush();  /* force draw */

  sleep(2);
  }

}

void reshape (int w, int h)
{
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 20.0, -1.0, 20.0, 1.5, 20.0 );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 5.0);

}

int main(int argc, char** argv)
{
  srand(time(NULL));  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(350, 350);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Psychedelic broom");
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMainLoop();
  return 0;

}

/* 
 *  by Mahesh Kumar
 *  parts of this program might have been adopted from or inspired by 
 *  sample programs in 
 *  "OpenGL Programming Guide", 
 *  Copyright (c) 1993-1997, Silicon Graphics, Inc.
 *  "Interactive Computer Graphics"  
 *  Copyright (c) 1997, Addison-Wesley,
 *  and/or those written by Dr. Sumanta Guha guha@cs.uwm.edu
 *  and/or those written by Dr. Ichiro Suzuki suzuki@cs.uwm.edu
 */

