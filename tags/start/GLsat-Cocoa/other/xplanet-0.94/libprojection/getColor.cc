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

#include "Location.h"
#include "Options.h"
#include "util.h"
#include "xplanet.h"

#include "libprojection.h"

void
getColor(const ProjectionRectangular *image_rect,
         const double lon, const double lat, const double darkening, 
         unsigned char pixel[3]) 
{
    if (opts.blend == 0)
    {
        int ilon, ilat;
        if (image_rect->sphericalToPixel(lon, lat, ilon, ilat))
	{
	    int ipos = 3 * (ilat*image_rect->getWidth() + ilon);
	    
	    for (int i = 0; i < 3; i++)
		pixel[i] = (unsigned char) (darkening 
					    * image_rect->rgb_data[ipos++]);
	}
	else
	{
	    memset(pixel, 0, 3);
	}

        return;
    }

    /*
      The point (lon, lat) falls into one of these four pixels.  The 
      color returned will be an area weighted average of these four.
    
      --- ---
      | 0 | 1 |
      --- ---
      | 2 | 3 |
      --- ---
    */
  
    int ix[4], iy[4];

    if (!image_rect->sphericalToPixel(lon, lat, ix[1], iy[2]))
    {
	memset(pixel, 0, 3);
	return;
    }


    if (ix[1] == 0)
    {
        ix[0] = image_rect->getWidth() - 1;
        ix[1] = 0;
    }
    else
    {
        ix[0] = ix[1] - 1;
    }

    iy[0] = iy[2] - 1;
    if (iy[0] == -1) iy[0] = 0;

    ix[2] = ix[0];
    ix[3] = ix[1];
    iy[1] = iy[0];
    iy[3] = iy[2];
  
    double t = lon - image_rect->xToLon(ix[2]);
    if (t > M_PI) t -= TWO_PI;
    if (t < -M_PI) t += TWO_PI;
    t /= image_rect->getDeltaLon();

    double u = ((lat - image_rect->yToLat(iy[2]))
                / image_rect->getDeltaLat());

    // Weights are from Numerical Recipes, 2nd Edition
    //        weight[0] = (1 - t) * u;
    //        weight[2] = (1-t) * (1-u);
    //        weight[3] = t * (1-u);
    double weight[4];
    weight[1] = t * u;
    weight[0] = u - weight[1];
    weight[2] = 1 - t - u + weight[1];
    weight[3] = t - weight[1];

    memset(pixel, 0, 3);
    for (int ii = 0; ii < 4; ii++)
    {
        int ipos = 3*(iy[ii]*image_rect->getWidth() + ix[ii]);
        weight[ii] *= darkening; 
        for (int jj = 0; jj < 3; jj++)
        {
            pixel[jj] += (unsigned char) 
                (weight[ii] * image_rect->rgb_data[ipos++]);
        }
    }
}

