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
#include <cstring>    // for strstr()
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>		// for sleep()
#include <vector>

#ifdef __APPLE_CC__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include "astro.h"
#include "color.h"
#include "constants.h"
#include "eci.h"
#include "graphics.h"
#include "image.h"
#include "lla.h"
#include "tle.h"

using namespace std;

static float					radius = e_R * 3.5;
static float                    camera[3] = {radius, 0, 0};

static int                      _mouseX      = 0;
static int                      _mouseY      = 0;

static bool                     _mouseLeft   = false;
static bool                     _mouseMiddle = false;
static bool                     _mouseRight  = false;

static int                      mainWindow;
static int                      textWindow;
static int						mainMenu, intervalMenu;

static long double				interval = 0.03333333333 / 24.0;

static bool                     positionWindow = true;
static bool                     eciOutput = true;

extern vector<TLE>				sats;

static vector<TLE>::iterator	posIter;
static ECI						posECI;
static LLA						posLLA;

static vector<TLE>::iterator	outputIter;
static ECI						outECI;
static LLA						outLLA;

static ECI                      sunPos;

static float					ang = 0;

static GLUnurbsObj				*theNurb = NULL;

static Image					noaa17;
static GLuint					noaa17_tex;

static Image					earth1;
static Image					earth2;
static Image					earth4;
static Image					earth8;
static Image					earth16;
static Image					earth32;
static Image					earth64;
static Image					earth128;
static Image					earth256;
static Image					earth512;
static Image					earth1024;
static Image					earth2048;
static GLuint					earth_tex;

static GLuint 					Sphere;
/*
 *
 */
void GLgraphics(int argc, char *argv[])
{
    outputIter = sats.begin();
    
    // Start GLUT routines
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    // Create the main window
    mainWindow = glutCreateWindow("GLsat version 0.3");
    // Initialize OpenGL
    GraphicsInit();
    glutDisplayFunc(GraphicsDisplay);
    
    //glScaled(1 / (e_R * 2), 1 / (e_R * 2), 1 / (e_R * 2));
    
    glutReshapeFunc(GraphicsReshapeMain);
    // Define keyboard function
    glutKeyboardFunc(GraphicsCallbackKeyboard);
    // Define function for special keys
    glutSpecialFunc(GraphicsCallbackSpecial);
    // Define mouse function
    glutMouseFunc(GraphicsCallbackMouse);
    //glutMouseFunc(mouse);
    glutMotionFunc(GraphicsCallbackMotion);
    glutIdleFunc(GraphicsDisplay);
    
    // For the coordinates sub-window:
    // Create the coordinates sub-window
    textWindow = glutCreateSubWindow(mainWindow, 0, 0, 500, 53);
    glutDisplayFunc(GraphicsText);
    glutReshapeFunc(GraphicsReshapeText);
    // Define keyboard function
    glutKeyboardFunc(GraphicsCallbackKeyboard);
    // Define function for special keys
    glutSpecialFunc(GraphicsCallbackSpecial);
    // Define mouse function
    glutMouseFunc(GraphicsCallbackMouse);
    //glutMouseFunc(mouse);
    // We don't want the text window to move, so no motion function
    //glutMotionFunc(GraphicsCallbackMotion);
    glutIdleFunc(GraphicsText);
    
    // Return to the main window
    glutSetWindow(mainWindow);
    // Create a pull down menu
    intervalMenu = glutCreateMenu(GraphicsIncrementMenu);
    //glutAddMenuEntry("Set Satellite Prediction Interval", 0);
    glutAddMenuEntry("2 minutes", 1);
    glutAddMenuEntry("5 minutes", 2);
    glutAddMenuEntry("10 minutes", 3);
    glutAddMenuEntry("15 minutes", 4);
    glutAddMenuEntry("30 minutes", 5);
    glutAddMenuEntry("1 hour", 6);
    
    mainMenu = glutCreateMenu(GraphicsMainMenu);
    glutAddMenuEntry("GLsat version 0.3", 0);
    glutAddMenuEntry("---------------------------------", 1);
    glutAddSubMenu("Set Satellite Prediction Interval", intervalMenu);
    
    // Now attach it
    glutAttachMenu( GLUT_RIGHT_BUTTON );
    
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
    // Clearing this to 0.0 is a good way to wind up with a black screen
    glClearDepth(1.0);
    // Set background to black
    glClearColor(0.0, 0.0, 0.0, 0.0);
    // Clear buffers to preset values
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    // Disable color blending (unless you want to see through the Earth)
    glDisable(GL_BLEND);
    // Enable backface culling
    glEnable(GL_CULL_FACE);
    // Enable normalize
    glEnable(GL_NORMALIZE);
    // Enable material properties
    glEnable(GL_COLOR_MATERIAL);
    // Generate surface normals automatically
    glEnable(GL_AUTO_NORMAL);
    // Blend by alpha values
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Cull the backside of objects
    glCullFace(GL_BACK);
    // Set depth buffer to writable
    glDepthMask(GL_TRUE);
    // Set depth buffer comparison
    glDepthFunc(GL_LEQUAL);
    // Set smooth (Gouraud) shading model
    glShadeModel(GL_SMOOTH);
    // 
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    // Turn on the lights
    glEnable(GL_LIGHTING);
    // Set up sunlight
    
    GLfloat light_ambient[] = {0.1, 0.1, 0.1, 1.0};
    GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_position[] = {e_R * 100, 0, e_R * 100, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);
    
    // Set up the texture maps
    GraphicsSetupTextureMaps();
    
    // Set up gluSphere
    GLUquadricObj *Quadric;
    Quadric = gluNewQuadric();
    gluQuadricTexture(Quadric, GL_TRUE);
    
    Sphere = glGenLists(1);
    glNewList(Sphere, GL_COMPILE);
    //gluSphere(Quadric, e_R, 96, 72);
    gluSphere(Quadric, e_R, 200, 200);
    glEndList();  
}


/*
 * GraphicsDisplay()
 *
 * The GLUT calls this function whenever the window must be redrawn.
 *  
 */
void GraphicsDisplay(void)
{
    glutSetWindow(mainWindow);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Switch from right-hand to left-hand coordinate space
    // glScalef(1.0, 1.0, -1.0);
    
    // Place the camera
    glLoadIdentity();
    gluLookAt(camera[0], camera[1], camera[2], 0, 0, 0, 0, 1, 0);
    
    // Rotate everything so that the North Pole is up
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    
    // Draw the Earth
    glPushMatrix();
    GraphicsBlueMarble();
    glPopMatrix();
    
    // Get the current Julian Day
    time_t currentTime = time(NULL);
    long double timeJD = julianDate(gmtime(&currentTime));
    
    // Move the light around
    glPushMatrix();
    sunPos = sunPositionLow(timeJD);
    GLfloat light_position[] = {sunPos.X, sunPos.Y, sunPos.Z, 1.0};
    //cout << sunPos.X << " " << sunPos.Y << " " << sunPos.Z << endl;
    //GLfloat light_position[] = {e_R * 100, 0, 0, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glPopMatrix();
    
    // Draw the satellites
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
        
        GraphicsDrawSat(*posIter, timeJD);
        
    }
    
    // Leave these in only if you want shit to move in real time!
    glutSwapBuffers();
    glFlush();
    glutPostRedisplay();
}


/*
 * GraphicsReshapeMain()
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
GraphicsReshapeMain(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    
    glMatrixMode(GL_PROJECTION);
    
    //glPushMatrix();
    glLoadIdentity();
    //glFrustum(_left, _right, _bottom, _top, _zNear, _zFar);
    gluPerspective(45, (GLfloat)w / (GLfloat)h, 10, 3000000);
    //glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    
    //glPushMatrix();
    glLoadIdentity();
    gluLookAt(camera[0], camera[1], camera[2], 0, 0, 0, 0, 1, 0);
    //glPopMatrix();
}


/*
 *
 */
void
GraphicsReshapeText(int w, int h)
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


/*
 *
 */
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
    
    // Leave these in only if you want shit to move in real time!
    glutSwapBuffers();
    glFlush();
    glutPostRedisplay();
}


/*
 *
 */
void
GraphicsSetupTextureMaps(void)
{
    // Load in the texture maps
    ifstream	ifile;
    
    // Original graphic: noaa-17.small.jpg
    // Found at: http://earthobservatory.nasa.gov/Newsroom/NasaNews/2002/200206249448.html
    // http://earthobservatory.nasa.gov/Newsroom/MediaResources/NOAA-M/noaa_lo_tn.jpg
    ifile.open("../img/noaa-17.small.ppm");
    if (ifile)
    {
        ifile >> noaa17;
        ifile.close();
        cout << "NOAA17 read!" << endl;
    }
    else
    {
        ifile.close();
        cout << "Error opening NOAA17 file!" << endl;
    }
    
    // Original graphic: land_ocean_ice_2048.jpg
    // Found at: http://flatplanet.sourceforge.net/maps/natural.html
    // http://flatplanet.sourceforge.net/maps/images/land_ocean_ice_2048.jpg
    // Land Surface, Ocean Color, and Sea Ice
    // from the Blue Marble page at NASA's Earth Observatory.
    ifile.open("../img/land_ocean_ice_1.ppm");
    if (ifile)
    {
        ifile >> earth1;
        ifile.close();
        cout << "EARTH1 read!" << endl;
    }
    else
    {
        ifile.close();
        cout << "Error opening EARTH1 file!" << endl;
    }
    
    ifile.open("../img/land_ocean_ice_2.ppm");
    if (ifile)
    {
        ifile >> earth2;
        ifile.close();
        cout << "EARTH2 read!" << endl;
    }
    else
    {
        ifile.close();
        cout << "Error opening EARTH2 file!" << endl;
    }
    
    ifile.open("../img/land_ocean_ice_4.ppm");
    if (ifile)
    {
        ifile >> earth4;
        ifile.close();
        cout << "EARTH4 read!" << endl;
    }
    else
    {
        ifile.close();
        cout << "Error opening EARTH4 file!" << endl;
    }
    
    ifile.open("../img/land_ocean_ice_8.ppm");
    if (ifile)
    {
        ifile >> earth8;
        ifile.close();
        cout << "EARTH8 read!" << endl;
    }
    else
    {
        ifile.close();
        cout << "Error opening EARTH8 file!" << endl;
    }
    
    ifile.open("../img/land_ocean_ice_16.ppm");
    if (ifile)
    {
        ifile >> earth16;
        ifile.close();
        cout << "EARTH16 read!" << endl;
    }
    else
    {
        ifile.close();
        cout << "Error opening EARTH16 file!" << endl;
    }
    
    ifile.open("../img/land_ocean_ice_32.ppm");
    if (ifile)
    {
        ifile >> earth32;
        ifile.close();
        cout << "EARTH32 read!" << endl;
    }
    else
    {
        ifile.close();
        cout << "Error opening EARTH32 file!" << endl;
    }
    
    ifile.open("../img/land_ocean_ice_64.ppm");
    if (ifile)
    {
        ifile >> earth64;
        ifile.close();
        cout << "EARTH64 read!" << endl;
    }
    else
    {
        ifile.close();
        cout << "Error opening EARTH64 file!" << endl;
    }
    
    ifile.open("../img/land_ocean_ice_128.ppm");
    if (ifile)
    {
        ifile >> earth128;
        ifile.close();
        cout << "EARTH128 read!" << endl;
    }
    else
    {
        ifile.close();
        cout << "Error opening EARTH128 file!" << endl;
    }
    
    ifile.open("../img/land_ocean_ice_256.ppm");
    if (ifile)
    {
        ifile >> earth256;
        ifile.close();
        cout << "EARTH256 read!" << endl;
    }
    else
    {
        ifile.close();
        cout << "Error opening EARTH256 file!" << endl;
    }
    
    ifile.open("../img/land_ocean_ice_512.ppm");
    if (ifile)
    {
        ifile >> earth512;
        ifile.close();
        cout << "EARTH512 read!" << endl;
    }
    else
    {
        ifile.close();
        cout << "Error opening EARTH512 file!" << endl;
    }
    
    ifile.open("../img/land_ocean_ice_1024.ppm");
    if (ifile)
    {
        ifile >> earth1024;
        ifile.close();
        cout << "EARTH1024 read!" << endl;
    }
    else
    {
        ifile.close();
        cout << "Error opening EARTH1024 file!" << endl;
    }
    
    ifile.open("../img/land_ocean_ice_2048.ppm");
    if (ifile)
    {
        ifile >> earth2048;
        ifile.close();
        cout << "EARTH2048 read!" << endl;
    }
    else
    {
        ifile.close();
        cout << "Error opening EARTH2048 file!" << endl;
    }
    
    // Set up NOAA 17 texture map
    cout << "noaa17.getwidth() = " << (int)noaa17.getwidth() << endl;
    cout << "noaa17.getheight() = " << (int)noaa17.getheight() << endl;
    cout << "noaa17.getdepth() = " << (int)noaa17.getdepth() << endl;
    
    int height = noaa17.getheight();
    int width = noaa17.getwidth();
    //static int noaa17_pixels[height][width][4];
    static GLubyte noaa17_pixels[64][128][4];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            noaa17_pixels[i][j][0] = (GLubyte) noaa17(j,i).red();
            noaa17_pixels[i][j][1] = (GLubyte) noaa17(j,i).green();
            noaa17_pixels[i][j][2] = (GLubyte) noaa17(j,i).blue();
            noaa17_pixels[i][j][3] = (GLubyte) 255;
        }
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glGenTextures(1, &noaa17_tex);
    glBindTexture(GL_TEXTURE_2D, noaa17_tex);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, noaa17_pixels);
    
    // Set up EARTH1 texture map
    cout << "earth1.getwidth() = " << (int)earth1.getwidth() << endl;
    cout << "earth1.getheight() = " << (int)earth1.getheight() << endl;
    cout << "earth1.getdepth() = " << (int)earth1.getdepth() << endl;
    
    height = earth1.getheight();
    width = earth1.getwidth();
    static GLubyte earth1_pixels[1][1][4];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            earth1_pixels[i][j][0] = (GLubyte) earth1(j,i).red();
            earth1_pixels[i][j][1] = (GLubyte) earth1(j,i).green();
            earth1_pixels[i][j][2] = (GLubyte) earth1(j,i).blue();
            earth1_pixels[i][j][3] = (GLubyte) 255;
        }
    }
    
    // Set up EARTH2 texture map
    height = earth2.getheight();
    width = earth2.getwidth();
    static GLubyte earth2_pixels[1][2][4];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            earth2_pixels[i][j][0] = (GLubyte) earth2(j,i).red();
            earth2_pixels[i][j][1] = (GLubyte) earth2(j,i).green();
            earth2_pixels[i][j][2] = (GLubyte) earth2(j,i).blue();
            earth2_pixels[i][j][3] = (GLubyte) 255;
        }
    }
    
    // Set up EARTH4 texture map
    height = earth4.getheight();
    width = earth4.getwidth();
    static GLubyte earth4_pixels[2][4][4];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            earth4_pixels[i][j][0] = (GLubyte) earth4(j,i).red();
            earth4_pixels[i][j][1] = (GLubyte) earth4(j,i).green();
            earth4_pixels[i][j][2] = (GLubyte) earth4(j,i).blue();
            earth4_pixels[i][j][3] = (GLubyte) 255;
        }
    }
    
    // Set up EARTH8 texture map
    height = earth8.getheight();
    width = earth8.getwidth();
    static GLubyte earth8_pixels[4][8][4];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            earth8_pixels[i][j][0] = (GLubyte) earth8(j,i).red();
            earth8_pixels[i][j][1] = (GLubyte) earth8(j,i).green();
            earth8_pixels[i][j][2] = (GLubyte) earth8(j,i).blue();
            earth8_pixels[i][j][3] = (GLubyte) 255;
        }
    }
    
    // Set up EARTH16 texture map
    height = earth16.getheight();
    width = earth16.getwidth();
    static GLubyte earth16_pixels[8][16][4];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            earth16_pixels[i][j][0] = (GLubyte) earth16(j,i).red();
            earth16_pixels[i][j][1] = (GLubyte) earth16(j,i).green();
            earth16_pixels[i][j][2] = (GLubyte) earth16(j,i).blue();
            earth16_pixels[i][j][3] = (GLubyte) 255;
        }
    }
    
    // Set up EARTH32 texture map
    height = earth32.getheight();
    width = earth32.getwidth();
    static GLubyte earth32_pixels[16][32][4];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            earth32_pixels[i][j][0] = (GLubyte) earth32(j,i).red();
            earth32_pixels[i][j][1] = (GLubyte) earth32(j,i).green();
            earth32_pixels[i][j][2] = (GLubyte) earth32(j,i).blue();
            earth32_pixels[i][j][3] = (GLubyte) 255;
        }
    }
    
    // Set up EARTH64 texture map
    height = earth64.getheight();
    width = earth64.getwidth();
    static GLubyte earth64_pixels[32][64][4];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            earth64_pixels[i][j][0] = (GLubyte) earth64(j,i).red();
            earth64_pixels[i][j][1] = (GLubyte) earth64(j,i).green();
            earth64_pixels[i][j][2] = (GLubyte) earth64(j,i).blue();
            earth64_pixels[i][j][3] = (GLubyte) 255;
        }
    }
    
    // Set up EARTH128 texture map
    height = earth128.getheight();
    width = earth128.getwidth();
    static GLubyte earth128_pixels[64][128][4];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            earth128_pixels[i][j][0] = (GLubyte) earth128(j,i).red();
            earth128_pixels[i][j][1] = (GLubyte) earth128(j,i).green();
            earth128_pixels[i][j][2] = (GLubyte) earth128(j,i).blue();
            earth128_pixels[i][j][3] = (GLubyte) 255;
        }
    }
    
    // Set up EARTH256 texture map
    height = earth256.getheight();
    width = earth256.getwidth();
    static GLubyte earth256_pixels[128][256][4];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            earth256_pixels[i][j][0] = (GLubyte) earth256(j,i).red();
            earth256_pixels[i][j][1] = (GLubyte) earth256(j,i).green();
            earth256_pixels[i][j][2] = (GLubyte) earth256(j,i).blue();
            earth256_pixels[i][j][3] = (GLubyte) 255;
        }
    }
    
    // Set up EARTH512 texture map
    height = earth512.getheight();
    width = earth512.getwidth();
    static GLubyte earth512_pixels[256][512][4];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            earth512_pixels[i][j][0] = (GLubyte) earth512(j,i).red();
            earth512_pixels[i][j][1] = (GLubyte) earth512(j,i).green();
            earth512_pixels[i][j][2] = (GLubyte) earth512(j,i).blue();
            earth512_pixels[i][j][3] = (GLubyte) 255;
        }
    }
    
    // Set up EARTH1024 texture map
    height = earth1024.getheight();
    width = earth1024.getwidth();
    static GLubyte earth1024_pixels[512][1024][4];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            earth1024_pixels[i][j][0] = (GLubyte) earth1024(j,i).red();
            earth1024_pixels[i][j][1] = (GLubyte) earth1024(j,i).green();
            earth1024_pixels[i][j][2] = (GLubyte) earth1024(j,i).blue();
            earth1024_pixels[i][j][3] = (GLubyte) 255;
        }
    }
    
    // Set up EARTH2048 texture map
    cout << "earth2048.getwidth() = " << (int)earth2048.getwidth() << endl;
    cout << "earth2048.getheight() = " << (int)earth2048.getheight() << endl;
    cout << "earth2048.getdepth() = " << (int)earth2048.getdepth() << endl;
    
    height = earth2048.getheight();
    width = earth2048.getwidth();
    static GLubyte earth2048_pixels[1024][2048][4];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            earth2048_pixels[i][j][0] = (GLubyte) earth2048(j,i).red();
            earth2048_pixels[i][j][1] = (GLubyte) earth2048(j,i).green();
            earth2048_pixels[i][j][2] = (GLubyte) earth2048(j,i).blue();
            earth2048_pixels[i][j][3] = (GLubyte) 255;
        }
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glGenTextures(1, &earth_tex);
    glBindTexture(GL_TEXTURE_2D, earth_tex);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2048, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, earth2048_pixels);
    glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, earth1024_pixels);
    glTexImage2D(GL_TEXTURE_2D, 2, GL_RGBA, 512, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, earth512_pixels);
    glTexImage2D(GL_TEXTURE_2D, 3, GL_RGBA, 256, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, earth256_pixels);
    glTexImage2D(GL_TEXTURE_2D, 4, GL_RGBA, 128, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, earth128_pixels);
    glTexImage2D(GL_TEXTURE_2D, 5, GL_RGBA, 64, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, earth64_pixels);
    glTexImage2D(GL_TEXTURE_2D, 6, GL_RGBA, 32, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, earth32_pixels);
    glTexImage2D(GL_TEXTURE_2D, 7, GL_RGBA, 16, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, earth16_pixels);
    glTexImage2D(GL_TEXTURE_2D, 8, GL_RGBA, 8, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, earth8_pixels);
    glTexImage2D(GL_TEXTURE_2D, 9, GL_RGBA, 4, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, earth4_pixels);
    glTexImage2D(GL_TEXTURE_2D, 10, GL_RGBA, 2, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, earth2_pixels);
    glTexImage2D(GL_TEXTURE_2D, 11, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, earth1_pixels);
    
}

/*
 *
 */
void
GraphicsBlueMarble(void)
{
    // Set up the material properties
    GLfloat earth_ambient[] = {0.1, 0.1, 0.1, 1.0};
    GLfloat earth_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat earth_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat earth_emissive[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat earth_shininess[] = {0.0};
    
    // Prime the colors
    //glColor3d(0.0, 0.0, 1.0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, earth_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, earth_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, earth_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, earth_emissive);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, earth_shininess);
    
    // Rotate axes to match ECI reference frame
    // Match North Pole with Z axis
    glRotatef(180.0, 1.0, 0.0, 0.0);
    // Match 0 degrees Longitude with X axis (maybe need more accurate texture map?)
    // Also, have to spin the Earth underneath the satellites
    
    // Get the current Julian Day
    time_t currentTime = time(NULL);
    long double timeJD = julianDate(gmtime(&currentTime));
    
    // Calculate the current Greenwich Mean Sidereal Time
    long double T_u = (timeJD - 2451545.0) / 36525;
    long double angle = 280.46061837 + (360.98564736629 * (timeJD - 2451545.0)) + (0.000387933 * T_u * T_u)
        - ((T_u * T_u * T_u) / 38710000);
    
    // Spin the world underneath (plus 125 degrees to offset the texture map)
    //glRotatef(fmod((-angle /*+ 125.0*/), (long double)360.0), 0.0, 0.0, 1.0);
    glRotatef(-angle + 90.0, 0.0, 0.0, 1.0);
    
    // Enable 2D texture mapping
    glEnable(GL_TEXTURE_2D);
    // Set up the texture map
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, earth_tex);
    
    // Insert big blue marble that you can see through
    //glutWireSphere(e_R, 24, 18);
    
    // Insert big blue marble (w/o texture maps)
    //glutSolidSphere(e_R, 24, 18);
    
    // Insert big blue marble (w/texture maps)
    // glu version
    glCallList(Sphere);
    
    // Disable 2D texture mapping
    glDisable(GL_TEXTURE_2D);
}


/*
 *
 */
void GraphicsDrawSat(TLE currentSat, long double &timeJD)
{
    // Put a big red square pixel where the satellite should be
    /*
     glPointSize(100);
     glBegin(GL_POINTS);
     glColor3f(1,0,0);
     glVertex3d(posECI.X, posECI.Y, posECI.Z);
     glEnd();
     */
    
    // Someday, these colors will be satellite dependent
    GLfloat sat_ambient[] = {0.1, 0.7, 0.1, 1.0};
    GLfloat sat_diffuse[] = {0.0, 1.0, 0.0, 1.0};
    GLfloat sat_specular[] = {0.0, 0.5, 0.0, 1.0};
    GLfloat sat_emissive[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat sat_shininess[] = {0.0};
    
    GLfloat white_ambient[] = {0.1, 0.1, 0.1, 1.0};
    GLfloat white_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat white_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat white_emissive[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat white_shininess[] = {0.0};  
    
    // Put a teapot where the satellite should be
    glPushMatrix();
    glColor3d(0.0, 1.0, 0.0);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, sat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK , GL_DIFFUSE, sat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, sat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, sat_emissive);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, sat_shininess);
    glTranslated(posECI.X, posECI.Y, posECI.Z);
    if((strstr(posECI.name.c_str(), "NOAA 17") != NULL))
    {
        GraphicsDrawNOAA17(); 
    }
    else
    {
        //glutSolidTeapot(100);
        glutSolidSphere(100, 18, 24);
    }
    glPopMatrix();
    
    // Slap a label next to the teapot
    glPushMatrix();
    glColor3d(0.0, 1.0, 0.0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, sat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, sat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, sat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, sat_emissive);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, sat_shininess);
    glRasterPos3d(posECI.X,posECI.Y,posECI.Z);
    printString(posECI.name);
    glPopMatrix();
    
    // draw a line from the satellite to the center of the earth
    glPushMatrix();
    glBegin(GL_LINES);
    glColor3d(1.0, 1.0, 1.0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, white_emissive);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, white_shininess);
    glVertex3d(posECI.X,posECI.Y,posECI.Z);
    glVertex3d(0.0, 0.0, 0.0);
    glEnd();
    glPopMatrix();
    
    // Start the NURBS curve for the satellite path
    GLfloat      ctlpoints[4][3];
    GLfloat      knots[7] = {0.0, 0.0, 1.0, 1.0, 2.0, 3.0, 3.0};
    ECI          tempECI;
    
    ctlpoints[0][0] = posECI.X;
    ctlpoints[0][1] = posECI.Y;
    ctlpoints[0][2] = posECI.Z;
    
    for (int i = 1; i < 4; i++)
    {
        tempECI = currentSat.position4(timeJD + (i * interval));
        ctlpoints[i][0] = tempECI.X;
        ctlpoints[i][1] = tempECI.Y;
        ctlpoints[i][2] = tempECI.Z;
        glPushMatrix();
        glTranslated(tempECI.X, tempECI.Y, tempECI.Z);
        //glutSolidCone(50, 100, 10, 10);
        glutSolidSphere(50, 18, 24);
        glPopMatrix();
    }
    
    // Perhaps a decent tutorial on OpenGL NURBS?
    // http://www.edm2.com/0608/opengl.html
    // http://www.jch.com/NURBS/
    
    /*
     if (!theNurb)
     theNurb = gluNewNurbsRenderer();
     gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 5.0);
     //gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);
     gluBeginCurve(theNurb);
     //gluNurbsCurve(theNurb, 7, knots, 1, &ctlpoints[0][0], 3, GL_MAP1_TEXTURE_COORD_2);
     //gluNurbsCurve(theNurb, 7, knots, 1, &ctlpoints[0][0], 3, GL_MAP1_NORMAL);
     gluNurbsCurve(theNurb, 7, knots, 3, &ctlpoints[0][0], 3, GL_MAP1_VERTEX_4);
     gluEndCurve(theNurb);
     */
    
    //gluDeleteNurbsRenderer(theNurb);
}


/*
 *
 */
void GraphicsDrawNOAA17(void)
{
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, noaa17_tex);
    glutSolidCube(100);
    glBegin(GL_QUADS); // Top
    glTexCoord2f(0.0, 0.0); glVertex3f(50.0, 50.0, 50);
    glTexCoord2f(1.0, 0.0); glVertex3f(50.0, -50.0, 50);
    glTexCoord2f(1.0, 1.0); glVertex3f(-50.0, -50.0, 50);
    glTexCoord2f(0.0, 1.0); glVertex3f(-50.0, 50.0, 50);
    glEnd();
    glBegin(GL_QUADS); // Left Front
    glTexCoord2f(0.0, 0.0); glVertex3f(50.0, 50.0, 50);
    glTexCoord2f(0.0, 1.0); glVertex3f(50.0, -50.0, 50);
    glTexCoord2f(1.0, 1.0); glVertex3f(50.0, -50.0, -50);
    glTexCoord2f(1.0, 0.0); glVertex3f(50.0, 50.0, -50);
    glEnd();
    glBegin(GL_QUADS); // Right Front
    glTexCoord2f(0.0, 0.0); glVertex3f(50.0, 50.0, 50);
    glTexCoord2f(0.0, 1.0); glVertex3f(50.0, 50.0, -50);
    glTexCoord2f(1.0, 1.0); glVertex3f(-50.0, 50.0, -50);
    glTexCoord2f(1.0, 0.0); glVertex3f(-50.0, 50.0, 50);    
    glEnd();
    glBegin(GL_QUADS); // Left Rear
    glTexCoord2f(0.0, 0.0); glVertex3f(50.0, -50.0, 50);
    glTexCoord2f(0.0, 1.0); glVertex3f(50.0, -50.0, -50);
    glTexCoord2f(1.0, 1.0); glVertex3f(-50.0, -50.0, -50);
    glTexCoord2f(1.0, 0.0); glVertex3f(-50.0, -50.0, 50);
    glEnd();
    glBegin(GL_QUADS); // Right Rear
    glTexCoord2f(0.0, 0.0); glVertex3f(-50.0, -50.0, 50);
    glTexCoord2f(0.0, 1.0); glVertex3f(-50.0, 50.0, 50);
    glTexCoord2f(1.0, 1.0); glVertex3f(-50.0, 50.0, -50);
    glTexCoord2f(1.0, 0.0); glVertex3f(-50.0, -50.0, -50);
    glEnd();
    glBegin(GL_QUADS); // Bottom
    glTexCoord2f(0.0, 0.0); glVertex3f(50.0, 50.0, -50);
    glTexCoord2f(0.0, 1.0); glVertex3f(50.0, -50.0, -50);
    glTexCoord2f(1.0, 1.0); glVertex3f(-50.0, -50.0, -50);
    glTexCoord2f(1.0, 0.0); glVertex3f(-50.0, 50.0, -50);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
}

/*
 * GraphicsCallbackKeyboard()
 *       Gets called when a button on the keyboard is pressed.
 *       x is a char containing the ascii value of the pressed button.
 *       mouse_x and mouse_y are the coordinates of the mouse in the
 *       window at time of keystroke.
 */
void GraphicsCallbackKeyboard(unsigned char key, int mouse_x, int mouse_y)
{
    switch (key)
    {
        case 'a':
        case 'A':
            camera[0] /= 1.3;
            camera[1] /= 1.3;
            camera[2] /= 1.3;
            radius /= 1.3;
            break;
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
        case 'n':
        case 'N':
            radius = e_R * 3.5;
            camera[0] = 1;
            camera[1] = radius;
            camera[2] = 1;
            break;
        case 'o':
        case 'O':
            /*
             outLLA = outECI;
             radius = outLLA.alt;
             camera[0] = outECI.X;
             camera[1] = outECI.Y;
             camera[2] = outECI.Z;
             */
            break;
        case 'p':
        case 'P':
            break;
        case 'q':
        case 'Q':
        case 27:
            exit(0);
            break;
        case 'r':
        case 'R':
            radius = e_R * 3.5;
            camera[0] = radius;
            camera[1] = 0;
            camera[2] = 0;
            ang = 0;
            break;
        case 's':
        case 'S':
            radius = e_R * 3.5;
            camera[0] = 1;
            camera[1] = -radius;
            camera[2] = 1;
            break;
        case 'w':
        case 'W':
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
        case 'x':
        case 'X':
            /*
             camera[0] = radius;
             camera[1] = 0;
             camera[2] = 0;
             */
            break;
        case 'y':
        case 'Y':
            /*
             camera[0] = 1;
             camera[1] = radius;
             camera[2] = 1;
             */
            break;
        case 'z':
        case 'Z':
            /*
             camera[0] = 0;
             camera[1] = 0;
             camera[2] = radius;
             */
            camera[0] *= 1.3;
            camera[1] *= 1.3;
            camera[2] *= 1.3;
            radius *= 1.3;
            break;
    }
    
    // Leave these in only if you want shit to move in real time!
    glutSwapBuffers();
    glFlush();
    glutPostRedisplay();
}


/*
 *
 */
void GraphicsCallbackSpecial(int key, int mouse_x, int mouse_y)
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
            outputIter--;
            if (outputIter == sats.begin() - 1)
            {
                outputIter = sats.begin();
            }
                break;
        case GLUT_KEY_RIGHT:
            outputIter++;
            if (outputIter == sats.end())
            {
                outputIter = sats.end() - 1;
            }
                break;
        case GLUT_KEY_UP:
            camera[0] /= 1.3;
            camera[1] /= 1.3;
            camera[2] /= 1.3;
            radius /= 1.3;
            break;
        case GLUT_KEY_DOWN:
            camera[0] *= 1.3;
            camera[1] *= 1.3;
            camera[2] *= 1.3;
            radius *= 1.3;
            break;
        case GLUT_KEY_PAGE_UP:
            break;
        case GLUT_KEY_PAGE_DOWN:
            break;
        case GLUT_KEY_HOME:
            outputIter = sats.begin();
            break;
        case GLUT_KEY_END:
            outputIter = sats.end() - 1;
            break;
        case GLUT_KEY_INSERT:
            break;
    }
    
    // Leave these in only if you want shit to move in real time!
    glutSwapBuffers();
    glFlush();
    glutPostRedisplay();
}


/*
 *
 */
void GraphicsCallbackMouse(int button, int state, int x, int y)
{
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
    
    // Leave these in only if you want shit to move in real time!
    glutSwapBuffers();
    glFlush();
    glutPostRedisplay();  
}


/*
 *
 */
void GraphicsCallbackMotion(int x, int y)
{
    if (_mouseLeft)
    {
        ang += (x - _mouseX) / e_R * 40 * M_PI;
        // just rotate around y axis for now
        // camera[1] += (y - _mouseY) / e_R * 2;
        camera[0] = cos(ang) * radius;
        camera[2] = sin(ang) * radius;
        glLoadIdentity();
        gluLookAt(camera[0], camera[1], camera[2], 0, 0, 0, 0, 1, 0);
        // Leave these in only if you want shit to move in real time!
        glutSwapBuffers();
        glFlush();
        glutPostRedisplay();
        _mouseX = x;
        _mouseY = y;
    }
}


/*
 *
 */
void
GraphicsMainMenu(int value)
{
    /* Does not a damned thing. In here for appearances only. */
}

/*
 *
 */
void
GraphicsIncrementMenu(int value)
{
    switch(value)
    {
        case 0:
            break;
        case 1:
            interval = 0.03333333333 / 24.0;
            break;
        case 2:
            interval = 0.08333333333 / 24.0;
            break;
        case 3:
            interval = 0.16666666667 / 24.0;
            break;
        case 4:
            interval = 0.25 / 24.0;
            break;
        case 5:
            interval = 0.5 / 24.0;
            break;
        case 6:
            interval = 1.0 / 24.0;
            break;
    }
    
    glutSetWindow( mainWindow );
    glutPostRedisplay();
}
