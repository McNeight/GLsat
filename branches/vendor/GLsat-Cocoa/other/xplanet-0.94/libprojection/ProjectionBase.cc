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
#include "ProjectionBase.h"
#include "util.h"

#include "libannotate/libannotate.h"

#include "getColor.h"

ProjectionBase::ProjectionBase()
{
}

ProjectionBase::ProjectionBase(const int w, const int h)
{
    width = w; 
    height = h;

    rgb_data = new unsigned char[3 * width * height];
    memset(rgb_data, 0, 3 * width * height);

    png_alpha = NULL;
    if (opts.transpng) 
    {
        png_alpha = new unsigned char[width * height];
        memset(png_alpha, 0, width * height);
    }
    
    rotate = (opts.getObsLon() != 0 
              || opts.getObsLat() != 0 
              || opts.rotate != 0);

    rotateXYZ(rot, opts.rotate, opts.getObsLat(), -opts.getObsLon());

    // limb darkening, gets overridden in ORTHOGRAPHIC mode
    darkening = 1.0;
}

ProjectionBase::~ProjectionBase()
{
    delete [] rgb_data;
    if (opts.transpng) delete [] png_alpha;
}

int 
ProjectionBase::getWidth() const
{
    return(width);
}

int
ProjectionBase::getHeight() const
{
    return(height);
}

void
ProjectionBase::drawRings()
{
}

void
ProjectionBase::setRadius(const double r)
{
    cerr << "ProjectionBase::setRadius() called.\n";
    exit(EXIT_FAILURE);
}

int 
ProjectionBase::sphericalToPixel(double lon, double lat, double r, 
                                 int &x, int &y) const
{
    cerr << "ProjectionBase::sphericalToPixel(lon,lat,r) called.\n";
    exit(EXIT_FAILURE);
    return(0);    // keep some compilers happy
}

void
ProjectionBase::createProjection(const ProjectionRectangular *image_rect)
{
    double lon, lat;
    for (int j = 0; j < height ; j++)
    {
        for (int i = 0; i < width; i++)
        {
            if (pixelToSpherical(i, j, lon, lat))
            {
                getColor(image_rect, lon, lat, darkening, 
                         rgb_data + 3 * (j * width + i));
                if (opts.transpng) png_alpha[j * width + i] = 255;
            }
        }
    }

    // set rotation matrix for grid and markers
    rotateZYX(rot, -opts.rotate, -opts.getObsLat(), opts.getObsLon());
}

void
ProjectionBase::drawGreatArc(const double lon1, const double lat1,
                             const double r1,
                             const double lon2, const double lat2,
                             const double r2,
                             const float spacing,
                             const unsigned char *color)
{
    double tc, dist;
    double step = spacing * deg_to_rad;
    double offset = M_PI;
    if (opts.projection == RECTANGULAR && opts.getObsLon() != 0
	&& !opts.mapbounds)
        offset += opts.getObsLon();

    calcGreatArc(lon1, lat1, lon2, lat2, tc, dist);

    double sin_lat1 = sin(lat1);
    double cos_lat1 = cos(lat1);
    double sin_tc = sin(tc);
    double cos_tc = cos(tc);

    if (r1 < 0 || r2 < 0)
    {
        for (double d = 0; d < dist; d += step)
        {
            double lat = asin(sin_lat1 * cos(d) 
                              + cos_lat1 * sin(d) * cos_tc);
            double dlon = atan2(sin_tc * sin(d) * cos_lat1, 
                                cos(d) - sin_lat1 * sin(lat));
            double lon = fmod(lon1 - dlon + M_PI, TWO_PI) - offset;

            placeDot(lon, lat, color);
        }
    }
    else
    {
        for (double d = 0; d < dist; d += step)
        {
            double lat = asin(sin_lat1 * cos(d) 
                              + cos_lat1 * sin(d) * cos_tc);
            double dlon = atan2(sin_tc * sin(d) * cos_lat1, 
                                cos(d) - sin_lat1 * sin(lat));
            double lon = fmod(lon1 - dlon + M_PI, TWO_PI) - offset;
            
            double r = r1 + d/dist * (r2 - r1);
            placeDot(lon, lat, r, color);
        }
    }
}

void
ProjectionBase::drawGreatArc(const double lon1, const double lat1, 
                             const double lon2, const double lat2,
                             const float spacing,
                             const unsigned char *color)
{
    drawGreatArc(lon1, lat1, -1, lon2, lat2, -1, spacing, color);
}

void
ProjectionBase::drawGrid()
{
    double lon, lat;
    int i;          // loop variable 

    // Grid points can be as close as 1/mag_factor degrees 
    int mag_factor = 10;
    int lon0 = -180, lon1 = 180, lat0 = -90, lat1 = 90;

    if (opts.getObsLon() != 0 && opts.projection == RECTANGULAR && !opts.mapbounds)
    {
        lon0 -= (int) (opts.getObsLon() / deg_to_rad);
        lon1 -= (int) (opts.getObsLon() / deg_to_rad);
    }

    lon0 *= mag_factor;
    lon1 *= mag_factor;
    lat0 *= mag_factor;
    lat1 *= mag_factor;

    int grid_step = mag_factor * 90 / (opts.grid1 * opts.grid2);
    if (grid_step < 1) grid_step = 1;

    for (i = lon0; i < lon1; i += (mag_factor * 90)/opts.grid1)
    {
        lon = ((double) i)/mag_factor * deg_to_rad;
        for (int j = lat0; j < lat1; j += grid_step)
        {
            lat = ((double) j) / mag_factor * deg_to_rad;
            placeDot(lon, lat);
        }
    }

    for (i = lon0; i < lon1; i += grid_step)
    {
        lon = ((double) i)/mag_factor * deg_to_rad;
        for (int j = lat0; j < lat1; j += (mag_factor * 90)/opts.grid1)
        {
            lat = ((double) j) / mag_factor * deg_to_rad;
            placeDot(lon, lat);
        }
    }
}

void
ProjectionBase::rotatePoint(double &lon, double &lat) const
{
    Location l(lat, lon, 1);
    l.sphericalToRectangular();
    l.rotate(rot);
    l.rectangularToSpherical();
    lon = l.lon;
    lat = l.lat;
}

void 
ProjectionBase::setBackground()
{
    if (opts.transpng) return;

    if (!opts.background.empty())
        readImageFile(opts.background, width, height, rgb_data);
    else
    {
        // add random stars
        int num_stars = (int) (width * height * opts.starfreq);
        srandom((unsigned int) opts.time.tv_sec);
        for (int i = 0; i < num_stars; i++)
        {
            int j = random() % width;
            int k = random() % height;
            int brightness = random() % 256;
            memset(rgb_data + 3 * (k * width + j), brightness, 3);
        }
    }
}

void
ProjectionBase::placeDot(const double lon, const double lat,
                         const double r, const unsigned char *color)
{
    int ilon=0, ilat=0;
    if (sphericalToPixel(lon, lat, r, ilon, ilat))
    {
        memcpy(rgb_data + 3 * (ilat * width + ilon), color, 3);
        if (opts.transpng) png_alpha[ilat * width + ilon] = 255;
    }
}

void
ProjectionBase::placeDot(const double lon, const double lat, 
                         const unsigned char *color)
{
    int ilon=0, ilat=0;
    if (sphericalToPixel(lon, lat, ilon, ilat))
    {
        memcpy(rgb_data + 3 * (ilat * width + ilon), color, 3);
        if (opts.transpng) png_alpha[ilat * width + ilon] = 255;
    }
}

void
ProjectionBase::placeDot(const double lon, const double lat)
{
    int ilon=0, ilat=0;
    if (sphericalToPixel(lon, lat, ilon, ilat))
    {
        memset(rgb_data + 3 * (ilat * width + ilon), 255, 3);
        if (opts.transpng) png_alpha[ilat * width + ilon] = 255;
    }
}

void
ProjectionBase::buildPhotoTable()
{
    table_size = 10;
    cosangle = new double[table_size];
    photo_function = new double[table_size];

    for (int i = 0; i < table_size; i++)
    {
        cosangle[i] = i / (table_size - 1.);
        photo_function[i] = sqrt(cosangle[i]);
    }
}

void
ProjectionBase::destroyPhotoTable()
{
    delete [] cosangle;
    delete [] photo_function;
}

double
ProjectionBase::getPhotoFunction(const double x) const
{
    if (x < 0) return(0);

    for (int i = 0; i < table_size; i++)
    {
        if (cosangle[i] > x) 
        {
            double frac = ((x - cosangle[i-1])
                           /(cosangle[i] - cosangle[i-1]));
            double returnval = (photo_function[i-1] 
                                + frac * (photo_function[i] 
                                          - photo_function[i-1]));
            return(returnval);
        }
    }

    return(1);
}
