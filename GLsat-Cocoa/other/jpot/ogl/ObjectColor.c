#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

GLint please_read_data=0;
FILE * fp=NULL;
GLfloat JPOT$1 = 1.0;
GLfloat JPOT$2 = 1.0;
GLfloat JPOT$3 = 1.0;
char *datafile = "ObjectColor.dat";


void sighandle(int);

void ReadData(void) {
	if ((fp = fopen(datafile, "r")) == NULL)
	{
	}
	else {
		while ( !feof(fp)) {
fscanf(fp," %f %f %f", &JPOT$1, &JPOT$2, &JPOT$3);
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


void init()
{  
  signal(SIGUSR2, sighandle);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

}

void display(void)
{  
ReadData();
  /* clear all pixels */
   glClear(GL_COLOR_BUFFER_BIT);   
  /* draw polygon (rectangle) with corners at 
   *  (0.25, 0.25, 0.0) and (0.75, 0.75, 0.0)
   */
  glColor3f(JPOT$1, JPOT$2, JPOT$3);

  glBegin(GL_POLYGON);
    glVertex3f(0.25, 0.25, 0.0);
    glVertex3f(0.75, 0.25, 0.0);
    glVertex3f(0.75, 0.75, 0.0);
    glVertex3f(0.25, 0.75, 0.0);
  glEnd();
  glFlush();
}

void idlefunc(void)
{
CheckRefresh();
}

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
  glutInitWindowSize(300, 300);
  glutInitWindowPosition(0,0);
  glutCreateWindow("Foreground - Object Color");
  init();
  glutDisplayFunc(display);
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
