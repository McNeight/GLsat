

#include <GL/glut.h>
#include <stdlib.h>
static GLfloat light0_position[] = {0.0, 2.0, 2.0, 1.0};
GLint ortho, winY;
GLfloat left = -5.0, right = 5.0, bottom = -5.0, top = 5.0, near = 2.0, 
      far = 20.0;   
GLfloat left1 = -1.0, right1 = 1.0, bottom1 = -1.0, top1 = 1.0, near1 = 2.0, 
      far1 = 20.0;  

GLfloat mat_amb_diff_white[]   = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_amb_diff_grey[]    = {0.5, 0.5, 0.5, 1.0};
GLfloat mat_amb_diff_red[]     = {1.0, 0.0, 0.0, 1.0};
GLfloat mat_amb_diff_green[]   = {0.0, 1.0, 0.0, 1.0};
GLfloat mat_amb_diff_blue[]    = {0.0, 0.0, 1.0, 1.0};
GLfloat mat_amb_diff_yellow[]  = {1.0, 1.0, 0.0, 1.0}; 
GLfloat mat_amb_diff_magenta[] = {1.0, 0.0, 1.0, 1.0}; 
GLfloat mat_amb_diff_cyan[]    = {0.0, 1.0, 1.0, 1.0}; 
void init(int argc, char * arg_vec[])

{ if(ortho==1){
    left = atoi (arg_vec[3]);
    right = atoi (arg_vec[4]);
    bottom = atoi (arg_vec[5]);
    top = atoi (arg_vec[6]);
    near = atoi (arg_vec[7]);
    far = atoi (arg_vec[8]);
  }
  else{
    left1 = atoi (arg_vec[3]);
    right1 = atoi (arg_vec[4]);
    bottom1 = atoi (arg_vec[5]);
    top1 = atoi (arg_vec[6]);
    near1 = atoi (arg_vec[7]);
    far1 = atoi (arg_vec[8]);
  }
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glShadeModel(GL_SMOOTH);   
  glEnable(GL_DEPTH_TEST);
  
}
void reshape (int w, int h)
{
	glViewport(0,0, (GLsizei) w, (GLsizei) h);
 	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	/*if (ortho != 1)
	  glFrustum(-1.0, 1.0, -1.0, 1.0, 2.0, 20.0);
	else
	  glOrtho(-5.5, 5.5, -5.5, 5.5, 2.0, 20.0); */
        if (ortho != 1)
	  glFrustum(left1, right1, bottom1, top1, near1, far1);
	else
	  glOrtho(left, right, bottom, top, near, far);
	glMatrixMode(GL_MODELVIEW);
}

void display()
{	
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  glLoadIdentity();          /* clear the matrix */
  gluLookAt(2, 4, 10, 0 , 0 , 0, 0, 1, 0);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);	
  
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_red);
  glBegin(GL_POLYGON);
  /* back */
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glVertex3f(1.0, 1.0, -1.0);
  glEnd();
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_green); 	
  
  glBegin(GL_POLYGON);
  /* left */
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glVertex3f(-1.0, 1.0, -1.0);
  glEnd();

  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_blue);
  glBegin(GL_POLYGON);
  /* right */
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glVertex3f(1.0, 1.0, 1.0);
  glEnd();
	
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_yellow);
  glBegin(GL_POLYGON);
  /* front */
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(1.0, 1.0, 1.0);
    glVertex3f(-1.0, 1.0, 1.0);
  glEnd();
 	
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_magenta);
  glBegin(GL_POLYGON);
  /* bottom */
    glNormal3f(0.0, -1.0, 0.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(-1.0, -1.0, 1.0);
  glEnd();
	
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_cyan);
  glBegin(GL_POLYGON);
  /* top */
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(1.0, 1.0, -1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glVertex3f(1.0, 1.0, 1.0);
    glEnd();
	
   glColor3f(1.0, 0.0, 0.0);
   glBegin(GL_LINES);
     glVertex3f(-5,0,0);
     glVertex3f(5,0,0);
   glEnd();
	
   glColor3f(0.0, 1.0, 0.0);
   glBegin(GL_LINES);   
     glVertex3f(0, -5, 0);
     glVertex3f(0, 5, 0);
   glEnd();
	
   glColor3f(0.0, 0.0, 1.0);
   glBegin(GL_LINES);   
     glVertex3f(0, 0, -5);
     glVertex3f(0, 0, 5);
   glEnd();
	
   glTranslatef(2, 0,- 10);
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_red);
   glutSolidSphere(1.0, 40, 40);
   glPopMatrix(); 
   glFlush();
}



int main(int argc, char** argv)
{
	
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE |GLUT_RGB|GLUT_DEPTH);
	ortho = atoi (argv[1]);
	winY = atoi(argv[2]);
	
	
	glutInitWindowSize(250, 250);
	glutInitWindowPosition(0, winY);
	if(ortho == 1)
	  glutCreateWindow("Orthographic Projection - glOrtho()");
	else
	  glutCreateWindow("Perspective Projection - glFrustum()");
	
	init(argc, argv); 
	
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








