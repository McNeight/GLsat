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

#include "constants.h"
#include <ctime>
#include <iostream>
#include "astro.h"
#include "eci.h"

using namespace std;

// Appendix III
// Planets: Periodic Terms

// There are 6 periodic series for heliocentric longitude
// with a maximum of 64 terms per series
// and 3 quantities per term
static long double earth_L_coeff[6][64][3] = {
{ // L0
  {175347046.0, 0, 0},                           // 1
  {3341656.0, 4.6692568, 6283.0758500},          // 2
  {34894.0, 4.62610, 12566.15170},               // 3
  {3497.0, 2.7441, 5753.3849}                    // 4
},

{ // L1
  {}
},

{ // L2
  {52919.0, 0.0, 0.0},                           // 1
  {8720.0, 1.0721, 6283.0758},                   // 2
  {309.0, 0.867, 12566.152},                     // 3
  {27.0, 0.05, 3.52},                            // 4
  {16.0, 5.19, 26.30},                           // 5
  {16.0, 3.68, 155.42},                          // 6
  {10.0, 0.76, 18849.23},                        // 7
  {9.0, 2.06, 77713.77},                         // 8
  {7.0, 0.83, 775.52},                           // 9
  {5.0, 4.66, 1577.34},                          // 10
  {4.0, 1.03, 7.11},                             // 11
  {4.0, 3.44, 5573.14},                          // 12
  {3.0, 5.14, 796.30},                           // 13
  {3.0, 6.05, 5507.55},                          // 14
  {3.0, 1.19, 242.73},                           // 15
  {3.0, 6.12, 529.69},                           // 16
  {3.0, 0.31, 398.15},                           // 17
  {3.0, 2.28, 553.57},                           // 18
  {2.0, 4.38, 5223.69},                          // 19
  {2.0, 3.75, 0.98}                              // 20
},

{ // L3
  {289.0, 5.844, 6283.076},                      // 1
  {35.0, 0.0, 0.0},                              // 2
  {17.0, 5.49, 12566.15},                        // 3
  {3.0, 5.20, 155.42},                           // 4
  {1.0, 4.72, 3.52},                             // 5
  {1.0, 5.30, 18849.23},                         // 6
  {1.0, 5.97, 242.73}                            // 7
},
  
{ // L4
  {114.0, 3.142, 0.0},                           // 1
  {8.0, 4.13, 6283.08},                          // 2
  {1.0, 3.84, 12566.15}                          // 3
},

{ // L5
  {1.0, 3.14, 0.0}                               // 1
}};

//
static long double earth_B_coeff[2][5][3] = {
{ // B0
  {280.0, 3.199, 84334.662},                     // 1
  {102.0, 5.422, 5507.553},                      // 2
  {80.0, 3.88, 5223.69},                         // 3
  {44.0, 3.70, 2352.87},                         // 4
  {32.0, 4.00, 1577.34}                          // 5
},

{ // B1
  {9.0, 3.90, 5507.55},                          // 1
  {6.0, 1.73, 5223.69}                           // 2
}};

//
static long double earth_R_coeff[5][40][3] = {
{ // R0
  {}
},

{ // R1
},

{ // R2
  {4359.0, 5.7846, 6283.0758},                   // 1
  {124.0, 5.579, 12566.152},                     // 2
  {12.0, 3.14, 0.0},                             // 3
  {9.0, 3.63, 77713.77},                         // 4
  {6.0, 1.87, 5573.14},                          // 5
  {3.0, 5.47, 18849.23}                          // 6
},

{ // R3
  {145.0, 4.273, 6283.076},                      // 1
  {7.0, 3.92, 12566.15}                          // 2
},

{ // R4
  {4.0, 2.56, 6283.08}                           // 1
}};


/*
 *
 */
struct tm
gregorianDate(const long double jd)
{
  /* Algorithm taken from pages 26-27 of */
  /*  _Astronomical Formulae for Calculators_ by Jean Meeus */

  /* Modified for use with C++ */
  int		alpha;
  int		A;
  int		B;
  int		C;
  int		D;
  int		E;
  long double	F;
  struct tm	now;
  long double	tempJD;
  long double	Z;

  tempJD = jd + 0.5;
  F = modf(tempJD, &Z);
  if (Z < 2299161.0)
  {
    A = (int)Z;
  }
  else
  {
    alpha = (int)((Z - 1867216.25) / (36524.25));
    A = (int)Z + 1 + alpha - (int)(alpha / 4);
  }

  B = A + 1524;
  C = (int)((B - 122.1) / (365.25));
  D = (int)(365.25 * C);
  E = (int)((B - D) / (30.6001));
  
  now.tm_mday = B - D - (int)(30.6001 * E);

  if (E < 13.5)
  {
    now.tm_mon = E - 2;		// tm_mon range is 0 to 11 months
  }
  else
  {
    now.tm_mon = E - 14;
  }

  if (now.tm_mon > 1.5)
  {
    // More Readable Way
    // now.tm_year = C - 4716 - 1900;

    // More Efficient Way
    now.tm_year = C - 6616;
  }
  else
  {
    // More Readable Way
    // now.tm_year = C - 4715 - 1900;

    // More Efficient Way
    now.tm_year = C - 6615;
  }
  
  // Take fractional day, multiply by 24, and skim off the integer hour
  F = modf((F * 24.0), &tempJD);
  now.tm_hour = (int)tempJD;
  // Take fractional hour, multiply by 60, and skim off the integer minute
  F = modf((F * 60.0), &tempJD);
  now.tm_min = (int)tempJD;
  // Take fractional minute, multiply by 60, and skim off the integer second
  F = modf((F * 60.0), &tempJD);
  now.tm_sec = (int)tempJD;
  // Determine the day of the week: (Julian day + 1.5) MOD 7
  now.tm_wday = (int)fmod((jd + 1.5), (long double)7.0);

  // begin day-of-year hack...
  // Calculate the year to be used for current Julian year
  C = now.tm_year + 1900 - 1;
  // Take the "adjusted" Julian date (without the 'B' factor of the
  // Gregorian -> Julian algorithm), and subtract the Julian year from
  // the adjusted date. This gives the day of the current year.
  // Subtract 1 because the range for the tm struct is 0 to 365 days
  // from January 1st.

  // More Readable Way
  // now.tm_yday = (int)(A - (int)(C * 365.25) - (int)(30.6001 * 14) - 1720994.5) - 1;
  
  // More Efficient Way
  now.tm_yday = (int)(A - (int)(C * 365.25) - 1721423.5);
  
  // Since GMT/UTC doesn't have Daylight Savings Time, set this to 0
  now.tm_isdst = 0;
  
  return (now);
}


/*
 *
 * Assumes input is within the Gregorian calendar, which is any date
 * after October 15th, 1582
 * 
 */
long double
julianDate(const struct tm *date)
{
  /* Algorithm taken from pages 23-25 of */
  /* _Astronomical Formulae for Calculators_ by Jean Meeus */

  /* Modified for use with C++ */
  int		A = 0;
  int		B = 0;
  long double	day = 0;
  long double	JD = 0;
  int		month = 0;
  int		year = 0;

  /* 
   * Casting from a floating-point variable to an integer
   * results in an automatic truncation (not rounding).
   *
   */
  
  // tm_year contains number of years since 1900
  year = date->tm_year + 1900;
  // tm_mon contains month number from 0 to 11
  month = date->tm_mon + 1;
  // convert d:h:m:s to decimal day
  day = date->tm_mday + (date->tm_hour / 24.0);
  day += (date->tm_min / minutesDay) + (date->tm_sec / secondsDay);

  // Begin algorithm
  if (month <= 2)
  {
    year = year - 1;
    month = month + 12;
  }

  A = ((int)(year / 100));
  B = 2 - A + ((int)(A / 4));

  JD = ((int)(365.25 * year)) + ((int)(30.6001 * (month + 1))) + day + 1720994.5 + B;

  return (JD);
}

//
long double
julianDateFromNow()
{
  /* time_t is seconds since UNIX epoch */
  /* tm is a time struct */
  time_t	now;
  tm		*now2;
  tm		now3;
  
  now = time(NULL);
  cout << now << endl;
  now2 = localtime(&now);
  cout << asctime(now2) << endl;

  now3.tm_year = 102;
  now3.tm_mon = 2;
  now3.tm_mday = 22;
  now3.tm_hour = 0;
  now3.tm_min = 0;
  now3.tm_sec = 0;
  cout << asctime(&now3) << endl;

  return 0;
}

// Quick and Dirty...
// Sideral Time (?)
long double
ThetaG_JD(long double jd_L)
{
  long double UT_L, TU_L, GMST_L, temp8;

  // cout << "jd_L = " << jd_L << endl;
  UT_L = modf(jd_L + 0.5, &temp8);
  // cout << "UT_L = " << UT_L << endl;
  // UT_L = temp8;
  jd_L = jd_L - UT_L;
  TU_L = (jd_L - 2451545.0)/36525;
  GMST_L = 24110.54841 + TU_L * (8640184.812866 + TU_L * (0.093104 - TU_L * 6.2e-6));
  GMST_L = GMST_L + secondsDay * omega_E * UT_L;

  modf(GMST_L / secondsDay, &temp8);
  temp8 = GMST_L - temp8 * secondsDay;
  if (temp8 < 0.0)
  {
    temp8 += secondsDay;
  }
  return (2.0 * PI * temp8 / secondsDay);
}

//
ECI
sunPosition(const long double jd)
{
  long double B = 0;
  long double B_coeff[2] = {0};
  long double beta;
//  long double e;
  long double epsilon_0;
  ECI         retVal;
  long double L = 0;
  long double L_coeff[6] = {0};
//  long double M;
  long double R;
  long double R_coeff[5] = {0};
  long double sun;
  long double T;
  long double tau;
  long double tempA = 0, tempB = 0, tempC = 0;
//  long double v;
  long double X;
  long double Y;
  long double Z;

  // p. 163 (25.1)
  T = (jd - 2451545.0) / 36525.0;
  // p. 218 (32.1)
  tau = T / 10.0;
  
  // Chapter 32: Positions of the Planets
  // First, find the heliocentric coordinates of the Earth
  // Calculate heliocentric ecliptical longitude L
  for (int i = 0; i < 6; i++)
  {
    int k = 0;
    switch(i)
    {
      case 0:
	k = 64;
	break;
      case 1:
	k = 34;
	break;
      case 2:
	k = 20;
	break;
      case 3:
	k = 7;
	break;
      case 4:
	k = 3;
	break;
      case 5:
	k = 1;
    }
    for (int j = 0; j < k; j++)
    {
      tempA = earth_L_coeff[i][j][0];
      tempB = earth_L_coeff[i][j][1];
      tempC = earth_L_coeff[i][j][2];
      L_coeff[i] += tempA * cos(tempB + (tempC * tau));
    }
    L += L_coeff[i] * pow(tau, i);
  }
  // Finish Him!
  L /= pow((long double)10.0, 8);
  
  // Calculate heliocentric ecliptical latitude B
  for (int i = 0; i < 2; i++)
  {
    int k = 0;
    switch(i)
    {
      case 0:
	k = 5;
	break;
      case 1:
	k = 2;
    }
    for (int j = 0; j < k; j++)
    {
      tempA = earth_B_coeff[i][j][0];
      tempB = earth_B_coeff[i][j][1];
      tempC = earth_B_coeff[i][j][2];
      B_coeff[i] += tempA * cos(tempB + (tempC * tau));
    }
    B += B_coeff[i] * pow(tau, i);
  }
  // Finish Him!
  B /= pow((long double)10.0, 8);

  // Calculate radius vector R
  for (int i = 0; i < 2; i++)
  {
    int k = 0;
    switch(i)
    {
      case 0:
	k = 40;
	break;
      case 1:
	k = 10;
	break;
      case 2:
	k = 6;
	break;
      case 3:
	k = 2;
	break;
      case 4:
	k = 1;
    }
    for (int j = 0; j < k; j++)
    {
      tempA = earth_R_coeff[i][j][0];
      tempB = earth_R_coeff[i][j][1];
      tempC = earth_R_coeff[i][j][2];
      R_coeff[i] += tempA * cos(tempB + (tempC * tau));
    }
    R += R_coeff[i] * pow(tau, i);
  }
  // Finish Him!
  R /= pow((long double)10.0, 8);
  
  // Find the solar coordinates
  // p. 147 _Astronomical Algorithms_ (22.2)
  epsilon_0 = 23.4392911111 - (0.013004166667 * T) - (0.000000163889 * T * T)
           - (0.000000503611 * T * T * T);

  sun = L + PI;
  beta = -B;
  
  X = R * cos(beta) * cos(sun);
  Y = R * cos(beta) * sin(sun);
  Z = R * sin(beta);
  
  return retVal;
}
