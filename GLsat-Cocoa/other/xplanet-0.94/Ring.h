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

#ifndef RING_H
#define RING_H

class Location;

class Ring
{
 public:
    Ring();
    ~Ring();

    // Compute the distance of the edge of Saturn's shadow on the
    // rings as a function of longitude.
    void buildShadowRadiusTable();

    // get the radius of the ring shadowing the specified location on
    // the planet
    double getShadowRadius(const Location &sunloc, const Location &point);

    // get the equation of the equatorial plane
    void getPlane(double &A, double &B, double &C, double &D);

    // check if the specified part of the ring is in shadow
    bool isInShadow(const double lon, const double r);

    // get the brightness on the lit side
    double getBrightness(const double lon, const double r);

    // get the brightness on the dark side
    double getBrightness(const double lon, const double r, const double t);

    double getTransparency(const double r);

    // set the size of each pixel, used to window average the ring
    // brightness/transparency
    void setDistPerPixel(const double d);

 private:
    double r_out;  // outer ring radius, units of saturn radii
    double dr_b;   // resolution of brightness grid
    double dr_t;   // resolution of transparency grid

    double dist_per_pixel; /* size of each pixel at the ring plane,
                              units of saturn radii */

    int num_t;
    double *radius_t;
    double *transparency;
    int window_t;  // each pixel contains this many transparency points

    int num_b;
    double *radius_b;
    double *brightness;
    int window_b;  // each pixel contains this many brightness points

    int num_s;
    double *shadow_cosangle;
    double *shadow_radius;

    double sunrot[3][3]; /* rotation matrix to shift the viewpoint
                            to the sun */

    // get the outer radius of the shadow of the planet on the rings
    double getShadowRadius(const double x);

    // get a window average of array
    double getValue(const double *array, const int size, const int window,
                    const double dr, const double r);

    // get a window average of array, accounts for shadowing by the planet
    double getValue(const double *array, const int size, const int window,
                    const double dr, const double r, const double lon);
};

extern Ring satring;

#endif
