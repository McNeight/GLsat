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

#include <iostream>
#include <string>
#include <GL/glut.h>
#include "tle.h"

using namespace std;

void init(vector<TLE> &satellites);
void reshape(int w, int h);
void reshapeText(int w, int h);
void display();
void key(unsigned char x, int mouse_x, int mouse_y);
void mouse(int button, int state, int x, int y);
void text();
void printString(string s);

#endif