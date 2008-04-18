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

#ifndef lint
static const char rcsid[] = "";
#endif

#ifdef DEBUG
#include <cfloat>		// for max and min floating point values
#include <cstdio>		// for printf()
#endif

#include <algorithm>
#include <cmath>
#include <ctime>
// #include <getopt.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>		// for sleep()
#include <vector>

//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <GL/glut.h>
#include <GLUT/glut.h>

#include "astro.h"
#include "constants.h"
#include "eci.h"
#include "graphics.h"
#include "lla.h"
#include "tle.h"

#define HalfSecond (0.5/SecondsPerDay)
#define theC 2.997925e5                    /* Kilometers/Second    */

using namespace std;

vector<TLE>		sats;

void grabTLE(char *input);

int main(int argc, char *argv[])
{
  vector<TLE>::iterator		satIter;
  
  cout.setf(ios::fixed);
  cout.setf(ios::showpoint);
  cout.precision(40);

  if (argc == 2)
  {
    grabTLE(argv[1]);
  }
  else
  {
    //cin >> sat;
    //cout << "Usage: " << argv[0] << " datafile.tle" << endl;
    //exit(1);
    grabTLE("../tle/noaa.txt");
  }
  
  for (satIter = sats.begin(); satIter != sats.end(); satIter++)
  {
    if (satIter->getValid())
    {
      cout << "Valid TLE for " << satIter->getName() << endl;
    }
  }

#ifdef DEBUG
  TLE			sat;
  struct tm		sputnik;
  long double		timeJD = 0;
  ECI			eci;
  LLA			location;
  int			i = 0;
  
  for (i = 0; i <= 1440; i += 360)
  {
    eci = sat.position4(i);
    // eci = sat.sgp(i);
    cout << eci << endl;
  }

  sputnik.tm_year = 57;	// 1957
  sputnik.tm_mon = 9;	// October
  sputnik.tm_mday = 4;	// 4th
  sputnik.tm_wday = 5;	// Friday
  sputnik.tm_hour = 19;	// 19 hours
  sputnik.tm_min = 26;	// 26 minutes
  sputnik.tm_sec = 24;	// 24 seconds past midnight UTC

  cout << "Sputnik 1 was launched" << endl;
  cout << asctime(&sputnik);
  timeJD = julianDate(&sputnik);
  sputnik = gregorianDate(timeJD);
  cout << asctime(&sputnik);
  cout << timeJD << endl;
  cout << "2436116.31" << endl << endl;

  while (1)
  {
    time_t	currentTime;
    long double	timeJD = 0;

    currentTime = time(NULL);
    cout << asctime(gmtime(&currentTime));
    cout << sat << endl;
    // use gmtime
    timeJD = julianDate(gmtime(&currentTime));
    eci = sat.position4(timeJD);
    cout << eci << endl;
    location = eci;
    cout << location << endl;
    location.switchOutput();
    cout << location << endl;
    sleep(1);
  }
#else

  GLgraphics(argc, argv);
  
#endif
  
  return 0;
}


/*
 * grabTLE()
 *
 * Break file input away from main()
 *
 */
void grabTLE(char *input)
{
  ifstream		ifile;
  TLE			sat;

  // Open, open, open, open...
  ifile.open(input);
  // If the file opens...
  if (ifile)
  {
    // Priming Read
    ifile >> sat;
    // Read the rest of the file & insert into vector
    while (!ifile.eof())
    {
      sats.push_back(sat);
      ifile >> sat;
    }
    ifile.close();
  }
  else
  {
    ifile.close();
    cout << "Error opening input file!" << endl;
    exit(1);
  }
}
