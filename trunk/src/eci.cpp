/*
 *  GLsat: An OpenGL-based 3D satellite tracker
 *  Copyright (C) 2002-2008  Neil McNeight <mcneight@users.sf.net>
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

//#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "eci.h"

using namespace std;

// Default Constructor
ECI::ECI()
{
  name = "NOLO";
  
  X = 0;
  Y = 0;
  Z = 0;

  dotX = 0;
  dotY = 0;
  dotZ = 0;

  JD = 0;
}

// Copy Constructor
ECI::ECI(const ECI &copy)
{
  name = copy.name;
  
  X = copy.X;
  Y = copy.Y;
  Z = copy.Z;

  dotX = copy.dotX;
  dotY = copy.dotY;
  dotZ = copy.dotZ;

  JD = copy.JD;
}

// Default Destructor
ECI::~ECI()
{
  // Here There Be Dragons!
}

// 8-parameter Constructor
ECI::ECI(string nm, long double px, long double py, long double pz, long double vx, long double vy, long double vz, long double t)
{
  name = nm;
  
  X = px;
  Y = py;
  Z = pz;

  dotX = vx;
  dotY = vy;
  dotZ = vz;

  JD = t;
}

//
void ECI::writeOutput(ostream &os) const
{
  os.precision(18);
  os << name << endl;
  os << "X = " << setw(25) << X << "\tdotX = " << setw(21) << dotX << endl;
  os << "Y = " << setw(25) << Y << "\tdotY = " << setw(21) << dotY << endl;
  os << "Z = " << setw(25) << Z << "\tdotZ = " << setw(21) << dotZ << endl;
  os << "Julian Day = " << JD << endl;
}

//
string ECI::stringX()
{
  ostringstream		output;

  output.precision(18);
  output << "X = " << X;
  
  return (output.str());
}

//
string ECI::stringY()
{
  ostringstream		output;

  output.precision(18);
  output << "Y = " << Y;
  
  return (output.str());
}

//
string ECI::stringZ()
{
  ostringstream		output;

  output.precision(18);
  output << "Z = " << Z;
  
  return (output.str());
}

//
void ECI::operator = (const ECI &rhs)
{
  name = rhs.name;
  
  X = rhs.X;
  Y = rhs.Y;
  Z = rhs.Z;

  dotX = rhs.dotX;
  dotY = rhs.dotY;
  dotZ = rhs.dotZ;

  JD = rhs.JD;
}

//
bool ECI::operator == (const ECI &rhs)
{
  bool retVal = false;

  if ((X == rhs.X) && (Y == rhs.Y) && (Z == rhs.Z))
  {
    if ((dotX == rhs.dotX) && (dotY == rhs.dotY) && (dotZ == rhs.dotZ))
    {
      if ((JD == rhs.JD) && (name == rhs.name))
      {
	retVal = true;
      }
    }
  }

  return (retVal);
}

//
ostream& operator<<(ostream &os, const ECI &eci)
{
  eci.writeOutput(os);
  return (os);
}

