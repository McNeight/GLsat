#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

GLint please_read_data=0;
FILE * fp=NULL;
GLint JPOT$1 = GL_TRUE;
char *datafile = "TriangulateEx.dat";
GLint i;

void sighandle(int);

void ReadData(void) {
	if ((fp = fopen(datafile, "r")) == NULL)
	{
	}
	else {
		while ( !feof(fp)) {
fscanf(fp," %d", &JPOT$1);
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


GLint showedge = 1;
/* put the (x,y,z)-coordinates of the 8 vertices of the annulus in 
   a 2D array */

static GLfloat vdata[8][3] = {
  {0.1, 0.1, 0.0}, {0.9, 0.1, 0.0}, {0.9, 0.9, 0.0}, {0.1, 0.9, 0.0}, 
  {0.25, 0.25, 0.0}, {0.75, 0.25, 0.0}, {0.75, 0.75, 0.0}, 
  {0.25, 0.75, 0.0} 
};

/* put the 3 vertex indexes of each of the 8 triangles of a 
 * triangulation of the annulus in a 2D array.
 * note that the triangles are ORIENTED CONSISTENTLY with (outward) 
 * faces having their vertices in counterclockwise order. 
 */

static GLuint tindices[8][3] = {
  {1, 4, 0}, {4, 1, 5}, {2, 5, 1}, {5, 2, 6}, 
  {3, 6, 2}, {6, 3, 7}, {0, 7, 3}, {7, 0, 4} 
};


void init (void) 
{
  signal(SIGUSR2, sighandle);
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glPolygonMode(GL_FRONT, GL_LINE);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}


void display(void)
{
ReadData();
   glClear (GL_COLOR_BUFFER_BIT);
  /* draws a square annulus formed between the rectangle with corners at
   * (0.25, 0.25, 0.0) and (0.75, 0.75, 0.0)  
   * and the rectangle with corners at
   * (0.10, 0.10, 0.0) and (0.90, 0.90, 0.0)
   */
   glColor3f (1.0, 1.0, 1.0);
   glBegin(GL_TRIANGLES);
   for (i = 0; i < 8; ++i) {
     glEdgeFlag(JPOT$1); 
     glVertex3fv(&vdata[tindices[i][0]][0]);
     glVertex3fv(&vdata[tindices[i][1]][0]);
     glEdgeFlag(GL_TRUE);
     glVertex3fv(&vdata[tindices[i][2]][0]);
   }
   glEnd();
   
   glFlush ();
}

void idlefunc(void) {
        CheckRefresh();
}
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize(300,300); 
   glutInitWindowPosition(0,0);
   glutCreateWindow("Triangulation - Edge Flags");
   init();
   glutDisplayFunc(display); 
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

/*
 * triangulated_annulus2.c
 * This program is hello.c of the red book modified by Sumanta Guha.
 */

/* Further modified by Ichiro Suzuki 
 * type "w" to toggle wireframe
 * Further modified my M. Kumar
 * While in the toggle mode type "e"
 * This allows you toggle between edge suppression and rendering 
 * interior edges. i.e. the use of glEdgeFlag(GL_FALSE) or otherwise
 * depress the  "ESC" key to exit
 */
