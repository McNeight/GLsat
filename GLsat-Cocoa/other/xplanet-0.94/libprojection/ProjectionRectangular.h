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

#ifndef PROJECTION_RECTANGULAR_H
#define PROJECTION_RECTANGULAR_H

#include "Location.h"
#include "ProjectionBase.h"

class ProjectionRectangular : public ProjectionBase
{
 public:
    ProjectionRectangular(const int w, const int h);
    ProjectionRectangular(const int w, const int h, const bool b);
    ~ProjectionRectangular();
    void createProjection(const double sunlon, const double sunlat, 
                          unsigned char *day, unsigned char *night);
    int pixelToSpherical(const int x, const int y, 
                         double &lon, double &lat);
    int sphericalToPixel(double lon, double lat, int &x, int &y) const;
    double getDeltaLon() const;
    double getDeltaLat() const;
    double xToLon(const int x) const;
    double yToLat(const int y) const;

 private:
    double del_lon;
    double del_lat;
    double *lon_array;
    double *lat_array;

    double start_lon;
    double start_lat;
    

    void setUpMap();
    void copyBlock(unsigned char *dest, unsigned char *src, 
                   const int x1, const int y1,
                   int x2, int y2);
    
};

#endif
