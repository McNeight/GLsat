/*
 *  norad.h v. 01.beta 03/17/2001
 *
 *  Header file for norad.c
 */

#ifndef RWGPS_H
#define RWGPS_H 1

#include <math.h>
#include <stdio.h>
#include <string.h>

/* Two-line-element satellite orbital data */
typedef struct
{
  double
  epoch, xndt2o, xndd6o, bstar, 
  xincl, xnodeo, eo, omegao, xmo, xno;
} tle_t; 

/* Vector type stucture */
typedef struct
{
  double 
  x,y,z, /* Components in three dimensions */
    mag; /* Magnitude of vector */
} vector_t;

/* Common arguments between deep-space functions */
typedef struct
{
  double
  /* Used by dpinit part of Deep() */
  eosq,sinio,cosio,betao,aodp,theta2,sing,cosg,
  betao2,xmdot,omgdot,xnodot,xnodp,

  /* Used by dpsec and dpper parts of Deep() */
  xll,omgadf,xnode,em,xinc,xn,t,

  /* Used by thetg and Deep() */
  ds50;
} deep_arg_t;

/* Table of constant values */
#define de2ra    1.74532925E-2
#define pi       3.1415926535898
#define pio2     1.5707963267949
#define x3pio2   4.71238898
#define twopi    6.2831853071796
#define e6a      1.0E-6
#define tothrd   6.6666667E-1
#define xj2      1.0826158E-3
#define xj3     -2.53881E-6     
#define xj4     -1.65597E-6
#define xke      7.43669161E-2
#define xkmper   6.378135E3
#define xmnpda   1.44E3
#define ae       1.0
#define ck2      5.413079E-4
#define ck4      6.209887E-7
#define f        3.352779E-3
#define ge       3.986008E5 
#define s        1.012229
#define qoms2t   1.880279E-09
#define secday   8.6400E4
#define omega_E  1.0027379
#define omega_ER 6.3003881
#define zns      1.19459E-5
#define c1ss     2.9864797E-6
#define zes      0.01675
#define znl      1.5835218E-4
#define c1l      4.7968065E-7
#define zel      0.05490
#define zcosis   0.91744867
#define zsinis   0.39785416
#define zsings  -0.98088458
#define zcosgs   0.1945905
#define zcoshs   1
#define zsinhs   0
#define q22      1.7891679E-6
#define q31      2.1460748E-6
#define q33      2.2123015E-7
#define g22      5.7686396
#define g32      0.95240898
#define g44      1.8014998
#define g52      1.0508330
#define g54      4.4108898
#define root22   1.7891679E-6
#define root32   3.7393792E-7
#define root44   7.3636953E-9
#define root52   1.1428639E-7
#define root54   2.1765803E-9
#define thdt     4.3752691E-3
#define rho      1.5696615E-1
#define dpinit   1 /* Deep-space initialization code */
#define dpsec    2 /* Deep-space secular code        */
#define dpper    3 /* Deep-space periodic code       */


/* Flow control flag definitions */
#define ALL_FLAGS              -1
#define  SGP_INITIALIZED_FLAG  0x0001
#define SGP4_INITIALIZED_FLAG  0x0002
#define SDP4_INITIALIZED_FLAG  0x0004
#define SGP8_INITIALIZED_FLAG  0x0008
#define SDP8_INITIALIZED_FLAG  0x0010
#define SIMPLE_FLAG            0x0020
#define DEEP_SPACE_EPHEM_FLAG  0x0040
#define LUNAR_TERMS_DONE_FLAG  0x0080
#define NEW_EPHEMERIS_FLAG     0x0100
#define DO_LOOP_FLAG           0x0200
#define RESONANCE_FLAG         0x0400
#define SYNCHRONOUS_FLAG       0x0800
#define EPOCH_RESTART_FLAG     0x1000

/* Funtion prototypes */
/* main.c */
int    main(void);
/* norad.c */
void   SGP (double tsince, tle_t *tle, vector_t *pos, vector_t *vel );
void   SGP4(double tsince, tle_t *tle, vector_t *pos, vector_t *vel);
void   SGP8(double tsince, tle_t *tle, vector_t *pos, vector_t *vel);
void   SDP4(double tsince, tle_t *tle, vector_t *pos, vector_t *vel);
void   SDP8(double tsince, tle_t *tle, vector_t *pos, vector_t *vel);
void   Deep(int ientry, tle_t *tle, deep_arg_t *deep_arg);
double ThetaG(double epoch, deep_arg_t *deep_arg);
double Julian_Date_of_Year(double year);
double AcTan(double sinx, double cosx);
double FMod2p(double x);
double Modulus(double arg1, double arg2);
void   select_ephemeris(tle_t *tle);
int    isFlagSet(int flag);
int    isFlagClear(int flag);
void   SetFlag(int flag);
void   ClearFlag(int flag);

#endif
