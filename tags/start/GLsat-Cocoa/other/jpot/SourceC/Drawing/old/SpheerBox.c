/* This program draws a red box and then draws a green sphere within 
 * the box. The top face of the box is opened by pressing 'o' and closed
 * by pressing 'c. 'e' = exit. 
 */

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14159265

static int Step = 0;

void display();

static GLfloat vdata[8][3] = {
  {1.0, -1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, -1.0}, {1.0, -1.0, -1.0}, 
  {-1.0, -1.0, 1.0}, {-1.0, 1.0, 1.0}, {-1.0, 1.0, -1.0}, {-1.0, -1.0, -1.0}
};

static GLuint tindices[6][4] = {
  {3,2,1,0}, {7,6,2,3}, {4,5,6,7}, {0,1,5,4}, {4,7,3,0}, {6,5,1,2} 
};

static GLfloat rdata[4][3] = { {0.0, -1.0, 1.0}, {0.0, -1.0, -1.0}, {0.0, 1.0, -1.0}, {0.0, 1.0, 1.0}};
static GLint rindex[4] = {0,3,2,1};

void display(){
  int i;
  double angle, Y, Z, NY, NZ;

/* vectors are defined to be used to specify material lighting properties */ 
  GLfloat mat_specular1[] = { 1.0, 0.0, 0.0, 1.0 };
  GLfloat mat_shininess1[] = { 50.0 };
  GLfloat mat_diffuse1[] = {0.9, 0.0, 0.0};
  GLfloat mat_specular2[] = { 0.0, 1.0, 0.0, 1.0 };
  GLfloat mat_shininess2[] = { 50.0 };
  GLfloat mat_diffuse2[] = {0.0, 0.9, 0.0};

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

/* material lighting properties are specified for the box */
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess1);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);

/* preceding each vertex definition is a normal vector definition, which 
 * is the specification of a normal direction to the surface at that vertex.
 * This is required as lighting has been enabled. Normal vectors are
 * used by OpenGL to calculate how the light striking an object is
 * reflected to a viewer. 
 * In this example, we use the co-ordinates of a vertex itself
 * to define its normal vector - this is simply the outward pointing
 * vector from the origin to the vertex, a reasonable choice in this case.
 */
  glBegin(GL_QUADS);
  for (i = 0; i < 5; ++i){
    glNormal3fv(&vdata[tindices[i][0]][0]);  
    glVertex3fv(&vdata[tindices[i][0]][0]);
    glNormal3fv(&vdata[tindices[i][1]][0]);  
    glVertex3fv(&vdata[tindices[i][1]][0]);
    glNormal3fv(&vdata[tindices[i][2]][0]);  
    glVertex3fv(&vdata[tindices[i][2]][0]);
    glNormal3fv(&vdata[tindices[i][3]][0]);
    glVertex3fv(&vdata[tindices[i][3]][0]);  
}
  glEnd();
  
  angle = ((float)Step/120) * PI;
  Y = 1 + 2 * sin(angle);
  Z = -1 + 2 * cos(angle);
  NY = cos(angle);
  NZ = -sin(angle);
  
  glBegin(GL_QUADS);
    glNormal3f (0, NY, NZ);
    glVertex3f (1.0, Y, Z);
    glNormal3f (0, NY, NZ);
    glVertex3f (1.0, 1.0, -1.0);
    glNormal3f (0, NY, NZ);
    glVertex3f (-1.0, 1.0, -1.0);
    glNormal3f (0, NY, NZ);
    glVertex3f (-1.0, Y, Z);
  glEnd();
     
  glBegin(GL_POLYGON);
    for (i = 0; i < 4; i++) {
      glNormal3fv(&rdata[rindex[i]][0]);
      glVertex3fv(&rdata[rindex[i]][0]);
    }
  glEnd();
 
  /* material lighting properties are specified for the sphere */
  
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular2);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess2);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
  glRotatef(90, 0.0, 1.0, 0.0);
  glTranslatef(0.0, 0.0, 0.5); 
  glutSolidTeapot(0.5);
  glTranslatef(0.0, 0.0, -0.5);
  glRotatef(-90, 0.0, 1.0, 0.0);
  glTranslatef(-0.5, 0.0, 0.0);
  glutSolidSphere (0.5, 10.0, 8.0);
  glTranslatef(0.5, 0.0, 0.0);
  
  glutSwapBuffers();
}

void init(void)
{
   GLfloat ambient[] = { 0.1, 0.1, 0.1, 1.0 };
   GLfloat diffuse[] = { 1.0, 1.0, 0.0, 1.0 };
   GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat position[] = { 0.0, 1.0, 3.0, 0.0 };
    
   GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };

/* Define properties of the light source */
   glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
   glLightfv(GL_LIGHT0, GL_POSITION, position);
    
/* Define properties of the ambient light */
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST); 

   glClearColor(0.0, 0.0, 0.0, 0.0);
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt (2.0, 5.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'e':
    exit(0);
    break;
  case 'o':
    if (Step < 120) Step++;
    glutPostRedisplay();
    break;
  case 'c':
    if (Step > 0) Step--;
    glutPostRedisplay();
    break;
  default:
    break;
  }
}

/*  Main Loop
 */
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
   glutInitWindowSize (400, 400);
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init();
   glutReshapeFunc(reshape);
   glutDisplayFunc(display);
   glutKeyboardFunc(keyboard);
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

