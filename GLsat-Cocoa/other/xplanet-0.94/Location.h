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

#ifndef LOCATION_H
#define LOCATION_H

class Location 
{
 public:
    double x,    // x axis points through (0,0)
        y,       // y axis points through (90,0)
        z;       // z axis points through north pole
    double lon,
        lat,
        range;
  
    Location(const double r);
    Location(const double latitude, const double longitude, 
             const double r);

    void setXYZ(const double a, const double b, const double c);

    double dot(const Location &l1);

    void print() const;

    void rotate(const double a[3][3]);

    void sphericalToRectangular();
    void rectangularToSpherical();
};

#endif
