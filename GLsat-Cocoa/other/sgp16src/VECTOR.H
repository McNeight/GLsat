#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <math.h> 
#include <float.h> 
#include <errno.h> 

#ifdef __cplusplus
extern "C" 
#endif

/* Already defined in  >stdafx.h<
typedef struct	tagVECTOR	{ 
	double x,y,z,w;
} VECTOR;
*/
#define M_PI PI
#define M2PI (2 * M_PI)
#define RTD (360 / M2PI)

#define REarth 6378140.0                /* meters of earth mean radius */
#define Ellipticity_of_Earth 0.00335281 /* Wertz p 99 eqn 4.13 */
#define AU 1.49597870E+11               /* meters to an astronomical unit*/
#define RMOON 1738.0E+03                /* moon radius in meters */
#define RSUN 696000.0E+03               /* sun radius in meters */
#define GMe 3.986005E+14                /* m^3/s^2 Wertz p 827 */ 
#define GMs 1.32712438E+20              /* m^3/s^2 Wertz p 827 */ 
#define GMm 4.902792965E+12             /* m^3/s^2 Note 74 */
#define m_to_ft 3.2808398950131234      /* feet per meter */
#define ft_to_nm 1.6457883369330454E-04 /* nautical miles per foot */
#define nm_to_m  1852.0                 /* meters per nautical mile */
#define speed_of_light 2.99792458E+08   /* m/s */
#define psi0_by_c 4.570E-06             /* N/m^2 Note 74: solar torque const */
#define rho0 2.1E-10                    /* kg/m^3 Note 73 */
#define h0 50000.0                      /* meters: scale factor for atmo dens*/
#define r0 240000.0                     /* altitude for rho0 */
#define lbm_to_kg 0.45359237            /* kg per lbm */ 
#define slug_to_kg 14.593902937206365   /* kg per slug */
#define gee 9.80665                     /* m/s^2 */

#define EARTHGEO 6.6103 /* radius of geosync if earth = 1.0 */
#define EARTHGEO_METERS (EARTHGEO*REarth) /* radius of geosync meters */

class CVector
{
public:
	CVector		();
	CVector		(VECTOR vIn);
	~CVector	();
public:
	VECTOR 	GetVector	();
	void 	SetVector	(VECTOR vIn);
	int		GetDepth	();
	void 	SetDepth	(int iDepth);
 


	double 	Dot		( VECTOR vIn );
	VECTOR	Cross	( VECTOR vIn );
	double 	Mag		( );
	VECTOR 	Unit	( VECTOR vIn );
	VECTOR	Diff	( VECTOR vIn );
	VECTOR 	Sum		( VECTOR vIn );
	VECTOR 	Scale	( double u );
//	void 	ToSphere( double *X, double *az, double *el, double *range );
protected:
	int 	m_iDepth;
	VECTOR	m_vector;

};



