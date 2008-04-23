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

#include <cmath>
#include "constants.h"

void drawEllipse(float xradius, float yradius)
{
  glBegin(GL_LINE_LOOP);

  for (int i=0; i < 360; i++)
  {
    long double degInRad = i * deg2rad;
    glVertex2f(cos(degInRad)*xradius,sin(degInRad)*yradius);
  }

  glEnd();
}

void drawCircle(float radius)
{
  glBegin(GL_LINE_LOOP);

  for (int i=0; i < 360; i++)
  {
    long double degInRad = i*deg2rad;
    glVertex2f(cos(degInRad)*radius,sin(degInRad)*radius);
  }

  glEnd();
}


