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
#include <GL/glut.h>
#include "astro.h"
#include "constants.h"
#include "eci.h"
#include "graphics.h"
//#include "lla.h"
//#include "tle.h"

using namespace std;

/*
 * init()
 *
 * Specifies desired OpenGL features.
 * If things like lighting and textures were in this demo, the
 * appropriate calls would be made here.
 * Must be called after glutCreateWindow().
 *
 */
void
init(void)
{
  cout << glGetString(GL_VENDOR) << " ";
  cout << glGetString(GL_RENDERER) << endl;
  cout << "OpenGL " << glGetString(GL_VERSION) << endl;
        
  glClearDepth(1.0);
  // Set background to black
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST | GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  /* glDisable(GL_DEPTH_TEST) */

}


/*
 * reshapeMain()
 *
 * The GLUT calls this function after the window has created, and
 * every time the window is reshaped.
 *
 * Specifies initial viewing options, including viewing area and model
 * orientation.
 *
 * Viewing transformations will be covered in detail in workshop 2.
 */
void
reshapeMain(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)w/h, 10, 300000);
  glMatrixMode(GL_MODELVIEW);
}


//
void
reshapeText(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, w, h, 0.0);
}


/*
 * Quick function for writing a string to a GL window
 * 
 */
void
printString(string s)
{
  string::iterator i;
  
  for (i = s.begin(); i != s.end(); i++)
  {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *i);
  }
}


/*
 *
 */
void
BlueMarble(void)
{
  // Insert big blue marble
  glColor3f(0,0,1);
  // glutSolidSphere(e_R, 24, 18);
  glutWireSphere(e_R, 24, 18);

  /*
   * from xplanet
void
Init(const ProjectionRectangular *image_rect)
{
    GLUquadricObj *Quadric;

    GLubyte *texImage = image_rect->rgb_data;

    glTexImage2D(GL_TEXTURE_2D, 0, 3, image_rect->getWidth(),
                 image_rect->getHeight(), 0,
                 GL_RGB, GL_UNSIGNED_BYTE, texImage);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Quadric = gluNewQuadric();
    gluQuadricTexture(Quadric, GL_TRUE);

    Sphere = glGenLists(1);
    glNewList(Sphere, GL_COMPILE);
    gluSphere(Quadric, radius, 48, 48);
    glEndList();
*/
  
}
