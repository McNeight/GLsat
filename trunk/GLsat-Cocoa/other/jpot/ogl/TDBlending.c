#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

GLint please_read_data=0;
FILE * fp=NULL;
GLfloat JPOT$1 = 0.2;
GLfloat JPOT$2 = 0.6;
GLfloat JPOT$3 = 0.1;
char *datafile = "TDBlending.dat";
GLint JPOT$swap = 1;


void sighandle(int);

void ReadData(void) {
	if ((fp = fopen(datafile, "r")) == NULL)
	{
	}
	else {
		while ( !feof(fp)) {
fscanf(fp," %f %f %f %d", &JPOT$1, &JPOT$2, &JPOT$3, &JPOT$swap);
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


GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};
GLfloat mat_shininess[] = {50};
GLfloat mat_amb_diff_black[] = {0.0, 0.0, 0.0, 0.2};
GLfloat mat_amb_diff_cyan[]  = {0.0, 1.0, 1.0, 0.6};
GLfloat mat_amb_diff_red[] = {1.0, 0.0, 0.0, 0.1};
GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};

void init(void)
{  
  signal(SIGUSR2, sighandle);
   glEnable (GL_BLEND);
   glShadeModel (GL_SMOOTH);
   glClearColor (1.0, 1.0, 1.0, 0.0);
   glEnable(GL_LIGHTING);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
}

void display(void)
{
ReadData();

   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   mat_amb_diff_black[3] = JPOT$1;
   mat_amb_diff_cyan[3] = JPOT$2;
   mat_amb_diff_red[3] = JPOT$3;
if (JPOT$swap == 1) {
   glPushMatrix();
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glTranslatef(0.0, 0.0, -10.0);
   glScalef(4.0, 4.0, 3.0);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);  
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_black);
   glutSolidCube(1.0);
   glPopMatrix();

   glPushMatrix();
   glTranslatef(-0.5, 0.0, -5.0);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);  
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_cyan);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glScalef(3.0, 2.0, 1.0);
   glutSolidCube(1.0);
   glPopMatrix();

   glPushMatrix();
   glTranslatef(1.5, 0.0, -2.0);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);  
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_red);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glutSolidSphere(1.25, 30, 30);
   glPopMatrix();
   glFlush();
} /*end swap*/
if (JPOT$swap == 0) {
   glPushMatrix();
   glTranslatef(1.5, 0.0, -2.0);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);  
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_red);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glutSolidSphere(1.25, 30, 30);
   glPopMatrix();

   glPushMatrix();
   glTranslatef(-0.5, 0.0, -5.0);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);  
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_cyan);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glScalef(3.0, 2.0, 1.0);
   glutSolidCube(1.0);
   glPopMatrix();

   glPushMatrix();
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glTranslatef(0.0, 0.0, -10.0);
   glScalef(4.0, 4.0, 3.0);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);  
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_black);
   glutSolidCube(1.0);
   glPopMatrix();
} /*end swap*/
}

void reshape(int w, int h)
{
  glViewport(0,0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, (GLfloat) w /(GLfloat) h, 1.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void idlefunc(void)
{
CheckRefresh();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB|GLUT_DEPTH);
   glutInitWindowSize (350, 350);
   glutInitWindowPosition(0, 0);
   glutCreateWindow ("Three translucent surfaces");
   init();
   glutDisplayFunc (display);
   glutReshapeFunc (reshape);
   glutIdleFunc(idlefunc);
glutIdleFunc(idlefunc);
   glutMainLoop();
   return 0;
}

/*
 * Copyright (c) 1993-1997, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED 
 * Permission to use, copy, modify, and distribute this software for 
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that 
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * US Government Users Restricted Rights 
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */


























