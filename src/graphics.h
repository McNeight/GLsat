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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include "tle.h"

using namespace std;

void GLgraphics(int argc, char *argv[]);
void GraphicsInit(void);
void GraphicsDisplay(void);
void reshapeMain(int w, int h);
void reshapeText(int w, int h);
void GraphicsText(void);
void keyboard(unsigned char key, int mouse_x, int mouse_y);
void special(int key, int mouse_x, int mouse_y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

// helper functions of my own
void printString(string s);
void BlueMarble(void);


// helper functions hacked in from gltZPR
// http://www.nigels.com/glt/gltzpr/
double vlen(double x,double y,double z);
void viewPos(double *px, double *py, double *pz, const int x, const int y, const int *viewport);
void   getMatrix();
void   invertMatrix(const GLdouble *m, GLdouble *out );


#endif
