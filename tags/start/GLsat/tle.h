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

#ifndef TLE_H
#define TLE_H

#include <iostream>
#include <string>
#include "constants.h"
#include "eci.h"

using namespace std;

class TLE
{
  private:
    // input strings, maintained for posterity and possible output
    string		firstLine;
    string		secondLine;
    
    long double		bStar;			// the SGP4 type drag coefficient
    int			checksum1;
    int			checksum2;
    char		classification;
    long double		dotn_o;			// the time rate of change of "mean" mean motion at epoch
						// divided by 2 ( 2.0 * dotn_o = \dot{n}_{o} )
						// also called First Time Derivative of the Mean Motion
						// Measured in revolutions per day per day
    long double		ddotn_o;		// the second time rate of change of "mean" mean motion at epoch
						// divided by 6 ( 6.0 * ddotn_o = \ddot{n}_{o} )
						// also called Second Time Derivative of the Mean Motion
    long double		e_o;			// the "mean" eccentricity at epoch
    int			elementSet;
    int			epheremisType;
    int			epochYear;
    long double		epochDay;
    long double		i_o;			// the "mean" inclination at epoch
    string		intLaunchDesig;
    int			intLaunchNumber;
    int			intLaunchYear;
    long double		M_o;			// the "mean" mean anomaly at epoch
						// measured in revolutions per day
    long double		n_o;			// the SGP type "mean" mean motion at epoch
    string		name;
    long double		omega_o;		// the "mean" argument of perigee at epoch
    long double		Omega_o;		// the "mean" longitude of ascending node at epoch
						// also called Right Ascension of Ascending Node
    int			revolution;
    int			satNumber;

    // Boolean values for initialization
    bool		deep;
    bool		sgpInit;
    bool		sgp4Init;
    bool		sdp4Init;
    bool		sgp8Init;
    bool		sdp8Init;
    bool		validChecksum;
    bool		validData;

    // variables computed once
    long double		a_1;
    long double		a30overk2;
    long double		beta_o;			// 1 - ( e_{o}^{2} )^\frac{1}{2}
    long double		beta_o2;		// 1 - ( e_{o}^{2} )
    long double		cosi_o;			// \cos(i_o)
    long double		e_o2;			// e_{o}^{2}
    long double		oneMinus5theta2;	// 1 - 5 \times \theta ^{2}
    long double		oneMinustheta2;		// 1 - \theta ^{2}
    long double		seventheta2Minus1;	// 7 \times \theta
    long double		sini_o;			// \sin(i_o)
    long double		sinM_o;			// \sin(M_o)
    long double		theta2;			// (?) \cos ^{2} (i_o)
    long double		theta4;			// (?) \theta ^{4}
    long double		x3thm1;			// (?) 3 \times \cos^{2} (i_o) - 1

    // time-based variables computed once
    long double		DS50;			// Days Since 1950 Jan 0.0 UT
    long double		GMST;			// Greenwich Mean Sidereal Time
    long double		JD;			// Julian Date of Epoch
    long double		T_U;			// Interval of time since epoch 2000 January 1.5 UT
    long double		Theta_G;		//
    long double		UT;			// Universal Time


    // variables computed once per model
    bool		simple;
    long double		a_o;
    long double		a_odp;			// \frac{a_o}{1 - \delta_o}
    long double		aycof;			// ???
    long double		coef;
    long double		coef1;
    long double		C_1;			// C_{1}
    long double		C_12;			// C_{1}^{2}
    long double		C_2;			// C_{2}
    long double		C_3;			// C_{3}
    long double		C_4;			// C_{4}
    long double		C_5;			// C_{5}
    long double		C_6;			// C_{6}
    long double		D_1;			// D_{1}
    long double		D_2;			// D_{2}
    long double		D_3;			// D_{3}
    long double		D_4;			// D_{4}
    long double		delmo;			// ???
    long double		delta_o;
    long double		delta_1;
    long double		domegadt;
    long double		dOmegadt;
    long double		eeta;			// e_o \times \eta
    long double		eta;			// \eta
    long double		eta2;			// \eta^{2}
    long double		L_o;
    long double		n_odp;			// \frac{n_o}{1 + \delta_o}
    long double		omgcof;			// ???
    long double		omgdot;			// ???
    long double		p_o;
    long double		perigee;
    long double		pinvsq;			// ???
    long double		psi2;			// \psi^{2}
    long double		q_o;
    long double		q_oms2t;		// ???
    long double		q_oms24;		// ???
    long double		s4;
    long double		t2cof;			// ???
    long double		t3cof;			// ???
    long double		t4cof;			// ???
    long double		t5cof;			// ???
    long double		xhdot1;			// ???
    long double		xi;			// \xi
    long double		xlcof;			// ???
    long double		xmcof;			// ???
    long double		xmdot;			// ???
    long double		xnodcf;			// ???
    long double		xnodot;			// ???
    
    // Functions for internal use only
    void checksum();
    void processData();
    void setupConstants();
    long double FMod2p(long double x);
    long double ArcTan(long double sinx, long double cosx);
    ECI sgp4(const long double timeJD);
    ECI sdp4(const long double timeJD);

  public:
    // Default Ctor and Dtor
    TLE();
    //~TLE();

    // Input and data verification
    void getInput(istream &is);
    string getName();
    bool getValid();
    bool getOrbit(long double &semiMaj, long double &semiMin, long double &eccen, long double &incl, long double &perigee, long double &ra);
    void writeOutput(ostream &os) const;

    // Do the math
    ECI sgp(const long double timeJD);
    ECI position4(const long double timeJD);
    // ECI position8(const long double timeJD);
};

istream& operator >> (istream &is, TLE &tle);
ostream& operator << (ostream &os, TLE &tle);

#endif
