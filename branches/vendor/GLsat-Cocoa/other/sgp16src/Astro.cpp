#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "math.h"
#include "sgpsdp.h" 
#include "vector.h" 


VECTOR CSGP4_SDP4::CalculateSolarPosition(double time)	//, solar_vector : vector);
{
	static VECTOR solar_vector;
	double DeltaET_year;
	double mjd,year,T,M,L,e,C,O,Lsa,nu,R,eps;
	mjd  = time - 2415020.0;
	year = 1900.0 + mjd/365.25;
	DeltaET_year = 26.465 + 0.747622*(year - 1950.0) + 1.886913*sin(2.0 * PI * (year - 1975.0)/33.0);

	T = (mjd + DeltaET_year/secday)/36525.0;
	M = (Modulus(358.47583 + Modulus(35999.04975*T,360.0) - (0.000150 + 0.0000033*T)*sqr(T),360.0)) * PI/180.0;
	L = (Modulus(279.69668 + Modulus(36000.76892*T,360.0) + 0.0003025*sqr(T),360.0)) * PI/180.0;
	e = 0.01675104 - (0.0000418 + 0.000000126*T)*T;
	C = ((1.919460 - (0.004789 + 0.000014*T)*T)*sin(M) + (0.020094 - 0.000100*T)*sin(2*M) + 0.000293*sin(3*M)) * PI/180.0;
	O = (Modulus(259.18 - 1934.142*T,360.0)) * PI/180.0;
	Lsa = Modulus(L + C - (0.00569 - 0.00479*sin(O)) * PI/180.0, PI * 2.0);
	nu  = Modulus(M + C,PI * 2.0);
	R   = 1.0000002*(1 - sqr(e))/(1 + e*cos(nu));
	eps = (23.452294 - (0.0130125 + (0.00000164 - 0.000000503*T)*T)*T + 0.00256*cos(O)) * PI/180.0;
	R   = AU*R;
	solar_vector.x = R*cos(Lsa);
	solar_vector.y = R*sin(Lsa)*cos(eps);
	solar_vector.z = R*sin(Lsa)*sin(eps);
	solar_vector.w = R;
	// end; {Procedure Calculate_Solar_Position}
	return solar_vector;
}

double CSGP4_SDP4::DepthOfEclipse(double time, VECTOR r1)
{
	double r1_r1,r1_r2,r2_r2,k,d,ds;
	VECTOR r2;
	Magnitude(&r1);
	r2 = CalculateSolarPosition(time);
//	solar_pos = r2;
	r1_r1 = sqr(r1.w);
	r1_r2 = -Dot(r1,r2);
	r2_r2 = sqr(r2.w);
	k = r1_r2/r2_r2;
	// {Calculate perpendicular distance from anti-solar vector}
	d = sqrt(r1_r1 - sqr(r1_r2)/r2_r2);
	// {Calculate shadow distance ds}
	ds = xkmper + k * (sr - xkmper);
	// {If d < ds, then satellite is in eclipse}
	if ( (k > 0.0) && (d < ds) )
		m_bEclipsed = TRUE;
	else
		m_bEclipsed = FALSE;
	return d - ds;
	// Depth_of_Eclipse := d - ds
	// end; {Function Depth_of_Eclipse}
}

BOOL CSGP4_SDP4::GetEclipsed()
{
	return m_bEclipsed;
}

void CSGP4_SDP4::CalculateLatLonAlt(double jdTime)
{
	m_vLLA = CalculateLatLonAlt(m_vPOS, jdTime);
	m_bLatLonAlt = TRUE;
}

VECTOR CSGP4_SDP4::CalculateLatLonAlt(VECTOR vPOS, double time)
{
// Reference:  The 1992 Astronomical Almanac, page K12. 
	static VECTOR vLLA;
	double lat,lon,alt;
	double theta,r,e2,phi,c;
	double arg1, arg2;

	vLLA.x = vLLA.y = vLLA.z = vLLA.w = 0.0;
	lat = lon = alt = 0.0;
	theta = r = e2 = phi = c = 0.0;

//	theta = atan2(vPOS.y,vPOS.x);
	theta = AcTan(vPOS.y,vPOS.x);
	
	arg1 = ThetaG(time);
	arg1 = theta - arg1;
	arg2 = 2.0* PI;

//	lon = Modulus(theta - ThetaG(time),2.0*PI);
	lon = Modulus(arg1, arg2);

	r = sqrt(sqr(vPOS.x) + sqr(vPOS.y));
	e2 = f*(2.0 - f);
	lat = AcTan(vPOS.z,r);
	do	{
		phi = lat;
		c = 1.0/sqrt(1.0 - e2*sqr(sin(phi)));
		lat = AcTan( vPOS.z + xkmper*c*e2*sin(phi),r);
	}	while (fabs(lat - phi) > 1E-10);//1E-7); For speeding up calculation 7 digit
										//is exact enough (123.45
	alt = r/cos(lat) - xkmper*c;

	vLLA.x = lat*180.0/PI;   // radians
	vLLA.y = lon*180.0/PI;   // radians
	vLLA.z = alt;			// kilometers
	vLLA.w = theta*180.0/PI; // radians
	return vLLA;
}

/*----------------------------------------------------------------------*/
//int CSGP4_SDP4::Eclipse( double *r, double *sun, double *moon, int *em, int *ee, char **which )
int CSGP4_SDP4::Eclipse( double *r, VECTOR *vSun, VECTOR *vMoon, int *em, int *ee, char **which )
{
  /* function to compute if object at location r is eclipsed by
     either the earth or moon.  ECI coordinates for all vectors.
     Inputs:
	r is the location of s/c
	sun is location of sun
	moon is location of moon
     Outputs
	which points to  message about result of the test
	em is zero if not eclipsed by moon
	ee is zero if not eclipsed by earth
     Returns 0 if no eclipse.
  */
	double us[3], um[3], ue[3];
	double alpha, beta, beta_sun;
	double mdist, edist, sdist;
	double x[3];
	double *sun = (double *)&vSun;
	double *moon= (double *)&vMoon; 
  *em = 0;               /* assume no eclipse */
  *ee = 0;
  /* find distances and directions to sun, moon and earth */
  VecDiff( sun, r, x, 3 ); sdist = VecMag( x, 3 );
  UnitVec( x, us, 3 );   /* direction towards the sun */            
  VecDiff( moon, r, x, 3 ); mdist = VecMag( x, 3 );
  UnitVec( x, um, 3 );   /* direction towards the moon */            
  VecScale( -1.0, r, ue, 3 ); /* vector to earth center */
  UnitVec( ue, ue, 3 );  /* direction to earth center */
  edist = VecMag( r, 3 );
  /* cannot have eclipse if sun is closest */
  *which = "No Eclipse";
  if (( sdist <= edist ) && (sdist <= mdist )) return 0;
  beta_sun = asin( RSUN / sdist );    /* half angle of sun */
  /* look for eclipse by earth */
  beta = asin( REarth / edist );      /* half angle of earth */
  alpha = acos( VecDot( ue, us, 3 )); /* angle from earth to sun */
  if ( alpha < (beta + beta_sun)) {   /* some kind of eclipse */
    *ee = 1;
    if (( beta >= beta_sun ) && ( alpha <= (beta - beta_sun))){ /* total */
      *which = "Total Eclipse by Earth"; return 1; }
    else {   /* partial */
      *which = "Partial Eclipse by Earth"; return 1; }
  }
  /* look for eclipse by moon */
  beta = asin( RMOON / mdist );      /* half angle of moon */
  alpha = acos( VecDot( um, us, 3 ));/* angle from moon to sun */
  if ( alpha < (beta + beta_sun)) {  /* some kind of eclipse */
    *em = 1;
    if (( beta >= beta_sun ) && ( alpha <= (beta - beta_sun))){ /* total */
      *which = "Total Eclipse by Moon"; return 1; }
    else {   /* partial */
      *which = "Partial Eclipse by Moon"; return 1; }
  }
  return 0;
}
/*----------------------------------------------------------------------*/
double CSGP4_SDP4::MoonPhase( double *Sun, double *Moon, double *Observer )
{
  /* Calculate the phase of the moon in degrees as seen by
     the observer.  180 deg is full.
  */
  double MO[3], UMO[3], USO[3], SO[3], C;
  VecDiff( Moon, Observer, MO, 3 );
  VecDiff( Sun, Observer, SO, 3);
  UnitVec( MO, UMO, 3 );
  UnitVec( SO, USO, 3 );
  C = VecDot( USO, UMO, 3 );
  return acos( C ) * RTD;
}
/**********************************************************************/
void CSGP4_SDP4::SunMoon( double T, double *Sun, double *Moon )
{
  /* locate the sun and moon in geocentric inertial coordinates
   input time in Julian days and get back sun and moon in meters.
  */

   /* sun ephemeris based on Slabinski's suneph subroutine 
   which is adapted from Newcomb's theory of the sun, A.P.A.E, 2nd ed.
   Vol 6 part 1 (Washington: Navy Dept., 1898), pp. 9-21 and
   described in Comsat Tech Memo CD-03-72.
   */

  static double ABERTN = 99.3664E-06;  /* constant of aberation in rad */
  static double T0 = 2415020.0;        /* epoch for this fourier series */
  double DT;
  double MEARTH, COSME, SINME, NURA;
  double LONSUN, COSL, SINL, RS, E, OB, SOB, COB;
  double LONPRT, NMOON, MR, MRi, LMMean, F, MM;
  double cosmm, cosf, sinmm, sinf, sin2d; 
  double cos2d, sin2f, cos2f, sin2mm, cos2mm; 
  double SL, CL, S, cosd, sind, LATMN, LONGMN;
  double MVENUS, MMARS, MJUP, DMNSUN;
  double myTrue[3];
  /* convert Julian time to ephemeris time wrt to epoch 1900 Jan 0.5 ET*/
  DT = (T - T0 + 6.31E-04 + 2.85E-08 * (T-2446066.5))/36525.0;
  /* compute earth mean anomaly, eqn 14 */
  MEARTH = fmod(6.25658358 + 628.30194572 * DT, M2PI );
  COSME = cos( MEARTH ); SINME = sin( MEARTH );
  /* Mean anomalies for perturbing bodies */
  MVENUS = fmod(3.70795199 + DT * 1021.32292286, M2PI);
  MMARS  = fmod(5.57772322 + DT *  334.05561740, M2PI);
  MJUP   = fmod(3.93187774 + DT *   52.96346477, M2PI);
  /* mean elongation of the moon from the sun, eqn 15 */
  DMNSUN = fmod(6.12152394 + DT * 7771.37719393, M2PI);
  /* celestial longitude of the mean asc node of the moon's orbit, eqn 16 */
  NMOON  = fmod(4.52360151 - DT *   33.757146246, M2PI);
  NURA = -83.55E-06 * sin( NMOON );   /* nutation of ra ?  eqn 25 */
  /* perturbation expressions are from Newcomb, Tables of the sun
     A.P.A.E. vol. 6, pp.14-18 (1898)*/
  LONPRT =  23.4553E-06 * cos( 5.220309 + MVENUS - MEARTH ) /*eqn 13 */
	  + 26.7908E-06 * cos(2.588556 + 2.0 * (MVENUS - MEARTH))
	  + 12.1058E-06 * cos(5.514251 + 2.0 * MVENUS - 3.0 * MEARTH)
	  +  9.9047E-06 * cos(6.001983 + 2.0 * (MEARTH - MMARS))
	  + 34.9454E-06 * cos(3.133418 + MEARTH - MJUP)
	  + 12.6051E-06 * cos(4.593997 - MJUP)
	  + 13.2402E-06 * cos(1.520967 + 2.0 * (MEARTH - MJUP))
	  + 31.0281E-06 * cos(4.035026 + DT * 0.352556)
	  + 31.2898E-06 * sin( DMNSUN ) + NURA - ABERTN;
  LONSUN = fmod(4.88162793 + DT * (628.33195099 + DT * 5.2844E-06), M2PI)
	   + SINME * (33495.79E-06 - 83.58E-06 * DT 
	   + COSME * (701.41E-06 - 3.50E-06 * DT + COSME * 20.44E-06))
	   + LONPRT;
  COSL = cos( LONSUN );
  SINL = sin( LONSUN );
  RS = AU * (1.0002806 - COSME * (1675.104E-05 - 4.180E-05 * DT
		       + COSME * 28.060E-05));
  /* true obliquity of the ecliptic, eqn 1 & 26 */
  OB = 0.40931970 - 2.27111E-04 * DT + 4.465E-05 * cos ( NMOON );
  COB = cos( OB ); SOB = sin( OB );
  E = NURA * COB;                   /* eqn of equinoxes, eqn 2 */
  /* convert to cartesian coordinates, eqns. 6-10 scaled to meters */
  Sun[0] = RS * (COSL + SINL * COB * E );  
  Sun[1] = RS * (SINL * COB - COSL * E );
  Sun[2] = RS * SINL * SOB; 

  /* lunar ephemeris is based on Lrown's Lunar Theory Ammended.
  The expressions are adapted from Woolard, theory of the rotation the
  Earth, A.P.A.E Vol.15, Part 1 (1953), Tables 4, 6, and 7.

  The moon vector is computed at the Earth's center in the geometric 
  direction of the moon.  Neglecting the light travel time of 1.3 sec 
  between Earth and moon gives a negligible error of 0.0002 deg for
  the moons's direction.
  */

  LMMean = fmod( 4.71996657 + DT * (8399.70914492 - DT * 1.9780E-05), M2PI);
  F      = fmod( 0.19636505 + DT * (8433.46629117 - DT * 5.6044E-05), M2PI);
  MM     = fmod( 5.16800034 + DT * (8328.69110367 + DT * 1.60425E-04),M2PI);
  DMNSUN -= (2.5065E-05 * DT * DT);   /* eqn 33 */
  NMOON  += (3.6264E-05 * DT * DT);   
  /* fundamental frequency sines and cosines */
  cosmm = cos(MM);     sinmm = sin(MM); 
  cosd  = cos(DMNSUN); sind  = sin(DMNSUN);
  cosf  = cos(F);      sinf  = sin(F);
  /* double frequency sines and cosines */
  sin2d   = 2.0 * sind * cosd;    cos2d  =  cosd * cosd  -  sind * sind;
  sin2f   = 2.0 * sinf * cosf;    cos2f  =  cosf * cosf  -  sinf * sinf;
  sin2mm  = 2.0 * sinmm * cosmm;  cos2mm = cosmm * cosmm - sinmm * sinmm;
  /* lunar longitude */
  LONGMN = sind * (-607.0 + 84.0 * COSME - 131.0 * cosmm )
       + sin2d * (11490.0 + 134.0 * cos2d + 239.0 * cos2f 
		     + 683.0 * COSME
         +  cosmm * (23098.0 + 392.0 * cos2d + 918.0 * COSME 
		  	      - 80.0 * cos2f + 138.0 * cos2mm)
         +  cos2mm * (1096.0 + 300.0 * cos2d))
       +  sin2f * (-1996.0 - 295.0 * cos2d 
	    + cosmm * (-411.0 - 20.0 * cos2d))
       +  SINME * (-3238.0 + 90.0 * cosd - 919.0 * cos2d - 72.0 * COSME
         +  cosmm * (-1248.0 - 1222.0 * cos2d) - 84.0 * cos2mm)
       + sin2mm * ( 3876.0 + 10.0 * COSME
	 + cos2d * ( -956.0 - 296.0 * cos2d ) )
       + (sin2mm * cosmm + cos2mm * sinmm ) * ( 175.0 -59.0 * cos2d )
       + 68.9 * sin( 6.042967 + 2.318888 * DT ) - 49.0 * sin( NMOON )
       + sinmm * ( sin2d * ( -1052.0 * SINME -72.0 * sin2f )
		   + COSME * ( 184.0 - 804.0 * cos2d )
		   + cos2d * ( -21305.0 - 352.0 * cos2d )
		   + cos2f * (  -27.0 + 8.0 * cos2d )
		   + 49.0 * cosd + 109936.0 ); 
  LONGMN = LONGMN * 1.0E-06 + LMMean;
  /* lunar latitude */
  LATMN = cosf * ( -49.0E-06 * sind
	     + sin2d * ( 3592.0E-06 +  119.0E-06 * COSME + 2008.0E-06 * cosmm
				+   85.0E-06 * cos2mm )
             + SINME  * (  -55.0E-06 -   24.0E-06 * cos2d - 112.0E-06 * cosmm )
	     + sinmm  * ( 9744.0E-06 - 1540.0E-06 * cos2d +  10.0E-06 * COSME )
	     + sin2mm * (  455.0E-06 -   49.0E-06 * cos2d ) )
         + sinf * (-3.0E-06 * cosd - 2454.0E-06 * cos2d - 60.0E-06 * cos2f
			  + 6.0E-06 * sinmm * SINME
             + sin2d * (-157.0E-06 * SINME+248.0E-06 * sinmm-79.0E-06 * sin2mm )
	     + COSME * (  -7.0E-06 -   53.0E-06 * cos2d )
	     + cosmm * (  50.0E-06 + 72.0E-06 * cos2d + 4.0E-06 * COSME )
	     + cos2mm * (147.0E-06 - 87.0E-06 * cos2d )
	     + 89473.0E-06 );
 SL = sin( LATMN ); CL = cos( LATMN );
 /* the following numerator is the inverse of the constant of sine 
    parallax for the moon, 3422.451 arc-sec, but iin radians. */
 MRi = 9999237.0 - 2858.0 * cosd
	+ sin2d * ( 6481.0 * SINME + sin2mm * (-1715.0 + 2144.0 * cos2d))
	+ sinmm * (-2083.0 * sin2f +  SINME * ( 6142.0 - 4067.0 * cos2d )
	  + sin2d * (91230.0 + 3260.0 * cos2d + 3027.0 * COSME))
	+ cos2d * (  82488.0 + 1526.0 * cos2d)
	+ COSME  * ( -1169.0 + 4727.0 * cos2d +   67.0 * cos2mm)
	+ cosmm  * (541309.0 - 2089.0 * cos2f + 3634.0 * cos2mm
	  + cos2d  * (109264.0         + 3764.0 * cos2d)
	  + SINME  * (  5691.0 * sin2d + 1350.0 * sinmm)
	  + COSME  * (   596.0         + 4091.0 * cos2d ))
        + cos2mm * ( 28598.0 + cos2d * ( -61.0 + 2204.0 * cos2d ));
  MR = REarth * 60.26816E+07 / MRi;
  myTrue[0] =  CL * cos( LONGMN );
  S = CL * sin( LONGMN );
  myTrue[1] =  ( S * COB - SL * SOB);
  Moon[2] = MR *  ( S * SOB + SL * COB );
  /* convert x and y components from true equinox to mean equinox of date
     so components are based on true equator and mean equinox. */
  Moon[0] = MR * ( myTrue[0] + myTrue[1] * NURA );
  Moon[1] = MR * ( myTrue[1] - myTrue[0] * NURA );
  return;
}
/*------------------------------------------------------------------------*/
