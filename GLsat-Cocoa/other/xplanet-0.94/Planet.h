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

#ifndef PLANET_H
#define PLANET_H

#include <cmath>
using namespace std;

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#define M_PI_2         1.57079632679489661923  /* pi/2 */
#endif

struct coordinates
{
    double lon;
    double lat;
    double dist;
    double alpha;
    double delta;
};

class Planet
{
 public:
    Planet(const double T, const body_name this_body);

    double calcObliquity(const double eps);
    void calcHeliocentric();
    void calcGeocentric(const double sunlon, const double sundist);
    void rotateEquator(const double in_lon, const double in_lat, 
                       double &out_lon, double &out_lat);
    void calcEcliptic(const double eps, double &lat, double &lon);
    void calcGalactic (double &lat, double &lon);
    coordinates calcCoordinates(coordinates body, const double eps);

    coordinates helio, geo;

 private:
    body_name body;
    double mlong,         // mean longitude
        axis,             // semimajor axis
        ecc,              // eccentricity
        incl,             // inclination
        perih,            // argument of perihelion
        ascnode,          // longitude of ascending node

        alpha0,           // right ascension of the north pole
        delta0,           // declination of the north pole

        null_meridian,    // orientation of zero longitude
        wdot;             // rotation rate
};

/* 
   flipped = 1 if planet's longitude increases to the east (e.g. earth),
   -1 if planet's longitude increases to the west (e.g. Mars),
*/
extern int flipped; 

/* Equatorial coordinates of galactic north pole */
const double GN_LON = 192.25 / 180 * M_PI;
const double GN_LAT = 27.4/180 * M_PI;

#endif
