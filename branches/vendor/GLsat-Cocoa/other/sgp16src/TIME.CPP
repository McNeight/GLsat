#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "math.h"
#include "sgpsdp.h" 

double CSGP4_SDP4::JulianDate (SYSTEMTIME st)
{
	double fDay, fDate;
	int iYear  = (int)st.wYear;
	int iMonth = (int)st.wMonth;
	int iDay   = (int)st.wDay;
	int iHour  = (int)st.wHour;
	int iMin   = (int)st.wMinute;
	int iSec   = (int)st.wSecond;
	
	int iEpocheYear = EpocheYear(iYear) * 1000; // 1999 to 99000 and 2032 to 32000.
	int iDayOfYear = DayOfYear(iYear, iMonth, iDay);
	double fFraction = FractionOfDay(iHour, iMin, iSec);

	fDate = iEpocheYear + iDayOfYear + fFraction;
	fDay = JulianDate(fDate);
	return fDay;	// The JulianDate of the CTime Date
}

double CSGP4_SDP4::JulianDate (CTime ct)
{
	// CTime is defined between 1970 and 2038 (Microsoft)
	double fDay, fDate;
	int iYear  = (int)ct.GetYear();
	int iMonth = (int)ct.GetMonth();
	int iDay   = (int)ct.GetDay();
	int iHour  = (int)ct.GetHour();
	int iMin   = (int)ct.GetMinute();
	int iSec   = (int)ct.GetSecond();

	int iEpocheYear = EpocheYear(iYear) * 1000; // 1999 to 99000 and 2032 to 32000.
	int iDayOfYear = DayOfYear(iYear, iMonth, iDay);
	double fFraction = FractionOfDay(iHour, iMin, iSec);

	fDate = iEpocheYear + iDayOfYear + fFraction;
	fDay = JulianDate(fDate);
	return fDay;	// The JulianDate of the CTime Date
}

double CSGP4_SDP4::JulianDate (double st)
// Astronomical Formulae for Calculators, Jean Meeus, pages 23-25
// Calculate Julian Date of 0.0 Jan year
{
// input like : 99200.27104438 => 1999, 200.27104438 days.
/*	double dYear;
	int iYear = (int)(st/1000.0);
	st -= iYear * 1000;
	if (iYear < 50) iYear +=1999;
	else iYear +=1899;
old time ... */
// Modification to support Y2K }
// Valid 1957 through 2056 }
	double fYear, fDay;
	int iYear = (int)(st/1000.0);
	fDay = st - iYear * 1000.0; // = 99200,271 - 99000.0 = 200,271
	if (iYear < 57) iYear += 2000;
	else iYear += 1900;
	fYear = JulianDateOfYear(iYear);
	return fYear + fDay; 
}

double CSGP4_SDP4::JulianDateOfYear (int yr)
{
	long A,B;
	int iYear = yr - 1;
	double fYear;
	A = (long)(iYear/100.0);
	B = 2 - A + (long)(A/4.0);
	fYear = (long)(365.25 * iYear)
          + 428.0 + 1720994.5 + B;
	return fYear;
}

int CSGP4_SDP4::EpocheYear (int iYear)
{	// returns 99 for 1999 and 23 for 2023
	int iEpocheYear;
	if (iYear > 2000)
		iEpocheYear = iYear - 2000;
	else 
		iEpocheYear = iYear - 1900;
	return iEpocheYear;
}

int CSGP4_SDP4::DayOfYear (int yr, int mo, int dy)
{
	// yr=1999 / mo=12 / dy=24 => 12/24/1999=christmas 1999
	// December is never used since the last Month of a year
	// is added by getting the days from that month.
    int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int i, iDay;

	iDay = 0;
	for (i=0; i < mo - 1; i++)	// mo-2 because zero based and just add until last month
		iDay += days[i];
	iDay += dy;

	if	(((yr %4) == 0) &&	// every four years
		(((yr %100) != 0) || ((yr %400) == 0)) &&	// but not at centuries other then every 400 years
		(mo > 2)) iDay += 1;		// Schaltjahr ???
	return iDay;
}

double CSGP4_SDP4::FractionOfDay (int hr, int min, int sec)
{
	double fFraction;
	fFraction = (double) (60.0*60.0*hr	+ 
						  60.0*min		+
						  (double)sec    )/(24.0*60.0*60.0);
//						  msec/100)/(24*60*60);Not precise enough for milliseconds
//  Fraction_of_Day := (hr + (min + (sec + msec/100)/60)/60)/24;
	return fFraction;
}


SYSTEMTIME CSGP4_SDP4::CalendarDate (double dJulian)
{
//  { Astronomical Formulae for Calculators, Jean Meeus, pages 26-27 }
    int		month, alpha;
    double	Z,A,B,C,D,E,F;
    double	day,year;
	double	hour, minute, second;	//, milliseconds;

	static SYSTEMTIME ret;
	F = modf(dJulian + 0.5,&Z);
	if (Z < 2299161)
		A = Z;
	else	{
		alpha = (int)((Z - 1867216.25)/36524.25);
		A = Z + 1.0 + alpha - (int)(alpha/4);
	}
	B   = A + 1524.0;
	modf((B - 122.1)/365.25, &C);
	modf(365.25 * C, &D);
	modf((B - D)/30.6001, &E);
	day = B - D - (int)(30.6001 * E) + F;
	
	if (E < 13.5)	month = round(E - 1.0);
	else			month = round(E - 13.0);
	if (month > 2.5)year  = C - 4716.0;
	else			year  = C - 4715.0;

	A = modf(day, &Z);		// A = fraction of day ...
	A = modf(A*24.0, &hour);
	A = modf(A*60.0, &minute);
	A = modf(A*60.0, &second);
//	A = modf(A*100,&milliseconds);

	ret.wYear			= (WORD)year;
	ret.wMonth			= (WORD)month;
	ret.wDay			= (WORD)day;
	ret.wHour			= (WORD)hour;
	ret.wMinute			= (WORD)minute;
	ret.wSecond			= (WORD)++second;
//	ret.wMilliseconds	= (WORD)milliseconds; Not enough precission in the double, Julian Date, for milliseconds ...
	ret.wMilliseconds	= 0;
	ret.wDayOfWeek		= 0;
	return ret;
}

double CSGP4_SDP4::SideralTime(double jd)
{
// Reference:  The 1992 Astronomical Almanac, page B6.
	double modulo;
	double UT,TU,GMST;
	UT   = modf(jd + 0.5, &TU);
	jd   -= UT;
	TU   = (double)(jd - 2451545.0)/36525.0;
	GMST = 24110.54841 + TU * (8640184.812866 + TU * (0.093104 - TU * 6.2E-6));
	GMST = (GMST + secday*omega_E*UT);	//%secday;

	modf (GMST/secday, &modulo);
	modulo = GMST - modulo* secday;
	if (modulo < 0.0)
		modulo += secday;
	return PI * 2.0 * modulo/secday;
}

double CSGP4_SDP4::ThetaG(double jd)
{
// Reference:  The 1992 Astronomical Almanac, page B6. 
	double	UT,TU,GMST;
  UT	= modf(jd + 0.5, &jd);
  TU	= (jd - 2451545.0)/36525.0;
  GMST	= 24110.54841 + TU * (8640184.812866 + TU * (0.093104 - TU * 6.2E-6));
  GMST	= Modulus(GMST + secday*omega_E*UT,secday);
  return (2.0*PI*GMST/secday);
}





