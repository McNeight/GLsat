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

#ifndef PROJECTIONORTHOGRAPHIC_H
#define PROJECTIONORTHOGRAPHIC_H

#include "ProjectionBase.h"

class Location;

class ProjectionOrthographic : public ProjectionBase
{
 public:
    ProjectionOrthographic(const int w, const int h);
    ~ProjectionOrthographic();

    void setRadius(const double r);

    int pixelToSpherical(const int x, const int y, 
                         double &lon, double &lat);
    int sphericalToPixel(double lon, double lat, int &x, int &y) const;
    int sphericalToPixel(double lon, double lat, double r, 
                         int &x, int &y) const;

    void createProjection(const ProjectionRectangular *image_rect);
    void drawRings();

 private:
    double cosalph, cosbeta, cosgamm; // direction cosines of ray
 
    bool calcIntersect(const Location &observer, Location &intersection, 
                       const double a, const double b, const double c, 
                       int &ilon, int &ilat,
                       const ProjectionRectangular *image_rect);
    bool calcIntersectDEM(const Location &observer, Location &intersection, 
                          const double a, const double b, const double c, 
                          int &ilon, int &ilat, const double range_squared,
                          const ProjectionRectangular *image_rect);
};

#endif
