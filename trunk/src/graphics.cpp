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
#include "lla.h"
#include "tle.h"

using namespace std;

float camerax = e_R * 2;
float cameray = e_R * 2;
float cameraz = e_R * 2;

static GLdouble _left   = 0.0;
static GLdouble _right  = 0.0;
static GLdouble _bottom = 0.0;
static GLdouble _top    = 0.0;
static GLdouble _zNear  = 1.0;
static GLdouble _zFar   = 300000.0;

static int  _mouseX      = 0;
static int  _mouseY      = 0;
static bool _mouseLeft   = false;
static bool _mouseMiddle = false;
static bool _mouseRight  = false;

static double _dragPosX  = 0.0;
static double _dragPosY  = 0.0;
static double _dragPosZ  = 0.0;

static double _matrix[16];
static double _matrixI[16];

static int mainWindow;
static int textWindow;

static bool positionWindow = true;
static bool eciOutput = true;

extern vector<TLE>		sats;

static vector<TLE>::iterator	posIter;
static ECI			posECI;
static LLA			posLLA;

static vector<TLE>::iterator	outputIter;
static ECI			outECI;
static LLA			outLLA;

/*
 *
 */
void GLgraphics(int argc, char *argv[])
{
  outputIter = sats.begin();
  
  // Start GLUT routines
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  // Create the main window
  mainWindow = glutCreateWindow("GLsat version 0.2");
  // Initialize OpenGL
    GraphicsInit();
    glutDisplayFunc(GraphicsDisplay);

  getMatrix();
  glScaled(1 / (e_R * 2), 1 / (e_R * 2), 1 / (e_R * 2));
  
    glutReshapeFunc(reshapeMain);
    // Define keyboard function
    glutKeyboardFunc(keyboard);
    // Define function for special keys
    glutSpecialFunc(special);
    // Define mouse function
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutIdleFunc(GraphicsDisplay);
  
  // For the coordinates sub-window:
  // Create the coordinates sub-window
  textWindow = glutCreateSubWindow(mainWindow, 0, 0, 500, 100);
    glutDisplayFunc(GraphicsText);
    glutReshapeFunc(reshapeText);
    // Define keyboard function
    glutKeyboardFunc(keyboard);
    // Define function for special keys
    glutSpecialFunc(special);
    // Define mouse function
    glutMouseFunc(mouse);
    // We don't want the text window to move, so no motion function
    //glutMotionFunc(motion);
    glutIdleFunc(GraphicsText);

  // Now loop it
  glutMainLoop();
}

/*
 * GraphicsInit()
 *
 * Specifies desired OpenGL features.
 * If things like lighting and textures were in this demo, the
 * appropriate calls would be made here.
 * Must be called after glutCreateWindow().
 *
 */
void
GraphicsInit(void)
{
  // Print out basic version information
  cout << glGetString(GL_VENDOR) << " ";
  cout << glGetString(GL_RENDERER) << endl;
  cout << "OpenGL " << glGetString(GL_VERSION) << endl;
       
  // Set depth buffer
  glClearDepth(1.0);
  // Set background to black
  glClearColor(0.0, 0.0, 0.0, 0.0);
  // Clear buffers to preset values
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Set smooth (Gouraud) shading
  glShadeModel(GL_SMOOTH);
  // Set depth buffer comparison
  glDepthFunc(GL_LESS);
  // Enable color blending and depth testing
  glEnable(GL_BLEND | GL_DEPTH_TEST);
  // Enable backface culling
  glEnable(GL_CULL_FACE);
  // Enable normalize
  glEnable(GL_COLOR_MATERIAL | GL_NORMALIZE);
  // Blend by alpha values
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // Turn on the lights
  //glEnable(GL_LIGHTING);
  // Set up sunlight
  /*
  GLfloat light_ambient[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_position[] = {e_R * 2, e_R * 2, e_R * 2, 0.0};
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);
  */
}


/*
 * GraphicsDisplay()
 *
 * The GLUT calls this function whenever the window must be redrawn.
 *  
 */
void GraphicsDisplay(void)
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

  //glLoadIdentity();

  // Switch from right-hand to left-hand coordinate space
  // glScalef(1.0, 1.0, -1.0);

  // Place the camera
  //gluLookAt(camerax, cameray, cameraz, 0, 0, 0, 0, 1, 0);
  
  //glTranslated(e_R * 2, e_R * 2, e_R * 2);

  /*
   * These two lines cause the rendered objects to rotate about the y axis.
   * To for faster rotation, increase rotationIncrement, and vice versa.
   * How fast this actually rotatesdepends on the speed of the computer
   * and graphics hardware.
   */

  // static float rotationIncrement = 1000;
  // glRotatef(rotationIncrement, 0, 1.0, 0);
  // glRotatef(rotationIncrement, 0, 0, 1.0);

  glPushMatrix();
    BlueMarble();
  glPopMatrix();

  currentTime = time(NULL);
  timeJD = julianDate(gmtime(&currentTime));
  
  for (posIter = sats.begin(); posIter != sats.end(); posIter++)
  {
    posECI = posIter->position4(timeJD);
    if (posIter == outputIter)
    {
      outECI = posECI;
    }
  
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

  // Leave these in only if you want shit to move in real time!
  glutSwapBuffers();
  glFlush();
  glutPostRedisplay();

  // sleep(1);
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
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  _top    = 1.0;
  _bottom = 1.0;
  _left   = -(GLdouble)w/(GLdouble)h;	
  _right  = -_left;
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //gluPerspective(60, (GLfloat)w/h, 10, 300000);
  glOrtho(_left, _right, _bottom, _top, _zNear, _zFar);
  glMatrixMode(GL_MODELVIEW);
}


//
void
reshapeText(int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
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

void GraphicsText(void)
{
  glutSetWindow(textWindow);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glColor3f(0.9, 0.9, 0.9);
  glRasterPos2i(0, 13);
  
  if (eciOutput)
  {
    // Name
    glRasterPos2i(0, 13);
    printString(outECI.name);
    // X
    glRasterPos2i(0, 26);
    printString(outECI.stringX());
    // Y
    glRasterPos2i(0, 39);
    printString(outECI.stringY());
    // Z
    glRasterPos2i(0, 52);
    printString(outECI.stringZ());
  }
  else
  {
    outLLA = outECI;
    // Name
    glRasterPos2i(0, 13);
    printString(outLLA.name);
    // Lat
    glRasterPos2i(0, 26);
    printString(outLLA.stringLat());
    // Y
    glRasterPos2i(0, 39);
    printString(outLLA.stringLon());
    // Z
    glRasterPos2i(0, 52);
    printString(outLLA.stringAlt());
  }
  
  // Testing stuff...
  
  glutSwapBuffers();
  glFlush();
  
  glutPostRedisplay();
}


/*
 *
 */
void
BlueMarble(void)
{
  // Insert big blue marble
  glColor3f(0,0,1);
  glutSolidSphere(e_R, 24, 18);
  //glutWireSphere(e_R, 24, 18);

  /*
   * from xplanet
void
GraphicsInit(const ProjectionRectangular *image_rect)
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

void special(int key, int mouse_x, int mouse_y)
{
  switch(key)
  {
    case GLUT_KEY_F1:
    case GLUT_KEY_F2:
    case GLUT_KEY_F3:
    case GLUT_KEY_F4:
    case GLUT_KEY_F5:
    case GLUT_KEY_F6:
    case GLUT_KEY_F7:
    case GLUT_KEY_F8:
    case GLUT_KEY_F9:
    case GLUT_KEY_F10:
    case GLUT_KEY_F11:
    case GLUT_KEY_F12:
      break;
    case GLUT_KEY_LEFT:
    case GLUT_KEY_RIGHT:
      break;
    case GLUT_KEY_UP:
      outputIter--;
      if (outputIter == sats.begin() - 1)
      {
	outputIter = sats.begin();
      }
      break;
    case GLUT_KEY_DOWN:
      outputIter++;
      if (outputIter == sats.end())
      {
	outputIter = sats.end() - 1;
      }
      break;
    case GLUT_KEY_PAGE_UP:
      outputIter = sats.begin();
      break;
    case GLUT_KEY_PAGE_DOWN:
      outputIter = sats.end() - 1;
      break;
    case GLUT_KEY_HOME:
    case GLUT_KEY_END:
    case GLUT_KEY_INSERT:
      break;
  }
}

/*
 *
 */
void mouse(int button, int state, int x, int y)
{
  int viewport[4];

  _mouseX = x;
  _mouseY = y;
  
  if (state == GLUT_UP)
  {
    switch(button)
    {
      case GLUT_LEFT_BUTTON:
	_mouseLeft = false;
	break;
      case GLUT_MIDDLE_BUTTON:
	_mouseMiddle = false;
	break;
      case GLUT_RIGHT_BUTTON:
	_mouseRight = false;
	break;
    }
  }
  else
  {
    switch(button)
    {
      case GLUT_LEFT_BUTTON:
	_mouseLeft = true;
	break;
      case GLUT_MIDDLE_BUTTON:
	_mouseMiddle = true;
	break;
      case GLUT_RIGHT_BUTTON:
	_mouseRight = true;
	break;
    }
  }

  //glutPostRedisplay();
  glGetIntegerv(GL_VIEWPORT, viewport);
  viewPos(&_dragPosX, &_dragPosY, &_dragPosZ, x, y, viewport);
}

/*
 *
 */
void motion(int x, int y)
{
  bool changed = false;
  
  const int dx = x - _mouseX;
  const int dy = y - _mouseY;
  
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  if ((dx != 0) && (dy != 0))
  {
    if (_mouseLeft)
    {
      double ax, ay, az;
      double bx, by, bz;
      double angle;

      ax = dy;
      ay = dx;
      az = 0.0;
      angle = vlen(ax,ay,az)/(double)(viewport[2]+1)*180.0;

      /* Use inverse matrix to determine local axis of rotation */
      bx = _matrixI[0]*ax + _matrixI[4]*ay + _matrixI[8]*az;
      by = _matrixI[1]*ax + _matrixI[5]*ay + _matrixI[9]*az;
      bz = _matrixI[2]*ax + _matrixI[6]*ay + _matrixI[10]*az;
      
      glRotatef(angle,bx,by,bz);

      changed = true;
    }
    else if (_mouseMiddle || (_mouseLeft && _mouseRight))
    {
      double s = exp((double)dy*0.1);
      glScalef(s,s,s);
      changed = true;
    }
  }

  _mouseX = x;
  _mouseY = y;
  
  if (changed)
  {
    getMatrix();
    glutPostRedisplay();
  }
}

/*
 *
 */
double vlen(double x,double y,double z)
{
	return sqrt(x*x+y*y+z*z);
}


/*
 *
 */
void viewPos(double *px,double *py,double *pz,const int x,const int y,const int *viewport)
{
  /*
   * Use the ortho projection and viewport information
   * to map from mouse co-ordinates back into world 
   * co-ordinates
   */

  *px = (double)(x-viewport[0])/(double)(viewport[2]);
  *py = (double)(y-viewport[1])/(double)(viewport[3]);

  *px = _left + (*px)*(_right-_left);
  *py = _top  + (*py)*(_bottom-_top);
  *pz = _zNear;
}

/*
 *
 */
void getMatrix()
{
  glGetDoublev(GL_MODELVIEW_MATRIX,_matrix);
  invertMatrix(_matrix,_matrixI);
}

/*
 *
 */
void 
invertMatrix(const GLdouble *m, GLdouble *out )
{

/* NB. OpenGL Matrices are COLUMN major. */
#define MAT(m,r,c) (m)[(c)*4+(r)]

/* Here's some shorthand converting standard (row,column) to index. */
#define m11 MAT(m,0,0)
#define m12 MAT(m,0,1)
#define m13 MAT(m,0,2)
#define m14 MAT(m,0,3)
#define m21 MAT(m,1,0)
#define m22 MAT(m,1,1)
#define m23 MAT(m,1,2)
#define m24 MAT(m,1,3)
#define m31 MAT(m,2,0)
#define m32 MAT(m,2,1)
#define m33 MAT(m,2,2)
#define m34 MAT(m,2,3)
#define m41 MAT(m,3,0)
#define m42 MAT(m,3,1)
#define m43 MAT(m,3,2)
#define m44 MAT(m,3,3)

   GLdouble det;
   GLdouble d12, d13, d23, d24, d34, d41;
   GLdouble tmp[16]; /* Allow out == in. */

   /* Inverse = adjoint / det. (See linear algebra texts.)*/

   /* pre-compute 2x2 dets for last two rows when computing */
   /* cofactors of first two rows. */
   d12 = (m31*m42-m41*m32);
   d13 = (m31*m43-m41*m33);
   d23 = (m32*m43-m42*m33);
   d24 = (m32*m44-m42*m34);
   d34 = (m33*m44-m43*m34);
   d41 = (m34*m41-m44*m31);

   tmp[0] =  (m22 * d34 - m23 * d24 + m24 * d23);
   tmp[1] = -(m21 * d34 + m23 * d41 + m24 * d13);
   tmp[2] =  (m21 * d24 + m22 * d41 + m24 * d12);
   tmp[3] = -(m21 * d23 - m22 * d13 + m23 * d12);

   /* Compute determinant as early as possible using these cofactors. */
   det = m11 * tmp[0] + m12 * tmp[1] + m13 * tmp[2] + m14 * tmp[3];

   /* Run singularity test. */
   if (det == 0.0) {
      /* printf("invert_matrix: Warning: Singular matrix.\n"); */
//	  memcpy(out,_identity,16*sizeof(double));
   }
   else {
      GLdouble invDet = 1.0 / det;
      /* Compute rest of inverse. */
      tmp[0] *= invDet;
      tmp[1] *= invDet;
      tmp[2] *= invDet;
      tmp[3] *= invDet;

      tmp[4] = -(m12 * d34 - m13 * d24 + m14 * d23) * invDet;
      tmp[5] =  (m11 * d34 + m13 * d41 + m14 * d13) * invDet;
      tmp[6] = -(m11 * d24 + m12 * d41 + m14 * d12) * invDet;
      tmp[7] =  (m11 * d23 - m12 * d13 + m13 * d12) * invDet;

      /* Pre-compute 2x2 dets for first two rows when computing */
      /* cofactors of last two rows. */
      d12 = m11*m22-m21*m12;
      d13 = m11*m23-m21*m13;
      d23 = m12*m23-m22*m13;
      d24 = m12*m24-m22*m14;
      d34 = m13*m24-m23*m14;
      d41 = m14*m21-m24*m11;

      tmp[8] =  (m42 * d34 - m43 * d24 + m44 * d23) * invDet;
      tmp[9] = -(m41 * d34 + m43 * d41 + m44 * d13) * invDet;
      tmp[10] =  (m41 * d24 + m42 * d41 + m44 * d12) * invDet;
      tmp[11] = -(m41 * d23 - m42 * d13 + m43 * d12) * invDet;
      tmp[12] = -(m32 * d34 - m33 * d24 + m34 * d23) * invDet;
      tmp[13] =  (m31 * d34 + m33 * d41 + m34 * d13) * invDet;
      tmp[14] = -(m31 * d24 + m32 * d41 + m34 * d12) * invDet;
      tmp[15] =  (m31 * d23 - m32 * d13 + m33 * d12) * invDet;

      memcpy(out, tmp, 16*sizeof(GLdouble));
   }

#undef m11
#undef m12
#undef m13
#undef m14
#undef m21
#undef m22
#undef m23
#undef m24
#undef m31
#undef m32
#undef m33
#undef m34
#undef m41
#undef m42
#undef m43
#undef m44
#undef MAT
}

