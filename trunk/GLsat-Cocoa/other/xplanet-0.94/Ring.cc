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

#include <cstdlib>
#include <cmath>
using namespace std;

#include "Location.h"
#include "Options.h"
#include "Ring.h"
#include "satrings.h"
#include "util.h"

Ring::Ring()
{
    int i;

    r_out = outer_radius/saturn_radius;
    dr_b = (outer_radius - inner_radius) / (LIT * saturn_radius);
    dr_t = (outer_radius - inner_radius) / (TRANSP * saturn_radius);

    int extra = 50;
    num_b = LIT + extra;
    num_t = TRANSP + extra;

    // brightness and transparency arrays are from the outside in
    radius_b = new double[num_b];
    for (i = 0; i < num_b; i++) 
        radius_b[i] = r_out - i * dr_b;

    brightness = new double[num_b];
    for (i = 0; i < LIT; i++) 
        brightness[i] = ring_brightness[i];

    for (i = 0; i < extra; i++)
        brightness[LIT + i] = ring_brightness[LIT-1];

    radius_t = new double[num_t];
    for (i = 0; i < num_t; i++) 
        radius_t[i] = r_out - i * dr_t;

    transparency = new double[num_t];
    for (i = 0; i < TRANSP; i++) 
        transparency[i] = ring_transparency[i];

    // bring the transparency up to 1 at the inner radius
    for (i = 0; i < extra; i++)
    {
        double weight = 1 - ((double) i) / (extra - 1);
        transparency[TRANSP + i] = (1 - (1 - ring_transparency[TRANSP-1]) 
                                    * weight);
    }

    double sunlat = getSunLat();
    double sunlon = getSunLon();
    rotateZYX(sunrot, 0, -sunlat, sunlon);

    num_s = 90;
    shadow_cosangle = new double[num_s];
    shadow_radius = new double[num_s];

}

Ring::~Ring()
{
    delete [] radius_b;
    delete [] brightness;

    delete [] radius_t;
    delete [] transparency;

    delete [] shadow_cosangle;
    delete [] shadow_radius;
}

// Compute the distance of the edge of Saturn's shadow on the rings as
// a function of longitude.
void
Ring::buildShadowRadiusTable()
{
    double sunlon = getSunLon();

    int i = 0, start_index = 0;
    double d_angle = M_PI/num_s;
    for (double angle = M_PI; angle > 0; angle -= d_angle)
    {
        shadow_cosangle[i] = cos(angle);
        shadow_radius[i] = 0;
        for (int j = start_index; j < num_t; j++)
        {
            if (isInShadow(sunlon + angle, radius_t[j]))
            {
                shadow_radius[i] = radius_t[j];
                start_index = j;
                break;
            }
        }
        if (shadow_radius[i] == 0)
        {
            num_s = i;
            shadow_radius[i] = radius_t[num_t - 1];
            break;
        }

        i++;
    }
}

/*
  If the part of the ring at the specified longitude and radius
  isn't visible from the Sun, it's in shadow.
*/
bool
Ring::isInShadow(const double lon, double r)
{
    Location l(0, lon, r);
    l.sphericalToRectangular();
    l.rotate(sunrot);
    l.rectangularToSpherical();
    
    if (l.x < 0)
    {
        double dist = sqrt(l.y * l.y + l.z * l.z);
        if (dist < 1) return(true);
    }
    
    return(false);
}

/*
  Given a subsolar point and a location on the planet surface, check
  if the surface location is in shadow by the rings, and if so, return
  the ring radius.
*/
double
Ring::getShadowRadius(const Location &sunloc, const Location &point)
{
    // If this point is on the same side of the rings as the sun,
    // there's no shadow.
    if(sunloc.lat * point.lat > 0) return(-1);

    /*
      Calculate the distance from the point to the ring plane in the
      direction of the sun.  At the ring plane, z = 0.  Note that the
      XYZ coordinates of sunloc are equal to the direction cosines of
      the ray from Saturn's center to the Sun.
    */
    double dist = point.z / sunloc.z;

    double x2 = point.x - sunloc.x * dist;
    double y2 = point.y - sunloc.y * dist;

    return(sqrt(x2 * x2 + y2 * y2));
}

/*
  Given a cos(longitude), return the radius of the outermost point of the
  planet's shadow on the ring.
*/
double Ring::getShadowRadius(const double x) 
{
    for (int i = 0; i < num_s; i++)
        if (shadow_cosangle[i] > x) 
        {
            double frac = ((x - shadow_cosangle[i-1])
                           /(shadow_cosangle[i] - shadow_cosangle[i-1]));
            double returnval = (shadow_radius[i-1] 
                                + frac * (shadow_radius[i] 
                                          - shadow_radius[i-1]));
            return(returnval);
        }

    return(0);
}

double 
Ring::getBrightness(const double lon, const double r)
{
    return(getValue(brightness, num_b, window_b, dr_b, r, lon));
}

double 
Ring::getBrightness(const double lon, const double r, const double t)
{
    double returnval;
    if (t == 1.0) 
    {
        returnval = 0;
    }
    else 
    {
        returnval = getValue(transparency, num_t, window_t, dr_t, r, lon);
    }
    return(returnval);
}

double
Ring::getTransparency(const double r)
{
    return(getValue(transparency, num_t, window_t, dr_t, r));
}

double
Ring::getValue(const double *array, const int size, const int window,
               const double dr, const double r)
{
    int i = (int) ((r_out - r)/dr);

    if (i < 0 || i >= size) return(-1.0);

    int j1 = i - window;
    int j2 = i + window;
    if (j1 < 0) j1 = 0;
    if (j2 >= size) j2 = size - 1;

    double sum = 0;
    for (int j = j1; j < j2; j++) sum += array[j];
    sum /= (j2 - j1);

    return(sum);
}

double
Ring::getValue(const double *array, const int size, const int window,
               const double dr, const double r, const double lon)
{
    double cos_lon = cos(lon);
    if (cos_lon > -0.55) return(getValue(array, size, window, dr, r));
    
    int i = (int) ((r_out - r)/dr);

    if (i < 0 || i >= size) return(-1.0);

    int j1 = i - window;
    int j2 = i + window;
    if (j1 < 0) j1 = 0;
    if (j2 >= size) j2 = size - 1;

    double shadow_rad = getShadowRadius(cos_lon);

    double r0 = r;
    double sum = 0;
    for (int j = j1; j < j2; j++) 
    {
        if (r0 < shadow_rad)
            sum += (opts.shade * array[j]);
        else
            sum += array[j];
        r0 += dr;
    }
    sum /= (j2 - j1);

    return(sum);
}

/*
  This routine gets the coefficients for the equation of the
  equatorial plane, which is in the form
  
  Ax + By + Cz + D = 0

  The equation of a plane containing the 3 points P1, P2, P3 is

  | x  - x1   y  - y1   z  - z1 |
  | x2 - x1   y2 - y1   z2 - z1 | = 0
  | x3 - x1   y3 - y1   z3 - z1 |

  Solving for A, B, C, and D, we get

  A = [(y2 - y1)(z3 - z1) - (y3 - y1)(z2 - z1)]
  B = [(x3 - x1)(z2 - z1) - (x2 - x1)(z3 - z1)]
  C = [(x2 - x1)(y3 - y1) - (x3 - x1)(y2 - y1)]
  D = -[x1 * A + y1 * B + z1 * C] 
*/
void 
Ring::getPlane(double &A, double &B, double &C, double &D)
{
    Location p1(0);
    Location p2(0);
    Location p3(0);

    p1.setXYZ( 1.,-1., 0.);
    p2.setXYZ( 1., 1., 0.); 
    p3.setXYZ(-1., 1.,-0.); 

    if (opts.getObsLon() != 0 || opts.getObsLat() != 0 || opts.rotate) 
    {
        p1.rectangularToSpherical();
        p2.rectangularToSpherical();
        p3.rectangularToSpherical();

        double rot_matrix[3][3];
        rotateZYX(rot_matrix, -opts.rotate, -opts.getObsLat(), opts.getObsLon());

        p1.rotate(rot_matrix);
        p2.rotate(rot_matrix);
        p3.rotate(rot_matrix);
    }

    A = ((p2.y - p1.y) * (p3.z - p1.z) - (p3.y - p1.y) * (p2.z - p1.z));
    B = ((p3.x - p1.x) * (p2.z - p1.z) - (p2.x - p1.x) * (p3.z - p1.z));
    C = ((p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y));
    D = -(p1.x * A + p1.y * B + p1.z * C);
}

void
Ring::setDistPerPixel(const double d)
{
    dist_per_pixel = d;
    window_b = (int) (dist_per_pixel / dr_b + 0.5);
    window_t = (int) (dist_per_pixel / dr_t + 0.5);

    window_b = window_b/2 + 1;
    window_t = window_t/2 + 1;
}

