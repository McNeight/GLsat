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

/*
  planet.cc - parameters for orbital and rotational motion.  Orbital
  parameters are from Chapter 23 of Astronomical Formulae for
  Calculators by Meeus, rotational parameters for other planets and
  satellites are from Davies et al. (1996), Celestial Mechanics 63,
  127--148.  

  Formulae for Pluto are taken from Paul Schlyter's page at 
  http://www.m2c3.com/alpocs/tdl1999/schlyter110199/ppcomp.html
  and from http://ssd.jpl.nasa.gov/elem_planets.html

  Formula for conversions to galactic coordinates from sect 29 of
  Practical Astronomy With Your Calculator by Duffett-Smith
  (3rd ed, 1988).
*/

#include <cmath>
#include <cstdlib>
#include <cstring>
using namespace std;

#include "Location.h"
#include "Options.h"
#include "Planet.h"
#include "util.h"

int flipped = 1;  
/*
  flipped = 1 if planet's longitude increases to the east (e.g. earth),
  -1 if planet's longitude increases to the west (e.g. Mars),
*/

Planet::Planet (const double T, const body_name this_body)
{
    double T2000 = T - 1;                      // Centuries from 2000.0
    double d = T2000 * 36525;                  // Days from 2000.0
    body = this_body;

    int i;
    
    switch (body)
    {
    case MERCURY:
        mlong = (poly(1.78179078E2, 1.4947407078E5, 3.011E-4, 0, T)
                 * deg_to_rad);
        axis = 0.3870986;
        ecc = poly(2.0561421E-1, 2.046E-5, -3E-8, 0, T);
        incl = (poly(7.002881, 1.8608E-3, -1.83E-5, 0, T)
                * deg_to_rad);
        perih = (poly(2.8753753E1, 3.702806E-1, 1.208E-4, 0, T)
                 * deg_to_rad);
        ascnode = (poly(4.7145944E1, 1.1852083, 1.739E-4, 0, T)
                   * deg_to_rad);
        flipped = -1; 

        alpha0 = 281.01 - 0.033 * T2000;
        delta0 = 61.45 - 0.005 * T2000;

        null_meridian = 329.68;
        wdot = 6.1385025;

        break;
    case VENUS:
        mlong = (poly(3.42767053E2, 5.851921191E4, 3.097E-4, 0, T)
                 * deg_to_rad);
        axis = 0.7233316;
        ecc = poly(6.82069E-3, -4.774E-5, 9.1E-8, 0, T);
        incl = (poly(3.393631, 1.0058E-3, -1.0E-6, 0, T)
                * deg_to_rad);
        perih = (poly(5.4384186E1, 5.081861E-1, -1.3864E-3, 0, T)
                 * deg_to_rad);
        ascnode = (poly(7.5779647E1, 8.9985E-1, 4.1E-4, 0, T)
                   * deg_to_rad);
        flipped = 1; 

        alpha0 = 272.76;
        delta0 = 67.16;

        null_meridian = 160.20;
        wdot = -1.4813688;
      
        break;
    case MOON:
        ascnode = (poly(259.183275, -1.9341420E3, 2.078E-3, 2.2E-6, T));
        incl = 5.16 * deg_to_rad;

        double E[14];
        E[ 1] = 125.045 -  0.0529921 * d;
        E[ 2] = 250.089 -  0.1059842 * d;
        E[ 3] = 260.008 + 13.0120009 * d;
        E[ 4] = 176.625 + 13.3407154 * d;
        E[ 5] = 357.529 +  0.9856003 * d;
        E[ 6] = 311.589 + 26.4057084 * d;
        E[ 7] = 134.963 + 13.0649930 * d;
        E[ 8] = 276.617 +  0.3287146 * d;
        E[ 9] =  34.226 +  1.7484877 * d;
        E[10] =  15.134 -  0.1589763 * d;
        E[11] = 119.743 +  0.0036096 * d;
        E[12] = 239.961 +  0.1643573 * d;
        E[13] =  25.053 + 12.9590088 * d;
        for (i = 1; i < 14; i++) E[i] *= deg_to_rad;

        alpha0 = 269.9949 + (0.0031 * T2000        - 3.8787 * sin(E[ 1])
                             - 0.1204 * sin(E[ 2]) + 0.0700 * sin(E[ 3])
                             - 0.0172 * sin(E[ 4]) + 0.0072 * sin(E[ 6])
                             - 0.0052 * sin(E[10]) + 0.0043 * sin(E[13]));
        delta0 =  66.5392 + (0.0130 * T2000        + 1.5419 * cos(E[ 1])
                             + 0.0239 * cos(E[ 2]) - 0.0278 * cos(E[ 3])
                             + 0.0068 * cos(E[ 4]) - 0.0029 * cos(E[ 6])
                             + 0.0009 * cos(E[ 7]) + 0.0008 * cos(E[10])
                             - 0.0009 * cos(E[13]));
        null_meridian = 38.3213 + (d * (13.17635815 - 1.4E-12 * d) 
                                   + 3.5610 * sin(E[ 1]) 
                                   + 0.1208 * sin(E[ 2])
                                   - 0.0642 * sin(E[ 3])
                                   + 0.0158 * sin(E[ 4])
                                   + 0.0252 * sin(E[ 5])
                                   - 0.0066 * sin(E[ 6])
                                   - 0.0047 * sin(E[ 7])
                                   - 0.0046 * sin(E[ 8])
                                   + 0.0028 * sin(E[ 9])
                                   + 0.0052 * sin(E[10])
                                   + 0.0040 * sin(E[11])
                                   + 0.0019 * sin(E[12])
                                   - 0.0044 * sin(E[13]));
        wdot = 0;
        flipped = 1;

        break;
    case MARS:
        mlong = (poly(2.93737334E2, 1.914169551E4, 3.107E-4, 0, T) 
                 * deg_to_rad);
        axis = 1.5236883;
        ecc = poly(9.33129E-2, 9.2064E-5, -7.7E-8, 0, T);
        incl = (poly(1.850333, -6.75E-4, 1.26E-5, 0, T) * deg_to_rad);
        perih = (poly(2.85431761E2, 1.0697667, 1.313E-4, 4.41E-6, T) 
                 * deg_to_rad);
        ascnode = (poly(4.8786442E1, 7.709917E-1, -1.4E-6, -5.33E-6, T) 
                   * deg_to_rad);
        flipped = -1;
      
        alpha0 = 317.681 - 0.108 * T2000;
        delta0 = 52.886 - 0.061 * T2000;
        null_meridian = 176.901;
        wdot = 350.891983;
      
        break;
    case JUPITER:
    case IO:
    case EUROPA:
    case GANYMEDE:
    case CALLISTO:
        mlong = (poly(2.38049257E2, 3.036301986E3, 3.347E-4, -1.65E-6, 
                      T) * deg_to_rad);
        axis = 5.202561;
        ecc = poly(4.833475E-2, 1.6418E-4, -4.676E-7, -1.7E-9, T);
        incl = poly(1.308736, -5.6961E-3, 3.9E-6, 0, T) * deg_to_rad;
        perih = (poly(2.73277558E2, 5.994317E-1, 7.0405E-4, -5.08E-6, T) 
                 * deg_to_rad);
        ascnode = (poly(9.9443414E1, 1.01053, 3.5222E-4, -8.51E-6, T) 
                   * deg_to_rad);

        flipped = -1;

        double J[9];
        J[1] =  73.32 + 91472.9 * T2000;
        J[2] =  24.62 + 45137.2 * T2000;
        J[3] = 283.90 +  4850.7 * T2000;
        J[4] = 355.80 +  1191.3 * T2000;
        J[5] = 119.90 +   262.1 * T2000;
        J[6] = 229.80 +    64.3 * T2000;
        J[7] = 352.25 +  2382.6 * T2000;
        J[8] = 113.35 +  6070.0 * T2000;
        for (i = 1; i < 9; i++) J[i] *= deg_to_rad;

        switch (body)
        {
        case JUPITER:
            alpha0 = 268.05 - 0.009 * T2000;
            delta0 = 64.49 + 0.003 * T2000;
          
            // System III (magnetic field rotation)
            null_meridian = 284.95;
            wdot = 870.536;
            break;
        case IO:
            alpha0 = (268.05 - 0.009 * T2000 + 0.094 * sin(J[3]) 
                      + 0.024 * sin(J[4]));
            delta0 = (64.50 + 0.003 * T2000 + 0.040 * cos(J[3])
                      + 0.011 * cos(J[4]));
            null_meridian = 200.39 - 0.085 * sin(J[3]) - 0.022 * sin(J[4]);
            wdot = 203.4889538;
            break;
        case EUROPA:
            alpha0 = (268.08 - 0.009 * T2000 + 1.086 * sin(J[4])
                      + 0.060 * sin(J[5]) + 0.015 * sin(J[6])
                      + 0.009 * sin(J[7]));
            delta0 = (64.51 + 0.003 * T2000 + 0.468 * cos(J[4])
                      + 0.026 * cos(J[5]) + 0.007 * cos(J[6])
                      + 0.002 * cos(J[7]));
            null_meridian = (35.67 - 0.980 * sin(J[4]) - 0.054 * sin(J[5])
                             - 0.014 * sin(J[6]) - 0.008 * sin(J[7]));
            wdot = 101.3747235;
            break;
        case GANYMEDE:
            alpha0 = (268.20 - 0.009 * T2000 - 0.037 * sin(J[4]) 
                      + 0.431 * sin(J[5]) + 0.091 * sin(J[6]));
            delta0 = (64.57 + 0.003 * T2000 - 0.016 * cos(J[4])
                      + 0.186 * cos(J[5]) + 0.039 * cos(J[6]));
            null_meridian = (44.04 + 0.033 * sin(J[4]) - 0.389 * sin(J[5])
                             - 0.082 * sin(J[6]));
            wdot = 50.3176081;
            break;
        case CALLISTO:
            alpha0 = (268.72 - 0.009 * T2000 - 0.068 * sin(J[5])
                      + 0.590 * sin(J[6]) + 0.010 * sin(J[8]));
            delta0 = (64.83 + 0.003 * T2000 - 0.029 * cos(J[5])
                      + 0.254 * cos(J[6]) - 0.004 * cos(J[8]));
            null_meridian = (259.73 + 0.061 * sin(J[5]) - 0.533 * sin(J[6])
                             - 0.009 * sin(J[8]));
            wdot = 21.5710715;
            break;
        default:
            break;
        }
        break;
    case SATURN:
    case TITAN:
    {
        mlong = (poly(2.66564377E2, 1.223509884E3, 3.245E-4, -5.8E-6, T) 
                 * deg_to_rad);
        axis = 9.554747;
        ecc = poly(5.589232E-2, -3.455E-4, -7.28E-7, 7.4E-10, T);
        incl = (poly(2.492519, -3.9189E-3, -1.549E-5, 4E-8, T) * deg_to_rad);
        perih = (poly(3.383078E2, 1.0852207, 9.7854E-4, 9.92E-6, T) 
                 * deg_to_rad);
        ascnode = (poly(1.12790414E2, 8.731951E-1, -1.5218E-4, -5.31E-6, T)
                   * deg_to_rad);
        flipped = -1;

        double S[8];
        S[7] = 29.80 - 52.1 * T2000;
        S[7] *= deg_to_rad;
        //      for (i = 1; i < 8; i++) S[i] *= deg_to_rad;
        switch (body)
        {
        case SATURN:
            alpha0 = 40.589 - 0.036 * T2000;
            delta0 = 83.537 - 0.004 * T2000;
            null_meridian = 38.90;
            wdot = 810.7939024;
            break;
        case TITAN:
            alpha0 = 36.41 - 0.036 * T2000 + 2.66 * sin(S[7]);
            delta0 = 83.94 - 0.004 * T2000 - 0.30 * cos(S[7]);
            null_meridian = 189.64 - 2.64 * sin(S[7]);
            wdot = 22.576978;
            break;
        default:
            break;
        }
    }
    break;
    case URANUS:
        mlong = (poly(2.4419747E2, 4.29863546E2, 3.16E-4, -6E-7, T) 
                 * deg_to_rad);
        axis = 19.21814;
        ecc = poly(4.63444E-2, -2.658E-5, 7.7E-8, 0, T);
        incl = (poly(7.72464E-1, 6.253E-4, 3.95E-5, 0, T) * deg_to_rad);
        perih = (poly(9.8071581E1, 9.85765E-1, -1.0745E-3, -6.1E-7, T) 
                 * deg_to_rad);
        ascnode = (poly(7.3477111E1, 4.986678E-1, 1.3117E-3, 0, T) 
                   * deg_to_rad);

        flipped = 1;

        alpha0 = 257.311;
        delta0 = -15.175;
        null_meridian = 203.81;
        wdot = -501.1600928;
        break;
    case NEPTUNE:
    {
        mlong = (poly(8.457994E1, 2.19885914E2, 3.205E-4, 0, T) 
                 * deg_to_rad);
        axis = 30.10957;
        ecc = poly(8.99704E-3, 6.33E-7, -2E-9, 0, T);
        incl = (poly(1.779242, -9.5436E-3, -9.1E-6, 0, T) * deg_to_rad);
        perih = (poly(2.76045975E2, 3.256394E-1, 1.4095E-4, 4.113E-6, T) 
                 * deg_to_rad);
        ascnode = (poly(1.30681389E2, 1.098935, 2.4987E-4, -4.718E-6, T) 
                   * deg_to_rad);
        
        flipped = -1;
        double N = (357.85 + 52.316 * T2000) * deg_to_rad;
        alpha0 = 299.36 + 0.70 * sin(N);
        delta0 = 43.46 - 0.51 * cos(N);
        null_meridian = 253.18 - 0.48 * sin(N);
        wdot = 536.3128492;
    }
    break;
    case PLUTO:
    {
        double S = ( 50.03 + 0.033459652 * d) * deg_to_rad;
        double P = (238.95 + 0.003968789 * d) * deg_to_rad;
        
        helio.lon = (238.9508 + 0.00400703 * d
                     - 19.799 * sin(P)   + 19.848 * cos(P)
                     +  0.897 * sin(2*P) -  4.956 * cos(2*P)
                     +  0.610 * sin(3*P) +  1.211 * cos(3*P)
                     -  0.341 * sin(4*P) -  0.190 * cos(4*P)
                     +  0.128 * sin(5*P) -  0.034 * cos(5*P)
                     -  0.038 * sin(6*P) +  0.031 * cos(6*P)
                     +  0.020 * sin(S-P) -  0.010 * cos(S-P)) * deg_to_rad;
        
        helio.lat = (-3.9082
                     - 5.453 * sin(P)   - 14.975 * cos(P)
                     + 3.527 * sin(2*P) + 1.673 * cos(2*P)
                     - 1.051 * sin(3*P) + 0.328 * cos(3*P)
                     + 0.179 * sin(4*P) - 0.292 * cos(4*P)
                     + 0.019 * sin(5*P) + 0.100 * cos(5*P)
                     - 0.031 * sin(6*P) - 0.026 * cos(6*P)
                     + 0.011 * cos(S-P)) * deg_to_rad;
        
        helio.dist = (40.72
                      + 6.68 * sin(P)   + 6.90 * cos(P)
                      - 1.18 * sin(2*P) - 0.03 * cos(2*P)
                      + 0.15 * sin(3*P) - 0.14 * cos(3*P));
        
        incl = (17.14175 + 11.07 * T) * deg_to_rad;
        perih = (224.06676 - 132.25 * T) * deg_to_rad;
        ascnode = (110.34307 - 37.33 * T) * deg_to_rad;

        alpha0 = 313.02;
        delta0 = 9.09;
        null_meridian = 236.77;
        wdot = -56.3623195;
        
        flipped = 1;
    }
    break;
    default:
        break;
    }

    alpha0 *= deg_to_rad;
    delta0 *= deg_to_rad;

#if 1
    null_meridian = fmod(null_meridian + wdot * d, 360.0) * deg_to_rad;
    if (null_meridian < 0.) null_meridian += TWO_PI;
#else
    /*
     * Does 'wdot' indicate the daily angular rotation based on
     * the _earth's_ ecliptic or the _planet's_?
     * If the latter, this may be more accurate.
     */
    double cd0 = fabs(cos(M_PI_2 - delta0));
    tmp = null_meridian * deg_to_rad - (alpha0 + M_PI_2);

    null_meridian = drem (atan2 (sin(tmp), cos(tmp) * cd0)
                          + wdot * d, TWO_PI);
    if ((null_meridian = atan2(sin(null_meridian) * cd0,
                               cos(null_meridian)) + alpha0 + M_PI_2) < 0.)
        null_meridian += TWO_PI;
    else if (null_meridian >= TWO_PI)
        null_meridian -= TWO_PI;
#endif
}

double
Planet::calcObliquity(const double eps)
{
    double tmp, pLon, pLat, obliquity;

    // convert planet's north pole to geocentric coordinates
    equatorialToEcliptic(alpha0, delta0, eps, pLon, pLat);

    // Find the angular distance between the perpendicular to the
    // planet's orbit and the planet's pole.  This is the obliquity.
    calcGreatArc (drem(ascnode - M_PI_2, TWO_PI), M_PI_2 - incl,
                  pLon, pLat, tmp, obliquity);
    if (flipped == 1)
        obliquity = M_PI - obliquity;

    return(obliquity);
}

void 
Planet::calcHeliocentric()
{
    if (body == PLUTO) return;

    mlong = drem(mlong, TWO_PI);
    double manom = mlong - perih - ascnode;
    double eccanom = kepler(ecc, manom);
    double nu = 2 * atan(sqrt((1 + ecc) / (1 - ecc)) 
                         * tan(eccanom/2));
    double lat = perih + nu;
    helio.lon = atan2(cos(incl) * sin(lat), cos(lat)) + ascnode;
    helio.lat = asin(sin(lat) * sin(incl));
    helio.dist = axis * (1 - ecc * cos(eccanom));
}

void 
Planet::calcGeocentric(const double sunlon, const double sundist)
{
    double N = helio.dist * cos(helio.lat) * sin(helio.lon - sunlon);
    double D = (helio.dist * cos(helio.lat) * cos(helio.lon - sunlon)
                + sundist);
    double dist_sinlat = helio.dist * sin(helio.lat);

    geo.dist = N*N + D*D + dist_sinlat * dist_sinlat;
    geo.dist = sqrt(geo.dist);
    geo.lon = atan2(N, D) + sunlon;
    geo.lat = asin(dist_sinlat) / geo.dist;
}

void
Planet::rotateEquator(const double in_lon, const double in_lat, 
                      double &out_lon, double &out_lat)
{
    double rot[3][3];
    rotateZYX(rot, 0, M_PI_2 - delta0, alpha0);

    Location l(in_lat, in_lon, 1);
    l.sphericalToRectangular();
    l.rotate(rot);
    l.rectangularToSpherical();
    out_lon = l.lon + M_PI_2;
    out_lat = l.lat;

    out_lon -= null_meridian;
    out_lon = drem(out_lon + M_PI, TWO_PI);
}

// Given the geo/heliocentric coordinates of a planet, find the
// subearth/solar point as appropriate.
coordinates
Planet::calcCoordinates(coordinates coords, const double eps)
{
    eclipticToEquatorial(coords.lon, coords.lat, eps, coords.alpha, 
                         coords.delta);

    double lon, lat;
    rotateEquator(coords.alpha, coords.delta, lon, lat);

    lat *= -1;
    lon = drem(lon + M_PI, TWO_PI);

    // position angle of the axis, from terrestrial north
    double axis_position = atan2(cos(delta0) * sin(alpha0 - coords.alpha),
                                 sin(delta0) * cos(coords.delta) 
                                 - (cos(delta0) * sin(coords.delta) 
                                    * cos(alpha0 - coords.alpha)));

    double light_time = 0;
    if (opts.position == EARTHSIDE && body != MOON)
        light_time = 5.7756E-3 * geo.dist * wdot * deg_to_rad;

    coordinates returnval = {lon, lat, axis_position, light_time, 0};
    return(returnval);  
}

/*
 * Calculate lat/lon of ecliptic north
 */
void
Planet::calcEcliptic(const double eps, double &lat, double &lon)
{
    double eq_lon, eq_lat; 

    eclipticToEquatorial(drem(ascnode - M_PI_2, TWO_PI), M_PI_2 - incl, 
                         eps, eq_lon, eq_lat);

    rotateEquator(eq_lon, eq_lat, lon, lat);
}

/*
 * Return direction of 'galactic north' on the planet's
 * coordinate system.
 */
void
Planet::calcGalactic (double &lat, double &lon)
{
    rotateEquator(GN_LON, GN_LAT, lon, lat);
}
