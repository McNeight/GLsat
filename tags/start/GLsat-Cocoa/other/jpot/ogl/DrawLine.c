#include <GL/glut.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

GLint please_read_data=0;
FILE * fp=NULL;
GLint JPOT$1 = GL_LINES;
GLfloat JPOT$2 = 5.0;
GLfloat JPOT$3 = 5.0;
GLfloat JPOT$4 = 10.0;
GLfloat JPOT$5 = 20.0;
GLfloat JPOT$6 = 20.0;
GLfloat JPOT$7 = 25.0;
GLfloat JPOT$8 = 25.0;
GLfloat JPOT$9 = 5.0;
char *datafile = "DrawLine.dat";


void sighandle(int);

void ReadData(void) {
	if ((fp = fopen(datafile, "r")) == NULL)
	{
	}
	else {
		while ( !feof(fp)) {
fscanf(fp," %d %f %f %f %f %f %f %f %f", &JPOT$1, &JPOT$2, &JPOT$3, &JPOT$4, &JPOT$5, &JPOT$6, &JPOT$7, &JPOT$8, &JPOT$9);
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


void init(void) 
{
  signal(SIGUSR2, sighandle);
  glClearColor (0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);   
}

void reshape (int w, int h) 
{
  glViewport(0,0, (GLsizei) w, (GLsizei) h); 
  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity(); 
  gluOrtho2D (0.0, (GLdouble) w/10, 0.0, (GLdouble) h/10);
  glMatrixMode(GL_MODELVIEW); 
}

void display(void)
{
ReadData();

   glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 
   glColor3f(1.0, 1.0, 1.0);
   
   glBegin(JPOT$1);
     glVertex2f(JPOT$2, JPOT$3);
     glVertex2f(JPOT$4, JPOT$5);
     glVertex2f(JPOT$6, JPOT$7);
     glVertex2f(JPOT$8, JPOT$9); 
   glEnd();

   glFlush ();
}

void idlefunc(void) {
        CheckRefresh();
}
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (300, 300); 
   glutInitWindowPosition (0, 0);
   glutCreateWindow (argv[0]);
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
