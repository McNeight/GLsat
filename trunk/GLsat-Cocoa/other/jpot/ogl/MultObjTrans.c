#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

GLint please_read_data=0;
FILE * fp=NULL;
GLfloat JPOT$1 = 0.0;
GLfloat JPOT$2 = 0.0;
GLfloat JPOT$3 = 0.0;
GLfloat JPOT$4 = 3.0;
GLfloat JPOT$5 = 0.0;
GLfloat JPOT$6 = 0.0;
char *datafile = "MultObjTrans.dat";


void sighandle(int);

void ReadData(void) {
	if ((fp = fopen(datafile, "r")) == NULL)
	{
	}
	else {
		while ( !feof(fp)) {
fscanf(fp," %f %f %f %f %f %f", &JPOT$1, &JPOT$2, &JPOT$3, &JPOT$4, &JPOT$5, &JPOT$6);
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
GLfloat mat_specular[]         = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_shininess[]        = {50.0};
GLfloat mat_amb_diff_white[]   = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_amb_diff_grey[]    = {0.5, 0.5, 0.5, 1.0};
GLfloat mat_amb_diff_green[]   = {0.0, 1.0, 0.0, 1.0};

void init()
{ 
  signal(SIGUSR2, sighandle);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);   
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
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
ReadData();
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);   
  glLoadIdentity();    
  gluLookAt(4, 5, 12, 0 , 0 , 0, 0, 1, 0);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  drawaxes();
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_green);

  glTranslatef(JPOT$1, JPOT$2, JPOT$3);
  glutSolidTeapot(1.0);
  glTranslatef(JPOT$4, JPOT$5, JPOT$6);  
  glutSolidSphere(1.0, 8, 8);

  glFlush();
}

void idlefunc(void) {
        CheckRefresh();
}
int main(int argc, char** argv)
{	
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE |GLUT_RGB|GLUT_DEPTH);
  glutInitWindowSize(350,350);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Case Study: Two Translation");
  init(argc, argv); 
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
glutIdleFunc(idlefunc);
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
