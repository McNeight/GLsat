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
#include <iostream>
using namespace std;

#include "Options.h"
#include "util.h"

#include "libprojection/libprojection.h"

void 
addSpecularMap(const char *mapname, int width, int height, 
               unsigned char *day)
{
    unsigned char *specfile = NULL;
    if (!readImageFile(mapname, width, height, specfile))
    {
        cerr << "Can't load specular reflection file\n";
        return;
    }

    // This creates a rectangular projection but doesn't allocate
    // memory for a map
    ProjectionRectangular image_rect(width, height, false);
    
    double sunlon = getSunLon();
    double sunlat = getSunLat();
    double tc, dist;

    calcGreatArc(sunlon, sunlat, opts.getObsLon(), opts.getObsLat(), 
                 tc, dist);

    double sin_lat1 = sin(sunlat);
    double cos_lat1 = cos(sunlat);
    double sin_tc = sin(tc);
    double cos_tc = cos(tc);
    double sin_d2 = sin(dist/2);
    double cos_d2 = cos(dist/2);
    
    double midlat = asin(sin_lat1 * cos_d2 
                         + cos_lat1 * sin_d2 * cos_tc);
    double dlon = atan2(sin_tc * sin_d2 * cos_lat1, 
                        cos_d2 - sin_lat1 * sin(midlat));
    double midlon = fmod(sunlon - dlon + M_PI, TWO_PI) - M_PI;
    
    Location midpoint(midlat, midlon, 1);
    midpoint.sphericalToRectangular();

    for (int j = 0; j < height; j++)
    {
        int ipos = 3 * j * width;
        double lat = image_rect.yToLat(j);
        for (int i = 0; i < width; i++)
        {
            double lon = image_rect.xToLon(i);
            Location point(lat, lon, 1);
            point.sphericalToRectangular();
            
            double x = 0.95 * point.dot(midpoint);
            if (x > 0.8) 
            {
                // I just picked these numbers to make it look good (enough)
                x = pow(x, 24);
                
                double brightness = x * specfile[ipos];
                double b255 = brightness/255;
                for (int k = 0; k < 3; k++)
                    day[ipos + k] = (unsigned char) 
                        (brightness + (1 - b255) * day[ipos + k]);
            }
            ipos += 3;
        }
    }

    deleteImage(specfile);
}
