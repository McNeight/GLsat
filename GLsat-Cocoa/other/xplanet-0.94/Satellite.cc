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

#include <cstring>
#include <iostream>
using namespace std;

#include <sys/time.h>

#include "util.h"

#include "Satellite.h"

using namespace sgp4sdp4;

Satellite::Satellite(char tle_lines[3][80])
{
    good = (Get_Next_Tle_Set(tle_lines, &tle) == 1);
    memcpy(tle_entry, tle_lines, 240);
}

Satellite::~Satellite()
{
}

int
Satellite::getID() const
{
    return(tle.catnr);
}

const char * const
Satellite::getName()
{
    return(tle.sat_name);
}

bool
Satellite::isGoodData() const
{
    return(good);
}

void
Satellite::loadTLE()
{
    Get_Next_Tle_Set(tle_entry, &tle);
    ClearFlag(ALL_FLAGS);
    select_ephemeris(&tle);  // SGP4 or SDP4 depending on period
}

void
Satellite::getSpherical(const time_t tv_sec, double &lat, double &lon,
                        double &alt)
{
    double jul_utc = julian(gmtime(&tv_sec)->tm_year + 1900,
                            gmtime(&tv_sec)->tm_mon + 1,
                            gmtime(&tv_sec)->tm_mday,
                            gmtime(&tv_sec)->tm_hour,
                            gmtime(&tv_sec)->tm_min,
                            gmtime(&tv_sec)->tm_sec);

    double year, day;
    /* Modification to support Y2K */
    /* Valid 1957 through 2056     */
    day = modf(tle.epoch*1E-3, &year)*1E3;
    if( year < 57 )
        year = year + 2000;
    else
        year = year + 1900;
    /* End modification */

    double jul_epoch = julian((int) year,1,0,0,0,0);
    jul_epoch += day;

    /* Zero vector for initializations */
    vector_t zero_vector = {0,0,0,0};
    vector_t pos = zero_vector;
    vector_t vel = zero_vector;

    double tsince = (jul_utc - jul_epoch) * xmnpda;

    if( isFlagSet(DEEP_SPACE_EPHEM_FLAG) )
        SDP4(tsince, &tle, &pos, &vel);
    else
        SGP4(tsince, &tle, &pos, &vel);
    
    /* Scale position and velocity vectors to km and km/sec */
    Convert_Sat_State( &pos, &vel );

    /* Satellite's predicted geodetic position */
    geodetic_t sat_geodetic;

    /** All angles in rads. Distance in km. Velocity in km/s **/
    /* Calculate satellite Lat North, Lon East and Alt. */
    Calculate_LatLonAlt(jul_utc, &pos, &sat_geodetic);

    lat = sat_geodetic.lat;
    lon = sat_geodetic.lon;
    alt = sat_geodetic.alt;

#if 0
    cout << "BEGIN getSpherical()\n";
    cout << getName() << endl;
    cout << "day = " << day << "\tyear = " << year << endl;
    cout << "tsince = " << tsince << endl;
    cout << pos.x << '\t' << pos.y << '\t' << pos.z << endl;
    cout << vel.x << '\t' << vel.y << '\t' << vel.z << endl;
    cout << lat/deg_to_rad << '\t' << lon/deg_to_rad << '\t' 
         << alt << endl;
    cout << "END getSpherical()\n\n";
#endif
}

void
Satellite::printTLE() const
{
    cout << "epoch = " << tle.epoch << endl;
    cout << "xndt2o = " << tle.xndt2o << endl;

    cout << "xndd6o = " << tle.xndd6o << endl;
    cout << "bstar = " << tle.bstar << endl;
    cout << "xincl = " << tle.xincl << endl;
    cout << "xnodeo = " << tle.xnodeo << endl;
    cout << "eo = " << tle.eo << endl;
    cout << "omegao = " << tle.omegao << endl;
    cout << "xmo = " << tle.xmo << endl;
    cout << "xno = " << tle.xno << endl;
    cout << "catnr = " << tle.catnr << endl;
    cout << "elset = " << tle.elset << endl;
    cout << "revnum = " << tle.revnum << endl;
    cout << "name = " << tle.sat_name << endl;
    cout << "idesg = " << tle.idesg << endl;
    cout << endl;
}

bool
Satellite::operator == (const Satellite &sat) const
{
    return(tle.catnr == sat.tle.catnr);
}
