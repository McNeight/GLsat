#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "math.h"
#include "sgpsdp.h" 

CSGP4_SDP4::CSGP4_SDP4 ()
{
	Init();
}

CSGP4_SDP4::CSGP4_SDP4(SATELLITE *pSatellite)
{
	Init();
	SetSatellite(pSatellite);
	return;
}

CSGP4_SDP4::CSGP4_SDP4 (char *cLine0, char *cLine1, char *cLine2)
{
	Init();
	SetSatellite (cLine0, cLine1, cLine2);
	return;
}

CSGP4_SDP4::~CSGP4_SDP4()
{
}

BOOL CSGP4_SDP4::SGP(double time)
{
	double tsince;
	BOOL ret;
	VECTOR pos,vel;
	m_bLatLonAlt = FALSE;
	m_fTime = time;
	tsince = (time - m_Sat.fJulianEpoch) * xmnpda;
	if (ideep == 0)
		ret = SGP4(tsince,&iflag,&pos,&vel);	// Calculation for near earth
	else
		ret = SDP4(tsince,&iflag,&pos,&vel);	// Calculation for the deep space
	if (ret == FALSE) return FALSE;	// Ooops, there was a mistake within the calculation !!!
	ConvertSatState(&pos, &vel);	// here the pos and vel vector are 
									// to be copied to m_vPOS and m_vVEL !!!
	return TRUE;
} // Procedure SGP

int CSGP4_SDP4::GetVersion(char *pBuffer, int iMaxLen)		// returns a pointer to the version of this dll
{
	static char	szVersion[] = {
			"NORAD SGP4 / SDP4 algorithm (WGS72) for GPS.\n"
			"Pascal Version 2.5 by Dr.TS Kelso in 1995 (tkelso@celestrak.com)\n"
			"Converted to C++ by Varol Okan in 1996\n"
			"DLL-Version 1.6 (18.Oct.2001)\n\n"
			"This version resolves the AcTan, and Lyddan - bug.\0"};

	strncpy(pBuffer, szVersion, iMaxLen);
    return strlen(szVersion);
}

void CSGP4_SDP4::SetSatellite (char *cLine0, char *cLine1, char *cLine2, bool bInitSatellite)
{
	memcpy(m_cLine0, cLine0, 22);
	memcpy(m_cLine1, cLine1, 69);
	memcpy(m_cLine2, cLine2, 69);
	m_cLine0[22] = m_cLine1[69] = m_cLine2[69] = 0;
	ConvertData();
	if (bInitSatellite)
		InitSatellite();
	return;
}

void *CSGP4_SDP4::GetSatellite()		// returns a pointer to th actual satellite
{
	return (void *) &m_Sat;
}

VECTOR	CSGP4_SDP4::GetUserPos(void)	{return m_vUPos;}
VECTOR	CSGP4_SDP4::GetUserVel(void)	{return m_vUVel;}
VECTOR	CSGP4_SDP4::GetObserver(void)	{return m_vObs;}
VECTOR	CSGP4_SDP4::GetRADec(void)		{return m_vRad;}
	
VECTOR CSGP4_SDP4::GetPos()	{return m_vPOS;}
VECTOR CSGP4_SDP4::GetVel() {return m_vVEL;}
double CSGP4_SDP4::GetTime(){return m_fTime;}
double CSGP4_SDP4::GetLat() 
{
	if (m_bLatLonAlt)return m_vLLA.x;
	else return NULL;
}
double CSGP4_SDP4::GetLon()
{
	if (m_bLatLonAlt)return m_vLLA.y;
	else return NULL;
}
double CSGP4_SDP4::GetAlt()
{
	if (m_bLatLonAlt)return m_vLLA.z;
	else return NULL;
}

double CSGP4_SDP4::GetFloat (int iStart, int iEnd, char *cLine) 
{
	CString cs;
	CString csLine (cLine);
	double ret;
	iStart --;
	cs = csLine.Mid(iStart, iEnd-iStart);
	ret = atof(cs);
	return ret;
}

long CSGP4_SDP4::GetInt   (int iStart, int iEnd, char *cLine) 
{
	CString cs;
	CString csLine(cLine);
	long ret;
	iStart --;
	cs = csLine.Mid(iStart, iEnd-iStart);
	ret = atoi(cs);
	return ret;
}

char *CSGP4_SDP4::GetString(int iStart, int iEnd, char *cLine) 
{
	static char cBuffer[100];
	iStart --;
	int iLength = abs(iEnd-iStart);
	if (iLength >= 100) iLength = 99;	// To be on the save side ...
	strncpy(cBuffer, cLine+iStart, iEnd-iStart);
	cBuffer[iLength] = 0;	// Strng terminator ...
	return (char *) &cBuffer;
	
}


void CSGP4_SDP4::ConvertSatState(VECTOR *pos,VECTOR *vel)
{
	pos->x *= xkmper;
	pos->y *= xkmper;
	pos->z *= xkmper;
	pos->w = sqrt(sqr(pos->x)+sqr(pos->y)+sqr(pos->z));
	vel->x *= xkmper/60.0;	// kilometers/second
	vel->y *= xkmper/60.0;	// kilometers/second
	vel->z *= xkmper/60.0;	// kilometers/second
	vel->w = sqrt(sqr(vel->x)+sqr(vel->y)+sqr(vel->z));
	// write to the public member ...
	m_vPOS.x = pos->x;
	m_vPOS.y = pos->y;
	m_vPOS.z = pos->z;
	m_vPOS.w = pos->w;
	m_vVEL.x = vel->x;
	m_vVEL.y = vel->y;
	m_vVEL.z = vel->z;
	m_vVEL.w = vel->w;

} //Procedure Convert_Sat_State

void CSGP4_SDP4::ConvertData()
{
	memcpy(m_Sat.cSatelliteName, m_cLine0, 22);
// Now everything is in butter !
//	CString	csHelp;
//	double	dEpochDay;
	int		iTemp;
	char	*pChar;
	m_Sat.iSatelliteNumber		= GetInt	(3 , 7,m_cLine1);	// = CString >catnr<
	m_Sat.iLaunchYear			= GetInt	(10,11,m_cLine1);
	m_Sat.iLaunchNumber			= GetInt	(12,14,m_cLine1);
	pChar						= GetString	(15,17,m_cLine1);
	memcpy(m_Sat.cLaunchPiece, pChar, 3);
	m_Sat.iEpochYear			= GetInt	(19,20,m_cLine1);	// = epoch 19, 14 lang
	m_Sat.fEpochDay				= GetFloat	(21,32,m_cLine1);	// = epoch 19, 14 lang
	m_Sat.fBalisticCoefficient	= GetFloat	(34,43,m_cLine1);	// xndt2o is never used so far ...
	m_Sat.fSecondMeanMotion 	= 0.0;							// xndd60 is never used so far ...
	m_Sat.iSecondMeanMotion 	= 0;							// xndd60 is never used so far ...
//	m_Sat.fSecondMeanMotion 	= GetFloat	(45,50,m_cLine1);	// xndd60 is never used so far ...
//	iexp 	= GetInt	(51,52,m_cLine1);						// iexp is never used so far ...
//	m_Sat.fBalisticCoefficient *= pow(10.0, iexp);				// --- '' ---
	
//	m_Sat.fRadiationCoefficient = GetFloat	(54,61,m_cLine1)*1e-5;	// = bstar 54, 6 lang
	// the input format is 12345-x convert to 0.12345^x
	iTemp	= GetInt	(60,61,m_cLine1);	// get the exponent, and make 0.xxxxx of it
	m_Sat.fRadiationCoefficient	= GetFloat	(54,59,m_cLine1)*pow(10,-5+iTemp);

	pChar						= GetString	(63,63,m_cLine1);
	memcpy(&m_Sat.cEmphemeristType, pChar, 2);
	m_Sat.iElementNumber		= GetInt	(65,68,m_cLine1);

	m_Sat.fInclination			= GetFloat	(9 ,16,m_cLine2);	// = xincl
	m_Sat.fRightAscending		= GetFloat	(18,25,m_cLine2);	// = xnodeo
	m_Sat.fEccentricity			= GetFloat	(27,33,m_cLine2)*1e-7;	// = eo
	m_Sat.fPeregee				= GetFloat	(35,42,m_cLine2);	// = omegao
	m_Sat.fMeanAnomaly			= GetFloat	(44,51,m_cLine2);	// = xmo
	m_Sat.fMeanMotion			= GetFloat	(53,63,m_cLine2);	// = xno
	m_Sat.iRevAtEpoch			= GetInt	(64,68,m_cLine2);

	m_Sat.fJulianEpoch = JulianDate (m_Sat.iEpochYear *1000.0+ m_Sat.fEpochDay);
//	m_Sat.fEpochFraction = modf(m_Sat.fEpochDay, &dEpochDay);
//	m_Sat.iEpochDay = (int) dEpochDay;
	m_Sat.iEpochDay = (int) m_Sat.fEpochDay;
	m_Sat.fEpochFraction = m_Sat.fEpochDay - (double)m_Sat.iEpochDay;

	m_Sat.iEpochYear += 1900;
	m_Sat.iLaunchYear += 1900;
}

void CSGP4_SDP4::Init()
{
// Normal variables for the 72'model !!!
	ae       = 1.0;
	tothrd   = 2.0/3.0;
	xkmper   = 6378.135;	// Earth equatorial radius - kilometers (WGS '72)
	f        = 1.0/298.26;	// Earth flattening (WGS '72)
	ge       = 398600.8;	// Earth gravitational constant (WGS '72)
	J2       = 1.0826158E-3;// J2 harmonic (WGS '72)
	J3       = -2.53881E-6;	// J3 harmonic (WGS '72)
	J4       = -1.65597E-6;	// J4 harmonic (WGS '72)
	ck2      = (double)J2/2.0;
	ck4      = -3.0*J4/8.0;
	xj3      = (double)J3;
	qo       = ae + 120.0/xkmper;
	s        = ae + 78.0/xkmper;
	e6a      = 1E-6;
	dpinit   = 1.0;			// Deep-space initialization code
	dpsec    = 2.0;			// Deep-space secular code
	dpper    = 3.0;			// Deep-space periodic code
	xmnpda	 = 1440.0;		// Minutes per day
	secday	 = 86400;		// seconds per day
	omega_E	 = 1.00273790934;	// earth rotation per sideral day
	xke    = sqrt(3600.0*ge/(xkmper*xkmper*xkmper));  //sqrt(ge) ER^3/min^2
	qoms2t = sqr(sqr(qo-s));              //(qo-s)^4 ER^4
}

void CSGP4_SDP4::InitSatellite()
{
// Convert sat datas to proper units ...
//	m_Sat.fRadiationCoefficient *= pow(10.0, ibexp)/ae;		// Already done
//	m_Sat.fBalisticCoefficient *= pow(10.0, iexp);			//  earlier !!!
	m_Sat.fRightAscending = DegToRad(m_Sat.fRightAscending);
	m_Sat.fPeregee = DegToRad(m_Sat.fPeregee);
	m_Sat.fMeanAnomaly = DegToRad(m_Sat.fMeanAnomaly);
	m_Sat.fInclination = DegToRad(m_Sat.fInclination);
	m_Sat.fMeanMotion *= 2.0*PI/xmnpda;
//	m_Sat.fSecondMeanMotion *= 2.0*PI/sqr(xmnpda);				// Never used variables ...
//	m_Sat.fBalisticCoefficient *= 2.0*PI/(xmnpda*xmnpda*xmnpda);// Never used variables ...
// determination if it is deep space or not ...
	double a1,a0,del1, del0,temp;
	a1 = pow (xke/m_Sat.fMeanMotion, tothrd);
	temp = (double)(1.5*ck2*(3.0*sqr(cos(m_Sat.fInclination))-1.0)/pow(1.0-m_Sat.fEccentricity*m_Sat.fEccentricity,1.5));
	del1 = temp /(a1*a1);
	a0 = a1*(1-del1*(0.5*tothrd+del1*(1.0+134.0/81.0*del1)));
	del0 = temp/(a0*a0);
	xnodp = m_Sat.fMeanMotion/(1.0+del0);
	if (2.0*PI/xnodp >= 225.0) ideep = 1;
	else ideep = 0;
	iflag = 1;
	
	m_bEclipsed = FALSE;
	sr = 696000.0;		// {Solar radius - kilometers (IAU 76)}
	AU = 1.49597870E8;	// {Astronomical unit - kilometers (IAU 76)}
}

void CSGP4_SDP4::SetSatellite(SATELLITE *pSatellite, bool bInitSatellite)
{
	int iEpochYear;
	SATELLITE *pSat = (SATELLITE *)pSatellite;
	memcpy(m_Sat.cSatelliteName, pSat->cSatelliteName, 22);
// Now everything is in butter !
	CString		csHelp;
	m_Sat.iSatelliteNumber		= pSat->iSatelliteNumber;
	m_Sat.iLaunchYear			= pSat->iLaunchYear;
	m_Sat.iLaunchNumber			= pSat->iLaunchNumber;
	memcpy(m_Sat.cLaunchPiece, pSat->cLaunchPiece, 3);
	m_Sat.iEpochYear			= pSat->iEpochYear;
	m_Sat.iEpochDay				= pSat->iEpochDay;
	m_Sat.fEpochDay				= pSat->fEpochDay;
	m_Sat.fEpochFraction		= pSat->fEpochFraction;
	m_Sat.fBalisticCoefficient	= pSat->fBalisticCoefficient;
	m_Sat.fSecondMeanMotion		= pSat->fSecondMeanMotion;
	m_Sat.iSecondMeanMotion		= pSat->iSecondMeanMotion;
	m_Sat.fRadiationCoefficient = pSat->fRadiationCoefficient;
	memcpy(m_Sat.cEmphemeristType, pSat->cEmphemeristType,2);
	m_Sat.iElementNumber		= pSat->iElementNumber;

	m_Sat.fInclination			= pSat->fInclination;
	m_Sat.fRightAscending		= pSat->fRightAscending;
	m_Sat.fEccentricity			= pSat->fEccentricity;
	m_Sat.fPeregee				= pSat->fPeregee;
	m_Sat.fMeanAnomaly			= pSat->fMeanAnomaly;
	m_Sat.fMeanMotion			= pSat->fMeanMotion;
	m_Sat.iRevAtEpoch			= pSat->iRevAtEpoch;

	iEpochYear = (int)(m_Sat.iEpochYear/100.0)*100;	// convert 1996 to 1900
	iEpochYear = m_Sat.iEpochYear - iEpochYear;		// but i need 96
	m_Sat.fJulianEpoch = JulianDate (iEpochYear*1000.0+m_Sat.iEpochDay+m_Sat.fEpochFraction);
	if (bInitSatellite)
		InitSatellite();
	return;
};





