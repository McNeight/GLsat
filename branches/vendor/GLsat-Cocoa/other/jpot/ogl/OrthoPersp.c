#include <GL/glut.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

GLint please_read_data=0;
FILE * fp=NULL;
GLfloat JPOT$1 = -1.0;
GLfloat JPOT$2 = 1.0;
GLfloat JPOT$3 = -1.0;
GLfloat JPOT$4 = 1.0;
GLfloat JPOT$5 = 2.0;
GLfloat JPOT$6 = 20.0;
GLfloat JPOT$7 = -5.0;
GLfloat JPOT$8 = 5.0;
GLfloat JPOT$9 = -5.0;
GLfloat JPOT$10 = 5.0;
GLfloat JPOT$11 = 2.0;
GLfloat JPOT$12 = 20.0;
char *datafile = "OrthoPersp.dat";


void sighandle(int);

void ReadData(void) {
	if ((fp = fopen(datafile, "r")) == NULL)
	{
	}
	else {
		while ( !feof(fp)) {
fscanf(fp," %f %f %f %f %f %f %f %f %f %f %f %f", &JPOT$1, &JPOT$2, &JPOT$3, &JPOT$4, &JPOT$5, &JPOT$6, &JPOT$7, &JPOT$8, &JPOT$9, &JPOT$10, &JPOT$11, &JPOT$12);
		}
		fclose(fp);
	}
}

void CheckRefresh()
{
  if(please_read_data) {
     please_read_data=0;
     glutPostRedisplay();
  }
}

void sighandle(int s)
{
  please_read_data=1;
  signal(SIGUSR2,sighandle);
}


static GLfloat light0_position[] = {0.0, 2.0, 2.0, 0.0};
GLfloat mat_amb_diff_white[]   = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_amb_diff_red[]     = {1.0, 0.0, 0.0, 1.0};
GLfloat mat_amb_diff_green[]   = {0.0, 1.0, 0.0, 1.0};
GLfloat mat_amb_diff_pink[]   = {1.0, 0.5, 0.5, 1.0};
GLint id1, id2;

void init()
{ 
  signal(SIGUSR2, sighandle);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glShadeModel(GL_SMOOTH);   
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

void drawtable()
{
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_pink);
  glPushMatrix();
  glTranslatef(0.0, -1.0, 0.0);
  glBegin(GL_QUADS);
    glVertex3f(-4.0, 0.0, 4.0);
    glVertex3f(-4.0, 0.0,-4.0);
    glVertex3f( 4.0, 0.0,-4.0);
    glVertex3f( 4.0, 0.0, 4.0);
  glEnd();
  glPopMatrix();
}

void drawpot()
{
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_green);
  glPushMatrix();
    glutSolidTeapot(1.0);
    glTranslatef(0.0, 0.0, -2.0);   
    glutSolidTeapot(1.0);
    glTranslatef(0.0, 0.0, 4.0);
    glutSolidTeapot(1.0);
  glPopMatrix();
}

void reshape1(int w, int h)
{
  glViewport(0,0,(GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(80, (GLfloat) w / (GLfloat) h, 1.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
}

void reshape2(int w, int h)
{
  glViewport(0,0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(JPOT$7, JPOT$8, JPOT$9, JPOT$10, JPOT$11, JPOT$12);
  glMatrixMode(GL_MODELVIEW);
}

void display1()
{	
ReadData();
  glutSetWindow(id1);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
         
  glFrustum(JPOT$1, JPOT$2, JPOT$3, JPOT$4, JPOT$5, JPOT$6);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(2, 4, 10, 0 , 0 , 0, 0, 1, 0);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);	
  
  drawtable();
  drawaxes();
  drawpot();

  glTranslatef(2, 0,- 10);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_red);
  glutSolidSphere(1.0, 40, 40);

  glFlush();
}

void display2()
{	
ReadData();
  glutSetWindow(id2);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();  
       
  glOrtho(JPOT$7, JPOT$8, JPOT$9, JPOT$10, JPOT$11, JPOT$12);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(2, 4, 10, 0 , 0 , 0, 0, 1, 0);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);	
  
  drawtable();
  drawaxes();
  drawpot();

  glTranslatef(2, 0,- 10);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_red);
  glutSolidSphere(1.0, 40, 40);

  glFlush();
}

void idlefunc(void) {
        CheckRefresh();
}
int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE |GLUT_RGB|GLUT_DEPTH);

  glutInitWindowSize(250, 250);
  glutInitWindowPosition(0,0);  
  id1 = glutCreateWindow("Perspective Projection - glFrustum()");
  init();
  glutDisplayFunc(display1);
  glutReshapeFunc(reshape1);

  glutInitWindowSize(250, 250);
  glutInitWindowPosition(0,260);  
  id2 = glutCreateWindow("Orthographic Projection - glOrtho()");
  init(); 
  glutDisplayFunc(display2);
  glutReshapeFunc(reshape2);

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








