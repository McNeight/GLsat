#include <GL/glut.h>
#include <stdlib.h>

static GLfloat year = 0.0, day = 0.0, moonday=0.0;
static GLfloat mday = 0.0, vday = 0.0, maday = 0.0;
static GLfloat jday = 0.0, jmoon1day = 0.0, jmoon2day = 0.0;
static GLfloat myear = 0.0, vyear = 0.0, mayear = 0.0, jyear = 0.0;

static GLfloat light0_position[] = {0.0, 3.5, 0.0, 1.0};
/*static GLdouble viewer[] = {2.0, 4.0, 30.0};*/
static GLdouble viewer[] = {0.0, 40.0, 0.0}; 
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
static GLfloat mat_amb_diff_brownish[]  = {1.0,0.4,0.25,1.0};
static GLfloat mat_amb_diff_blueish[]  = {0.1,0.4,0.8,1.0};
static GLfloat mat_amb_diff_reddish[]  = {0.8,0.4,0.1,1.0};
static GLfloat mat_amb_diff_yellowish[]= {0.9,0.9,0.0,1.0}; 
static GLfloat mat_amb_diff_yellowish1[]= {0.6,0.6,0.2,1.0}; 
static GLfloat mat_specular_dull[]     = {0.0, 0.0, 0.0, 1.0};
static GLfloat mat_amb_diff_cyan_dull[]= {0.0, 0.4, 0.4, 1.0}; 

void init(void) 
{
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  /* will normally show only front face, but...
  glPolygonMode(GL_FRONT,GL_FILL);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  */
  /* since back "face" appears in wireframe mode */ 
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
	     1.0, 0.0, 0.0);
  /* light source position */
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  /* to add more ambient light */
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
        	mat_amb_diff_red);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission_red);
  glutSolidSphere(1.75, 30, 30);      /* draw sun */
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_no_emission);

  glPushMatrix();
  glRotatef(myear, 0.0, 1.0, 0.0);    /* find direction of mercury */
  glTranslatef(2.75, 0.0, 0.0);       /* go to position of mercury */
  glRotatef(mday, 0.0, 1.0, 0.0);     /* rotate mercury about its axis */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
		mat_amb_diff_grey);
  glutSolidSphere(0.5, 20, 20);        /* draw mercury */
  glPopMatrix();

  glPushMatrix();
  glRotatef(vyear, 0.0, 1.0, 0.0);    /* find direction of venus */
  glTranslatef(5.0, 0.0, 0.0);       /* go to position of venus */
  glRotatef(vday, 0.0, 1.0, 0.0);     /* rotate venus about its axis */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
        	mat_amb_diff_yellowish);
  glutSolidSphere(0.95, 20, 20);        /* draw venus*/
  glPopMatrix();

  glPushMatrix();
  glRotatef(year, 0.0, 1.0, 0.0);    /* find direction of earth */
  glTranslatef(8.0, 0.0, 0.0);       /* go to position of earth */

  glPushMatrix();  /* memorize state just after reaching position of 
		       earth */
  glRotatef(day, 0.0, 1.0, 0.0);     /* rotate earth about its axis */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
		mat_amb_diff_cyan);
  glutSolidSphere(0.85, 20, 20);        /* draw earth */
  glPopMatrix();   /* return to state just after reaching earth */

  glRotatef(moonday, 0.0, 1.0, 0.0); /* find direction of moon */
  glTranslatef(1.5, 0.0, 0.0);       /* go to position of moon */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
		mat_amb_diff_green);
  glutSolidSphere(0.35, 20, 20);        /* draw moon*/
  glPopMatrix();

  glPushMatrix();
  glRotatef(mayear, 0.0, 1.0, 0.0);    /* find direction of mars */
  glTranslatef(11.0, 0.0, 0.0);       /* go to position of mars */
  glRotatef(maday, 0.0, 1.0, 0.0);     /* rotate mars about its axis */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
		mat_amb_diff_yellowish1);
  glutSolidSphere(0.92, 20,20);        /* draw mars*/
  glPopMatrix();

  glPushMatrix();
  glRotatef(jyear, 0.0, 1.0, 0.0);    /* find direction of Jupiter */
  glTranslatef(16.35, 0.0, 0.0);       /* go to position of Jupiter */
  glPushMatrix();  /* memorize state just after reaching position of 
		       Jupiter */
  glRotatef(jday, 0.0, 1.0, 0.0);     /* rotate jupiter about its axis */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
		mat_amb_diff_reddish);
  glutSolidSphere(1.6, 20, 20);        /* draw jupiter */
  glPopMatrix();   /* return to state just after reaching jupiter */

  glPushMatrix();
  glRotatef(jmoon1day, 0.0, 1.0, 0.0); /* find direction of 1'st moon */
  glTranslatef(2.65, 0.0, 0.0);       /* go to position of moon */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
		mat_amb_diff_blue);
  glutSolidSphere(0.65, 20, 20);        /* draw 1'st moon*/
  glPopMatrix();

  glPushMatrix();
  glRotatef(jmoon2day, 0.0, 1.0, 0.0); /* find direction of 2'nd moon */
  glTranslatef(4.2, 0.0, 0.0);       /* go to position of moon */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, 
		mat_amb_diff_brownish);
  glutSolidSphere(0.34, 20, 20);        /* draw 2'nd  moon*/
  glPopMatrix();
  glPopMatrix();

  glPopMatrix();
  glFlush();
  glutSwapBuffers();
}

void revolve(void)
{
  /* 1 year = 360 days */
  mday = mday + 4.0;
   if (mday > 360.0) mday = mday - 360.0;
  myear = myear + 2.0;
   if (myear > 360.0) myear = myear - 360.0;
  vday = vday + 3.0;
   if (vday > 360.0) vday = vday - 360.0;
  vyear = vyear + 1.0;
   if (vyear > 360.0) vyear = vyear - 360.0;
  day = day + 2.0;
   if (day > 360.0) day = day - 360.0;
  year = year + 0.2;
   if (year > 360.0) year = year - 360.0;
  moonday = moonday + 0.5;
   if (moonday > 360.0) moonday = moonday - 360.0;
  maday = maday + 1.5;
   if (maday > 360.0) maday = maday - 360.0;
  mayear = mayear + 0.1;
   if (mayear > 360.0) mayear = mayear - 360.0;
  jday = jday + 3.0;
   if (jday > 360.0) jday = jday - 360;
  jyear = jyear + 0.05;
   if (jyear > 360.0) jyear = jyear - 360.0;
  jmoon1day = jmoon1day + 2.0;
   if(jmoon1day > 360.0) jmoon1day = jmoon1day - 360;
  jmoon2day = jmoon2day - 1.0;
   if (jmoon2day < 360.0) jmoon2day = jmoon2day + 360;
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
      }
    else {
      filled = 1;
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    }; 
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
  glutInitWindowSize(400, 400); 
  glutInitWindowPosition(0, 0);
  glutCreateWindow("A partial solar system");
  init();
  glutDisplayFunc(display); 
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutKeyboardFunc(keyboard);
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
