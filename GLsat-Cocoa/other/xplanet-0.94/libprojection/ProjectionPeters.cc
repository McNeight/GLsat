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

// This projection contributed by Martin Pool <mbp@linuxcare.com> 

#include <cmath>
using namespace std;

#include "Options.h"
#include "ProjectionPeters.h"
#include "util.h"

/*
 * This class implements the Peters projection, which is an
 * area-preserving variant of the Mercator projection.  I'm not 100%
 * sure this is the authentic Peters algorithm, but it seems to have
 * the right properties.
 *
 * Lines of latitude run straight across the map, and area is more or
 * less preserved.  So, lines of latitude squish together towards the
 * poles, and spread out around the Equator.
 *
 * Longitude maps directly to X (with a scaling factor).
 *
 * Y is proportional to the sin of latitude, adjusted so that 0 is in
 * the center and +1/-1 at the edges.
 *
 * This projection always preserves the aspect ratio, since it already
 * tends to distort the vertical scale.  So if it's not displayed in a
 * 2:1 window there will be black space.
 *
 * This projection cannot rotate vertically because I can't work out
 * what it should look like in that case.  Sorry.
 */

ProjectionPeters::ProjectionPeters(const int w, const int h) 
    : ProjectionBase(w, h)  // call the Projection constructor
{
    wd = (int) ((width * opts.radius) / 50);
    ht = wd/2;

    setBackground();
}

int
ProjectionPeters::pixelToSpherical(const int x, const int y,
                                   double &lon, double &lat)
{
    double X = ((double) (x - width/2)) * TWO_PI / wd;
    double Y = ((double) (height - 2*y)) / ht;

    if (Y < -1 || Y > +1)
        return(0);

    lat = asin(Y);
    lon = X;
  
    if (rotate) rotatePoint(lon, lat);
  
    if (lon > M_PI) lon -= TWO_PI;
    else if (lon < -M_PI) lon += TWO_PI;

    return(1);
}

int
ProjectionPeters::sphericalToPixel(double lon, double lat, int &x, int &y) const
{
    if (rotate) rotatePoint(lon, lat);
  
    double X = lon;
    double Y = sin(lat);

    if (X >= M_PI) X -= TWO_PI;
    else if (X < -M_PI) X += TWO_PI;

    x = (int) ((wd * X / TWO_PI) + width / 2);
    y = (int) (height / 2 - (Y * ht/2));

    if (x < 0 || x >= width) return(0);
    if (y < 0 || y >= height) return(0);
    return(1);
}
