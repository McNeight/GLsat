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
#include <cstdlib>		/* for atoi and strtod */
#include <iostream>
#include <string>
#include "astro.h"
#include "constants.h"
#include "eci.h"
#include "lla.h"
#include "tle.h"

using namespace std;

//
void TLE::checksum()
{
  int		i = 0;
  int		sum1 = 0;
  int		sum2 = 0;

  // If the string is long enough,
  // grab the checksum digits at position 69,
  if (firstLine.length() > 68)
  {
    checksum1 = firstLine.at(68) - 48;
  }

  if (secondLine.length() > 68)
  {
    checksum2 = secondLine.at(68) - 48;
  }

  // Process each string and sum up the digits
  // ASCII '0' == decimal 48, so use each char as
  // an int and then offset by 48. May not work
  // with other character sets.
  for (i = 0; i < 68; i++)
  {
    if (isdigit(firstLine.at(i)))
    {
      sum1 += (firstLine.at(i) - 48);
    }
    else if (firstLine.at(i) == '-')
    {
      sum1++;
    }
    
    if (isdigit(secondLine.at(i)))
    {
      sum2 += (secondLine.at(i) - 48);
    }
    else if (secondLine.at(i) == '-')
    {
      sum2++;
    }
  }

// Debugging output for checking both the
// accumulated sum as well as the checksum
// of each line
#ifdef DEBUG
  cout << endl << "Check the actual sum against the checksum" << endl;
  cout << sum1 << " " << checksum1 << endl;
  cout << sum2 << " " << checksum2 << endl << endl;
#endif

  if (((sum1 % 10) == checksum1) && ((sum2 % 10) == checksum2))
  {
    validChecksum = true;
  }
}

//
void TLE::processData()
{
  long double	temp = 0;

  // Assume data is valid
  validData = true;
  
  //
  satNumber = atoi(firstLine.substr(2, 5).c_str());
  classification = firstLine.at(7);

  //
  intLaunchYear = atoi(firstLine.substr(9, 2).c_str());
  intLaunchNumber = atoi(firstLine.substr(11, 3).c_str());
  intLaunchDesig = firstLine.substr(14, 3);

  //
  epochYear = atoi(firstLine.substr(18, 2).c_str());
  epochDay = strtod(firstLine.substr(20, 12).c_str(), (char **)NULL);

  //
  dotn_o = strtod(firstLine.substr(33, 10).c_str(), (char **)NULL);

  // Bring in the value, put a decimal point on the left hand side,
  // and then process where it should be shifted to
  ddotn_o = strtod(firstLine.substr(44, 6).c_str(), (char **)NULL);
  ddotn_o *= 1.0e-5;
  ddotn_o *= pow(10.0, atof(firstLine.substr(50, 2).c_str()));

  // Bring in the value, put a decimal point on the left hand side,
  // and then process where it should be shifted to
  bStar = strtod(firstLine.substr(53, 6).c_str(), (char **)NULL);
  bStar *= 1.0e-5;
  bStar *= pow(10.0, atof(firstLine.substr(59, 2).c_str()));
  
  //
  epheremisType = (firstLine.at(62) - 48);

  //
  elementSet = atoi(firstLine.substr(64, 4).c_str());
  
#ifdef DEBUG
  cout << endl << "Check the first line of input" << endl;
  cout << satNumber << endl;
  cout << firstLine.substr(20, 12) << endl;
  cout << epochDay << endl;
  cout << firstLine.substr(33, 10) << endl;
  cout << dotn_o << endl;
  cout << firstLine.substr(44, 8) << endl;
  cout << ddotn_o << endl;
  cout << firstLine.substr(53, 8) << endl;
  cout << bStar << endl;
#endif
  
  if (satNumber != (atoi(secondLine.substr(2, 5).c_str())))
  {
    validData = false;
  }
  
  //
  i_o = strtod(secondLine.substr(8, 8).c_str(), (char **)NULL);
  if ((i_o < 0) || (i_o > 180))
  {
    validData = false;
  }

  //
  Omega_o = strtod(secondLine.substr(17, 8).c_str(), (char **)NULL);
  if ((Omega_o < 0) || (Omega_o > 360))
  {
    validData = false;
  }

  //
  e_o = strtod(secondLine.substr(26, 7).c_str(), (char **)NULL);
  e_o *= pow(10.0, -7.0);
  
  //
  omega_o = strtod(secondLine.substr(34, 8).c_str(), (char **)NULL);
  if ((omega_o < 0) || (omega_o > 360))
  {
    validData = false;
  }

  //
  M_o = strtod(secondLine.substr(43, 8).c_str(), (char **)NULL);
  if ((M_o < 0) || (M_o > 360))
  {
    validData = false;
  }

  //
  n_o = strtod(secondLine.substr(52, 11).c_str(), (char **)NULL);

  //
  revolution = atoi(secondLine.substr(63, 5).c_str());

#ifdef DEBUG
  cout << endl << "Check the second line of input" << endl;
  cout << secondLine.substr(8, 8) << endl;
  cout << i_o << endl;
  cout << secondLine.substr(17, 8) << endl;
  cout << Omega_o << endl;
  cout << secondLine.substr(26, 7) << endl;
  cout << e_o << endl;
  cout << secondLine.substr(34, 8) << endl;
  cout << omega_o << endl;
  cout << secondLine.substr(43, 8) << endl;
  cout << M_o << endl;
  cout << secondLine.substr(52, 11) << endl;
  cout << n_o << endl;
  cout << secondLine.substr(63, 5) << endl;
  cout << revolution << endl;
#endif

  // convert degrees to radians
  i_o *= deg2rad;
  M_o *= deg2rad;
  omega_o *= deg2rad;
  Omega_o *= deg2rad;

  // convert year (Y2K compliant)
  if (epochYear < 57)
  {
    epochYear += 2000;
  }
  else
  {
    epochYear += 1900;
  }

  // convert other data
  temp = (2.0 * PI) / minutesDay / minutesDay;
  n_o = n_o * temp * minutesDay;
  dotn_o *= temp;
  ddotn_o = ddotn_o * temp / minutesDay;
  bStar /= a_E;

}

//
string TLE::getName()
{
  return (name);
}

//
void TLE::setupConstants()
{
  int			A;			// For computing Juilan Date of Epoch
  int			B;			// For computing Julian Date of Epoch
  long double		temp;			// Used to keep epochDay safe when using modf()
    
  // Initialize the "constants"
  // Data dependant variables which are constant
  // throughout all orbital models

  a_1 = pow((k_e / n_o), twoThirds);
  cosi_o = cos(i_o);
  sini_o = sin(i_o);
  theta2 = cosi_o * cosi_o;
  oneMinus5theta2 = 1.0 - 5.0 * theta2;
  oneMinustheta2 = 1.0 - theta2;
  seventheta2Minus1 = 7.0 * theta2 - 1.0;
  theta4 = pow(cosi_o, 4);
  x3thm1 = 3.0 * theta2 - 1.0;
  e_o2 = e_o * e_o;
  beta_o2 = 1.0 - e_o2;
  beta_o = sqrt(beta_o2);
  sinM_o = sin(M_o);
  a30overk2 = -J_3 * pow(a_E, 3) / k_2;
#ifdef MATH_OUT
    cout << endl << "$ a_{1} = " << a_1 << " $"<< endl;
    cout << endl << "$ \\cos(i_{o} = " << cosi_o << " $"<< endl;
    cout << endl << "$ \\theta^{2} = " << theta2 << " $"<< endl;
    cout << endl << "$ x3thm1 = " << x3thm1 << " $"<< endl;
    cout << endl << "$ \\e_{o}^{2} = " << e_o2 << " $"<< endl;
#endif
  // Convert Year to Julian Year (Jan 0.0 of epochYear)
  A = (int)((epochYear - 1) / 100);
  B = 2 - A + (int)(A / 4);
  JD = (int)(365.25 * (epochYear - 1)) + (int)(30.6001 * 14.0) + 1720994.5 + B;

  // Add number of days from epochDay
  JD += epochDay;

#ifdef DEBUG
  cout << "Julian Day: " << JD << endl;
  name = name + "(INVALID!)";
#endif
  
  // Calculate UT, T_U, GMST, DS50 and Theta_G
  UT = modf(epochDay,&temp);
  T_U = (JD - 2451545.0) / 36525.0;
  // GMST = 24110.54841 + (8640184.812866 * T_U) + (0.093104 * T_{U}^{2}) + (6.2E-6 * T_{U}^{3})
  GMST = 24110.54841 + T_U * (8640184.812866 + T_U * (0.093104 - T_U * 6.2E-6));
  GMST = fmod(GMST + secondsDay * omega_E * UT, secondsDay);
  DS50 = JD - 2433281.5 + UT;
  Theta_G = (2.0 * PI) * GMST / secondsDay;
  Theta_G = fmod(6.3003880987 * DS50 + 1.72944494, (2.0 * PI));

  // Check if deep() routines are needed
  // Borrowing delta_1, a_o, delta_o and n_odp
  temp = (1.5 * k_2 * x3thm1) / (beta_o * beta_o2);
  delta_1 = temp / (a_1 * a_1);
  a_o = a_1 * ((((-134.0 / 81.0) * delta_1 - 1.0) * delta_1 - oneThird) * delta_1 + 1);
  delta_o = temp / (a_o * a_o);
  n_odp = n_o / (1 + delta_o);
  if (((2.0 * PI) / n_odp) >= 225.0)
  {
    deep = true;
  }
  temp = 0;
  delta_1 = 0;
  a_o = 0;
  delta_o = 0;
  n_odp = 0;
  
#ifdef DEBUG
  // For testing...
  cout.precision(30);
  cout << "Testing precision of k_2 and k_4 calculated versus constant values" << endl;
  cout << k_2 << " " << k_4 << endl;
  cout << 5.413079E-4 << "    " << 6.209887E-7 << endl;
#endif
  
}

// Redundant, but included for now
long double TLE::FMod2p(long double x)
{
  int i;
  long double ret_val;

  ret_val = x;
  i = (int)(ret_val / (2.0 * PI));
  ret_val -= i * (2.0 * PI);
  if (ret_val < 0) ret_val += (2.0 * PI);

  return (ret_val);
}

// Redundant, but included for now
long double TLE::ArcTan(long double sinx, long double cosx)
{
  long double retVal = 0;

  if (cosx == 0)
  {
    if (sinx > 0)
    {
      retVal = PI_2;
    }
    else
    {
      retVal = (3.0 * PI_2);
    }
  }
  else
  {
    if (cosx > 0)
    {
      if (sinx > 0)
      {
        retVal = atan(sinx/cosx);
      }
      else
      {
        retVal = (2.0 * PI) + atan(sinx/cosx);
      }
    }
    else
    {
      retVal = (PI + atan(sinx/cosx));
    }
  }
  return retVal;
}


//
TLE::TLE()
{
  checksum1 = -1;
  checksum2 = -1;

  deep = false;
  sgpInit = false;
  sgp4Init = false;
  sdp4Init = false;
  sgp8Init = false;
  sdp8Init = false;
  
  validChecksum = false;
  validData = false;
}

// Snags a two-line element
void TLE::getInput(istream &is)
{
  // 
  getline(is, name);
  // For removing control characters at the end of the line
  if (iscntrl(*(name.end() - 1)))
  {
    name.erase(name.end() - 1);
  }
  // Get the first line of data
  getline(is, firstLine);
  // Get the second line of data
  getline(is, secondLine);
  // Process the checksum
  checksum();
  // Filter through the data and extract the numbers
  processData();
  // Set up constants used by all orbital models
  setupConstants();
}

//
bool TLE::getValid()
{
  return (validChecksum && validData);
}

//
bool TLE::getOrbit(long double &semiMaj, long double &semiMin, long double &eccen, long double &incl, long double &perigee, long double &ra)
{
  semiMaj = a_odp;
  semiMin = beta_o;
  eccen = e_o;
  incl = i_o;
  ra = Omega_o;
  perigee = omega_o;

  return (getValid());
}

//
void TLE::writeOutput(ostream &os) const
{
  cout << name << endl;
  cout << firstLine << endl;
  cout << secondLine << endl;
}

/*
 * sgp()
 *
 * SGP (Simplified General Perturbations) Model
 * 
 */
ECI TLE::sgp(const long double timeJD)
{
  // Formula Variables
  long double		a = 0;
  long double		a_xnsl = 0;
  long double		a_ynsl = 0;
  long double		dotr = 0;
  long double		e = 0;
  long double		ecosE = 0;
  long double		esinE = 0;
  long double		e_L2 = 0;
  long double		Eomega_1 = 0;
  long double		cosEomega_1 = 0;
  long double		sinEomega_1 = 0;
  long double		i_k = 0;
  long double		cosi_k = 0;
  long double		sini_k = 0;
  long double		L = 0;
  long double		L_s = 0;
  long double		omega_s_o = 0;
  long double		Omega_s_o = 0;
  long double		Omega_k = 0;
  long double		cosOmega_k = 0;
  long double		sinOmega_k = 0;
  long double		p = 0;
  long double		p_L = 0;
  long double		p_L2 = 0;
  long double		r = 0;
  long double		rdotv = 0;
  long double		r_k = 0;
  long double		tt_0 = 0;
  long double		u = 0;
  long double		cosu = 0;
  long double		cos2u = 0;
  long double		sinu = 0;
  long double		sin2u = 0;
  long double		u_k = 0;
  long double		cosu_k = 0;
  long double		sinu_k = 0;
  long double		U = 0;
  long double		xmx = 0;
  long double		xmy = 0;

  // Position Variables
  long double		U_x = 0;
  long double		U_y = 0;
  long double		U_z = 0;
  long double		V_x = 0;
  long double		V_y = 0;
  long double		V_z = 0;
  long double		X = 0;
  long double		dotX = 0;
  long double		Y = 0;
  long double		dotY = 0;
  long double		Z = 0;
  long double		dotZ = 0;

  ECI			*info;

  // Formula Temporary Variables
  long double		temp = 0;
  long double		temp2 = 0;

  // Iterators
  int			i = 0;

#ifdef DEBUG
  cout << endl << "Beginning of sgp()" << endl << endl;
  // Run it with debug input
  tt_0 = timeJD;
#else
  // Setup (t - t_0)
  tt_0 = (timeJD - JD) * minutesDay;
#endif
  
  if (!sgpInit)
  {
    sgpInit = true;
    sgp4Init = false;
    sdp4Init = false;
    sgp8Init = false;
    sdp8Init = false;

    C_1 = k_2 * 1.5;
    C_2 = k_2 / 4.0;
    C_3 = k_2 / 2.0;
    C_4 = J_3 * (pow(a_E, 3)) / (k_2 * 4.0);

    D_1 = C_3 * sini_o * sini_o;
    D_2 = C_2 * seventheta2Minus1;
    D_3 = C_1 * cosi_o;
    D_4 = D_3 * sini_o;

    // here is where a_1 was moved from

    // \[\delta_1=\dfrac34J_2\dfrac{a_E{}^2}{a_1{}^2}\dfrac{(3\cos^2i_o-1)}
    delta_1 = (C_1 / (a_1 * a_1)) * ((3.0 * theta2) - 1.0) / (pow(beta_o2, (long double)1.5));
#ifdef MATH_OUT
    cout << endl << "$ \\delta_{1} = " << delta_1 << " $"<< endl;
#endif
    
    a_o = a_1 * ((((-134.0 / 81.0) * delta_1 - 1.0) * delta_1 - oneThird) * delta_1 + 1);
#ifdef MATH_OUT
    cout << endl << "$ a_{o} = " << a_o << " $" << endl;
#endif
    
    p_o = a_o * beta_o2;
#ifdef MATH_OUT
    cout << endl << "$ p_{o} = " << p_o << " $" << endl;
#endif

    q_o = a_o * (1.0 - e_o);
#ifdef MATH_OUT
    cout << endl << "$ q_{o} = " << q_o << " $" << endl;
#endif

    L_o = M_o + omega_o + Omega_o;
#ifdef MATH_OUT
    cout << endl << "$ L_{o} = " << L_o << " $" << endl;
#endif

    dOmegadt = -2.0 * C_1 * cosi_o * (n_o / (p_o * p_o));
#ifdef MATH_OUT
    cout << endl << "$ \\frac{d\\Omega}{dt} = \\dot{\\Omega} = " << dOmegadt << " $" << endl;
#endif

    //domegadt = (0.75)*J_2*((a_E * a_E)/(p_o * p_o))*n_o*(5.0*(cosi_o * cosi_o)-1.0);
    domegadt = C_1 * (n_o / (p_o * p_o)) * (5.0 * theta2 - 1.0);
#ifdef MATH_OUT
    cout << endl << "$ \\frac{d\\omega}{dt} = \\dot{\\omega} = " << domegadt << " $" << endl;
#endif

    C_5 = C_4 * 0.5 * sini_o * (cosi_o * 5.0 + 3.0) / (cosi_o + 1.0);
#ifdef MATH_OUT
    cout << endl << "$ C_{5} = " << C_5 << " $" << endl;
#endif

    C_6 = C_4 * sini_o;
#ifdef MATH_OUT
    cout << endl << "$ C_{6} = " << C_6 << " $" << endl;
#endif
  }
  
  /* Update for secular gravity and atmospheric drag */
  // \[a=a_o\left\{\dfrac{n_o}{n_o+2\left(\dfrac{\dn_o}2\right)
  // \dt+3\left(\dfrac{\ddn_o}6\right)\dt^2}\right\}^{\frac23}\]
  a = (((3.0 * ddotn_o) * tt_0 + (2.0 * dotn_o)) * tt_0 + n_o);
  a = a_o * pow((n_o / a), twoThirds);
#ifdef MATH_OUT
  cout << endl << "$ a = " << a << " $" << endl;
#endif

  //temp = n_o + (2.0 * (dotn_o / 2.0) * tt_0) + (3.0 * (ddotn_o / 6.0) * (tt_0 * tt_0));
  //temp = n_o / temp;
  //temp = pow(temp, twoThirds);
  //a = a_o * temp;
  
  if (a > q_o)
  {
    e = 1.0 - (q_o / a);
  }
  else
  {
    e = 1.0e-6;
  }
#ifdef MATH_OUT
  cout << endl << "$ e = " << e << " $" << endl;
#endif

  p = a * (1.0 - (e * e));
#ifdef MATH_OUT
  cout << endl << "$ p = " << p << " $" << endl;
#endif

  Omega_s_o = Omega_o + (dOmegadt * tt_0);
#ifdef MATH_OUT
  cout << endl << "$ \\Omega_{s_{o}} = " << Omega_s_o << " $" << endl;
#endif

  omega_s_o = omega_o + (domegadt * tt_0);
#ifdef MATH_OUT
  cout << endl << "$ \\omega_{s_{o}} = " << omega_s_o << " $" << endl;
#endif

  // fmod(L_s, (2.0 * PI)) replaces FMOD2P function in original source
  L_s = L_o + ((n_o + domegadt + dOmegadt) * tt_0) + ((dotn_o) * (tt_0 * tt_0)) + ((ddotn_o) * pow(tt_0, 3));
#ifdef NEW_FUNCTIONS
  L_s = fmod(L_s, (2.0 * PI));
#else
  L_s = FMod2p(L_s);
#endif
#ifdef MATH_OUT
  cout << endl << "$ L_{s} = " << L_s << " $" << endl;
#endif

  a_xnsl = e * cos(omega_s_o);
#ifdef MATH_OUT
  cout << endl << "$ a_xNSL = " << a_xnsl << " $" << endl;
#endif

  a_ynsl = (e * sin(omega_s_o) - (C_6 / p));
#ifdef MATH_OUT
  cout << endl << "$ a_yNSL = " << a_ynsl << " $" << endl;
#endif

  // fmod(L, (2.0 * PI)) replaces FMOD2P function in original source
  L = L_s - (C_5 / p) * a_xnsl;
#ifdef NEW_FUNCTIONS
  L = fmod(L, (2.0 * PI));
#else
  L = FMod2p(L);
#endif
  
  /* Solve Kepler's Equation */
  // fmod(U, (2.0 * PI)) replaces FMOD2P function in original source
  U = L - Omega_s_o;
#ifdef NEW_FUNCTIONS
  U = fmod(U, (2.0 * PI));
#else
  U = FMod2p(U);
#endif

  /* Iterative solution to deriving (E + \omega) */
  Eomega_1 = U;
  // while (fabs(temp2) > 1.0e-6)
  for (i = 0; i < 10; i++)
  {
    sinEomega_1 = sin(Eomega_1);
    cosEomega_1 = cos(Eomega_1);
    if (fabs(temp2) > 1.0e-6)
    {
      break;
    }
    temp2 = 1.0 - (cosEomega_1 * a_xnsl) - (sinEomega_1 * a_ynsl);
    temp2 = (U - (a_ynsl * cosEomega_1) + (a_xnsl * sinEomega_1) - Eomega_1)/temp2;
    temp = fabs(temp2);
    if (temp > 1.0)
    {
      temp2 = temp / temp2;
    }
    Eomega_1 += temp2;
  }
  
  /* Short period preliminary quantities */
  ecosE = (a_xnsl * cosEomega_1) + (a_ynsl * sinEomega_1);

  esinE = (a_xnsl * sinEomega_1) - (a_ynsl * cosEomega_1);
  
  e_L2 = (a_xnsl * a_xnsl) + (a_ynsl * a_ynsl);

  p_L = a * (1.0 - e_L2);

  p_L2 = p_L * p_L;

  r = a * (1.0 - ecosE);
#ifdef MATH_OUT
  cout << endl << "$ r = " << r << " $" << endl;
#endif

  dotr = k_e * (sqrt(a) / r) * esinE;
#ifdef MATH_OUT
  cout << endl << "$ \\dot{r} = " << dotr << " $" << endl;
#endif

  rdotv = k_e * (sqrt(p_L) / r);
#ifdef MATH_OUT
  cout << endl << "$ r \\dot{v} = " << rdotv << " $" << endl;
#endif

  temp = esinE / (1.0 + sqrt(1.0 - e_L2));

  sinu = (a / r) * (sinEomega_1 - a_ynsl - (a_xnsl * temp));
#ifdef MATH_OUT
  cout << endl << "$ \\sin(u) = " << sinu << " $" << endl;
#endif

  cosu = (a / r) * (cosEomega_1 - a_xnsl + (a_ynsl * temp));
#ifdef MATH_OUT
  cout << endl << "$ \\cos(u) = " << cosu << " $" << endl;
#endif

  // atan2(sinu, cosu) replaces ACTAN function in original source
#ifdef NEW_FUNCTIONS
  u = atan2(sinu, cosu);
#else
  u = ArcTan(sinu, cosu);
#endif

#ifdef MATH_OUT
  cout << endl << "$ u = " << u << " $" << endl;
#endif

  /* Update for short periodics */
  sin2u = (cosu + cosu) * sinu;
  cos2u = 1.0 - (2.0 * sinu * sinu);

  r_k = r + (D_1 / p_L) * cos2u;
  u_k = u - (D_2 / p_L2) * sin2u;
  Omega_k = Omega_s_o + (D_3 / p_L2 * sin2u);
  i_k = i_o + (D_4 / p_L2 * cos2u);

  /* Orientation vectors */
  sinu_k = sin(u_k);
  cosu_k = cos(u_k);

  sinOmega_k = sin(Omega_k);
  cosOmega_k = cos(Omega_k);

  sini_k = sin(i_k);
  cosi_k = cos(i_k);

  xmx = -sinOmega_k * cosi_k;
  xmy =  cosOmega_k * cosi_k;
  
  U_x = xmx * sinu_k + cosOmega_k * cosu_k;
  U_y = xmy * sinu_k + sinOmega_k * cosu_k;
  U_z = sini_k * sinu_k;

  V_x = xmx * cosu_k - cosOmega_k * sinu_k;
  V_y = xmy * cosu_k - sinOmega_k * sinu_k;
  V_z = sini_k * cosu_k;

  X = r_k * U_x;
  Y = r_k * U_y;
  Z = r_k * U_z;

  dotX = rdotv * V_x + dotr * U_x;
  dotY = rdotv * V_y + dotr * U_y;
  dotZ = rdotv * V_z + dotr * U_z;

  X *= (e_R / a_E);
  Y *= (e_R / a_E);
  Z *= (e_R / a_E);

  dotX *= (e_R / 60.0); // * (minutesDay / secondsDay)
  dotY *= (e_R / 60.0); // * (minutesDay / secondsDay)
  dotZ *= (e_R / 60.0); // * (minutesDay / secondsDay)

  info = new ECI(name, X, Y, Z, dotX, dotY, dotZ, timeJD);

  return *info;
}

/*
 * sgp4()
 *
 * SGP4 (Simplified General Perturbations) Model Number 4
 * 
 */
ECI TLE::sgp4(const long double timeJD)
{
  // Formula Variables
  long double		a = 0;
  long double		a_xN = 0;
  long double		a_yN = 0;
  long double		a_yNL = 0;
  long double		beta = 0;
  long double		betal = 0;
  long double		cosepw = 0;
  long double		deltaomega = 0;
  long double		deltaM = 0;
  long double		dotr = 0;
  long double		dotr_k = 0;
  long double		e = 0;
  long double		ecosE = 0;
  long double		esinE = 0;
  long double		e_L2 = 0;
  long double		epw = 0;
  long double		i_k = 0;
  long double		cosi_k = 0;
  long double		sini_k = 0;
  long double		L = 0;
  long double		L_L = 0;
  long double		L_T = 0;
  long double		M_DF = 0;
  long double		M_p = 0;
  long double		n = 0;
  long double		omega = 0;
  long double		omega_DF = 0;
  long double		Omega = 0;
  long double		Omega_DF = 0;
  long double		Omega_k = 0;
  long double		cosOmega_k = 0;
  long double		sinOmega_k = 0;
  long double		p_L = 0;
  long double		r = 0;
  long double		rdotf = 0;
  long double		rdotf_k = 0;
  long double		r_k = 0;  
  long double		sinepw = 0;
  long double		tt_0 = 0;
  long double		tt_02 = 0;
  long double		tt_03 = 0;
  long double		tt_04 = 0;
  long double		u = 0;
  long double		cosu = 0;
  long double		cos2u = 0;
  long double		sinu = 0;
  long double		sin2u = 0;
  long double		u_k = 0;
  long double		cosu_k = 0;
  long double		sinu_k = 0;
  long double		U = 0;
  long double		xmx = 0;
  long double		xmy = 0;

  // Position Variables
  long double		U_x = 0;
  long double		U_y = 0;
  long double		U_z = 0;
  long double		V_x = 0;
  long double		V_y = 0;
  long double		V_z = 0;
  long double		X = 0;
  long double		dotX = 0;
  long double		Y = 0;
  long double		dotY = 0;
  long double		Z = 0;
  long double		dotZ = 0;

  ECI			*info;

  // Formula Temporary Variables
  long double		temp1 = 0;
  long double		temp2 = 0;
  long double		temp3 = 0;
  long double		temp4 = 0;
  long double		temp5 = 0;
  long double		temp6 = 0;
  long double		tempa = 0;
  long double		tempe = 0;
  long double		templ = 0;

  
  // Iterators
  //int			i = 0;
  
#ifdef DEBUG
  cout << endl << "Beginning of sgp4()" << endl << endl;
  // Run it with debug input
  tt_0 = timeJD;
#else
  // Setup (t - t_0)
  tt_0 = (timeJD - JD) * minutesDay;
#endif

  if (!sgp4Init)
  {
    sgpInit = false;
    sgp4Init = true;
    sdp4Init = false;
    sgp8Init = false;
    sdp8Init = false;

    // here is where a_1 was moved from
    // here is where cosi_o was moved from
    // here is where theta2 was moved from

    temp1 = (1.5 * k_2 * x3thm1) / (beta_o * beta_o2);
    
    delta_1 = temp1 / (a_1 * a_1);
#ifdef MATH_OUT
    cout << endl << "$ \\delta_{1} = " << delta_1 << " $"<< endl;
#endif

    a_o = a_1 * ((((-134.0 / 81.0) * delta_1 - 1.0) * delta_1 - oneThird) * delta_1 + 1);
#ifdef MATH_OUT
    cout << endl << "$ a_{o} = " << a_o << " $" << endl;
#endif

    delta_o = temp1 / (a_o * a_o);
#ifdef MATH_OUT
    cout << endl << "$ \\delta_{o} = " << delta_o << " $"<< endl;
#endif

    n_odp = n_o / (1 + delta_o);
#ifdef MATH_OUT
    cout << endl << "$ n_{o}^{''} = " << n_odp << " $"<< endl;
#endif

    a_odp = a_o / (1 - delta_o);
#ifdef MATH_OUT
    cout << endl << "$ a_{o}^{''} = " << a_odp << " $"<< endl;
#endif

    /* 
     * For epoch perigee less than 220 kilometers, the "simple" flag is set
     * and the equations are truncated to linear variation in sqrt a
     * and quadratic variation in mean anomaly.  Also, the C_3 term,
     * the delta omega term, and the delta m term are dropped.
     * 
     */
    if ((a_odp * (1 - e_o) / a_E) < (220.0 / e_R + a_E))
    {
      simple = true;
    }
    else
    {
      simple = false;
    }

    // For perigee between 98km and 156 km, the values of s and qoms2t are altered
    s4 = s;
    q_o = 120.0;					// constant in SGP4 model
    //q_oms2t = 1.88027916e-9;				// constant version
    q_oms2t = pow(((q_o - s_o) * (a_E / e_R)), 4);	// calculated version
#ifdef MATH_OUT
    cout << endl << "$ (q_o = " << q_oms2t << " $" << endl;
#endif
    q_oms24 = q_oms2t;
    perigee = (a_odp * (1 - e_o) - a_E) * e_R;
    if (perigee < 156)
    {
      if (perigee <= 98)
      {
	s4 = 20.0;
      }
      else
      {
	s4 = perigee - s_o;
      }
      q_oms24 = pow((q_o - s4) * a_E / e_R, 4);
      s4 = s4 / e_R + a_E;
    }

    pinvsq = 1.0 / (a_odp * a_odp * beta_o2 * beta_o2);

    xi = 1.0 / (a_odp - s4);
#ifdef MATH_OUT
    cout << endl << "$ \\xi = " << xi << " $" << endl;
#endif

    eta = a_odp * e_o * xi;
#ifdef MATH_OUT
    cout << endl << "$ eta = " << eta << " $" << endl;
#endif

    eta2 = eta * eta;
#ifdef MATH_OUT
    cout << endl << "$ eta^{2} = " << eta2 << " $" << endl;
#endif

    eeta = e_o * eta;
#ifdef MATH_OUT
    cout << endl << "$ e_{o} \\times eta = " << eeta << " $" << endl;
#endif

    psi2 = fabs(1.0 - eta2);
#ifdef MATH_OUT
    cout << endl << "$ \\psi^{2} = " << psi2 << " $" << endl;
#endif

    coef = q_oms24 * pow(xi, 4);
#ifdef MATH_OUT
    cout << endl << "$ coef = " << coef << " $" << endl;
#endif
    
    coef1 = coef / pow(psi2, (long double)3.5);
#ifdef MATH_OUT
    cout << endl << "$ coef1 = " << coef1 << " $" << endl;
#endif

    C_2 = coef1 * n_odp * (a_odp * (1.0 + 1.5 * eta2 + eeta * (4.0 + eta2)) +
	  0.75 * k_2 * xi / psi2 * x3thm1 * (8.0 + 3.0 * eta2 * (8.0 + eta2)));
    
    C_1 = bStar * C_2;

    // here is where sini_o was moved from
    // here is where a30overk2 was moved from

    C_3 = coef * xi * a30overk2 * n_odp * a_E * sini_o / e_o;

    // here is where oneMinustheta2 was moved from
    
    // Fasten your seatbelts, it's going to be a bumpy night.
    C_4 = 2.0 * n_odp * coef1 * a_odp * beta_o2 * (eta * (2.0 + 0.5 * eta2) +
	  e_o * (0.5 + 2.0 * eta2) - 2.0 * k_2 * xi / (a_odp * psi2) * (-3.0
	  * x3thm1 * (1.0 - 2.0 * eeta + eta2 * (1.5 - 0.5 * eeta)) + 0.75 *
	  oneMinustheta2 * (2.0 * eta2 - eeta * (1.0 + eta2)) * cos(2.0 * omega_o)));

    C_5 = 2.0 * coef1 * a_odp * beta_o2 * (1.0 + (2.75 * (eta2 + eeta)) + (eeta * eta2));

#ifdef MATH_OUT
    cout << endl << "$ \\C_{1} = " << C_1 << " $" << endl;
    cout << endl << "$ \\C_{2} = " << C_2 << " $" << endl;
    cout << endl << "$ \\C_{3} = " << C_3 << " $" << endl;
    cout << endl << "$ \\C_{4} = " << C_4 << " $" << endl;
    cout << endl << "$ \\C_{5} = " << C_5 << " $" << endl;
#endif
    
    // here is where theta4 was moved from

    temp1 = 3.0 * k_2 * pinvsq * n_odp;
    temp2 = temp1 * k_2 * pinvsq;
    temp3 = 1.25 * k_4 * pinvsq * pinvsq * n_odp;
    xmdot = n_odp + 0.5 * temp1 * beta_o * x3thm1 + 0.0625 * temp2 * beta_o * (13.0 - 78.0 * theta2 + 137 * theta4);
    // here is where oneMinus5theta2 was moved from
    omgdot = -0.5 * temp1 * oneMinus5theta2 + 0.0625 * temp2 * (7.0 - 114.0 * theta2 + 395.0 * theta4) + temp3 * (3.0 - 36.0 * theta2 + 49.0 * theta4);
    xhdot1 = -temp1 * cosi_o;
    xnodot = xhdot1 + (0.5 * temp2 * (4.0 - 19.0 * theta2) + 2.0 * temp3 * (3.0 - 7.0 * theta2)) * cosi_o;
    omgcof = bStar * C_3 * cos(omega_o);
    xmcof = -twoThirds * coef * bStar * a_E / eeta;
    xnodcf = 3.5 * beta_o2 * xhdot1 * C_1;
    t2cof = 1.5 * C_1;
    xlcof = 0.125 * a30overk2 * sini_o * (3.0 + 5.0 * cosi_o)/(1.0 + cosi_o);
    aycof = 0.25 * a30overk2 * sini_o;
    delmo = pow((1.0 + eta * cos(M_o)),3);
    // here is where sinM_o was moved from
    // here is where seventheta2Minus1 was moved from
    if (!simple)
    {
      C_12 = C_1 * C_1;
      D_2 = 4.0 * a_odp * xi * C_12;
      temp1 = D_2 * xi * C_1 / 3.0;
      D_3 = (17.0 * a_odp + s4) * temp1;
      D_4 = 0.5 * temp1 * a_odp * xi * (221.0 * a_odp + 31.0 * s4) * C_1;
      t3cof = D_2 + (2.0 * C_12);
      t4cof = 0.25 * ((3.0 * D_3) + (C_1 * (12.0 * D_2 + 10.0 * C_12)));
      t5cof = 0.2 * ((3.0 * D_4) + (12.0 * C_1 * D_3) + (6.0 * D_2 * D_2) + (15.0 * C_12 * (2.0 * D_2 + C_12)));
    }
  }
  // Holy smack, that's a lot of setting up...

  /* Update for secular gravity and atmospheric drag. */
  M_DF = M_o + (xmdot * tt_0);
  omega_DF = omega_o + (omgdot * tt_0);
  Omega_DF = Omega_o + (xnodot * tt_0);

  omega = omega_DF;
  M_p = M_DF;
  
  tt_02 = tt_0 * tt_0;
  
  Omega = Omega_DF + (xnodcf * tt_02);

  tempa = 1.0 - (C_1 * tt_0);
  tempe = bStar * C_4 * tt_0;
  templ = t2cof * tt_02;

  if (!simple)
  {
    deltaomega = omgcof * tt_0;
    deltaM = xmcof * (pow(1.0 + eta * cos(M_DF),3) - delmo);
    temp1 = deltaomega + deltaM;
    M_p = M_DF + temp1;
    omega = omega_DF - temp1;
    tt_03 = tt_02 * tt_0;
    tt_04 = tt_03 * tt_0;
    tempa = tempa - (D_2 * tt_02) - (D_3 * tt_03) - (D_4 * tt_04);
    tempe = tempe + (bStar * C_5 * (sin(M_p) - sinM_o));
    templ = templ + (t3cof * tt_03) + tt_04 * (t4cof + (t5cof * tt_0));
  }

  a = a_odp * pow(tempa,2);
  e = e_o - tempe;
  L = M_p + omega + Omega + (n_odp * templ);
  beta = sqrt(1.0 - (e * e));
  n = k_e / pow(a, (long double)1.5);

  /* Long period periodics */
  a_xN = e * cos(omega);
  temp1 = 1.0 / (a * beta * beta);
  L_L = temp1 * xlcof * a_xN;
  a_yNL = temp1 * aycof;
  L_T = L + L_L;
  a_yN = e * sin(omega) + a_yNL;

  /* Solve Kepler's' Equation */
  // fmod(L_T - Omega, (2.0 * PI)) replaces FMOD2P function in original source
#ifdef NEW_FUNCTIONS
  U = fmod(L_T - Omega, (2.0 * PI));
#else
  U = FMod2p(L_T - Omega);
#endif
  temp2 = U;
  
  //for (i = 0; i < 20; i++)
  while ((fabs(epw - temp2)) > (1.0e-6))
  {
    sinepw = sin(temp2);
    cosepw = cos(temp2);
    temp3 = a_xN * sinepw;
    temp4 = a_yN * cosepw;
    temp5 = a_xN * cosepw;
    temp6 = a_yN * sinepw;
    epw = (U - temp4 + temp3 - temp2) / (1.0 - temp5 - temp6) + temp2;
    /*
    if (fabs(epw - temp2) <= 1.0e-10)
    {
      break;
    }
    */
    temp2 = epw;
  }

  /* Short period preliminary quantities */
  ecosE = temp5 + temp6;
  esinE = temp3 - temp4;
  e_L2 = (a_xN * a_xN) + (a_yN * a_yN);
  temp1 = (1.0 - e_L2);
  p_L = a * temp1;
  r = a * (1.0 - ecosE);
  temp2 = 1.0 / r;
  dotr = k_e * sqrt(a) * esinE * temp2;
  rdotf = k_e * sqrt(p_L) * temp2;
  temp3 = a * temp2;
  betal = sqrt(temp1);
  temp4 = 1.0 / (1.0 + betal);
  cosu = temp3 * (cosepw - a_xN + a_yN * esinE * temp4);
  sinu = temp3 * (sinepw - a_yN - a_xN * esinE * temp4);
#ifdef NEW_FUNCTIONS
  u = atan2(sinu, cosu);
#else
  u = ArcTan(sinu, cosu);
#endif
  sin2u = 2.0 * sinu * cosu;
  cos2u = 2.0 * cosu * cosu - 1.0;
  temp1 = 1.0 / p_L;
  temp2 = k_2 * temp1;
  temp3 = temp2 * temp1;

  /* Update for short periodics */
  r_k = r * (1.0 - (1.5 * temp3 * betal * x3thm1)) + (0.5 * temp2 * oneMinustheta2 * cos2u);
#ifdef MATH_OUT
  cout << endl << "$ r_{k} = " << r_k << " $" << endl;
#endif
  u_k = u - 0.25 * temp3 * seventheta2Minus1 * sin2u;
  Omega_k = Omega + 1.5 * temp3 * cosi_o * sin2u;
  i_k = i_o + 1.5 * temp3 * cosi_o * sini_o * cos2u;
  
  dotr_k = dotr - n * temp2 * oneMinustheta2 * sin2u;
  rdotf_k = rdotf + n * temp2 * (oneMinustheta2 * cos2u + 1.5 * x3thm1);
  
  /* Orientation vectors */
  sinu_k = sin(u_k);
  cosu_k = cos(u_k);

  sinOmega_k = sin(Omega_k);
  cosOmega_k = cos(Omega_k);
  
  sini_k = sin(i_k);
  cosi_k = cos(i_k);

  xmx = (-1.0 * sinOmega_k) * cosi_k;
  xmy =  cosOmega_k * cosi_k;
#ifdef MATH_OUT
  cout << endl << "$ xmx = " << xmx << " $" << endl;
  cout << endl << "$ xmy = " << xmy << " $" << endl;
#endif
  
  U_x = (xmx * sinu_k) + (cosOmega_k * cosu_k);
  U_y = (xmy * sinu_k) + (sinOmega_k * cosu_k);
  U_z = sini_k * sinu_k;
  
  V_x = (xmx * cosu_k) - (cosOmega_k * sinu_k);
  V_y = (xmy * cosu_k) - (sinOmega_k * sinu_k);
  V_z = sini_k * cosu_k;

  /* Position and velocity */
  X = r_k * U_x;
  Y = r_k * U_y;
  Z = r_k * U_z;

  dotX = (dotr_k * U_x) + (rdotf_k * V_x);
  dotY = (dotr_k * U_y) + (rdotf_k * V_y);
  dotZ = (dotr_k * U_z) + (rdotf_k * V_z);

  X *= (e_R / a_E);
  Y *= (e_R / a_E);
  Z *= (e_R / a_E);

  dotX *= (e_R / 60.0); // * (minutesDay / secondsDay)
  dotY *= (e_R / 60.0); // * (minutesDay / secondsDay)
  dotZ *= (e_R / 60.0); // * (minutesDay / secondsDay)

  info = new ECI(name, X, Y, Z, dotX, dotY, dotZ, timeJD);
 
  return *info;
}

//
ECI TLE::sdp4(const long double timeJD)
{
  ECI	dead;

  return (dead);
}

ECI TLE::position4(const long double timeJD)
{
  ECI		retVal;

  if (deep)
  {
    retVal = sdp4(timeJD);
  }
  else
  {
    retVal = sgp4(timeJD);
  }

  return (retVal);
}

//
istream& operator >> (istream &is, TLE &tle)
{
  tle.getInput(is);
  return (is);
}

//
ostream& operator << (ostream &os, TLE &tle)
{
  tle.writeOutput(os);
  return (os);
}
