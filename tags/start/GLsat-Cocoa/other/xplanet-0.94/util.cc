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
#include <cstdio>
#include <cstring>
#include <ctime>
#include <list>
#include <iostream>
#include <string>
using namespace std;

#include <errno.h>
#include <sys/time.h>

#include "auxfiles.h"
#include "Options.h"
#include "util.h"
#include "xplanet.h"

#include "libimage/libimage.h"

#include "checkfuncs.h"

static double sunlat, sunlon, sundist;
static double moonlat, moonlon, moondist;

#include <unistd.h>
extern char **environ;

#ifndef HAVE_DREM
/*
 * Like fmod, but returned value in in the range of (-0.5y .. +0.5y]
 * as opposed to (-y .. +y)
 */

double
drem (const double x, const double y)
{
    double d = fmod(x, y);
    if (errno == 0)
    {
        if (d <= -y/2)
            d += y;
        else if (d >= y/2)
            d -= y;
    }
    return d;
}
#endif /* !HAVE_DREM */

void
removeFromEnvironment(const char *name)
{
#ifdef HAVE_UNSETENV
    unsetenv(name);
#else
    string badname = name;
    badname += "=";

    // I found this useful code on groups.google.com.  It's based on
    // sudo's code, where the environment is cleaned up before
    // executing anything.
    char **cur, **move;
    for (cur = environ; *cur; cur++) {
	if (strncmp(*cur, badname.c_str(), badname.length()) == 0)
	{
   	    /* Found variable; move subsequent variables over it */
	    for (move = cur; *move; move++)
		*move = *(move + 1);
	    cur--;
	}
    }

#endif
}

double 
poly(const double a1, const double a2, const double a3, const double a4,
     const double t)
{
    return(a1 + t*(a2 + t*(a3 + t*a4)));
}

double 
julian(int year, int month, int day, int hour, int min, int sec)
{
    if(month < 3) 
    {
        year -= 1;
        month += 12;
    }      
  
    // assume it's the Gregorian calendar (after 1582 Oct 15)

    int a = year/100;
    int b = 2 - a + a/4;
    int c = int(365.25 * year);
    int d = int(30.6001 * (month + 1));
    double e = day + ((sec/60. + min) / 60. + hour) / 24.;

    double jd = b + c + d + e + 1720994.5;

    return(jd);
}

double 
julianCentury(const time_t tv_sec)
{
    double jd = julian(gmtime(&tv_sec)->tm_year + 1900,
                       gmtime(&tv_sec)->tm_mon + 1,
                       gmtime(&tv_sec)->tm_mday,
                       gmtime(&tv_sec)->tm_hour,
                       gmtime(&tv_sec)->tm_min,
                       gmtime(&tv_sec)->tm_sec);
    
    return((jd - 2415020)/36525);
}

double 
kepler(const double e, const double M)
{
    double E = M;
    double delta = 1;

    while (fabs(delta) > 1E-10)
    {
        delta = (M + e * sin(E) - E)/(1 - e * cos(E));
        E += delta;
    }
    return(E);
}

double
gmst(const double T, const time_t tv_sec)
{      
    // Sidereal time at Greenwich at 0 UT
    double g = poly(6.6460656, 2400.051262, 0.00002581, 0, T);

    // Now find current sidereal time at Greenwich
    double currgmt = (gmtime(&tv_sec)->tm_hour 
                      + gmtime(&tv_sec)->tm_min/60.
                      + gmtime(&tv_sec)->tm_sec/3600.);
    currgmt *= 1.002737908;
    g += currgmt;
    g = fmod(g, 24.0);
    if (g < 0) g += 24;
    return(g);
}

double
calcObliquity(const double T)
{
    return (poly(23.452294, -1.30125E-2, -1.64E-6, 5.03E-7, T)
            * deg_to_rad);
}

void
eclipticToEquatorial(const double ec_lon, const double ec_lat, const double eps,
                     double &eq_lon, double &eq_lat)
{
    eq_lat = asin(sin(eps) * sin(ec_lon) * cos(ec_lat) + sin(ec_lat) * cos(eps));

    eq_lon = atan2(cos(eps) * sin(ec_lon) - tan(ec_lat) * sin(eps), cos(ec_lon));
}

void
equatorialToEcliptic(const double eq_lon, const double eq_lat, const double eps,
                     double &ec_lon, double &ec_lat)
{
    ec_lon = atan2(sin(eq_lon) * cos(eps) + tan(eq_lat) * sin(eps),
                   cos(eq_lon));
    ec_lat = asin(sin(eq_lat) * cos(eps) -
                  cos(eq_lat) * sin(eps) * sin(eq_lon));
}

void
calcGreatArc(const double slon, const double slat,
             const double elon, const double elat,
             double &trueCourse, double &dist)
{
    /*
     * Equations are from http://www.best.com/~williams/avform.html
     * returned trueCourse is relative to latitude north
     */
    double sin_slat = sin(slat), cos_slat = cos(slat);
    double sin_elat = sin(elat), cos_elat = cos(elat);
    double dlon = slon - elon;

    // Arc length between points (in radians)
    dist = acos(sin_slat * sin_elat + cos_slat * cos_elat * cos(dlon));

    // True course
    trueCourse = fmod(atan2(sin(dlon) * cos_elat, 
                            cos_slat * sin_elat 
                            - sin_slat * cos_elat * cos(dlon)), TWO_PI);
}

void 
rotateXYZ(double matrix[3][3], const double angle_x, 
          const double angle_y, const double angle_z)
{
    if (angle_x == 0 && angle_y == 0 && angle_z == 0) 
    {
        for (int j = 0; j < 3; j++)
            for (int i = 0; i < 3; i++)
                matrix[j][i] = (i == j ? 1 : 0 );
        return;
    }

    /*
      matrix to first rotate reference frame angle_x radians through 
      x axis, then angle_y radians through y axis, and lastly 
      angle_z radians through z axis.  Positive rotations are counter-
      clockwise looking down the axis.
    */

    double cosx = cos(angle_x);
    double cosy = cos(angle_y);
    double cosz = cos(angle_z);
    double sinx = sin(angle_x);
    double siny = sin(angle_y);
    double sinz = sin(angle_z);

    matrix[0][0] =  cosy * cosz;
    matrix[0][1] =  sinx * siny * cosz + cosx * sinz;
    matrix[0][2] = -cosx * siny * cosz + sinx * sinz;
    matrix[1][0] = -cosy * sinz;
    matrix[1][1] = -sinx * siny * sinz + cosx * cosz;
    matrix[1][2] =  cosx * siny * sinz + sinx * cosz;
    matrix[2][0] =  siny;
    matrix[2][1] = -sinx * cosy;
    matrix[2][2] =  cosx * cosy;

}

void 
rotateZYX(double matrix[3][3], const double angle_x, 
          const double angle_y, const double angle_z)
{
    if (angle_x == 0 && angle_y == 0 && angle_z == 0) 
    {
        for (int j = 0; j < 3; j++)
            for (int i = 0; i < 3; i++)
                matrix[j][i] = (i == j ? 1 : 0 );
        return;
    }

    /*
      matrix to first rotate reference frame angle_z radians through 
      z axis, then angle_y radians through y axis, and lastly 
      angle_x radians through x axis.  Positive rotations are counter-
      clockwise looking down the axis.
    */
    double cosx = cos(angle_x);
    double cosy = cos(angle_y);
    double cosz = cos(angle_z);
    double sinx = sin(angle_x);
    double siny = sin(angle_y);
    double sinz = sin(angle_z);

    matrix[0][0] =  cosy * cosz;
    matrix[0][1] =  cosy * sinz;
    matrix[0][2] = -siny;
    matrix[1][0] = -cosx * sinz + sinx * siny * cosz;
    matrix[1][1] =  sinx * siny * sinz + cosx * cosz;
    matrix[1][2] =  sinx * cosy;
    matrix[2][0] =  cosx * siny * cosz + sinx * sinz;
    matrix[2][1] = -sinx * cosz + cosx * siny * sinz;
    matrix[2][2] =  cosx * cosy;
}

bool
fileExists(string &filename)
{
    if (opts.debug) cout << "Looking for file " << filename << endl;
    FILE *f = fopen(filename.c_str(), "r");

    if (f == NULL) return(false);

    fclose(f);
    return(true);
}

/*
  Check for the existence of an image file in the -mapdir directory/ies
  (if specified), then the current directory, the images/ subdirectory
  in the current directory, and finally the prefix directory specified
  in auxfiles.h.
*/
bool
imageFileExists(string &filename)
{
    string newname;

    if (opts.debug) cout << "\nLooking for image file ...\n";

    if (opts.mapdir.size() > 0)
    {
        list<string>::iterator p = opts.mapdir.begin();
        while (p != opts.mapdir.end())
        {
            newname = *p;
            newname += separator;
            newname += filename;
            
            if (fileExists(newname))
            {
                filename = newname;
                return(true);
            }
            
            p++;
        }
    }

    if (fileExists(filename)) return(true);

    newname = "images";
    newname += separator;
    newname += filename;

    if (fileExists(newname))
    {
        filename = newname;
        return(true);
    }            
    
    newname = prefix;
    newname += separator;
    newname += "images";
    newname += separator;
    newname += filename;

    if (fileExists(newname))
    {
        filename = newname;
        return(true);
    }            
    
    return(false);
}

void
deleteImage(unsigned char *&rgb_data)
{
    if (rgb_data != NULL) delete_image(&rgb_data);
}

//  readImageFile
//
//  Save the RGB pixel array of the image file into rgb_data.  If
//  width or height are < 0, width and height are set to image file's
//  dimensions.  Otherwise rgb_data is scaled to supplied width &
//  height.  
int
readImageFile(string filename, int &width, int &height, 
              unsigned char *&rgb_data)
{
    int image_width, image_height;

    if (!imageFileExists(filename)) return(0);

    if (!read_image(filename.c_str(), &image_width, &image_height, 
                    &rgb_data))
    {
        return(0);
    }
  
    if (width < 0 || height < 0)
    {
        width = image_width;
        height = image_height;
    }
    else
    {
        if (opts.projection == RECTANGULAR)
            resize_image(&rgb_data, image_width, image_height, 
                         width, height, opts.blend);
        else
            resize_image(&rgb_data, image_width, image_height,
                         width, height, 0);
    }
    
    return(1);
}

/* Based on Chapter 18 of Astronomical Formulae for Calculators by Meeus */
void 
sunpos(const double T)
{
    double L = (poly(2.7969668E2, 3.600076892E4, 3.025E-4, 0, T) 
                * deg_to_rad);
    double M = (poly(3.5847583E2, 3.599904975E4, -1.5E-4, -3.3E-6, T) 
                * deg_to_rad);
    double ecc = poly(1.675104E-2, -4.18E-5, -1.26E-7, 0, T);
    double eccanom = kepler(ecc, M);
    double nu = 2 * atan(sqrt((1 + ecc) / (1 - ecc)) * tan(eccanom / 2));
    double theta = L + nu - M;

    sunlon = theta;
    sunlat = 0;
    sundist = 1.0000002 * (1 - ecc * cos(eccanom));
}

double
getSunLat()
{
    return(sunlat);
}

double
getSunLon()
{
    return(sunlon);
}

double
getSunDist()
{
    return(sundist);
}

void
setSunLat(double lat)
{
    sunlat = lat;
}

void
setSunLon(double lon)
{
    sunlon = lon;
}

double
getMoonLat()
{
    return(moonlat);
}

double
getMoonLon()
{
    return(moonlon);
}

double
getMoonDist()
{
    return(moondist);
}

void
setMoonLat(double lat)
{
    moonlat = lat;
}

void
setMoonLon(double lon)
{
    moonlon = lon;
}

