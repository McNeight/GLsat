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

#ifndef SATELLITE_H
#define SATELLITE_H

namespace sgp4sdp4
{
#include "libsgp4sdp4/sgp4sdp4.h"
}

class Satellite
{
 public:
    Satellite(char tle_line[3][80]);

    ~Satellite();
    bool isGoodData() const;
    int getID() const;
    const char * const getName();
    void getSpherical(const time_t tv_sec, double &lat, double &lon,
                      double &alt);
    void loadTLE();

    void printTLE() const;

    bool operator == (const Satellite &sat) const;
 
 private:
    bool good;     // if TLE is in the right format
    char tle_entry[3][80];
     
    sgp4sdp4::tle_t tle;
};

#endif
