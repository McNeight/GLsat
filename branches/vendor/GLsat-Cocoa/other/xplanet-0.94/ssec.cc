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

#include "Options.h"
#include "util.h"

#include "libimage/libimage.h"

/*
  Convert lon, lat coordinates to screen coordinates for the Mollweide
  projection.
*/
static void
sphericalToPixel(const int width, const int height, 
                 const double lon, const double lat, 
                 int &x, int &y)
{
    double theta = lat;
    double del_theta = 1;
    while (fabs(del_theta) > 1e-5)
    {
        del_theta = -((theta + sin(theta) - M_PI * sin(lat)) 
                      / (1 + cos(theta)));
        theta += del_theta;
    }
    theta /= 2;

    double X = lon / M_PI * cos(theta);
    double Y = sin(theta);

    x = (int) ((X + 1) * width/2);
    y = (int) (height/2 * (1 - Y));
}

/*
  Convert x, y coordinates on the screen to lon, lat for a rectangular
  projection.
*/
static void
pixelToSpherical(const int width, const int height, 
                 const int x, const int y, 
                 double &lon, double &lat)
{
    lon = (x + 0.5) * TWO_PI / width - M_PI;
    lat = M_PI_2 - (y + 0.5) * M_PI / height;
}

static int
equalizeHistogram(unsigned char *&rgb, const int width, const int height)
{
    int i;
    int *hist = new int[256];
    memset(hist, 0, 256 * sizeof(int));
    for (i = 0; i < 3 * width * height; i += 3) hist[(int) rgb[i]]++;

    int *ihist = new int[256];
    ihist[0] = hist[0];
    for (i = 1; i < 256; i++) ihist[i] = ihist[i-1] + hist[i];

    // replace the histogram by an intensity map
    double denom = (double) (ihist[255] - ihist[0]);
    if (denom < 1) return(0);

    for (i = 0; i < 256; i++)
        hist[i] = (int) (255 * (ihist[i] - ihist[0])/denom);

    for (i = 0; i < 3 * width * height; i++) 
        rgb[i] = (unsigned char) hist[(int) rgb[i]];

    delete [] hist;
    delete [] ihist;

    return(1);
}

/*
  This routine reads in a global cloud image downloaded from
  http://www.ssec.wisc.edu/data/comp/latest_moll.gif 
  and reprojects and resizes the image, gets rid of the ugly pink
  coastlines, and stretches the contrast.
*/
int
readSsecImage(string filename, int &width, int &height, 
              unsigned char *&rgb_data)
{
    int i, j, ii, jj;

    int image_width = -1, image_height = -1;
    unsigned char *temp = NULL;
    if (!readImageFile(filename, image_width, image_height, temp))
        return(0);

    // There's a 20 pixel border on the left & right and a 10 pixel border
    // on the top and bottom
    crop_image(&temp, image_width, image_height, 10, 20, image_width - 20,
               image_height - 40);
    image_height -= 40;
    image_width -= 20;

    // This array will hold the final cloud image
    rgb_data = (unsigned char *) malloc(3 * image_width * image_height);
    if (rgb_data == NULL)
    {
        cerr << "Can't allocate memory for cloud image\n";
        return(0);
    }

    memset(rgb_data, 0, 3 * image_width * image_height);

    // This converts the mollweide projection to rectangular
    double lon, lat;
    int ipos = 0;
    for (j = 0; j < image_height; j++)
        for (i = 0; i < image_width; i++)
        {
            pixelToSpherical(image_width, image_height, i, j, lon, lat);
            sphericalToPixel(image_width, image_height, lon, lat, ii, jj);
            memcpy(rgb_data + ipos, temp + 3 * (jj * image_width + ii), 3);
            ipos += 3;
        }

    delete_image(&temp);

    int avg;
    int npoints;
    int avgwhole = 0;
    int npointswhole = 0;

    // Replace pink outlines by the average value in a 10x10 pixel square.
    for (j = 0; j < 31; j++)
        for (i = 0; i < 62; i++)
        {
            avg = 0;
            npoints = 0;
            for (jj = 0; jj < 10; jj++)
                for (ii = 0; ii < 10; ii++)
                {
                    ipos = 3*((10 * j + jj) * 620 + 10 * i + ii);
                    if (!(rgb_data[ipos] == 0xff && rgb_data[ipos+1] == 0
                          && rgb_data[ipos+2] == 0xff))
                    {
                        npoints++;
                        avg += (int) rgb_data[ipos];
                        npointswhole++;
                        avgwhole += (int) rgb_data[ipos];
                    }
                }
            if (npoints != 0) avg = (int) (avg / (double) npoints);

            for (jj = 0; jj < 10; jj++)
                for (ii = 0; ii < 10; ii++)
                {
                    ipos = 3*((10 * j + jj) * 620 + 10 * i + ii);
                    if (rgb_data[ipos] == 0xff && rgb_data[ipos+1] == 0
                        && rgb_data[ipos+2] == 0xff) 
                        memset(rgb_data + ipos, avg, 3);
                }
        }

    // Fill in the poles
    if (npointswhole != 0) 
        avgwhole = (int) (avgwhole / (double) npointswhole);
    for (i = 0; i < image_width * image_height; i++)
    {
        ipos = 3 * i;
        if (rgb_data[ipos] < 0x03) memset(rgb_data + ipos, avgwhole, 3);
    }

    // Smooth out the seam at 180 degrees Longitude
    double eastVal, westVal;
    int eastIndex, westIndex;
    for (i = 0; i < image_height - 1; i++)
    {
        eastIndex = 3 * (i * image_width + 1);
        westIndex = 3 * ((i + 1) * image_width - 2);
        eastVal = (double) rgb_data[eastIndex];
        westVal = (double) rgb_data[westIndex];
        memset(rgb_data + eastIndex - 3, 
               (int) (eastVal + (westVal - eastVal)/3), 3);
        memset(rgb_data + westIndex + 3, 
               (int) (westVal + (eastVal - westVal)/3), 3);
    }

    equalizeHistogram(rgb_data, image_width, image_height);

    if (width < 0 || height < 0)
    {
        width = image_width;
        height = image_height;
    }
    else
        resize_image(&rgb_data, image_width, image_height, width, height, 1);

    return(1);
}

