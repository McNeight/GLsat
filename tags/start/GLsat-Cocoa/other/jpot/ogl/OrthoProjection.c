#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

GLint please_read_data=0;
FILE * fp=NULL;
GLfloat JPOT$1 = -5.0;
GLfloat JPOT$2 = 5.0;
GLfloat JPOT$3 = -5.0;
GLfloat JPOT$4 = 5.0;
GLfloat JPOT$5 = 2.0;
GLfloat JPOT$6 = 20.0;
GLfloat JPOT$7 = 2.0;
GLfloat JPOT$8 = 4.0;
GLfloat JPOT$9 = 13.0;
GLfloat JPOT$10 = 0.0;
GLfloat JPOT$11 = 0.0;
GLfloat JPOT$12 = 0.0;
GLfloat JPOT$13 = 0.0;
GLfloat JPOT$14 = 1.0;
GLfloat JPOT$15 = 0.0;
char *datafile = "OrthoProjection.dat";


void sighandle(int);

void ReadData(void) {
	if ((fp = fopen(datafile, "r")) == NULL)
	{
	}
	else {
		while ( !feof(fp)) {
fscanf(fp," %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &JPOT$1, &JPOT$2, &JPOT$3, &JPOT$4, &JPOT$5, &JPOT$6, &JPOT$7, &JPOT$8, &JPOT$9, &JPOT$10, &JPOT$11, &JPOT$12, &JPOT$13, &JPOT$14, &JPOT$15);
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


GLfloat mat_amb_diff_white[]     = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_amb_diff_green[]     = {0.0, 1.0, 0.0, 1.0};
GLfloat mat_amb_diff_cyan[]      = {0.0, 1.0, 1.0, 1.0}; 
GLfloat light_ambient[]          = {0.3, 0.3, 0.3, 1.0};
static GLfloat light0_position[] = {0.0, 2.0, 2.0, 0.0};
static double viewer[]           = {18.0, 8.0, 16.0};

static int id1, id2;
int inverse_exist = 0;
GLfloat matrix[16] = {0.99, 0, -0.15, 0.00, -0.04, 0.96, -0.29, 0, 0.15, 0.29, 0.95, 0.0, 2, 4, 13, 0.83};
GLfloat tmat[4][4], temp[4][4];
GLint i, j;

void transpose(float trans[4][4]);

void inverse(float a[4][4]);

void inverse(float a[4][4])
{   
  float 
    t14, t15, t17, t19, t20, t22, t24, t25,
    t27, t29, t32, t33, t35, t37, t38, t40,
    t42, t43, t46, t49, t50, t52, t54, t57,
    t60, t63, t65, t71, t73, t75, t77, t81,
    t83, t85, t87, t101, t103, t105, t107, 
    t109, t111, t115, t117, t119, t121, t123, 
    t125, t129, t131, t133, t135, divisor;
  
  inverse_exist = 0;

  t14 = a[0][0]*a[1][1];
  t15 = a[2][2]*a[3][3];
  t17 = a[2][3]*a[3][2];
  t19 = a[0][0]*a[2][1];
  t20 = a[1][2]*a[3][3];
  t22 = a[1][3]*a[3][2];
  t24 = a[0][0]*a[3][1];
  t25 = a[1][2]*a[2][3];
  t27 = a[1][3]*a[2][2];
  t29 = a[1][0]*a[0][1];
  t32 = a[1][0]*a[2][1];
  t33 = a[0][2]*a[3][3];
  t35 = a[0][3]*a[3][2];
  t37 = a[1][0]*a[3][1];
  t38 = a[0][2]*a[2][3];
  t40 = a[0][3]*a[2][2];
  t42 = -t14*t15+t14*t17+t19*t20-t19*t22-t24*t25+t24*t27+t29*t15-t29*t17-
    t32*t33+t32*t35+t37*t38-t37*t40;
  t43 = a[2][0]*a[0][1];
  t46 = a[2][0]*a[1][1];
  t49 = a[2][0]*a[3][1];
  t50 = a[0][2]*a[1][3];
  t52 = a[0][3]*a[1][2];
  t54 = a[3][0]*a[0][1];
  t57 = a[3][0]*a[1][1];
  t60 = a[3][0]*a[2][1];
  t63 = -t43*t20+t43*t22+t46*t33-t46*t35-t49*t50+t49*t52+t54*t25-t54*t27-
    t57*t38+t57*t40+t60*t50-t60*t52;
  
  if((t42 + t63) == 0) { 
    inverse_exist=1;
    return; 
  }
  
  t65 = 1/(t42+t63);
  t71 = a[2][1]*a[0][2];
  t73 = a[2][1]*a[0][3];
  t75 = a[3][1]*a[0][2];
  t77 = a[3][1]*a[0][3];
  t81 = a[0][1]*a[1][2];
  t83 = a[0][1]*a[1][3];
  t85 = a[1][1]*a[0][2];
  t87 = a[1][1]*a[0][3];
  t101 = a[1][0]*a[2][2];
  t103 = a[1][0]*a[2][3];
  t105 = a[2][0]*a[1][2];
  t107 = a[2][0]*a[1][3];
  t109 = a[3][0]*a[1][2];
  t111 = a[3][0]*a[1][3];
  t115 = a[0][0]*a[2][2];
  t117 = a[0][0]*a[2][3];
  t119 = a[2][0]*a[0][2];
  t121 = a[2][0]*a[0][3];
  t123 = a[3][0]*a[0][2];
  t125 = a[3][0]*a[0][3];
  t129 = a[0][0]*a[1][2];
  t131 = a[0][0]*a[1][3];
  t133 = a[1][0]*a[0][2];
  t135 = a[1][0]*a[0][3];
  tmat[0][0] = -(a[1][1]*a[2][2]*a[3][3]-a[1][1]*a[2][3]*a[3][2]
                 -a[2][1]*a[1][2]*a[3][3]+a[2][1]*a[1][3]*a[3][2]
                 +a[3][1]*a[1][2]*a[2][3]-a[3][1]*a[1][3]*a[2][2])*t65;
  tmat[0][1] = (a[0][1]*a[2][2]*a[3][3]-a[0][1]*a[2][3]*a[3][2]-t71*a[3][3]+
		t73*a[3][2]+t75*a[2][3]-t77*a[2][2])*t65;
  tmat[0][2] = (-t81*a[3][3]+t83*a[3][2]+t85*a[3][3]-t87*a[3][2]-t75*a[1][3]+
		t77*a[1][2])*t65;
  tmat[0][3] = (t81*a[2][3]-t83*a[2][2]-t85*a[2][3]+t87*a[2][2]+t71*a[1][3]-
		t73*a[1][2])*t65;
  tmat[1][0] = (t101*a[3][3]-t103*a[3][2]-t105*a[3][3]+t107*a[3][2]+t109*a[2]
		[3]-t111*a[2][2])*t65;
  tmat[1][1] = -(t115*a[3][3]-t117*a[3][2]-t119*a[3][3]+t121*a[3][2]+t123*a[2]
		 [3]-t125*a[2][2])*t65;
  tmat[1][2] = -(-t129*a[3][3]+t131*a[3][2]+t133*a[3][3]-t135*a[3][2]-t123*a
		 [1][3]+t125*a[1][2])*t65;
  tmat[1][3] = -(t129*a[2][3]-t131*a[2][2]-t133*a[2][3]+t135*a[2][2]+t119*a[1]
		 [3]-t121*a[1][2])*t65;
  tmat[2][0] = -(t32*a[3][3]-t103*a[3][1]-t46*a[3][3]+t107*a[3][1]+t57*a[2][3]
		 -t111*a[2][1])*t65;
  tmat[2][1] = (-t117*a[3][1]+t19*a[3][3]-t43*a[3][3]+t121*a[3][1]+t54*a[2][3]
		-t125*a[2][1])*t65;
  tmat[2][2] = -(t14*a[3][3]-t131*a[3][1]-t29*a[3][3]+t135*a[3][1]+t54*a[1][3]
		 -t125*a[1][1])*t65;
  tmat[2][3] = (t14*a[2][3]-t131*a[2][1]-t29*a[2][3]+t135*a[2][1]+t43*a[1][3]-
		t121*a[1][1])*t65;
  tmat[3][0] = -(-t32*a[3][2]+t101*a[3][1]+t46*a[3][2]-t105*a[3][1]-t57*a[2]
		 [2]+t109*a[2][1])*t65;
  tmat[3][1] = (-t19*a[3][2]+t115*a[3][1]+t43*a[3][2]-t119*a[3][1]-t54*a[2][2]
		+t123*a[2][1])*t65;
  tmat[3][2] = (t14*a[3][2]-t129*a[3][1]-t29*a[3][2]+t133*a[3][1]+t54*a[1][2]-
		t123*a[1][1])*t65;
  tmat[3][3] = (-t14*a[2][2]+t129*a[2][1]+t29*a[2][2]-t133*a[2][1]-t43*a[1][2]
		+t119*a[1][1])*t65; 
}


void transpose(float trans[4][4])
{
  int i, j;
  
  for(i=0; i<4; i++) {
    for(j=0; j<4; j++) 
      temp[j][i] = trans[i][j] ;
  }
  for(i=0; i<4; i++)
    for(j=0; j<4; j++)
      tmat[i][j] = temp[i][j];
}  

void init() 
{
  signal(SIGUSR2, sighandle);
  /* select clearing color to black */
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

void drawlocalaxes()
{
  glLineWidth(2.0);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_cyan);
  glBegin(GL_LINES);
    glVertex3f(-3,0,0);
    glVertex3f(3,0,0);
    glVertex3f(0,-3,0);
    glVertex3f(0,3,0);
    glVertex3f(0, 0, -3);
    glVertex3f(0, 0, 3);
  glEnd();
}

void reshape(int w, int h)
{
   glViewport(0,0,(GLsizei) w, (GLsizei) h);
   /* set up projection transformation */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(80, (GLfloat) w / (GLfloat) h, 1.0, 100.0);
   glMatrixMode(GL_MODELVIEW);
}

void display(void)
{
ReadData();
  glutSetWindow(id1);
  /* clear all pixels, clear depth bit */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* set up projection transformation */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  /* for perspective projection specify a viewing frustum */
  glOrtho(JPOT$1, JPOT$2, JPOT$3, JPOT$4, JPOT$5, JPOT$6);
  /* set up viewing transformation */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  /* specify viewpoint */
  gluLookAt(JPOT$7, JPOT$8, JPOT$9, JPOT$10, JPOT$11, JPOT$12, JPOT$13, JPOT$14, JPOT$15);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) tmat);
  transpose(tmat);
  inverse(tmat);
  transpose(tmat);
  drawaxes();

  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_green);
  glutSolidTeapot(1.0);

  glFlush();
}

void display2(void)
{  
ReadData();
  glutSetWindow(id2);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  /* specify viewpoint */
  gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);	
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_green);
  glutSolidTeapot(1.0);
   /* Draw the local coordinate system */
  drawlocalaxes();
  if (inverse_exist == 0) {
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_white);
    glPushMatrix();
      for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++)
	  matrix[i*4+j] = tmat[i][j];
      glMultMatrixf(matrix);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glLineWidth(2.0);

     /* draw the actual orthographic volume */
     /* Draw the front face of the viewing volume */
      glBegin(GL_POLYGON);
        glVertex3f(JPOT$1, JPOT$4, -JPOT$5);
	glVertex3f(JPOT$1, JPOT$3, -JPOT$5);
	glVertex3f(JPOT$2, JPOT$3, -JPOT$5);
	glVertex3f(JPOT$2, JPOT$4, -JPOT$5);
      glEnd();
     
      /* Draw the left face of the viewing volume */
      glBegin(GL_POLYGON);
        glVertex3f(JPOT$1, JPOT$3,  -JPOT$6);
	glVertex3f(JPOT$1, JPOT$3,  -JPOT$5);
	glVertex3f(JPOT$1, JPOT$4, -JPOT$5);
	glVertex3f(JPOT$1, JPOT$4, -JPOT$6);
      glEnd();
     
     /* Draw the right face of the viewing volume */
     glBegin(GL_POLYGON);
       glVertex3f(JPOT$2, JPOT$3, -JPOT$6);
       glVertex3f(JPOT$2, JPOT$3, -JPOT$5);
       glVertex3f(JPOT$2, JPOT$4, -JPOT$5);
       glVertex3f(JPOT$2, JPOT$4, -JPOT$6);
     glEnd();
     
     /* Draw the bottom face of the viewing volume */
     glBegin(GL_POLYGON);
       glVertex3f(JPOT$1, JPOT$3, -JPOT$5);
       glVertex3f(JPOT$1, JPOT$3, -JPOT$6);
       glVertex3f(JPOT$2, JPOT$3, -JPOT$6);
       glVertex3f(JPOT$2, JPOT$3, -JPOT$5);
     glEnd();
     
     /* Draw the top face of the viewing volume */
     glBegin(GL_POLYGON);
       glVertex3f(JPOT$1, JPOT$4, -JPOT$5);
       glVertex3f(JPOT$1, JPOT$4, -JPOT$6);
       glVertex3f(JPOT$2, JPOT$4, -JPOT$6);
       glVertex3f(JPOT$2, JPOT$4, -JPOT$5);
     glEnd();
     
     /* Draw the back face of the viewing volume */
     glBegin(GL_POLYGON);
       glVertex3f(JPOT$1, JPOT$4, -JPOT$6);
       glVertex3f(JPOT$1, JPOT$3, -JPOT$6);
       glVertex3f(JPOT$2, JPOT$3, -JPOT$6);
       glVertex3f(JPOT$2, JPOT$4, -JPOT$6);
     glEnd();
  glPopMatrix();
  }

  glFlush();
}


void idlefunc(void) {
        CheckRefresh();
}
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

   /* First window */
   glutInitWindowSize(300,300); 
   glutInitWindowPosition(0,0);
   id1 = glutCreateWindow("Orthographic Projection - Window 1");
   init();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape); 

   /* Second window */
   glutInitWindowSize(300,300);
   glutInitWindowPosition(0, 310);
   id2 = glutCreateWindow("Orthorgraphic Projection - Window 2");
   init();
   glutDisplayFunc(display2);
   glutReshapeFunc(reshape); 

   /* Common part */
glutIdleFunc(idlefunc);
   glutMainLoop();
   return 0;   
}

/* 
 *  by Binh Phu Le and Mahesh Kumar
 *  parts of this program might have been adopted from or inspired by 
 *  sample programs in 
 *  "OpenGL Programming Guide", 
 *  Copyright (c) 1993-1997, Silicon Graphics, Inc.
 *  "Interactive Computer Graphics"  
 *  Copyright (c) 1997, Addison-Wesley,
 *  and/or those written by Dr. Sumanta Guha guha@cs.uwm.edu
 *  and/or those written by Dr. Ichiro Suzuki suzuki@cs.uwm.edu
 */
