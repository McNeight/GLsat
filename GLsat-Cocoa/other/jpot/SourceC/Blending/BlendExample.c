#include <stdio.h>
#include <GL/glut.h>

GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};
GLfloat mat_shininess[] = {50};
GLfloat mat_amb_diff_red[] = {1.0, 0.0, 0.0, 0.5};
GLfloat mat_amb_diff_green[] = {0.0, 1.0, 0.0, 1.0};
GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};

void init(void)
{  
   glEnable (GL_BLEND);
   glShadeModel (GL_SMOOTH);
   glClearColor (1.0, 1.0, 1.0, 0.0);
   glEnable(GL_LIGHTING);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
}

void display(void)
{

   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);  
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_green);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glutSolidTeapot(0.5);

   /*$$$[Alpha , f, 0.5, 0.0, 1.0, c]$$$*/
   mat_amb_diff_red[3] = JPOT$1;
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_red);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   
   glPushMatrix();
   glTranslatef(0.0, 0.0, 0.5);
   glBegin(GL_TRIANGLES);
     glVertex3f(-1.0, -1.0, 0.0);
     glVertex3f(-1.0, 1.0, 0.0);
     glVertex3f(0.1, 0.0, 0.0);
     glVertex3f(1.0, 1.0, 0.0);
     glVertex3f(1.0, -1.0, 0.0);
     glVertex3f(-0.1, 0.0, 0.0);
   glEnd();
   glPopMatrix();
   glFlush();
}

void reshape(int w, int h)
{
  glViewport(0,0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  gluPerspective(30.0, (GLfloat) w /(GLfloat) h, 1.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB|GLUT_DEPTH);
   glutInitWindowSize (350, 350);
   glutInitWindowPosition(0, 0);
   glutCreateWindow ("Blending Example");
   init();
   glutDisplayFunc (display);
   glutReshapeFunc (reshape);
   glutMainLoop();
   return 0;
}
	
/* 
 *  By Binh Phu Le
 *  parts of this program might have been adopted from or inspired by 
 *  sample programs in 
 *  "OpenGL Programming Guide", 
 *  Copyright (c) 1993-1997, Silicon Graphics, Inc.
 *  "Interactive Computer Graphics"  
 *  Copyright (c) 1997, Addison-Wesley,
 *  and/or those written by Dr. Sumanta Guha guha@cs.uwm.edu
 *  and/or those written by Dr. Ichiro Suzuki suzuki@cs.uwm.edu
 */
