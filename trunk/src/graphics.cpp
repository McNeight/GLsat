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

#include <algorithm>
#include <cmath>
#include <ctime>
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

using namespace std;

TLE		sat;
vector<TLE>	sats;
ECI		pos;

long double A;
long double B;
long double E;
long double I_o;
long double PER;
long double RA;

float camerax = e_R * 2;
float cameray = e_R * 2;
float cameraz = e_R * 2;

/*
 * init()
 *
 * Specifies desired OpenGL features.
 * If things like lighting and textures were in this demo, the
 * appropriate calls would be made here.
 * Must be called after glutCreateWindow().
 *
 */
void init(vector<TLE> &satellites)
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
  sats = satellites;
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
void reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)w/h, 10, 300000);
  glMatrixMode(GL_MODELVIEW);
}

//
void reshapeText (int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0F, 1.0F, 0.0F, 1.0F);
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
  vector<TLE>::iterator		i;
  time_t			currentTime;
  long double			timeJD = 0;
  LLA				lla;
  
  glutSetWindow(1);
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
  
  for (i = sats.begin(); i != sats.end(); i++)
  {
    pos = i->position4(timeJD);
  
#ifdef DEBUG
    cout << asctime(gmtime(&currentTime));
    cout << pos << endl;
    lla = pos;
    cout << lla << endl;
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
      glVertex3d(pos.X, pos.Y, pos.Z);
    glEnd();
    */
    
    // Put a teapot where the satellite should be
    glPushMatrix();
      glColor3d(0.0, 1.0, 0.0);
      glTranslated(pos.X, pos.Y, pos.Z);
      glutWireTeapot(100);
    glPopMatrix();

    // Slap a label next to the teapot
    glRasterPos3d(pos.X,pos.Y,pos.Z);
    printString(pos.name);

    // draw a line from the satellite to the center of the earth
    glBegin(GL_LINES);
      glColor3d(1.0, 1.0, 1.0);
      glVertex3d(pos.X,pos.Y,pos.Z);
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
  string temp("blah");
  ostringstream temp2;
  
  glutSetWindow(2);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glColor3f(0.9, 0.9, 0.9);
  glRasterPos2f(0.0, 0.0);
  
  // temp = pos.name;
  temp2 << "X = " << pos.X << endl;
  printString(temp2.str());
  glRasterPos2f(0.30, 0.30);
  printString(temp);
  printString(pos.name);
  cout << pos << endl;
  
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
 * key()
 *       Gets called when a button on the keyboard is pressed.
 *       x is a char containing the ascii value of the pressed button.
 *       mouse_x and mouse_y are the coordinates of the mouse in the
 *       window at time of keystroke.
 */
void key(unsigned char x, int mouse_x, int mouse_y)
{
  if (x == 'm')
  {
  }
  else if (x == 'o')
  {
    camerax = pos.X;
    cameray = pos.Y;
    cameraz = pos.Z;
  }
  else if (x == 'q')
  {
    exit(0);
  }
  else if (x == 's')
  {
    camerax *= 1.3;
    cameray *= 1.3;
    cameraz *= 1.3;
  }
  else if (x == 'w')
  {
    camerax /= 1.3;
    cameray /= 1.3;
    cameraz /= 1.3;
  }
  else if (x == 'x')
  {
    camerax = 10000;
    cameray = 0;
    cameraz = 0;
  }
  else if (x == 'y')
  {
    camerax = 1;
    cameray = 10000;
    cameraz = 1;
  }
  else if (x == 'z')
  {
    camerax = 0;
    cameray = 0;
    cameraz = 10000;
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

