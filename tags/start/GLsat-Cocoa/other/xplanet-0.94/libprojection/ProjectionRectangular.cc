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
#include "ProjectionRectangular.h"
#include "Ring.h"
#include "util.h"

ProjectionRectangular::ProjectionRectangular(const int w, const int h) 
    : ProjectionBase (w, h)  // call the Projection constructor
{
    setUpMap();
}

ProjectionRectangular::ProjectionRectangular(const int w, const int h, const bool b) 
{
    width = w; 
    height = h;
    rgb_data = NULL;

    setUpMap();
}

ProjectionRectangular::~ProjectionRectangular()
{
    delete [] lat_array;
    delete [] lon_array;
}

void
ProjectionRectangular::setUpMap()
{
    double map_width;
    double map_height;

    if (opts.mapbounds)
    {
        map_width = (opts.map_lrx - opts.map_ulx) * deg_to_rad;
        map_height = (opts.map_uly - opts.map_lry) * deg_to_rad;
        start_lon = opts.map_ulx * deg_to_rad;
        start_lat = opts.map_uly * deg_to_rad;
    }
    else
    {
        map_width = TWO_PI;
        map_height = M_PI;
        start_lon = -M_PI;
        start_lat = M_PI_2;
    }

    del_lon = map_width/width;
    del_lat = map_height/height;

    int i;

    lon_array = new double[width];
    for (i = 0; i < width; i++) 
        lon_array[i] = (i + 0.5) * del_lon + start_lon;

    lat_array = new double[height];
    for (i = 0; i < height; i++)
        lat_array[i] = start_lat - (i + 0.5) * del_lat;
}

double 
ProjectionRectangular::getDeltaLon() const
{
    return(del_lon);
}

double 
ProjectionRectangular::getDeltaLat() const
{
    return(del_lat);
}

/*
  Given lon, lat, returns x, y such that:
  lon_array[x-1] < lon < lon_array[x]
  lat_array[y-1] > lat > lat_array[y]

  where 0 <= x < width
  0 <= y < height
*/
int
ProjectionRectangular::sphericalToPixel(double lon, double lat,
                                        int &x, int &y) const
{
    if (lon > M_PI) lon -= TWO_PI;
    else if (lon < -M_PI) lon += TWO_PI;
          
    x = (int) ((lon - start_lon)/del_lon + 0.5);

    if (opts.mapbounds)
    {
        if (x < 0 || x >= width) return(0);
    }
    else
    {
        if (x >= width) x -= width;
        else if (x < 0) x += width;
    }

    if (opts.display == ANIMATE) x = width - x;

    y = (int) ((start_lat - lat)/del_lat + 0.5);

    if (opts.mapbounds && (y < 0 || y >= height)) return(0);

    if (y >= height) y = height - 1;
    
    return(1);
}

int
ProjectionRectangular::pixelToSpherical(const int x, const int y, 
                                        double &lon, double &lat)
{
    lon = xToLon(x);
    lat = yToLat(y);
    return(1);
}

double
ProjectionRectangular::xToLon(const int x) const
{
    return(lon_array[x]);
}

double
ProjectionRectangular::yToLat(const int y) const
{
    return(lat_array[y]);
}

void
ProjectionRectangular::createProjection(const double sunlon, 
                                        const double sunlat, 
                                        unsigned char *day, 
                                        unsigned char *night)
{
    int i, j, ii, jj;

    if (opts.fuzz == 0) 
    {
        // number of rows at top and bottom that are in polar day/night
        int ipolar = abs((int) (sunlat/del_lat));

        if (sunlat < 0) // North pole is dark
            copyBlock(day, night, 0, 0, width, ipolar);
        else            // South pole is dark
            copyBlock(day, night, 0, height - ipolar, width, height);

        // subsolar longitude pixel column - this is where it's noon
        int inoon = int (width/2 * (sunlon / M_PI - 1)); 
        while (inoon < 0) inoon += width;
        while (inoon >= width) inoon -= width;

        for (i = ipolar; i < height - ipolar; i++) 
        { 
            double length_of_day, H0;

            /* compute length of daylight as a fraction of the day at 
               the current latitude.  Based on Chapter 42 of Astronomical 
               Formulae for Calculators by Meeus. */

            H0 = tan(lat_array[i]) * tan(sunlat);
            if (H0 > 1) length_of_day = 1;
            else if (H0 < -1) length_of_day = 0;
            else length_of_day = 1 - (acos(H0) / M_PI);

            // ilight = number of pixels from noon to the terminator
            int ilight = (int) (width/2 * length_of_day + 0.5);  

            /* idark = number of pixels that are in darkness at the current 
               latitude */
            int idark = width - 2 * ilight;

            // start at the evening terminator
            int start_row = i * width;
            int ipos = inoon + ilight;

            for (j = 0; j < idark; j++) 
            {
                if (ipos >= width) ipos -= width;
                memcpy(day + 3 * (start_row + ipos), 
                       night + 3 * (start_row + ipos), 3);
                ipos++;
            }
        } 
    }
    else  // opts.fuzz > 0
    {
        unsigned char *tempRGB = new unsigned char[3 * width * height];
        memcpy(tempRGB, day, 3 * width * height);

        Location sunloc(sunlat, sunlon, 1);
        sunloc.sphericalToRectangular();

        double border = sin(opts.fuzz * deg_to_rad);

        // break the image up into a 100x100 grid
        int istep = width/100;
        int jstep = height/100;
        if (istep == 0) istep = 1; 
        if (jstep == 0) jstep = 1;

        for (i = 0; i < width; i += istep)
        {
            int istep2 = istep;    // number of pixels to the end of the line
            if (i + istep > width) istep2 = width - i;
            for (j = 0; j < height; j += jstep)
            {
                Location point(lat_array[j], lon_array[i], 1);
                point.sphericalToRectangular();

                double x = point.dot(sunloc);
                if (x < -2*border)  // NIGHT
                {
                    copyBlock(tempRGB, night, i, j, i + istep, j + jstep);
                }
                else if (x < 2*border ) // TWILIGHT
                {
                    for (jj = 0; jj < jstep; jj++)
                    {
                        int ilat = j + jj;
                        if (ilat >= height) break;
                        for (ii = 0; ii < istep; ii++)
                        {
                            int ilon = i + ii;
                            if (ilon >= width) ilon -= width;
                            Location tmp(lat_array[ilat], 
                                         lon_array[ilon], 1);
                            tmp.sphericalToRectangular();
                            double dayweight = ((border + tmp.dot(sunloc))
                                                / (2 * border));
                            int ipos = 3 * (ilat * width + ilon);
                            if (dayweight < 0)
                            {
                                memcpy(tempRGB + ipos, night + ipos, 3);
                            }
                            else if (dayweight < 1) 
                            {
                                dayweight = (1 - cos(dayweight * M_PI)) / 2;
                                for (int k = 0; k < 3; k++)
                                {
                                    float color = (dayweight * day[ipos] 
                                                   + ((1 - dayweight) 
                                                      * night[ipos]));
                                    tempRGB[ipos++] = (unsigned char) color;
                                }
                            }
                        } // for ( ii = ... ) block
                    }     // for ( jj = ... ) block
                }         // end TWILIGHT block

            }             // for ( j = ... ) block
        }                 // for ( i = ... ) block

        if (opts.body == SATURN) 
        {
            Ring satring;
            for (j = 0; j < height; j++)
            {
                // If this point is on the same side of the rings as
                // the sun, there's no shadow.
                if (sunloc.lat * lat_array[j] > 0) continue;

                satring.setDistPerPixel(del_lat 
                                        / fabs(sin(lat_array[j])));
                for (i = 0; i < width; i++)
                {
                    Location point(lat_array[j], lon_array[i], 1);
                    point.sphericalToRectangular();

                    // If it's night, skip this one
                    if (point.dot(sunloc) < -2 * border) continue;

                    double ring_radius = satring.getShadowRadius(sunloc,
                                                                 point);
                    double t = satring.getTransparency(ring_radius);
                    if (t > 0)
                    {
                        int ipos = 3 * (j * width + i);
                        for (int k = 0; k < 3; k++)
                        {
                            tempRGB[ipos] = (unsigned char) 
                                (t * tempRGB[ipos]
                                 + ((1 - t) * night[ipos]));
                            ipos++;
                        }
                    }
                }
            }
        }

        memcpy(day, tempRGB, 3 * width * height);
        delete [] tempRGB;
    }                   // end (opts.fuzz > 0) block

    if (opts.display == ANIMATE) 
    {
        int ipos = 0;
        for (j = 0; j < height; j++)
        {
            int istart = 3 * j * width;
            for (i = 3 * (width - 1); i >= 0; i -= 3)
            {
                memcpy (rgb_data + istart + i, day + ipos, 3);
                ipos += 3;
            }
        }
    }
    else
    {
        memcpy(rgb_data, day, (3 * width * height));
    }
}

void
ProjectionRectangular::copyBlock(unsigned char *dest, unsigned char *src,
                                 const int x1, const int y1, int x2, int y2)
{
    if (x2 >= width) x2 = width;
    if (y2 >= height) y2 = height;

    for (int j = y1; j < y2; j++)
    {
        memcpy(dest + 3 * (width * j + x1), src + 3 * (width * j + x1), 
               3 * (x2 - x1));

    }
}
