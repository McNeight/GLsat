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

#ifndef PROJECTIONBASE_H
#define PROJECTIONBASE_H

#include <string>
using namespace std;

class ProjectionRectangular;

class ProjectionBase
{
 public:
    unsigned char *rgb_data;
    unsigned char *png_alpha;   // alpha channel (opacity) for PNG output

    ProjectionBase();
    ProjectionBase(const int w, const int h);
    virtual ~ProjectionBase();

    int getWidth() const;
    int getHeight() const;

    virtual void drawRings();
    virtual void setRadius(const double r);

    virtual int pixelToSpherical(const int x, const int y, 
                                 double &lon, double &lat) = 0;
    virtual int sphericalToPixel(double lon, double lat,
                                 int &x, int &y) const = 0;
    virtual int sphericalToPixel(double lon, double lat, double r, 
                                 int &x, int &y) const;

    virtual void createProjection(const ProjectionRectangular *image_rect);
    void drawGreatArc(const double lon1, const double lat1, const double r1,
                      const double lon2, const double lat2, const double r2,
                      const float spacing, const unsigned char *color);
    void drawGreatArc(const double lon1, const double lat1, 
                      const double lon2, const double lat2, 
                      const float spacing, const unsigned char *color);
    void drawGrid();

 protected:
    int width, height;
    double radius;
    double rot[3][3];
    bool rotate;

    void rotatePoint(double &lon, double &lat) const;
    void setBackground();

    // for the photometric function
    int table_size;
    double darkening;
    double *cosangle;
    double *photo_function;

    void buildPhotoTable();
    void destroyPhotoTable();
    double getPhotoFunction(const double x) const;

 private:
    void placeDot(const double lon, const double lat);
    void placeDot(const double lon, const double lat, 
                  const unsigned char *color);
    void placeDot(const double lon, const double lat, const double r,
                  const unsigned char *color);
};

#endif
