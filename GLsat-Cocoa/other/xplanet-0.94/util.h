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

#ifndef UTIL_H
#define UTIL_H

#include <cmath>
#include <ctime>
#include <string>
using namespace std;

#include "checkfuncs.h"

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#define M_PI_2         1.57079632679489661923  /* pi/2 */
#endif

const double deg_to_rad = M_PI/180.;
const double TWO_PI = 2 * M_PI;

#ifndef HAVE_DREM
extern double drem (const double x, const double y);
#endif /* !HAVE_DREM */

extern void removeFromEnvironment(const char *name);

extern double kepler(const double e, const double M);
extern double poly(const double a1, const double a2, const double a3, 
                   const double a4, const double t);
extern double julian(int year, int month, int day, int hour, int min,
                     int sec);
extern double julianCentury(const time_t tv_sec);
extern double calcObliquity(const double T);
extern void eclipticToEquatorial(const double ec_lon, const double ec_lat, 
                                 const double eps,
                                 double &eq_lon, double &eq_lat);
extern void equatorialToEcliptic(const double eq_lon, const double eq_lat,
                                 const double eps,
                                 double &ec_lon, double &ec_lat);
extern void calcGreatArc(const double slon, const double slat,
                         const double elon, const double elat,
                         double &trueCourse, double &dist);

extern double gmst(const double T, const time_t tv_sec);

extern void rotateXYZ(double matrix[3][3], const double angle_x, 
                      const double angle_y, const double angle_z);
extern void rotateZYX(double matrix[3][3], const double angle_x, 
                      const double angle_y, const double angle_z);
extern void deleteImage(unsigned char *&rgb_data);
extern int readImageFile(string filename, int &width, int &height, 
                         unsigned char *&rgb_data);

extern bool fileExists(string &filename);

extern bool imageFileExists(string &filename);

extern void sunpos(const double T);

extern double getSunLat();
extern double getSunLon();
extern double getSunDist();

extern void setSunLat(double lat);
extern void setSunLon(double lon);

extern double getMoonLat();
extern double getMoonLon();
extern double getMoonDist();

extern void setMoonLat(double lat);
extern void setMoonLon(double lon);

#endif
