
/* 
 *  by Ichiro Suzuki
 *  parts of this program might have been adopted from or inspired by 
 *  sample programs in 
 *  "OpenGL Programming Guide", 
 *  Copyright (c) 1993-1997, Silicon Graphics, Inc.
 *  "Interactive Computer Graphics"  
 *  Copyright (c) 1997, Addison-Wesley,
 *  and/or those written by Sumanta Guha
 */

#include <GL/glut.h>
#include <stdlib.h>

static GLfloat spin = 0.0;

/* vector to specify the position of light source */ 
static GLfloat light0_position[] = {3.0, 2.0, 1.0, 1.0};


void init(void) 
{
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_SMOOTH);

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
}


void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(45.0, (GLfloat) w / (GLfloat) h, 2.0, 20.0);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(2.0, 4.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


void display(void)
{

   /* vectors to specify material properties */ 
   GLfloat mat_specular[]         = {1.0, 1.0, 1.0, 1.0};
   GLfloat mat_shininess[]        = {50.0};
   GLfloat mat_amb_diff_white[]   = {1.0, 1.0, 1.0, 1.0};
   GLfloat mat_amb_diff_grey[]    = {0.5, 0.5, 0.5, 1.0};
   GLfloat mat_amb_diff_red[]     = {1.0, 0.0, 0.0, 1.0};
   GLfloat mat_amb_diff_green[]   = {0.0, 1.0, 0.0, 1.0};
   GLfloat mat_amb_diff_blue[]    = {0.0, 0.0, 1.0, 1.0};
   GLfloat mat_amb_diff_yellow[]  = {1.0, 1.0, 0.0, 1.0}; 
   GLfloat mat_amb_diff_magenta[] = {1.0, 0.0, 1.0, 1.0}; 
   GLfloat mat_amb_diff_cyan[]    = {0.0, 1.0, 1.0, 1.0}; 

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glPushMatrix();

   /* set position of light source */
   glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

   /* draw a tabletop */
   /* specify material properties */
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_grey);

   /* 
    * preceding each vertex definition is a normal vector definition,
    * which specifies a normal direction to the surface at that vertex.
    * This is required as lighting has been enabled. Normal vectors are
    * used by OpenGL to calculate how the light striking an object is
    * reflected to the viewer. 
    * For each face all four vertices are given the same normal 
    * perpendicular to the face. Thus a vertex shared by more than one
    * face has actually a normal for each of those faces.
    */

   glBegin(GL_POLYGON);
      glNormal3f( 0.0, 1.0, 0.0);  
      glVertex3f(-3.0,-1.0,-3.0);
      glVertex3f(-3.0,-1.0, 3.0);
      glVertex3f( 3.0,-1.0, 3.0);
      glVertex3f( 3.0,-1.0,-3.0);
   glEnd();

   /* to rotate "spin" degrees about y-axis */
   glRotatef(spin, 0.0, 1.0, 0.0); 

   /* draw a 2x2x2 cube centered at the origin */

   /* back */
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_red);
   glBegin(GL_POLYGON);
      glNormal3f( 0.0, 0.0,-1.0);  
      glVertex3f( 1.0,-1.0,-1.0); /* 1 */
      glVertex3f(-1.0,-1.0,-1.0); /* 0 */
      glVertex3f(-1.0, 1.0,-1.0); /* 3 */
      glVertex3f( 1.0, 1.0,-1.0); /* 2 */
   glEnd();

   /* left */
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_green);
   glBegin(GL_POLYGON);
      glNormal3f(-1.0, 0.0, 0.0);  
      glVertex3f(-1.0,-1.0,-1.0); /* 0 */
      glVertex3f(-1.0,-1.0, 1.0); /* 4 */
      glVertex3f(-1.0, 1.0, 1.0); /* 7 */
      glVertex3f(-1.0, 1.0,-1.0); /* 3 */
   glEnd();

   /* right */
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_blue);
   glBegin(GL_POLYGON);
      glNormal3f( 1.0, 0.0, 0.0);  
      glVertex3f( 1.0,-1.0, 1.0); /* 5 */
      glVertex3f( 1.0,-1.0,-1.0); /* 1 */
      glVertex3f( 1.0, 1.0,-1.0); /* 2 */
      glVertex3f( 1.0, 1.0, 1.0); /* 6 */
   glEnd();

   /* front */
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_yellow);
   glBegin(GL_POLYGON);
      glNormal3f( 0.0, 0.0, 1.0);  
      glVertex3f(-1.0,-1.0, 1.0); /* 4 */
      glVertex3f( 1.0,-1.0, 1.0); /* 5 */
      glVertex3f( 1.0, 1.0, 1.0); /* 6 */
      glVertex3f(-1.0, 1.0, 1.0); /* 7 */
   glEnd();

   /* bottom */
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_magenta);
   glBegin(GL_POLYGON);
      glNormal3f( 0.0,-1.0, 0.0);  
      glVertex3f(-1.0,-1.0,-1.0); /* 0 */
      glVertex3f( 1.0,-1.0,-1.0); /* 1 */
      glVertex3f( 1.0,-1.0, 1.0); /* 5 */
      glVertex3f(-1.0,-1.0, 1.0); /* 4 */
   glEnd();

   /* top */
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_cyan);
   glBegin(GL_POLYGON);
      glNormal3f( 0.0, 1.0, 0.0);  
      glVertex3f( 1.0, 1.0,-1.0); /* 2 */
      glVertex3f(-1.0, 1.0,-1.0); /* 3 */
      glVertex3f(-1.0, 1.0, 1.0); /* 7 */
      glVertex3f( 1.0, 1.0, 1.0); /* 6 */
   glEnd();

   glPopMatrix();

   glFlush();
   glutSwapBuffers();
}


void spinDisplay(void)
{
   spin = spin + 2.0;
   if (spin > 360.0)
      spin = spin - 360.0;
   glutPostRedisplay();  
}


void mouse(int button, int state, int x, int y) 
{
   switch (button) {
      case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN)
            glutIdleFunc(spinDisplay); 
         break;
      case GLUT_MIDDLE_BUTTON:
      case GLUT_RIGHT_BUTTON:
         if (state == GLUT_DOWN)
            glutIdleFunc(NULL);
         break;
      default:
         break;
   }
}


/* exit program with "esc" key */
void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
	 exit(0);
	 break; 
   }
}

   
/* 
 *  Request double buffer display mode.
 *  Register mouse and keyboard input callback functions
 */
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(300, 300); 
   glutInitWindowPosition(100, 100);
   glutCreateWindow(argv[0]);
   init();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape); 
   glutMouseFunc(mouse);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}

