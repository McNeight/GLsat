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

//#include <ctime>
#include "constants.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "astro.h"
#include "eci.h"
#include "lla.h"

using namespace std;

//
void LLA::convertDMS()
{
  long double	tempLat;
  long double	tempLon;
  long double	temp;

  // First, for latitude, skim off the degrees
  tempLat = modf(lat, &temp);
  latDeg = (int)temp;
  // Next, multiply the remainder by 60 and skim off the minutes
  tempLat = modf(fabs(tempLat * 60.0), &temp);
  latMin = (int)temp;
  // Last, multiply the remainder by 60 and put it into latSec
  latSec = 60.0 * tempLat;

  // Next, for longitude, skim off the degrees
  tempLon = modf(lon, &temp);
  lonDeg = (int)temp;
  // Next, multiply the remainder by 60 and skim off the minutes
  tempLon = modf(fabs(tempLon * 60.0), &temp);
  lonMin = (int)temp;
  // Last, multiply the remainder by 60 and put it into latSec
  lonSec = 60.0 * tempLon;

}

//
void LLA::convertDD()
{
  lat = latDeg + (latMin / 60.0) + (latSec / 3600.0);
  lon = lonDeg + (lonMin / 60.0) + (lonSec / 3600.0);
}

// Default Constructor
LLA::LLA()
{
  lat = 0;
  lon = 0;
  alt = 0;

  latDeg = 0;
  latMin = 0;
  latSec = 0;
  lonDeg = 0;
  lonMin = 0;
  lonSec = 0;

  JD = 0;

  DMS = false;
}

// Copy Constructor
LLA::LLA(const LLA &copy)
{
  name = copy.name;
  
  lat = copy.lat;
  latDeg = copy.latDeg;
  latMin = copy.latMin;
  latSec = copy.latSec;

  lon = copy.lon;
  lonDeg = copy.lonDeg;
  lonMin = copy.lonMin;
  lonSec = copy.lonSec;

  alt = copy.alt;

  JD = copy.JD;

  DMS = copy.DMS;
}

// Default Destructor
LLA::~LLA()
{
  // Here There Be Dragons!
}

// 5-parameter Constructor (for decimal degrees)
LLA::LLA(string nm, long double la, long double lo, long double al, long double t)
{
  name = nm;
  
  lat = la;
  lon = lo;
  alt = al;

  JD = t;

  DMS = false;
  convertDMS();
}

// 9-parameter Constructor (for degrees, minutes, and seconds)
LLA::LLA(string nm, int lad, int lam, long double las, int lod, int lom, long double los, long double al, long double t)
{
  name = nm;

  latDeg = lad;
  latMin = lam;
  latSec = las;

  lonDeg = lod;
  lonMin = lom;
  lonSec = los;

  alt = al;

  JD = t;

  DMS = true;
  convertDD();
}

//
void LLA::writeOutput(ostream &os) const
{
  os.precision(18);
  os.setf(ios::right);
  os.width(4);
  
  if (DMS)
  {
    os << name << endl;
    os << "Latitude   = " << setw(4) << latDeg << "\260";
    os << setw(3) << latMin << "\' ";
    os << latSec << "\"" << endl;
    os.setf(ios::right);
    os.width(4);
    os << "Longitude  = " << setw(4) << lonDeg << "\260";
    os << setw(3) << lonMin << "\' ";
    os << lonSec << "\"" << endl;
    os << "Altitude   = " << setw(26) << alt << " km" << endl;
    os << "Julian Day = " << setw(26) << JD << endl;
  }
  else
  {
    os << name << endl;
    os << "Latitude   = " << setw(26) << lat << endl;
    os << "Longitude  = " << setw(26) << lon << endl;
    os << "Altitude   = " << setw(26) << alt << " km" << endl;
    os << "Julian Day = " << setw(26) << JD << endl;
  }
}

//
void LLA::switchOutput()
{
  DMS = !DMS;
}

//
string LLA::stringLat()
{
  ostringstream		output;

  output.precision(18);
  output.setf(ios::right);
  output.width(4);
  
  if (DMS)
  {
    output << "Latitude   = " << setw(4) << latDeg << "\260";
    output << setw(3) << latMin << "\' " << latSec << "\"";
  }
  else
  {
    output << "Latitude   = " << setw(26) << lat;
  }
  
  return (output.str());
}

//
string LLA::stringLon()
{
  ostringstream		output;

  output.precision(18);
  output.setf(ios::right);
  output.width(4);
  
  if (DMS)
  {
    output << "Longitude  = " << setw(4) << lonDeg << "\260";
    output << setw(3) << lonMin << "\' " << lonSec << "\"";
  }
  else
  {
    output << "Longitude  = " << setw(26) << lon;
  }
  
  return (output.str());
}

//
string LLA::stringAlt()
{
  ostringstream		output;

  output.precision(18);
  output.setf(ios::right);
  output.width(4);
  
  output << "Altitude   = " << alt;
  
  return (output.str());
}


//
void LLA::operator = (const LLA &rhs)
{
  name = rhs.name;
  
  lat = rhs.lat;
  latDeg = rhs.latDeg;
  latMin = rhs.latMin;
  latSec = rhs.latSec;

  lon = rhs.lon;
  lonDeg = rhs.lonDeg;
  lonMin = rhs.lonMin;
  lonSec = rhs.lonSec;

  alt = rhs.alt;

  JD = rhs.JD;

  DMS = rhs.DMS;
}

bool LLA::operator == (const LLA &rhs)
{
  bool retVal = false;

  if (DMS)
  {
    if ((latDeg == rhs.latDeg) && (latMin == rhs.latMin) && (latSec == rhs.latSec))
    {
      if ((lonDeg == rhs.lonDeg) && (lonMin == rhs.lonMin) && (lonSec == rhs.lonSec))
      {
	if ((alt == rhs.alt) && (JD == rhs.JD) && (name == rhs.name))
	{
	  retVal = true;
	}
      }
    }
  }
  else
  {
    if ((lat == rhs.lat) && (lon == rhs.lon) && (alt == rhs.alt))
    {
      if (JD == rhs.JD)
      {
	retVal = true;
      }
    }
  }
  
  return (retVal);
}

//
void LLA::operator = (const ECI &rhs)
{
  long double	theta_mine = 0;
  long double	r_mine = 0;
  long double	e2 = 0;
  long double	phi = 0;
  long double	c = 0;

  name = rhs.name;
  
  theta_mine = atan2(rhs.Y, rhs.X);

  lon = fmod(theta_mine - ThetaG_JD(rhs.JD), (long double)(2.0 * PI));
  
  r_mine = sqrt((rhs.X * rhs.X) + (rhs.Y * rhs.Y));
  e2 = f * (2.0 - f);
  lat = atan2(rhs.Z, r_mine);
  while (fabs(lat - phi) > 1.0e-10)
  {
    phi = lat;
    c = 1.0 / sqrt(1 - e2 * pow(sin(phi), 2));
    lat = atan2(rhs.Z + e_R * c * e2 * sin(phi), r_mine);
  }
  
  alt = r_mine / cos(lat) - e_R * c;

  if (lon > PI)
  {
    lon -= (2.0 * PI);
  }
  else if (lon < -PI)
  {
    lon += (2.0 * PI);
  }

  lat *= rad2deg;
  lon *= rad2deg;

  JD = rhs.JD;

  DMS = false;
  convertDMS();
}

//
ostream& operator << (ostream &os, const LLA &lla)
{
  lla.writeOutput(os);
  return (os);
}
