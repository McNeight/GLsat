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
#include <cstring>
#include <iostream>
using namespace std;

#include "Location.h"
#include "util.h"

Location::Location(const double r) 
{
    lon = 0.;
    lat = 0.;
    range = r;
}

Location::Location(const double latitude, const double longitude, 
                   const double r)
{
    lat = latitude;
    lon = drem(longitude, TWO_PI);
    range = r;
}

void
Location::setXYZ(const double a, const double b, const double c)
{
    x = a; 
    y = b;
    z = c;

    range = sqrt(x*x + y*y + z*z);
}

double
Location::dot(const Location &l1)
{
    return(x * l1.x + y * l1.y + z * l1.z);
}

void 
Location::rotate(const double a[3][3]) 
{
    double newx = a[0][0] * x + a[0][1] * y + a[0][2] * z;
    double newy = a[1][0] * x + a[1][1] * y + a[1][2] * z;
    double newz = a[2][0] * x + a[2][1] * y + a[2][2] * z;

    x = newx;
    y = newy;
    z = newz;
}

void 
Location::sphericalToRectangular() 
{
    double cos_lat = cos(lat);
    x = range * cos(lon) * cos_lat;
    y = range * sin(lon) * cos_lat;
    z = range * sin(lat);
}

void 
Location::rectangularToSpherical() 
{
    if (range == 0)
    {
        lat = 0;
        lon = 0;
        return;
    }

    lat = M_PI_2 - acos(z/range);
    lon = atan2(y,x);
}

void
Location::print() const
{
    cout << "x = " << x << "\ty = " << y << "\tz = " << z << endl
         << "lon = " << lon/deg_to_rad << "\tlat = " << lat/deg_to_rad 
         << "\trange = " << range << endl;
}
