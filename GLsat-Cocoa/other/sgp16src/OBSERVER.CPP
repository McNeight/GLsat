#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "math.h"
#include "sgpsdp.h" 

// Calculates the User pos and vel, and return m_vUPos and m_vUVel
void CSGP4_SDP4::CalculateUserPosVel(VECTOR *geodetic, double time)
{
//Procedure Calculate_User_PosVel(var geodetic : vector;
//                                        time : double;
//                         var obs_pos,obs_vel : vector);
// Reference:  The 1992 Astronomical Almanac, page K11
	double mfactor = 2.0*PI*omega_E/secday;
	double	lat,lon,alt,theta,c,s,achcp;
	lat = geodetic->x;
	lon = geodetic->y;
	alt = geodetic->z;
	theta = Modulus(ThetaG(time) + lon,2.0*PI);
	geodetic->w = theta;						// {LMST}
	c = 1.0/sqrt(1.0 + f*(f - 2.0)*sqr(sin(lat)));
	s = sqr(1.0 - f)*c;
	achcp = (xkmper*c + alt)*cos(lat);
	m_vUPos.x = achcp*cos(theta);			// {kilometers}
	m_vUPos.y = achcp*sin(theta);
	m_vUPos.z = (xkmper*s + alt)*sin(lat);
	m_vUPos.w = sqrt(sqr(m_vUPos.x) + sqr(m_vUPos.y) + sqr(m_vUPos.z));
	m_vUVel.x = -mfactor*m_vUPos.y;		// {kilometers/second}
	m_vUVel.y =  mfactor*m_vUPos.x;
	m_vUVel.z =  0.0;
	m_vUVel.w = sqrt(sqr(m_vUVel.x) + sqr(m_vUVel.y));
}
// Calculates the Observer data for a given Satellite and returns m_vObs
BOOL CSGP4_SDP4::CalculateObs(VECTOR pos, VECTOR vel, VECTOR geodetic, double time)
{
// Procedure Calculate_Obs(pos,vel,geodetic : vector;
//                                    time : double;
//                             var obs_set : vector);
	BOOL	visible;
	double	lat,lon,alt,theta,sin_lat,cos_lat,sin_theta,cos_theta;
	double	el,azim,top_s,top_e,top_z;
	VECTOR	range,rgvel;
	CalculateUserPosVel(&geodetic,time);	//,obs_pos,obs_vel);
	rgvel.x = vel.x - m_vUVel.x;
	rgvel.y = vel.y - m_vUVel.y;
	rgvel.z = vel.z - m_vUVel.z;
	range.x = pos.x - m_vUPos.x;
	range.y = pos.y - m_vUPos.y;
	range.z = pos.z - m_vUPos.z;
	range.w = sqrt(sqr(range.x) + sqr(range.y) + sqr(range.z));
	lat = geodetic.x;
	lon = geodetic.y;
	alt = geodetic.z;
	theta = geodetic.w;
	sin_lat = sin(lat);
	cos_lat = cos(lat);
	sin_theta = sin(theta);
	cos_theta = cos(theta);
	top_s = sin_lat*cos_theta*range.x + sin_lat*sin_theta*range.y - cos_lat*range.z;
	top_e = -sin_theta*range.x + cos_theta*range.y;
	top_z = cos_lat*cos_theta*range.x + cos_lat*sin_theta*range.y + sin_lat*range.z;
	azim = atan(-top_e/top_s);	//{Azimuth}
	if (top_s > 0.0)	azim += PI;
	if (azim < 0.0)	    azim += 2.0*PI;
	el = asin(top_z/range.w);
	m_vObs.x = azim;            //{Azimuth (radians)}
	m_vObs.y = el;				//{Elevation (radians)}
	m_vObs.z = range.w;			//{Range (kilometers)}
	m_vObs.w = range.x*rgvel.x + range.y*rgvel.y + range.z*rgvel.z;
	m_vObs.w /= range.w;	    //{Range Rate (kilometers/second)}
//{ Corrections for atmospheric refraction }
//{ Reference:  Astronomical Algorithms by Jean Meeus, pp. 101-104 }
//{ Note:  Correction is meaningless when apparent elevation is below horizon }
	m_vObs.y += DegToRad((1.02/tan(DegToRad(RadToDeg(el)+10.3/(RadToDeg(el)+5.11))))/60.0);
	if (m_vObs.y >= 0)	visible = TRUE;
	else	{
		m_vObs.y = el;	//{Reset to true elevation}
		visible = FALSE;
	}
	return visible;
}
// Calculates the Observer data in topocentric data and return m_vRad
void CSGP4_SDP4::CalculateRADec(VECTOR pos, VECTOR vel, VECTOR geodetic, double time)
{
//Procedure Calculate_RADec(pos,vel,geodetic : vector;
//                                      time : double;
//                               var obs_set : vector);
//{ Reference:  Methods of Orbit Determination by Pedro Ramon Escobal, pp. 401-402}
	double	phi,theta,sin_theta,cos_theta,sin_phi,cos_phi,az,el,Lxh,Lyh,Lzh;
	double	Sx,Ex,Zx,Sy,Ey,Zy,Sz,Ez,Zz,Lx,Ly,Lz,cos_delta,sin_alpha,cos_alpha;
	m_vRad.x = 0.0; m_vRad.y = 0.0; m_vRad.z = 0.0; m_vRad.w = 0.0;
	BOOL visible = CalculateObs(pos,vel,geodetic,time);	//,obs_set);
	if (visible)	{
		az = m_vObs.x;
		el = m_vObs.y;
//		az = m_vRad.x;
//		el = m_vRad.y;
		phi   = geodetic.x;
		theta = Modulus(ThetaG(time) + geodetic.y,2.0*PI);
		sin_theta = sin(theta);
		cos_theta = cos(theta);
		sin_phi = sin(phi);
		cos_phi = cos(phi);
		Lxh = -cos(az)*cos(el);
		Lyh =  sin(az)*cos(el);
		Lzh =  sin(el);
		Sx = sin_phi*cos_theta;
		Ex = -sin_theta;
		Zx = cos_theta*cos_phi;
		Sy = sin_phi*sin_theta;
		Ey = cos_theta;
		Zy = sin_theta*cos_phi;
		Sz = -cos_phi;
		Ez = 0.0;
		Zz = sin_phi;
		Lx = Sx*Lxh + Ex*Lyh + Zx*Lzh;
		Ly = Sy*Lxh + Ey*Lyh + Zy*Lzh;
		Lz = Sz*Lxh + Ez*Lyh + Zz*Lzh;
		m_vRad.y= asin(Lz);						//{Declination (radians)}
		cos_delta  = sqrt(1 - sqr(Lz));
		sin_alpha  = Ly/cos_delta;
		cos_alpha  = Lx/cos_delta;
		m_vRad.x = AcTan(sin_alpha,cos_alpha);	//{Right Ascension (radians)}
		m_vRad.x = Modulus(m_vRad.x,2.0*PI);
    }
}

