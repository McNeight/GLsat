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
 * ECI - Earth Centered Inertial coordinates
 *
 */
#ifndef ECI_H
#define ECI_H

//#include <ctime>
#include <iostream>
#include <string>

using namespace std;

class ECI
{
  //private:
  public:
    string	name;
    
    long double X;
    long double Y;
    long double Z;

    long double dotX;
    long double dotY;
    long double dotZ;

    long double JD;
  private:
    //tm		now;
  public:
    ECI();
    ECI(const ECI &copy);
    ~ECI();
    ECI(string nm, long double px, long double py, long double pz, long double vx, long double vy, long double vz, long double t);
    // writes output to ostream os
    void writeOutput(ostream &os) const;

    string stringX();
    string stringY();
    string stringZ();

    void operator = (const ECI &rhs);
    bool operator == (const ECI &rhs);

    // void operator = (const LLA &rhs);
};

ostream& operator << (ostream &os, const ECI &eci);

#endif

