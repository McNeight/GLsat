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

#include <cmath>
#include <ctime>
#include <iostream>
#include "astro.h"
#include "constants.h"

using namespace std;

/*
 *
 */
struct tm gregorianDate(const long double jd)
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
  F = modfl(tempJD, &Z);
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
  F = modfl((F * 24), &tempJD);
  now.tm_hour = (int)tempJD;
  // Take fractional hour, multiply by 60, and skim off the integer minute
  F = modfl((F * 60), &tempJD);
  now.tm_min = (int)tempJD;
  // Take fractional minute, multiply by 60, and skim off the integer second
  F = modfl((F * 60), &tempJD);
  now.tm_sec = (int)tempJD;
  // Determine the day of the week: (Julian day + 1.5) MOD 7
  now.tm_wday = (int)fmodl((jd + 1.5), 7.0);

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
long double julianDate(const struct tm *date)
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
long double julianDateFromNow()
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
long double ThetaG_JD(long double jd_L)
{
  long double UT_L, TU_L, GMST_L, temp8;

  // cout << "jd_L = " << jd_L << endl;
  UT_L = modfl(jd_L + 0.5, &temp8);
  // cout << "UT_L = " << UT_L << endl;
  // UT_L = temp8;
  jd_L = jd_L - UT_L;
  TU_L = (jd_L - 2451545.0)/36525;
  GMST_L = 24110.54841 + TU_L * (8640184.812866 + TU_L * (0.093104 - TU_L * 6.2e-6));
  GMST_L = GMST_L + secondsDay * omega_E * UT_L;

  modfl(GMST_L / secondsDay, &temp8);
  temp8 = GMST_L - temp8 * secondsDay;
  if (temp8 < 0.0)
  {
    temp8 += secondsDay;
  }
  return (2.0 * PI * temp8 / secondsDay);
}

