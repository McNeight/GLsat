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

#ifndef lint
static const char rcsid[] = "";
#endif

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
#include "graphics.h"
#include "lla.h"
#include "tle.h"

#define HalfSecond (0.5/SecondsPerDay)
#define theC 2.997925e5                    /* Kilometers/Second    */

using namespace std;

void display(void);
void text(void);
void keyboard(unsigned char key, int mouse_x, int mouse_y);
void mouse(int button, int state, int x, int y);

TLE			sat;
vector<TLE>		sats;
vector<TLE>::iterator	satIter;
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

  for (satIter = sats.begin(); satIter != sats.end(); satIter++)
  {
    if (satIter->getValid())
    {
      cout << "Valid TLE for " << satIter->getName() << endl;
    }
  }

#ifdef DEBUG
  struct tm		sputnik;
  long double		timeJD = 0;
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
  mainWindow = glutCreateWindow("GLsat version 0.2");
  // Initializa OpenGL
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshapeMain);
  // Define keyboard function
  glutKeyboardFunc(keyboard);
  //glutMouseFunc(mouse);
  glutIdleFunc(display);
  // For the coordinates sub-window:
  // Create the coordinates sub-window
  textWindow = glutCreateSubWindow(mainWindow, 0, 0, 500, 100);
  glutDisplayFunc(text);
  glutReshapeFunc(reshapeText);
  // Define keyboard function
  glutKeyboardFunc(keyboard);
  glutIdleFunc(text);
  glutMainLoop();
  
#endif
  
  return 0;
}



/*
 * display()
 *
 * The GLUT calls this function whenever the window must be redrawn.
 *  
 */
void display(void)
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

  BlueMarble();

  currentTime = time(NULL);
  timeJD = julianDate(gmtime(&currentTime));
  
  for (satIter = sats.begin(); satIter != sats.end(); satIter++)
  {
    posECI = satIter->position4(timeJD);
  
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


void text(void)
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
    case 27:
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

