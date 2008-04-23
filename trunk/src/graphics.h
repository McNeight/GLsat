/*
 *  GLsat: An OpenGL-based 3D satellite tracker
 *  Copyright (C) 2002-2008  Neil McNeight <mcneight@users.sf.net>
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

#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "tle.h"

using namespace std;

void GLgraphics               (int argc, char *argv[]);
void GraphicsInit             (void);
void GraphicsDisplay          (void);
void GraphicsReshapeMain      (int w, int h);
void GraphicsReshapeText      (int w, int h);
void GraphicsText             (void);

// Callback routines
void GraphicsCallbackKeyboard (unsigned char key, int mouse_x, int mouse_y);
void GraphicsCallbackSpecial  (int key, int mouse_x, int mouse_y);
void GraphicsCallbackMouse    (int button, int state, int x, int y);
void GraphicsCallbackMotion   (int x, int y);
void GraphicsMainMenu					(int value);
void GraphicsIncrementMenu    (int value);
  
// helper functions of my own
void printString							(string s);
void GraphicsSetupTextureMaps (void);
void GraphicsBlueMarble				(void);
void GraphicsDrawSat					(TLE currentSat, long double &timeJD);
void GraphicsDrawNOAA17				(void);

#endif
