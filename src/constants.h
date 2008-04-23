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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef WIN32
#define _USE_MATH_DEFINES
#endif

#include <cmath>

// Set up Pi to be either double or long double,
// depending on what is defined
#ifdef M_PIl
  const static long double	PI = M_PIl;
#else
  const static long double	PI = M_PI;
#endif

#ifdef M_PI_2l
  const static long double	PI_2 = M_PI_2l;
#else
  const static long double	PI_2 = M_PI_2;
#endif

// Constants for use with calculations
// World Geodetic System 1972 (WGS72)
// Physical and Geopotential Constants
const static long double	a_E = 1.0;		// Radius of the Earth
const static long double	e_R = 6378.135;		// Radius of the Earth (km)
const static long double	f = 1.0 / 298.26;	// Ellipticity of the Earth
const static long double	g_e = 398600.8;		// Earth Gravitational Constant
//const static long double	k_e = 7.43669161e-2;	// From Spacetrack #3
const static long double	k_e = 7.43669161331734132e-2;	// sqrt(G * M)?


// Gravity Field for Earth
// J_2 == Dynamical Form Factor for Earth
const static long double	J_2 = 1.0826158e-3;
const static long double	J_3 = -0.253881e-5;
const static long double	J_4 = -0.165597e-5;

const static long double	k_2 = (1.0 / 2.0) * J_2 * (a_E * a_E);
const static long double	k_4 = (-3.0 / 8.0) * J_4 * (pow(a_E, 4));

// Value for 1984
const static long double	omega_E = 1.00273791;	// Earth rotations per sidereal day

//const static long double	s = 1.01222928;		// constant version
const static long double	s_o = 78.0;		// ???
const static long double	s = a_E * (1.0 + (s_o / e_R));	// computed version

// From http://ssd.jpl.nasa.gov/astro_constants.html
// Also http://ssd.jpl.nasa.gov/phys_props_planets.html
const static long double	G = 6.67259e-11;	// kg^{-1} m^3 s^{-2}
const static long double	M = 59.736e23;		// Mass of the Earth (kg)

const static long double	minutesDay = 1440.0;	// 24 * 60
const static long double	secondsDay = 86400.0;	// 24 * 60 * 60
const static long double	deg2rad = PI / 180.0;	// \Pi radians = 180 degrees
const static long double	rad2deg = 180.0 / PI;	// 180 degrees = \Pi radians
const static long double	oneThird = 1.0 / 3.0;	// \frac{1}{3}
const static long double	twoThirds = 2.0 / 3.0;	// \frac{2}{3} (why not?)

#endif
