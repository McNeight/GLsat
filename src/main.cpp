/*
 *  GLsat: An OpenGL-based 3D satellite tracker
 *  Copyright (C) 2002  Neil McNeight <mcneight@umich.edu>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

#ifdef DEBUG
#include <cfloat>		// for max and min floating point values
#include <cstdio>		// for printf()
#endif

#include <algorithm>
#include <cmath>
#include <ctime>
// #include <getopt.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>		// for sleep()
#include <vector>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "astro.h"
#include "constants.h"
#include "eci.h"
// #include "graphics.h"
#include "lla.h"
#include "tle.h"

#define HalfSecond (0.5/SecondsPerDay)
#define C 2.997925e5                    /* Kilometers/Second    */

using namespace std;

void init();
void display();
void text();
void reshapeMain(int w, int h);
void reshapeText(int w, int h);
void keyboard(unsigned char key, int mouse_x, int mouse_y);
void mouse(int button, int state, int x, int y);
void printString(string s);


TLE			sat;
vector<TLE>		sats;
vector<TLE>::iterator	it;
ECI			posECI;
LLA			posLLA;

long double A;
long double B;
long double E;
long double I_o;
long double PER;
long double RA;

float camerax = e_R * 2;
float cameray = e_R * 2;
float cameraz = e_R * 2;

int mainWindow;
int textWindow;

bool positionWindow = true;
bool eciOutput = true;

int main(int argc, char *argv[])
{
  ifstream		ifile;

  cout.setf(ios::fixed);
  cout.setf(ios::showpoint);
  cout.precision(40);
  
#ifdef DEBUG
  // Thank you, Professor Flanigan for the code
  // and Professor Brehob for helping me to understand what it means

  struct tm	sputnik;
  float		temp1 = M_PI;
  double	temp2 = M_PI;
  long double	temp3 = M_PI;
  long double	timeJD = 0;

  printf("\n\nPertinent values for the floating point data types:\n");
  printf("    (values for float, double, and long double, respectively.)\n\n");
  printf("     decimal digit precision:     %8d       %8d        %8d\n",FLT_DIG,DBL_DIG,LDBL_DIG);
  printf("     minimum exponent of 10:      %8d       %8d        %8d\n",FLT_MIN_10_EXP,DBL_MIN_10_EXP,LDBL_MIN_10_EXP);
  printf("     maximum exponent of 10:      %8d       %8d        %8d\n",FLT_MAX_10_EXP,DBL_MAX_10_EXP,LDBL_MAX_10_EXP);
  printf("     minimum positive value:  %1.6e  %1.6e  %1.6Le\n",FLT_MIN,DBL_MIN,LDBL_MIN);
  printf("     maximum positive value:  %1.6e  %1.6e  %1.6Le\n",FLT_MAX,DBL_MAX,LDBL_MAX);
  
  cout << endl << "   " << M_PI << endl;
  cout << sizeof(temp1) << "  " << temp1 << endl;
  cout << sizeof(temp2) << "  " << temp2 << endl;
  cout << sizeof(temp3) << " " << temp3 << endl;
  
  temp1 = PI;
  temp2 = PI;
  temp3 = PI;
  
  cout << endl << "   " << PI << endl;
  cout << sizeof(temp1) << "  " << temp1 << endl;
  cout << sizeof(temp2) << "  " << temp2 << endl;
  cout << sizeof(temp3) << " " << temp3 << endl;
#endif

  if (argc > 1)
  {
    // Open, open, open, open...
    ifile.open(argv[argc-1]);
    // If the file opens...
    if (ifile)
    {
      // Priming Read
      ifile >> sat;
      // Read the rest of the file & insert into vector
      while (!ifile.eof())
      {
	sats.push_back(sat);
        ifile >> sat;
      }
    }
    ifile.close();
  }
  else
  {
    cin >> sat;
  }

  for (it = sats.begin(); it != sats.end(); it++)
  {
    if (it->getValid())
    {
      cout << "Valid TLE for " << it->getName() << endl;
    }
  }

#ifdef DEBUG
  ECI			eci;
  LLA			location;
  int			i = 0;
  
  for (i = 0; i <= 1440; i += 360)
  {
    eci = sat.position4(i);
    // eci = sat.sgp(i);
    cout << eci << endl;
  }

  sputnik.tm_year = 57;	// 1957
  sputnik.tm_mon = 9;	// October
  sputnik.tm_mday = 4;	// 4th
  sputnik.tm_wday = 5;	// Friday
  sputnik.tm_hour = 19;	// 19 hours
  sputnik.tm_min = 26;	// 26 minutes
  sputnik.tm_sec = 24;	// 24 seconds past midnight UTC

  cout << "Sputnik 1 was launched" << endl;
  cout << asctime(&sputnik);
  timeJD = julianDate(&sputnik);
  sputnik = gregorianDate(timeJD);
  cout << asctime(&sputnik);
  cout << timeJD << endl;
  cout << "2436116.31" << endl << endl;

  while (1)
  {
    time_t	currentTime;
    long double	timeJD = 0;

    currentTime = time(NULL);
    cout << asctime(gmtime(&currentTime));
    cout << sat << endl;
    // use gmtime
    timeJD = julianDate(gmtime(&currentTime));
    eci = sat.position4(timeJD);
    cout << eci << endl;
    location = eci;
    cout << location << endl;
    location.switchOutput();
    cout << location << endl;
    sleep(1);
  }
  
#else

  // Start GLUT routines
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  // Create the main window
  mainWindow = glutCreateWindow("GLsat version 0.1");
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshapeMain);
  glutKeyboardFunc(keyboard);
  //glutMouseFunc(mouse);
  glutIdleFunc(display);
  // For the coordinates sub-window
  textWindow = glutCreateSubWindow(mainWindow, 0, 0, 500, 100);
  glutDisplayFunc(text);
  glutReshapeFunc(reshapeText);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(text);
  glutMainLoop();
  
#endif
  
  return 0;
}


/*
 * init()
 *
 * Specifies desired OpenGL features.
 * If things like lighting and textures were in this demo, the
 * appropriate calls would be made here.
 * Must be called after glutCreateWindow().
 *
 */
void init()
{
#ifdef DEBUG
  cout << glGetString(GL_VENDOR) << " ";
  cout << glGetString(GL_RENDERER) << endl;
  cout << "OpenGL " << glGetString(GL_VERSION) << endl;
#endif
        
  glClearDepth(1.0);
  // Set background to black
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST | GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  /* glDisable(GL_DEPTH_TEST) */
  // sat.getOrbit(A, B, E, I_o, PER, RA);
#ifdef DEBUG
  cout << A << endl;
  cout << B << endl;
  cout << E << endl;
#endif
}

/*
 * reshape()
 *
 * The GLUT calls this function after the window has created, and
 * every time the window is reshaped.
 *
 * Specifies initial viewing options, including viewing area and model
 * orientation.
 *
 * Viewing transformations will be covered in detail in workshop 2.
 */
void reshapeMain(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)w/h, 10, 300000);
  glMatrixMode(GL_MODELVIEW);
}

//
void reshapeText(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, w, h, 0.0);
}

/*
 * display()
 *
 * The GLUT calls this function whenever the window must be redrawn.
 *  
 */
void display()
{
  /*
  long double x = 0;
  long double y = 0;
  long double z = 0;
  */
  time_t			currentTime;
  long double			timeJD = 0;
  
  glutSetWindow(mainWindow);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  // Switch from right-hand to left-hand coordinate space
  // glScalef(1.0, 1.0, -1.0);

  // Place the camera
  gluLookAt(camerax, cameray, cameraz, 0, 0, 0, 0, 1, 0);


  /*
   * These two lines cause the rendered objects to rotate about the y axis.
   * To for faster rotation, increase rotationIncrement, and vice versa.
   * How fast this actually rotatesdepends on the speed of the computer
   * and graphics hardware.
   */

  // static float rotationIncrement = 1000;
  // glRotatef(rotationIncrement, 0, 1.0, 0);
  // glRotatef(rotationIncrement, 0, 0, 1.0);

  // Insert big blue marble
  glColor3f(0,0,1);
  // glutSolidSphere(e_R, 24, 18);
  glutWireSphere(e_R, 24, 18);

  currentTime = time(NULL);
  timeJD = julianDate(gmtime(&currentTime));
  
  for (it = sats.begin(); it != sats.end(); it++)
  {
    posECI = it->position4(timeJD);
  
#ifdef DEBUG
    cout << asctime(gmtime(&currentTime));
    cout << posECI << endl;
    posLLA = posECI;
    cout << posLLA << endl;
#endif

  // x represents semi-major axis
  // y represents semi-minor axis
  // z represents orbital inclination
  // 
  // x = 
  // y = A_0 * sqrt(1 - e_0 * e_0)
  //   =  beta_0
  // z = inclination * sin(i)
  /*
  glBegin(GL_LINE_LOOP);
    glColor3f(1,0,0);
    // go from 0 to 2 \Pi in \frac{\Pi}{4} steps
    for (i = 0; i < (2 * PI); i += M_PI_4)
    {
      glVertex3f( (cos(i + RA) * (A * e_R)),
                  (sin(i + PER) * (B * e_R)),
                  (I_o * sin(i)) * e_R
                );
    }
  glEnd();
  */
  
    // Put a big red square pixel where the satellite should be
    /*
    glPointSize(100);
    glBegin(GL_POINTS);
      glColor3f(1,0,0);
      glVertex3d(posECI.X, posECI.Y, posECI.Z);
    glEnd();
    */
    
    // Put a teapot where the satellite should be
    glPushMatrix();
      glColor3d(0.0, 1.0, 0.0);
      glTranslated(posECI.X, posECI.Y, posECI.Z);
      glutWireTeapot(100);
    glPopMatrix();

    // Slap a label next to the teapot
    glRasterPos3d(posECI.X,posECI.Y,posECI.Z);
    printString(posECI.name);

    // draw a line from the satellite to the center of the earth
    glBegin(GL_LINES);
      glColor3d(1.0, 1.0, 1.0);
      glVertex3d(posECI.X,posECI.Y,posECI.Z);
      glVertex3d(0.0, 0.0, 0.0);
    glEnd();

  }

  glutSwapBuffers();
  glFlush();
        
  glutPostRedisplay();

  // sleep(1);
}

void text()
{
  glutSetWindow(textWindow);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glColor3f(0.9, 0.9, 0.9);
  glRasterPos2i(0, 13);
  
  if (eciOutput)
  {
    // Name
    glRasterPos2i(0, 13);
    printString(posECI.name);
    // X
    glRasterPos2i(0, 26);
    printString(posECI.stringX());
    // Y
    glRasterPos2i(0, 39);
    printString(posECI.stringY());
    // Z
    glRasterPos2i(0, 52);
    printString(posECI.stringZ());
  }
  else
  {
    posLLA = posECI;
    // Name
    glRasterPos2i(0, 13);
    printString(posLLA.name);
    // Lat
    glRasterPos2i(0, 26);
    printString(posLLA.stringLat());
    // Y
    glRasterPos2i(0, 39);
    printString(posLLA.stringLon());
    // Z
    glRasterPos2i(0, 52);
    printString(posLLA.stringAlt());
  }
  
  // Testing stuff...
  
  glutSwapBuffers();
  glFlush();
  
  glutPostRedisplay();
}

/*
 * Quick function for writing a string to a GL window
 * 
 */
void printString(string s)
{
  string::iterator i;
  
  for (i = s.begin(); i != s.end(); i++)
  {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *i);
  }
}

/*
 * keyboard()
 *       Gets called when a button on the keyboard is pressed.
 *       x is a char containing the ascii value of the pressed button.
 *       mouse_x and mouse_y are the coordinates of the mouse in the
 *       window at time of keystroke.
 */
void keyboard(unsigned char key, int mouse_x, int mouse_y)
{
  switch (key)
  {
    case 'd':
    case 'D':
      posLLA.switchOutput();
      break;
    case 'l':
    case 'L':
      eciOutput = !eciOutput;
      break;
    case 'm':
    case 'M':
      break;
    case 'o':
    case 'O':
      camerax = posECI.X;
      cameray = posECI.Y;
      cameraz = posECI.Z;
      break;
    case 'p':
    case 'P':
      if (positionWindow)
      {
	glutSetWindow(textWindow);
	glutHideWindow();
      }
      else
      {
	glutSetWindow(textWindow);
	glutShowWindow();
      }
      positionWindow = !positionWindow;
      break;
    case 'q':
    case 'Q':
      exit(0);
      break;
    case 's':
    case 'S':
      camerax *= 1.3;
      cameray *= 1.3;
      cameraz *= 1.3;
      break;
    case 'w':
    case 'W':
      camerax /= 1.3;
      cameray /= 1.3;
      cameraz /= 1.3;
      break;
    case 'x':
    case 'X':
      camerax = 10000;
      cameray = 0;
      cameraz = 0;
      break;
    case 'y':
    case 'Y':
      camerax = 1;
      cameray = 10000;
      cameraz = 1;
      break;
    case 'z':
    case 'Z':
      camerax = 0;
      cameray = 0;
      cameraz = 10000;
      break;
  }

  glutPostRedisplay();
}

/*
 *
 */
void mouse(int button, int state, int x, int y)
{
  if ((button == GLUT_LEFT_BUTTON) && (state==GLUT_DOWN))
  {
    camerax += x;
    cameray += y;
  }

  if(button == GLUT_LEFT_BUTTON && state==GLUT_UP)
  {
    
  }
  glutPostRedisplay();
}


