#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

GLint please_read_data=0;
FILE * fp=NULL;
GLfloat JPOT$1 = 1.0;
GLfloat JPOT$2 = 1.0;
GLfloat JPOT$3 = 1.0;
char *datafile = "BackColor.dat";


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
   glClearColor(JPOT$1, JPOT$2, JPOT$3, 0.0);
   glClear(GL_COLOR_BUFFER_BIT);
   glFlush();
}

/*
 *  Declare initial window size, position, and display mode
 *  (single buffer and RGBA).  Call initialization routines.
 *  Register callback function to display graphics.
 *  Enter main loop and process events.
 */

void idlefunc(void) {
        CheckRefresh();
}
int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
  glutInitWindowSize(250, 250);
  glutInitWindowPosition(0,0);
  glutCreateWindow("Background Color");
  init();
  glutDisplayFunc(display);
  glutIdleFunc(idlefunc);
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
 *  and/or those written by Dr. Sumanta Guha guha@cs.uwm.edu
 *  and/or those written by Dr. Ichiro Suzuki suzuki@cs.uwm.edu
 */



