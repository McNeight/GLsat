/*
 *  GLsat: An OpenGL-based 3D satellite tracker
 *  Copyright (C) 2002  Neil McNeight <mcneight@umich.edu>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

/*
 * LLA - Latitude, Longitude and Altitude coordinates
 *
 */
#ifndef LLA_H
#define LLA_H

#include <iostream>
#include <string>
#include "eci.h"

using namespace std;

class LLA
{
  public:
    // Name of satellite being tracked
    string		name;
    
    // Latitude in decimal degrees
    long double		lat;
    // Latitude in degrees, minutes and seconds
    int			latDeg;
    int			latMin;
    long double		latSec;
    
    // Longitude in decimal degrees
    long double		lon;
    // Longitude in degrees, minutes and seconds
    int			lonDeg;
    int			lonMin;
    long double		lonSec;
    
    // Altitude in kilometers
    long double		alt;

    // Julian date of calculation
    long double		JD;

    // Use decimal degrees or degrees, minutes and seconds output
    bool		DMS;
  private:
    // Automagically convert decimal degrees to DMS and vice-versa
    // when using a constructor
    void convertDMS();
    void convertDD();
  public:
    LLA();
    LLA(const LLA &copy);
    ~LLA();
    LLA(string nm, long double la, long double lo, long double al, long double t);
    LLA(string nm, int lad, int lam, long double las, int lod, int lom, long double los, long double al, long double t);
    // Writes output to ostream os
    void writeOutput(ostream &os) const;
    // Switches output from decimal degrees to DMS and back
    void switchOutput();

    string stringLat();
    string stringLon();
    string stringAlt();

    void operator = (const LLA &rhs);
    bool operator == (const LLA &rhs);

    void operator = (const ECI &rhs);
};

ostream& operator << (ostream &os, const LLA &eci);

#endif

