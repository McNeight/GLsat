#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

GLint please_read_data=0;
FILE * fp=NULL;
GLfloat JPOT$1 = 1.0;
GLfloat JPOT$2 = 1.0;
GLfloat JPOT$3 = 1.0;
GLfloat JPOT$4 = 1.0;
GLfloat JPOT$5 = 0.0;
GLfloat JPOT$6 = 0.0;
GLfloat JPOT$7 = 0.0;
GLfloat JPOT$8 = 1.0;
GLfloat JPOT$9 = 1.0;
GLfloat JPOT$10 = 1.0;
GLfloat JPOT$11 = 1.0;
GLfloat JPOT$12 = 1.0;
GLfloat JPOT$13 = 1.0;
char *datafile = "Light.dat";


void sighandle(int);

void ReadData(void) {
	if ((fp = fopen(datafile, "r")) == NULL)
	{
	}
	else {
		while ( !feof(fp)) {
fscanf(fp," %f %f %f %f %f %f %f %f %f %f %f %f %f", &JPOT$1, &JPOT$2, &JPOT$3, &JPOT$4, &JPOT$5, &JPOT$6, &JPOT$7, &JPOT$8, &JPOT$9, &JPOT$10, &JPOT$11, &JPOT$12, &JPOT$13);
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


GLfloat mat_amb_diff_white[]   = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_amb_diff_red[]   = {1.0, 0.0, 0.0, 1.0};
GLfloat material_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat material_shininess[] = { 50 };
GLfloat material_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat material_ambient[] = {1.0, 1.0, 1.0, 1.0};
GLfloat material_emission[] = {0.0, 0.0, 0.0, 1.0};
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0};
GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0};

void init(void)
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
  gluLookAt(2, 4, 5, 0 , 0 , 0, 0, 1, 0);
  glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);  
  glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
  glMaterialfv(GL_FRONT, GL_EMISSION, material_emission);
  light_position[0] =JPOT$1; light_position[1] = JPOT$2; light_position[2] = JPOT$3; light_position[3] = JPOT$4;
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  light_ambient[0] =JPOT$5; light_ambient[1] = JPOT$5; light_ambient[2] = JPOT$7;
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  light_diffuse[0] =JPOT$8; light_diffuse[1] = JPOT$9; light_diffuse[2] = JPOT$10;
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  light_specular[0] = JPOT$11; light_specular[1] = JPOT$12; light_specular[2] = JPOT$13;
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glPushMatrix();
    glTranslatef(light_position[0], light_position[1], light_position[2] - 0.2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_red);
    glScalef(0.1, 0.1, 0.1);
    glutSolidSphere(1.0, 8, 8);
  glPopMatrix();
  drawaxes();
  glTranslatef(0, 0, -3);
  glutSolidTeapot(1.0);
  glFlush();
}

void idlefunc(void) {
        CheckRefresh();
}
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (350, 350); 
   glutInitWindowPosition (0, 0);
   glutCreateWindow ("Material Ambience");
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
