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
#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;

#include "Location.h"
#include "Options.h"
#include "Planet.h"
#include "ProjectionRectangular.h"
#include "ProjectionOrthographic.h"
#include "Ring.h"
#include "satrings.h"
#include "util.h"
#include "xplanet.h"

#include "libannotate/libannotate.h"

#include "getColor.h"

ProjectionOrthographic::ProjectionOrthographic(const int w, const int h) 
    : ProjectionBase(w, h) // call the Projection constructor
{
    setBackground();

    if ((opts.position == DAYSIDE) || (opts.position == NIGHTSIDE))
        opts.fuzz = 0;

    // get radius in pixels
    radius = 0.01 * opts.radius * height;

    // scale the image so the rings fit
    if (opts.body == SATURN) 
        radius = radius * (saturn_radius / outer_radius);

    opts.radius = radius;
    radius = 1;

    buildPhotoTable();
}

ProjectionOrthographic::~ProjectionOrthographic() 
{
    destroyPhotoTable();
}

void
ProjectionOrthographic::setRadius(const double r)
{
    radius = r;
}

int 
ProjectionOrthographic::pixelToSpherical(const int x, const int y, 
                                         double &lon, double &lat)
{
    cerr << "ProjectionOrthographic::pixelToSpherical called\n";
    exit(0);
    return(0);    // keep some compilers happy
}

int
ProjectionOrthographic::sphericalToPixel(double lon, double lat,
                                         int &x, int &y) const
{
    Location l(lat, lon, radius);
    l.sphericalToRectangular();
    l.rotate(rot);
    l.rectangularToSpherical();

    if (l.x < 0) return(0);
    x = opts.centx + ((int) (l.y * opts.radius + 0.5));
    if (x < 0 || x >= width) return(0);

    y = opts.centy - ((int) (l.z * opts.radius + 0.5));
    if (y < 0 || y >= height) return(0);

    return(1);
}

int
ProjectionOrthographic::sphericalToPixel(double lon, double lat, double r,
                                         int &x, int &y) const
{
    Location l(lat, lon, r);
    l.sphericalToRectangular();
    l.rotate(rot);
    l.rectangularToSpherical();

    x = opts.centx + ((int) (l.y * opts.radius));
    if (x < 0 || x >= width) return(0);

    y = opts.centy - ((int) (l.z * opts.radius));
    if (y < 0 || y >= height) return(0);

    if (l.x < 0)
    {
        double dx = x - opts.centx;
        double dy = y - opts.centy;
        double dist = sqrt(dx*dx + dy*dy);
        if (dist < opts.radius) return(0);
    }
    return(1);
}

void
ProjectionOrthographic::createProjection(const ProjectionRectangular *image_rect)
{
    if (opts.radius == 0) return;
    double dist_per_pixel = 1. / opts.radius;
    dist_per_pixel /= cos(1/opts.range);

    double rad_per_pixel = dist_per_pixel / opts.range;

    // The rendered globe is contained in a square with sides of length
    // iside and upper left corner at (istart, jstart).
    int iside = (int) (2 * opts.radius) + 1;

    if (!opts.dem.empty()) iside = (int) (iside * (1 + opts.demscale));

    // (istart, jstart) is the upper left corner of the square 
    // containing the rendered globe.
    int istart = opts.centx - iside/2;
    int jstart = opts.centy - iside/2;

    double range_squared = opts.range * opts.range;

    double range_dpp_squared = (range_squared 
                                / (dist_per_pixel * dist_per_pixel));
 
    double a = 1.0;
    double c = range_squared - 1;

    Location intersection(opts.range);

    Location observer(opts.range);
    observer.sphericalToRectangular();

    int i1 = istart;
    int i2 = istart + iside;
    int j1 = jstart;
    int j2 = jstart + iside;

    if (i1 < 0) i1 = 0;
    if (i2 >= width) i2 = width;
    if (j1 < 0) j1 = 0;
    if (j2 >= height) j2 = height;

    for (int j = j1; j < j2; j++) 
    {
        int jdist = opts.centy - j;         // decreases downward
        int jdist_squared = jdist * jdist;
        cosgamm = jdist / sqrt(jdist_squared + range_dpp_squared);
        for (int i = i1; i < i2; i++) 
        {
            int idist = i - opts.centx;         // increases to the right
            int idist_squared = idist * idist;
            double dist = sqrt((double) (idist_squared + jdist_squared));
            cosalph = -cos(dist * rad_per_pixel);
            dist = sqrt(idist_squared + range_dpp_squared);
            cosbeta = idist / dist;
            double b = 2 * (cosalph * observer.x + cosbeta * observer.y 
                            + cosgamm * observer.z);
          
            int ilon, ilat;

            if (opts.dem.empty())
            {
                /* If the ray doesn't intersect the globe go to the 
                   next pixel */
                if (!calcIntersect(observer, intersection, a, b, c,
                                   ilon, ilat, image_rect)) 
                    continue;
            }
            else 
            {
                if (!calcIntersectDEM(observer, intersection, a, b, c,
                                      ilon, ilat, range_squared, image_rect))
                    continue;
            }
              
            unsigned char pixel[3] = {0,0,0};

            if (!opts.dem.empty())
            {
                memcpy(pixel, image_rect->rgb_data + 
                       3 * (ilat*image_rect->getWidth() + ilon), 3);
            }
            else
            {
                getColor(image_rect, intersection.lon, intersection.lat,
                         darkening, pixel);
            }

            memcpy(rgb_data + 3 * (j * width + i), pixel, 3);
            if (opts.transpng) png_alpha[j * width + i] = 255;
        }
    }

    // set rotation matrix for grid and markers
    rotateZYX(rot, -opts.rotate, -opts.getObsLat(), opts.getObsLon());
}

/* 
   Returns the indices of the lon and lat of the intersection between the 
   ray and the globe.
*/
bool
ProjectionOrthographic::calcIntersect(const Location &observer, 
                                      Location &intersection, 
                                      const double a, const double b, 
                                      const double c, int &ilon, int &ilat,
                                      const ProjectionRectangular *image_rect)
{
    double det = b*b - 4*a*c;
    
    if (det < 0) return(false);

    double sqrtdet = sqrt(det);
    double t1 = -b + sqrtdet;
    double t2 = -b - sqrtdet;
    double t = (t1 < t2 ? t1 : t2) / (2*a);
              
    intersection.setXYZ(observer.x + cosalph*t,
                        observer.y + cosbeta*t,
                        observer.z + cosgamm*t);

    // The cosine of the observer-center-surface point is the same as
    // the unrotated intersection.x
    darkening = getPhotoFunction(intersection.x);

    if (rotate) 
        intersection.rotate(rot);

    intersection.rectangularToSpherical();

    image_rect->sphericalToPixel(intersection.lon, intersection.lat,
                                 ilon, ilat);

    return(true);
}

/*
  Compute the lon and lat of the intersection at each of the larger
  and smaller radii.  Check the true elevation from the DEM at each
  lon, lat in between.  If the true elevation > radius at (lon, lat),
  color in the pixel.  
*/
bool
ProjectionOrthographic::calcIntersectDEM(const Location &observer, 
                                         Location &intersection, 
                                         const double a, const double b, 
                                         const double c, int &ilon, 
                                         int &ilat,
                                         const double range_squared,
                                         const ProjectionRectangular *image_rect)
{
    int start_lon;      // lon, lat at r = 1 + opts.demscale;
    int start_lat;
    int end_lon;        // lon, lat at r = 1
    int end_lat;

    double c1 = range_squared - 
        (1 + opts.demscale) * (1 + opts.demscale);

    /* If the ray doesn't intersect the larger radius go to 
       the next pixel */
    if (!calcIntersect(observer, intersection, a, b, c1, 
                       start_lon, start_lat, image_rect)) return(false);

    int icalc = calcIntersect(observer, intersection, a, b, c, 
                              end_lon, end_lat, image_rect);

    if (icalc == 0)
    {
        /* In this case we had a hit at the larger radius but
           a miss at the smaller.  */
        ilon = -1;
        ilat = -1;
    }
    else
    {
        ilon = end_lon;
        ilat = end_lat;
    }

    /* If the ray doesn't intersect the smaller radius, increase
       the radius until we get a hit */
    double maxr = 1 + opts.demscale;
    double scale = opts.demscale;
    double step = 0.01 * opts.demscale;
    while (icalc == 0)
    {
        scale -= step;
        c1 = range_squared - (maxr - scale) * (maxr - scale);
        icalc = calcIntersect(observer, intersection, a, 
                              b, c1, end_lon, end_lat, image_rect);
    } 

    scale /= opts.demscale;

    int idel_lon = end_lon - start_lon;
    if (abs(idel_lon) > image_rect->getWidth()/2) 
    {
        if (idel_lon > 0) 
            idel_lon -= image_rect->getWidth();
        else if (idel_lon < 0) 
            idel_lon += image_rect->getWidth();
    }
    int idel_lat = end_lat - start_lat;
    int length = (int) (sqrt( (double) (idel_lon * idel_lon 
                                        + idel_lat * idel_lat) ) 
                        + 0.5);
                  
    for (int il = 0; il < length; il++)
    {
        double frac = ((double) il) / length;
        int currlon = (int) (frac * idel_lon + start_lon + 0.5);
        if (currlon > image_rect->getWidth()) 
            currlon -= image_rect->getWidth();
        if (currlon < 0) currlon += image_rect->getWidth();
        int currlat = (int) (frac * idel_lat + start_lat + 0.5);
        double altitude = 255 * (1 - scale * frac);
        int ipos = currlat * image_rect->getWidth() + currlon;
        if (elevation[ipos] > altitude) 
        {
            ilat = currlat;
            ilon = currlon;
            break;
        }
    }
    // There was no intersection, so skip to next pixel 
    if (ilat == -1 || ilon == -1) return(false);

    return(true);
}

void
ProjectionOrthographic::drawRings()
{
    // We won't see the rings from the equatorial plane
    if (opts.getObsLat() == 0) return;

    rotateXYZ(rot, opts.rotate, opts.getObsLat(), -opts.getObsLon());

    // check if we're on the opposite side of the ring plane from the sun
    double sunlon = getSunLon();
    double sunlat = getSunLat();
    bool lit_side = (sunlat * opts.getObsLat() > 0);

    double dist_per_pixel = 1. / opts.radius;
    dist_per_pixel /= cos(1/opts.range);
    const double min_dist_per_pixel = dist_per_pixel;

    double rad_per_pixel = dist_per_pixel / opts.range;
    double range_squared = opts.range * opts.range;
    double range_dpp_squared = (range_squared 
                                / (dist_per_pixel * dist_per_pixel));
 
    Location intersection(opts.range);

    Location observer(opts.range);
    observer.sphericalToRectangular();

    Ring satring;
    dist_per_pixel /= fabs(sin(opts.getObsLat()));
    satring.setDistPerPixel(dist_per_pixel);

    double A, B, C, D;
    satring.getPlane(A, B, C, D);
    satring.buildShadowRadiusTable();

    unsigned char color[3];

    int iside = (int) (2 * opts.radius * (outer_radius / saturn_radius));

    int istart = opts.centx - iside/2;
    int jstart = opts.centy - iside/2;
    int iend = istart + iside;
    int jend = jstart + iside;

    if (istart < 0) istart = 0;
    if (iend >= width) iend = width;
    if (jstart < 0) jstart = 0;
    if (jend >= height) jend = height;

    for (int jj = jstart ; jj < jend; jj++)
    {
        int jdist = opts.centy - jj;         // decreases downward
        int jdist_squared = jdist * jdist;
        cosgamm = jdist / sqrt(jdist_squared + range_dpp_squared);
        for (int ii = istart; ii < iend; ii++)
        {
            int idist = ii - opts.centx;         // increases to the right
            int idist_squared = idist * idist;
            double dist = sqrt((double) (idist_squared + jdist_squared));
            cosalph = -cos(dist * rad_per_pixel);
            dist = sqrt(idist_squared + range_dpp_squared);
            cosbeta = idist / dist;
            
            double denom = (A * cosalph + B * cosbeta + C * cosgamm);
            if (denom == 0) continue;

            double u = -(A * observer.x + D) / denom;
            
            intersection.setXYZ(observer.x + cosalph*u,
                                observer.y + cosbeta*u,
                                observer.z + cosgamm*u);
            
            // Check if we're off the screen
            double X = opts.centx + intersection.y * opts.radius;
            if (X < 0 || X >= width) continue;
            
            double Y = opts.centy - intersection.z * opts.radius;
            if (Y < 0 || Y >= height) continue;
            
            bool behind_disk = false;
            double weight = -1;

            // Check if we're behind the planet
            if (intersection.x < 0)
            {
                const double dx = X - opts.centx;
                const double dy = Y - opts.centy;
                const double dist = sqrt(dx*dx + dy*dy);
                if (dist < opts.radius)
                {
                    const double edge_weight = 0.5;
                    const double center_weight = -100;
                    weight = center_weight + (dist/opts.radius) *
                        (edge_weight - center_weight);
                    if (weight <= 0) continue;
                    behind_disk = true;
                }
            }

            if (rotate) 
                intersection.rotate(rot);

            intersection.rectangularToSpherical();

            double dpp = dist_per_pixel * fabs(cos(opts.getObsLon()
                                                   - intersection.lon));
            if (dpp < min_dist_per_pixel) dpp = min_dist_per_pixel;
            satring.setDistPerPixel(dpp);

            double t = satring.getTransparency(intersection.range);
            if (t < 0) continue;

            double b;
            if (lit_side)
                b = satring.getBrightness(intersection.lon - sunlon, 
                                          intersection.range);
            else
                b = satring.getBrightness(intersection.lon - sunlon,
                                          intersection.range, t);

            if (b < 0) continue;
            
            for (int j = 0; j < 3; j++)
                color[j] = (unsigned char) (ring_color[j] * b);

            if (opts.transpng)
            {
                png_alpha[jj * width + ii] = (unsigned char) ((1-t) * 255);
                t = 0;
            }

            int ipos = 3 * (jj * width + ii);

            if (behind_disk)
            {
                for (int i = 0; i < 3; i++)
                    rgb_data[ipos++] += (unsigned char) (weight * color[i]);
            }
            else

            {
                for (int i = 0; i < 3; i++)
                    rgb_data[ipos] = (unsigned char) ((1 - t) * color[i]
                                                      + t * rgb_data[ipos++]);
            }

        }
    }

    rotateZYX(rot, -opts.rotate, -opts.getObsLat(), opts.getObsLon());
}
