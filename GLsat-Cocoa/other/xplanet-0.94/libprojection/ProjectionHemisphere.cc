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

//   This projection was contributed by Richard Rognlie <rrognlie@gamerz.net>

#include <cmath>
using namespace std;

#include "Options.h"
#include "ProjectionHemisphere.h"
#include "util.h"

ProjectionHemisphere::ProjectionHemisphere(const int w, const int h) 
    : ProjectionBase(w, h) // call the Projection constructor
{
    setBackground();

    radius = 0.01 * opts.radius;

    if (width/2 < height)
        dispscale = width/2;
    else
        dispscale = height;

    buildPhotoTable();
}

ProjectionHemisphere::~ProjectionHemisphere() 
{
    destroyPhotoTable();
}

int
ProjectionHemisphere::pixelToSpherical(const int x, const int y, 
                                       double &lon, double &lat)
{
    double X = (x - opts.centx) / dispscale;
    double Y = - (y - opts.centy) / dispscale;

    double arg = Y / radius;
    if (fabs (arg) > 1) return(0);
    lat = asin (arg);

    if (X<0) 
    {
        X += radius;
        arg = -X / (radius * cos(lat));
        if (fabs(arg) > 1) return(0);
        lon = acos(arg) - M_PI;
    }
    else 
    {
        X -= radius;
        arg = -X / (radius * cos(lat));
        if (fabs(arg) > 1) return(0);
        lon = acos(arg);
    }

    // This was Richard's original code
    //  if (opts.position != FIXED) lon -= M_PI/2;

    lon -= M_PI/2; 

    darkening = getPhotoFunction(fabs(cos(lon) * cos(lat)));

    if (rotate) rotatePoint(lon, lat);

    if (lon > M_PI) lon -= TWO_PI;
    else if (lon < -M_PI) lon += TWO_PI;

    return(1);
}

int
ProjectionHemisphere::sphericalToPixel(double lon, double lat, 
                                       int &x, int &y) const
{
    if (rotate) rotatePoint(lon, lat);

    // This was Richard's original code
    //  if (opts.position != FIXED) lon += M_PI/2;

    lon += M_PI/2;

    if (lon > M_PI) lon -= TWO_PI;

    double Y = radius * sin(lat);
    double X;

    if (lon < 0) 
    {
        X = -radius + radius * cos(lat) * sin(lon-3*M_PI/2);
    }
    else 
    {
        X =  radius + radius * cos(lat) * sin(lon-M_PI/2);
    }

    x = opts.centx + (int) (dispscale * X);
    if (x < 0 || x >= width) return(0);

    y = opts.centy - (int) (dispscale * Y);
    if (y < 0 || y >= height) return(0);

    return(1);
}
