
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

/* planetwithmoon.c */

/* 
 *  modified by Ichiro Suzuki
 *  from planet.c Copyright (c) 1993-1997, Silicon Graphics, Inc.
 *  added a moon to the planet (earth)
 *  added light source at the poition of the sun
 *  usual mouse/keyboard inputs: left clik to start animation
 *  viewer position can be changed by "x", "X", etc
 *  "w" to see wireframe
 */


#include <GL/glut.h>
#include <stdlib.h>

static GLfloat year = 0.0, day = 0.0, moonday=0.0;

static GLfloat light0_position[] = {0.0, 1.5, 0.0, 1.0};
static GLdouble viewer[] = {2.0, 4.0, 25.0}; 
static GLfloat lmodel_ambient[] = {0.3, 0.3, 0.3, 1.0 };

/* vectors to specify material properties */ 

static GLfloat mat_specular[]          = {1.0, 1.0, 1.0, 1.0};
static GLfloat mat_shininess[]         = {50.0};
static GLfloat mat_emission_red[]      = {0.6, 0.0, 0.0, 1.0};
static GLfloat mat_emission_blue[]     = {0.0, 0.0, 1.0, 1.0};
static GLfloat mat_emission_green[]    = {0.0, 1.0, 0.0, 1.0};
static GLfloat mat_no_emission[]       = {0.0, 0.0, 0.0, 1.0};
static GLfloat mat_amb_diff_white[]    = {1.0, 1.0, 1.0, 1.0};
static GLfloat mat_amb_diff_grey[]     = {0.5, 0.5, 0.5, 1.0};
static GLfloat mat_amb_diff_red[]      = {1.0, 0.0, 0.0, 1.0};
static GLfloat mat_amb_diff_green[]    = {0.0, 1.0, 0.0, 1.0};
static GLfloat mat_amb_diff_blue[]     = {0.0, 0.0, 1.0, 1.0};
static GLfloat mat_amb_diff_yellow[]   = {1.0, 1.0, 0.0, 1.0}; 
static GLfloat mat_amb_diff_magenta[]  = {1.0, 0.0, 1.0, 1.0}; 
static GLfloat mat_amb_diff_cyan[]     = {0.0, 1.0, 1.0, 1.0}; 

static GLfloat mat_amb_diff_blueish[]  = {0.1,0.4,0.8,1.0};
static GLfloat mat_amb_diff_reddish[]  = {0.8,0.4,0.1,1.0};
static GLfloat mat_amb_diff_yellowish[]= {0.8,0.8,0.2,1.0}; 

static GLfloat mat_specular_dull[]     = {0.0, 0.0, 0.0, 1.0};
static GLfloat mat_amb_diff_cyan_dull[]= {0.0, 0.4, 0.4, 1.0}; 


void init(void) 
{
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_SMOOTH);
   glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
}


void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60.0, (GLfloat) w / (GLfloat) h, 1.0, 50.0);
   glMatrixMode(GL_MODELVIEW);
}


void display(void)
{

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glPushMatrix();
   glLoadIdentity();
   /* update viewer position */
   gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 
	     0.0, 1.0, 0.0);
   /* light source position */
   glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
   /* to add more ambient light */
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
		mat_amb_diff_red);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission_red);
   glutSolidSphere(1.0, 12, 12);      /* draw sun */
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_no_emission);
   glRotatef(year, 0.0, 1.0, 0.0);    /* find direction of earth */
   glTranslatef(8.0, 0.0, 0.0);       /* go to position of earth */
   glPushMatrix();  /* memorize state just after reaching position of 
		       earth */
   glRotatef(day, 0.0, 1.0, 0.0);     /* rotate earth about its axis */
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
		mat_amb_diff_cyan);
   glutSolidSphere(1.0, 8, 8);        /* draw earth */
   glPopMatrix();   /* return to state just after reaching earth */
   glRotatef(moonday, 0.0, 1.0, 0.0); /* find direction of moon */
   glTranslatef(3.0, 0.0, 0.0);       /* go to position of moon */
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
		mat_amb_diff_green);
   glutSolidSphere(0.5, 8, 8);        /* draw moon*/
   glPopMatrix();
   glFlush();
   glutSwapBuffers();
}


void revolve(void)
{
   /* 1 year = 36 days */
   day = day + 2.0;
   if (day > 360.0) day = day - 360.0;
   year = year + 0.2;
   if (year > 360.0) year = year - 360.0;
   moonday = moonday + 0.5;
   if (moonday > 360.0) moonday = moonday - 360.0;
   glutPostRedisplay();  
}


void mouse(int button, int state, int x, int y) 
{
   switch (button) {
      case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN)
            glutIdleFunc(revolve); 
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

void keyboard(unsigned char key, int x, int y)
{
   static int filled=1;

   switch (key) {
      case 'w':
	 if (filled) {
	   filled = 0;
	   glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	   //glPolygonMode(GL_FRONT,GL_LINE);
	 }
	 else {
	   filled = 1;
	   glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	 }; 
         glutPostRedisplay();
         break;

      /* control viewer position */
      case 'x':                
         viewer[0] = viewer[0] + 0.2;
         glutPostRedisplay();
         break;
      case 'X':                 
         viewer[0] = viewer[0] - 0.2;
         glutPostRedisplay();
         break;
      case 'y':                
         viewer[1] = viewer[1] + 0.2;
         glutPostRedisplay();
         break;
      case 'Y':                 
         viewer[1] = viewer[1] - 0.2;
         glutPostRedisplay();
         break;
      case 'z':                
         viewer[2] = viewer[2] + 0.2;
         glutPostRedisplay();
         break;
      case 'Z':                 
         viewer[2] = viewer[2] - 0.2;
         glutPostRedisplay();
         break;

      case 27:
         exit(0);
         break;
      default:
         break;
   }
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(500, 500); 
   glutInitWindowPosition(0, 0);
   glutCreateWindow(argv[0]);
   init();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutMouseFunc(mouse);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}


