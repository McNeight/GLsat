/****************************************************************************
    Xplanet 0.94 - render an image of a planet into an X window
    Copyright (C) 2002 Hari Nair <hari@alumni.caltech.edu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#include <cmath>
using namespace std;

#include "Options.h"
#include "ProjectionMercator.h"
#include "util.h"

ProjectionMercator::ProjectionMercator(const int w, const int h) 
    : ProjectionBase(w, h)  // call the Projection constructor
{
}

int
ProjectionMercator::pixelToSpherical(const int x, const int y, 
                                     double &lon, double &lat)
{
    double X = ((double) (x - width/2)) * TWO_PI / width;
    double Y = ((double) (height/2 - y)) * 1.5 * M_PI / height;

    lat = atan(sinh(Y));
    lon = X;
  
    if (rotate) rotatePoint(lon, lat);

    if (lon > M_PI) lon -= TWO_PI;
    else if (lon < -M_PI) lon += TWO_PI;

    return(1);
}

int
ProjectionMercator::sphericalToPixel(double lon, double lat, int &x, int &y) const
{
    if (rotate) rotatePoint(lon, lat);
  
    double X = lon;
    double Y = log (tan(M_PI/4 + lat/2));

    if (X >= M_PI) X -= TWO_PI;
    else if (X < -M_PI) X += TWO_PI;

    x = (int) ((width * X / TWO_PI) + width / 2);
    y = (int) (height / 2 - (height * Y / (1.5 * M_PI)));
  
    if (y < 0 || y >= height) return(0);
    return (1);
}
