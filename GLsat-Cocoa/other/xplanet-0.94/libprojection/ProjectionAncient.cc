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
#include "ProjectionAncient.h"
#include "util.h"

ProjectionAncient::ProjectionAncient(const int w, const int h) 
    : ProjectionBase(w, h) // call the Projection constructor
{
    setBackground();

    radius = 0.01 * opts.radius;

    if (width/2 < height)
        dispscale = width/2;
    else
        dispscale = height;
}

int
ProjectionAncient::pixelToSpherical(const int x, const int y,
                                    double &lon, double &lat)
{
    double X = (x - opts.centx) / dispscale;
    double Y = - (y - opts.centy) / dispscale;
    double rho,c;

    if (X<0) {
        X += radius;
        rho = sqrt(X*X + Y*Y);
        if (rho > radius) return(0);

        c = M_PI/2 * rho / radius;

        if (rho == 0) 
        {
            lat = 0;
            lon = 0;
        }
        else 
        {       
            double arg = Y * sin(c) / rho;
            lat = asin(arg);
            lon = atan2(X * sin(c), rho * cos(c)) - M_PI/2;
        }
    }
    else 
    {
        X -= radius;
        rho = sqrt(X*X + Y*Y);
        if (rho > radius) return(0);

        c = M_PI/2 * rho / radius;

        if (rho == 0) 
        {
            lat = 0;
            lon = 0;
        }
        else 
        {       
            double arg = Y * sin(c) / rho;
            lat = asin(arg);
            lon = atan2(X * sin(c), rho * cos(c)) + M_PI/2;
        }
    }

    // This was Richard's original code
    //  if (opts.position != FIXED) lon -= M_PI/2;

    lon -= M_PI/2; 

    if (rotate) rotatePoint(lon, lat);

    if (lon > M_PI) 
        lon -= TWO_PI;
    else if (lon < -M_PI) 
        lon += TWO_PI;

    return(1);
}

int
ProjectionAncient::sphericalToPixel(double lon, double lat, int &x, int &y) const
{
    if (rotate) rotatePoint(lon, lat);

    // This was Richard's original code
    //  if (opts.position != FIXED) lon += M_PI/2;

    lon += M_PI/2;

    if (lon > M_PI) lon -= TWO_PI;

    double c,k,X,Y;

    if (lon < 0) 
    {
        lon += M_PI/2;
        c = acos(cos(lat) * cos(lon));
        k = 2*radius * c / (M_PI * sin(c));
        X = -radius + k * cos(lat) * sin(lon);
        Y = k * sin(lat);
    }
    else 
    {
        lon -= M_PI/2;
        c = acos(cos(lat) * cos(lon));
        k = 2*radius * c / (M_PI * sin(c));
        X = radius + k * cos(lat) * sin(lon);
        Y = k * sin(lat);
    }

    x = opts.centx + (int) (dispscale * X);
    if (x < 0 || x >= width) return(0);

    y = opts.centy - (int) (dispscale * Y);
    if (y < 0 || y >= height) return(0);

    return(1);
}
