/*  
   Simple Tracking Program (STP)


   All the Satellite/Sun/Moon logic is based on software 
   developed by James R. Miller (G3RUH):

      PLAN13 (Version 1.30 February 24, 1993)
      Sun/Moon (private communication)
      ATTHIS (Version 2.10 June 21, 1994)

   Utilizes satellite keplerian data in nasa 2 line format or
   Kepler file format

   This code written by C. Harper (KD4QIO) (responsible for errors)

   Packet:      kd4qio@k4bft.#hsv.al.usa.na

   Internet:    harper@huntsville.sparta.com 
                kd4qio@amsat.org

   Snail Mail:  P.O Box 18786
                Huntsville Al,35804-8786

   Compiliers:  Borland Turbo C++ Version 3.1
                GNU gcc-2.4.5 

   Ideas/Software/Help from James R. Miller (G3RUH)

   Suggestions/Debugging by Mark Phillips (N2RPZ)
*/

#define Version "1.8 beta"
#define MSDOS

#define BLANK " "

/*  
    BLANKS is number of blank spaces placed prior to a line for
    prediction mode.  Allows three-hole-punch to be used

                1234567890
*/
#define BLANKS "    "

#define BasicYear 1900.0

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*  define certain things based on Unix or MS-DOS platform */

#ifdef UNIX

#define MaxSat 300

#include "stp.unx"

#define strcmpi(a,b) strcasecmp(a,b)
#define strncmpi(a,b,n) strncasecmp(a,b,n)

#else

#define MaxSat 50

#include "stp.dos"
#include <bios.h>
#include <conio.h>
#include <dos.h>

#endif

FILE *KeplerDataFile,*ModeDataFile,*NasaDataFile;
FILE *OutputFile,*RevolutionDataFile,*SelectFile;
FILE *ControlFile;
FILE *PlotFile1,*PlotFile2,*PlotFile3;

#ifdef UNIX
void ConstantDoppler(int argv, char *argc[]);
#endif

void AddQth(float Latitude, float Longitude,float Altitude, char QthName[]);
void AttHis(int Option,int argv, char *argc[]);
void CalcConstants(void);
void Celestial(int argv, char *argc[]);
void ClearTheScreen(void);
void CodeInfo(void);
void DashLine(FILE *WhereToPrint);
void DataPrint(float Currentday);
void DetermineAosLos(int Relative);
void DoCheckSum(char FileName[],int PrintFlag);
void Doppler(float Frequency);
void EclipseSatellitePredict(int argv,char *argc[]);
void EditKepler(void);
void EditNasa(void);
void FootPrintInit(void);
void FootPrint(void);
void Help(void);
void HourMinuteSecond(float time,FILE *WhereToWrite,int DayOfTheWeek);
void InitConstants(void);
void InitAntenna(void);
void InitQth(void);
void InitSatellite(void);
void InitSun(void);
void MatchQth(char WhatToMatch[]);
void MonthDay(float DN);
void MoonAzimuthElevation(int PrintOut);
void MutualRealTime(int argv,char *argc[]);
void MutualViewing(int argv,char *argc[]);
void ObserverSatelliteGeometry(void);
void PercivedSignalStrength(FILE *WhereToWrite);
void PrintHeader(int OptionFlag,float CurrentDay);
void ReadKepler(void);
void ReadKeps(void);
void ReadLocalQth(void);
void ReadMode(void);
void ReadNasa(void);
void ReadOtherQth(void);
void RealTimeDisplay(int argv,char *argc[],int WhatToShow);
void RunCheckSum(int argv, char *argc[]);
void RevolutionCorrection(void);
void SatelliteDump(void);
void SatellitePosition(void);
void SatellitePredict(int argv,char *argc[],int Option);
void SelectSatellites(void);
void SquintRangeRate(void);
void SunAzimuthElevation(int PrintOut);
void SunData(void);
void UTCLOCTime(void);
void WhatMode(void);

char *GetStringFromUser(int *m);

int CheckSum(FILE *InFile,int line[]);
int DeltTime(float time);
int GetIntNumberFromUser(int *m);
int GridLocator(char *loc,float *Latitude, float *Longitude);
int InputCheck(int minans,int maxans);
int KeyEvent(void);
int RealEndOfLine(char string[]);

float AINT(float xyz);
float ATAN2(float x,float y);
float DayNumber(float Y,float M,float D);
float FindAOS(void);
float FindLOS(void);
float FNatn(float Y,float X);
float GetFloatNumberFromUser(int *m);
float LinInt(int npt,float x[],float y[],float xin);
float LOSAngle(float AzimuthToCheck);
float P0to360(float ans);
float TemperatureOfSky(float Frequency);

char *MonthNames[] = {"January","February","March","April","May","June",
   "July","August","September","October","November","December"};

char *DayNames[] = {"Sunday","Monday","Tuesday","Wednesday","Thursday",
   "Friday","Saturday"};

#define MaxKeyWords 12
char *KeyWords[13] = {"satellite:","catalog number:","epoch time:",
                      "element set:","inclination:","ra of node:",
                      "eccentricity:","arg of perigee:",
                      "mean anomaly:","mean motion:","decay rate:",
                      "epoch rev:","checksum:"};
int LenKeyWords[MaxKeyWords+1];

typedef struct {
     int SquintOpt,NumModes,SatNum,EleSet;
     float YE,TE,IN,RA,EC,WP,MA,MM,M2,RV,N0,A0,B0,SI,CI,PC,QD,WD,DC;
     float DE,Beacon,Alat,Alon,ax,ay,az,Period;
     char SatsName[40];
               } SatRec;
SatRec SATS[MaxSat];
int NumberOfSats;

int NumberOfSatsSelected,WhichSatelliteSelected[MaxSat],UpDown[MaxSat];

typedef struct {
      float RiseDay,RiseHour,RiseMinute,RiseSecond;
      float SetDay,SetHour,SetMinute,SetSecond;
               } RSRec;
RSRec RiseSet[MaxSat];

#define MaxModes 16
typedef struct {
    int MinPhase,MaxPhase;
    char ModeStr[20];
               } ModeRec;
ModeRec Modes[MaxSat][MaxModes];

#define MaxQth 90
typedef struct {
     float LA,LO,HT,Ux,Uy,Uz,Ex,Ey,Ez,Nx,Ny,Nz,Ox,Oy,Oz;
     float V0x,V0y,V0z;
     char QthLoc[40];
               } QthRec;
QthRec Qth[MaxQth];
int NumberOfQth;

#define MaxObsAng 360
float ObsAzimuth[MaxObsAng],ObsAngle[MaxObsAng];
int NumberOfObsAngles;

int TRUE;

float ONE,ZERO,PI,TWOPI,PIOVR2,D2R,THIRD;
float RE,GM,J2,YM,YT,YG,G0,MAS0,MASD,INS,EQC1,EQC2,EQC3,WW,WE,W0;
float CNS,SNS,FL,RP,ZZ,RE2,AstroUnit;

float Year,Month,Day,Hour,Minute,Second;

float Sx,Sy,Sz,Vx,Vy,Vz,Ax,Ay,Az;
float Rx,Ry,Rz,RangeToSatellite,U,E,N,Azimuth,Elevation;
float AltitudeOfSatellite,RangeRate,SquintAngle;
float RevolutionNumber,CurrentMAd,CurrentMA;
float AP,RAAN;
float MeanAnomaly;

float CXx,CXy,CXz,CYx,CYy,CYz,CZx,CZy,CZz;
float SATx,SATy,SATz,ANTx,ANTy,ANTz,VELx,VELy,VELz;

int MonthOld,DayOld;

float HourOld,MinuteOld,SecondOld,RevolutionOld;
float TimeSinceEpoch,DayNow,TimeNow;
float LocalDay,LocalYear;

float DayStart;

float TempDay,TempHour,TempMin,TempSec;

float Alat,Alon;

float GHAA,GHAE,MRSE,MASE;
float Illumination,SunAzimuth,SunElevation;
float SUNx,SUNy,SUNz,SinOfSunAngle;
float CurrentSunAz,CurrentSunEl,CurrentMoonAz,CurrentMoonEl;
double DistanceToSun,DistanceToMoon;

float LO1,LO2,LO3,LO4,LO5,LO6,LO7,LO8,LO9,LO10,LO11,LO12;
float DM0,LA1,LA2,LA3,LA4,LA5,LA6,LA7;
float HP0,HP1,HP2,HP3,HP4,HP5,LM0,LMD,MA0,MAD,F0,FD,D0,DD;
float MoonEpoch;

int GMTmon,GMTdaym,GMTdayw,GMTdayy,GMTyear,GMThour,GMTmin,GMTsec;
int LOCmon,LOCdaym,LOCdayw,LOCdayy,LOCyear,LOChour,LOCmin,LOCsec;

float JulianDate;

int WhichObserver,WhichSatellite;
int MonthPointer,DayPointer;

int LengthOfLine;

char line1[80],line2[80],line3[80],SatName[40];
char FileName[80],CurrentMode[5],ECL[6];

int RunningCounter,RunningCounterOld;
float Step,Minutes,Seconds;

float FindStep,FindAngleError,FindStepMinimum;

float FrequencyModeB,FrequencyModeS;
float BandWidthModeB,BandWidthModeS;
float ReceiveGainModeB,ReceiveGainModeS;
float ReceiveNoiseModeB,ReceiveNoiseModeS;
float AverageNumberOfUsers;

#define MaxFootprint 45
float SinOfA[MaxFootprint],CosOfA[MaxFootprint];
float FootprintLat[MaxFootprint],FootprintLon[MaxFootprint];
int NumberOfFootprint;
 
/*  
   flags....

    PlusMinus     0 use 0-360 on azimuth output
                  1 use -180 to 180

    PlotFlag      0 no plot files generated
                  1 dump data files time,az,el,lat,lon,alt

    GroundTrace   0 plot file contains lat,lon,alt
                  1 plot file contains lat,lon,0
*/

int PlusMinus,PlotFlag,GroundTrace;

char PlotTemp[80];
int NumberOfPlots;

#define SGN(x) ((x) < 0 ? (-1.0) : (1.0))
#define ABS(x) ((x) < 0 ? (-(x)) : (x))

main(int argv, char *argc[])
{
   int Option;

/*  initialize and compute constants */

   InitConstants();
   CalcConstants();

/*  get local qth info */

   ReadLocalQth();

/*  if no command line arguments dump out info on code */

   if (argv <= 1)CodeInfo();

/*  read keplerian elements */

   ReadKeps();

/*  read mode file */

   ReadMode();

/*  what does the user want */

   switch (*argc[1]){
      case 'A':
         Option = 0;
         DetermineAosLos(Option);
         break;

      case 'a':
         Option = 1;
         DetermineAosLos(Option);
         break;

      case 'c':
         Celestial(argv,argc);
         break;

      case 'C':
         RunCheckSum(argv,argc);
         break;

      case 'd':
         SatelliteDump();
         break;
         
      case 'e':
         if (KeplerDataFile == NULL){
            EditNasa();
         }else{
            EditKepler();
         }
         break;

      case 'E':
         EclipseSatellitePredict(argv,argc);
         break;

      case 'h':
         Help();
         break;

      case 'j':
         AttHis(0,argv,argc);
         break;

      case 'J':
         AttHis(1,argv,argc);
         break;

      case 'm':
         MutualViewing(argv,argc);
         break;

      case 'M':
         MutualRealTime(argv,argc);
         break;

      case 'p':
         Option=0;
         SatellitePredict(argv,argc,Option);
         break;
      
      case 'P':
         Option=1;
         SatellitePredict(argv,argc,Option);
         break;
      
      case 'r':
         Option = 0;
         RealTimeDisplay(argv,argc,Option);
         break;

      case 'R':
         Option = 1;
         RealTimeDisplay(argv,argc,Option);
         break;

      case 's':
         Option = 2;
         RealTimeDisplay(argv,argc,Option);
         break;

      case 'S':
         SelectSatellites();
         break;

      default:
         printf (" Cant Match Option %s\n",argc[1]);
         exit(-1);
   }

   return 0;
}
void AddQth(float Latitude, float Longitude, float Altitude, char QthName[])
/* 
  add additional Qth information

  Latitude   Degrees
  Longitude  Degrees
  Altitude   Meters
*/
{
   float D,SL,CL,Rx,Rz,SO,CO;

   NumberOfQth++;

   Qth[NumberOfQth].LA = Latitude/D2R;
   Qth[NumberOfQth].LO = Longitude/D2R;
   Qth[NumberOfQth].HT = Altitude/1000.0;

   strcpy(Qth[NumberOfQth].QthLoc,QthName);

   CL = cos(Qth[NumberOfQth].LA);
   SL = sin(Qth[NumberOfQth].LA);
   CO = cos(Qth[NumberOfQth].LO);
   SO = sin(Qth[NumberOfQth].LO);

   D = sqrt(RE2*CL*CL + ZZ*SL*SL);
   Rx = RE2/D + Qth[NumberOfQth].HT;
   Rz = ZZ/D + Qth[NumberOfQth].HT;

/* observers unit vectors UP EAST and NORTH in geocentric coords */

   Qth[NumberOfQth].Ux = CL*CO;
   Qth[NumberOfQth].Ex = -SO;
   Qth[NumberOfQth].Nx = -SL*CO;
   Qth[NumberOfQth].Uy = CL*SO;
   Qth[NumberOfQth].Ey = CO;
   Qth[NumberOfQth].Ny = -SL*SO;
   Qth[NumberOfQth].Uz = SL;
   Qth[NumberOfQth].Ez = ZERO;
   Qth[NumberOfQth].Nz = CL;

/* observers XYZ coords at earths surface */

   Qth[NumberOfQth].Ox = Rx*Qth[NumberOfQth].Ux;
   Qth[NumberOfQth].Oy = Rx*Qth[NumberOfQth].Uy;
   Qth[NumberOfQth].Oz = Rz*Qth[NumberOfQth].Uz;

/* observers velocity in geocentric coords (vz=0) */

   Qth[NumberOfQth].V0x = -Qth[NumberOfQth].Oy*W0;
   Qth[NumberOfQth].V0y =  Qth[NumberOfQth].Ox*W0;
   Qth[NumberOfQth].V0z = ZERO;
}
float AINT(float xyz)
{
/* mimic the aint function */

   return xyz-fmod(xyz,ONE);
}
float ATAN2(float x,float y)
/* 
 arc tan function with user controlled option to return +- pi or 0-2pi
*/
{
   float temp;

   if (PlusMinus){
      temp = atan2(x,y);
   }else{
      temp = atan2(x,y);
      if (temp < ZERO){
         temp = TWOPI + temp;
      }
   }
   return temp;
}
void AttHis(int Option,int argv, char *argc[])
/*  compute current attitude given an inital set */
{
   FILE *AttHisFile;

   char dummy[2];
   int i,j,nc;
   float T,W,Q,CW,SW,SI,CI,CQ,SQ,A[4][4],U[4],V[4];
   float SVx,SVy,SVz;
   float StartMonth,StartDay,EndDay,StartYear;
   float TotalDays,C,S,MAS,TAS;

/*  
  if user did not include satellite name on command line
  then show what we got and ask...
*/

   if (argv <= 2){
      for (i=0;i<=NumberOfSats;i++){
         printf (" %s\n",SATS[i].SatsName);
      }

      printf (" Which Satellite from the list ?  ");

      gets(SatName);
   } else {
      strcpy(SatName,argc[2]);
   }

   nc=strlen(SatName);

/*  can we match it */

   WhichSatellite=-1;

   for (i=0;i<=NumberOfSats;i++){
      if (strnicmp(SatName,SATS[i].SatsName,nc) == 0){
         if (WhichSatellite == -1){
            WhichSatellite = i;
         }else{
            printf (" Multiple Names.  Please be more specific \n");
            exit(-1);
         }
      }
   }

   if (WhichSatellite < 0){
      printf (" Could Not Match Satellite %s in List\n",SatName);
      exit (-1);
   }else{
      printf (" Matched your input as %s\n",SATS[WhichSatellite].SatsName);
   }

/*  ask user for inital values */
   printf (" Initial Attitude <Alon Alat> (AO-10   240 -5): ");
   scanf("%f %f",&Alon,&Alat);

   printf (" Date above data valid for (AO-10  1 24 1994): ");
   scanf("%f %f %f",&StartMonth,&StartDay,&StartYear);

/*  eat carriage return */
   gets(dummy);

   Alon /= D2R;
   Alat /= D2R;

/*  initialize sun */
   InitSun();

   DayNow = DayNumber(StartYear,StartMonth,StartDay);
   
/* 
  Compute the Plane to Interial Transformation Matrix A[][] 

  use data based on current satellite epoch
*/

   T = (DayNow-SATS[WhichSatellite].DE)-SATS[WhichSatellite].TE;
   W = SATS[WhichSatellite].WP + SATS[WhichSatellite].WD*T;
   Q = SATS[WhichSatellite].RA + SATS[WhichSatellite].QD*T;

   CW = cos(W);
   SW = sin(W);
   SI = sin(SATS[WhichSatellite].IN);
   CI = cos(SATS[WhichSatellite].IN);
   CQ = cos(Q);
   SQ = sin(Q);

   W  = FNatn(SW,CW);
   Q  = FNatn(SQ,CQ);

   A[1][1] = CW*CQ - SW*CI*SQ;
   A[1][2] =-SW*CQ - CW*CI*SQ;
   A[1][3] = SI*SQ;
   A[2][1] = CW*SQ + SW*CI*CQ;
   A[2][2] =-SW*SQ + CW*CI*CQ;
   A[2][3] =-SI*CQ;
   A[3][1] = SW*SI;
   A[3][2] = CW*SI;
   A[3][3] = CI;

/* Determine attitude in Celestial coordinates AttPTOC()*/

   U[1] = cos(Alat)*cos(Alon);
   U[2] = cos(Alat)*sin(Alon);
   U[3] = sin(Alat);
 
/* Plane to Celestial Transform PTOC()*/

   for (i=1;i<=3;i++){
      V[i] = ZERO;
      for (j=1;j<=3;j++){
         V[i] = V[i] + A[i][j]*U[j];
      }
   } 
/* end of PTOC() */

   SVx = V[1];
   SVy = V[2];
   SVz = V[3];

/* finished with AttPTOC() */

/*  
  case 0 is compute for today
  case 1 is compute from today forward
*/

/*  get current time */
   UTCLOCTime();

   Year  = (float)GMTyear + BasicYear;
   Month = (float)GMTmon  + ONE;
   Day   = (float)GMTdaym;

   DayNow = DayNumber(Year,Month,Day);

/* 
  Compute the Plane to Interial Transformation Matrix A[][] 

  use data based on current satellite epoch

  compute for today 
*/

   T = (DayNow-SATS[WhichSatellite].DE)-SATS[WhichSatellite].TE;
   W = SATS[WhichSatellite].WP + SATS[WhichSatellite].WD*T;
   Q = SATS[WhichSatellite].RA + SATS[WhichSatellite].QD*T;

   CW = cos(W);
   SW = sin(W);
   SI = sin(SATS[WhichSatellite].IN);
   CI = cos(SATS[WhichSatellite].IN);
   CQ = cos(Q);
   SQ = sin(Q);

   W  = FNatn(SW,CW);
   Q  = FNatn(SQ,CQ);

   A[1][1] = CW*CQ - SW*CI*SQ;
   A[1][2] =-SW*CQ - CW*CI*SQ;
   A[1][3] = SI*SQ;
   A[2][1] = CW*SQ + SW*CI*CQ;
   A[2][2] =-SW*SQ + CW*CI*CQ;
   A[2][3] =-SI*CQ;
   A[3][1] = SW*SI;
   A[3][2] = CW*SI;
   A[3][3] = CI;

/*  Attitude in Celestial to Plane AttCTOP() */

   U[1] = SVx;
   U[2] = SVy;
   U[3] = SVz;

/*  Celestial to Plane CTOP() */

   for (i=1;i<=3;i++){
      V[i] = ZERO;
      for (j=1;j<=3;j++){
         V[i] = V[i] + A[j][i]*U[j];
      }
   } 
/*  end of CTOP() */

   Alat = asin(V[3]);
   Alon = FNatn(V[2],V[1]);

   switch (Option){
      case 0:
         TimeSinceEpoch = (DayNow  - SATS[WhichSatellite].DE) + 
                          (ZERO    - SATS[WhichSatellite].TE);       

/*  ma of sun round its orbit */
         MAS = MASE + (MASD*TimeSinceEpoch)/D2R; 

/*  suns true anomaly */
         TAS = MRSE + WW*TimeSinceEpoch
              +EQC1*sin(MAS)
              +EQC2*sin(2.0*MAS)
              +EQC3*sin(3.0*MAS);

         C = cos(TAS);
         S = sin(TAS);

/*  sun unit vector celestial coords....assuming circular orbit */
         SUNx = C;
         SUNy = S*CNS;
         SUNz = S*SNS;

/*  sin of sun angle */
         SinOfSunAngle = -(SVx*SUNx + SVy*SUNy + SVz*SUNz);

/*  illumination */

         Illumination = 100.0*sqrt(ONE-SinOfSunAngle*SinOfSunAngle);

         printf (" Current Attitude: Alat/Alon %6.1f/%6.1f",
            Alat*D2R,
            Alon*D2R);

         printf (" Sun Angle = %6.1f Illum = %6.1f\n",
            -asin(SinOfSunAngle)*D2R,
            Illumination);
         break;

      case 1:

         printf (" How Many Days Ahead to Predict: ");
         TotalDays=GetFloatNumberFromUser(&nc);

         printf (" Step Size (Days): ");
         Step=GetFloatNumberFromUser(&nc);

         StartDay = DayNow;
         EndDay   = StartDay + TotalDays;

         printf (" Filename to write output to <cr> for screen: ");

         strcpy(FileName,GetStringFromUser(&nc));

         if (nc > 0){
            if ((OutputFile = fopen(FileName,"w")) == 0){
               printf ("Can't open file %s\n",FileName);
               exit(-1);
            }
            fprintf (OutputFile,"        Date         Amsat");
            fprintf (OutputFile,"   Alat   Alon   SunAng  Illu\n");
         }else{
            OutputFile = stdout;
         }

         AttHisFile = fopen ("atthis.dat","w");

         fprintf (AttHisFile,"5\nAmsat Day\nAlat\nAlon\n");
         fprintf (AttHisFile,"Sun Angle\nIllumination\n");

         for (DayNow=StartDay;DayNow<=EndDay;DayNow=DayNow+Step){

            T = (DayNow-SATS[WhichSatellite].DE)-SATS[WhichSatellite].TE;
            W = SATS[WhichSatellite].WP + SATS[WhichSatellite].WD*T;
            Q = SATS[WhichSatellite].RA + SATS[WhichSatellite].QD*T;

            CW = cos(W);
            SW = sin(W);
            SI = sin(SATS[WhichSatellite].IN);
            CI = cos(SATS[WhichSatellite].IN);
            CQ = cos(Q);
            SQ = sin(Q);

            W  = FNatn(SW,CW);
            Q  = FNatn(SQ,CQ);
         
            A[1][1] = CW*CQ - SW*CI*SQ;
            A[1][2] =-SW*CQ - CW*CI*SQ;
            A[1][3] = SI*SQ;
            A[2][1] = CW*SQ + SW*CI*CQ;
            A[2][2] =-SW*SQ + CW*CI*CQ;
            A[2][3] =-SI*CQ;
            A[3][1] = SW*SI;
            A[3][2] = CW*SI;
            A[3][3] = CI;

/*  Attitude in Celestial to Plane AttCTOP() */

            U[1] = SVx;
            U[2] = SVy;
            U[3] = SVz;
         
         /*  Celestial to Plane CTOP() */

            for (i=1;i<=3;i++){
               V[i] = ZERO;
               for (j=1;j<=3;j++){
                  V[i] = V[i] + A[j][i]*U[j];
               }
            } 
/*  end of CTOP() */

            Alat = asin(V[3]);
            Alon = FNatn(V[2],V[1]);

            MonthDay(DayNow);

            TimeSinceEpoch = (DayNow  - SATS[WhichSatellite].DE) + 
                             (ZERO    - SATS[WhichSatellite].TE);       

/*  ma of sun round its orbit */
            MAS = MASE + (MASD*TimeSinceEpoch)/D2R; 

/*  suns true anomaly */
            TAS = MRSE + WW*TimeSinceEpoch
                 +EQC1*sin(MAS)
                 +EQC2*sin(2.0*MAS)
                 +EQC3*sin(3.0*MAS);

            C = cos(TAS);
            S = sin(TAS);

/*  sun unit vector celestial coords....assuming circular orbit */
            SUNx = C;
            SUNy = S*CNS;
            SUNz = S*SNS;

/*  sin of sun angle */
            SinOfSunAngle = -(SVx*SUNx + SVy*SUNy + SVz*SUNz);

/*  illumination */

            Illumination = 100.0*sqrt(ONE-SinOfSunAngle*SinOfSunAngle);

            fprintf (OutputFile," %9s %2.0f %4.0f",
               MonthNames[MonthPointer],
               LocalDay,
               LocalYear);

            fprintf (OutputFile," %8.1f %6.1f %6.1f %6.1f %6.1f\n",
               DayNow - 722100.0,
               Alon*D2R,
               Alat*D2R,
               -asin(SinOfSunAngle)*D2R,
               Illumination);

            fprintf (AttHisFile," %8.1f %6.1f %6.1f %6.1f %6.1f\n",
               DayNow - 722100.0,
               Alon*D2R,
               Alat*D2R,
               -asin(SinOfSunAngle)*D2R,
               Illumination);
         }

         fclose (OutputFile);
         fclose (AttHisFile);

         break;
   }
}
void CalcConstants(void)
{
   WW = TWOPI/YT;       /* earths rotation rate, radians/whole day */
   WE = TWOPI + WW;     /* earths rotation rate, radians/day */
   W0 = WE/86400.0;     /* earths rotation rate, radians/sec */

   G0 /=D2R;
 
   INS /=D2R;
   CNS = cos(INS);
   SNS = sin(INS);
 
   FL = ONE/298.257;    /* IAU-76 earths ellipsoid */
   RP = RE*(ONE-FL);
   ZZ = RP*RP;
   RE2= RE*RE;
}
void Celestial(int argv, char *argc[])
/*  return the sun/moon elevation and azimuth */
{
   float TEG,DE,TE;

   WhichObserver = 0;

/*  initailize Qth parameters */

   InitQth();

   TEG = -1.0;

/*  get update rate */

   if (argv == 3){
      Step = atof(argc[2]);
   }else{
      Step = 1.0;
   }

   while (bioskey(1) == 0){
      UTCLOCTime();
      
      Year  = (float)GMTyear + BasicYear;
      Month = (float)GMTmon  + ONE;
      Day   = (float)GMTdaym;

      DayNow = DayNumber(Year,Month,Day);

      Hour   = (float)GMThour;
      Minute = (float)GMTmin;
      Second = (float)GMTsec;

      TimeNow = (Hour + Minute/60.0 + Second/3600.0)/24.0;

/*  if TEG is undefined then bring Sun and Moon data to current time */

      if (TEG < ZERO){
         DE  = DayNow;
         TE  = TimeNow;
         TEG = DE-DayNumber(YG,ONE,ZERO)+TE;

         GHAE = G0 + TEG*WE ;
         MRSE = G0 + TEG*WW + PI;
         MASE = (MAS0 + MASD*TEG)/D2R;
      }

      TimeSinceEpoch = (DayNow  - DE) + (TimeNow - TE);

/*  rotate everything */

      GHAA = GHAE + WE*TimeSinceEpoch;

      if (DeltTime(Step)){
         ClearTheScreen();

         printf (" %02.0f:%02.0f:%02.0f",
           Hour,
           Minute,
           Second);

/*  get the sun */

         SunAzimuthElevation(1);
         printf ("\n");
/*  
  print blanks equivalent to %02.0f:%02.0f:%02.0f
                  bxx:xx:xx
*/
         printf ("         ");

/* get the moon */

         MoonAzimuthElevation(1);
         printf ("\n");

/*  save current time for output determination */

         HourOld   = Hour;
         MinuteOld = Minute;
         SecondOld = Second;
      }
   }
}
int CheckSum(FILE *InFile,int line[])
/*  determine checksum */
{
   int nlen,idum,ic,i;

   idum = 0;
   nlen = 0;

/*  
  fgetc returns the ascii code for each character it reads in

  68 characters in a line (reads up to checksum value)
*/

   for (i=1;i<=68;i++){
      ic = fgetc(InFile);

      if (ic == -1) {
         return 0;
      }

      line[nlen]=ic;
      nlen++;

/*  
  if a number....48 == 0   57 == 9 then convert to the numeric value
*/
      if (ic >= 48 && ic <= 57){
         idum=idum+ic-48;
      }

/*  
   if a + then add 2
   if a - then add 1
*/
      if (ic == 43)idum += 2;
      if (ic == 45)idum += 1;
   }

/*  mod 10 */
   idum=idum-10*(idum/10);

/*  read checksum value and convert from ascii to number */

   ic = fgetc(InFile);

   line[nlen]=ic;
   nlen++;

/*  read last character on line (carriage return) */
   ic=fgetc(InFile);

   line[nlen]=ic;

   return idum;
}
void ClearTheScreen(void)
/*  clear screen utility */
{
#ifdef UNIX
   system ("clear");
#else
   clrscr();
#endif
}
void CodeInfo(void)
/*  program information */
{
/*  send bells */
   printf ("\a\a\a\a\n");
   ClearTheScreen();

   printf (" Simple Tracking Program (STP) Version: %s\n",Version);
   printf ("\n");
   printf (" Propagation model, squint calculation, Sun");
   printf (" illumination logic\n");
   printf (" Sun/Moon position model based on software developed");
   printf (" by G3RUH.\n");
   printf ("\n");
   printf (" This implimentation by KD4QIO.\n");
   printf ("\n");
   printf (" STP is run using command line options:\n");
   printf ("\n");
   printf ("     STP <option> <parameter>\n");
   printf ("     Options are CASE sensitive... r and R are different.\n");
   printf ("\n");
   printf (" A list of options available are shown next.\n");
   printf ("\n");
   
   printf (" Press ENTER to continue\n");
   gets (line1);
   ClearTheScreen();

   printf ("  NONE Gets this list\n");
   printf ("\n");
   printf ("  a....AOS/LOS times (relative time) \n");
   printf ("  A....AOS/LOS times (absolute time) \n");
   printf ("  c....Sun/Moon real time display\n");
   printf ("  C....Perform Check Sum calculations on 2-line nasa file\n");
   printf ("           (can put filename on command line... STP C nasa.dat)\n");
   printf ("  d....Dump information about satellites (beacon freq,");
   printf (" alat,alon, # of modes)\n");
   printf ("  e....View/Edit orbital parameters\n");
   printf ("  h....Simple suggestions\n");
   printf ("  m....Mutual visibility calculation\n");
   printf ("  M....Real Time Mutual visibility.  Use the");
   printf (" following to add observers\n");
   printf ("         g-add based on grid locator\n");
   printf ("         l-add based on lat,lon,alt\n");
   printf ("         o-add based on names in qth.dat\n");
   printf ("  p....Prediction for a single satellite\n");
   printf ("  r....Real-time table of currently visible satellites,");
   printf (" additional parameter\n");
   printf ("           after r is update rate in seconds");
   printf (" (default is 1 second)\n");
   printf ("  R....Real-time table of all satellites, additional parameter");
   printf ("  after R is\n");
   printf ("           update rate in seconds (default is 1 second)\n");
   printf ("  s....Same as option R but uses a selected set of satellites\n");
   printf ("  S....Selects satellites for option s\n");
   printf ("\n");

   printf (" Press ENTER to continue\n");
   gets (line1);
   ClearTheScreen();

   printf (" EXAMPLE:\n\n");
   printf (" Real-Time Table (currently visible)...stp r 5");
   printf (" (update every 5 seconds)\n");
   printf ("\n");
   printf (" Real-Time Table (all satellites)......stp R 10");
   printf (" (update every 10 seconds)\n");
   printf ("\n");
   printf (" AOS/LOS Table (relative time).........stp a\n");
   printf ("\n");
   printf (" This version can handle up to %d Satellites",MaxSat);
   printf (" and up to %d QTH locations\n",MaxQth);
   printf ("\n");

   if (PlusMinus){
      printf (" Azimuth is NEGATIVE West of North and POSITIVE East of North\n");
      printf (" e.g. -90 is due west, +90 is due east\n");
   }else{
      printf (" Azimuth of 0 is north, 90 east, 180 south and 270 west\n");
   }
   printf ("\n");

   printf (" FindStep        = %8.2f Minutes\n",
      FindStep*1440.0);
   printf ("\n");
   printf (" FindAngleError  = %8.2f Degree(s)\n",
      FindAngleError*D2R);
   printf ("\n");
   printf (" FindStepMinimum = %8.2f Minutes\n",
      FindStepMinimum*1440.0);
   printf ("\n");

   printf (" PlotFlag is");
   if (PlotFlag){
      printf (" on\n");

      printf (" GroundTrace is");
      if (GroundTrace){
         printf (" on\n");
      }else{
         printf (" off\n");
      }
   }else{
      printf (" off\n");
   }

   exit(0);
}
float DayNumber(float Yinput,float Minput,float Dinput)
/* convert date to day number */
{
   float Y,M,D,DayNumb;
   float A,B;

/*  protect input values */

   Y = Yinput;
   M = Minput;
   D = Dinput;

/*  if january or february */

   if (M <= 2.0) {
      Y = Y - ONE;
      M = M + 12.0;
   }

   DayNumb =AINT(Y*YM) + AINT((M+ONE)*30.6001) + D;

/*  complete julian date calculation */
 
   A = AINT(Y/100.0);
   B = 2.0 - A + AINT(A/4.0);
   JulianDate = DayNumb + B + 1720994.5;
   
   return DayNumb - 428.0;
}
void DashLine(FILE *WhereToPrint)
/*  print dashed line */
{
   int i;

   for (i=0;i<=LengthOfLine;i++)fprintf (WhereToPrint,"-");
   fprintf (WhereToPrint,"\n");
}
void DataPrint(float CurrentDay)
/* print predicted data */
{
   float Altitude;

   PrintHeader(0,CurrentDay);

   fprintf (OutputFile,"%s",BLANKS);

   if (Step < ONE){
      Seconds = 60.0*(Minute-AINT(Minute));
      Minutes = AINT(Minute);
      fprintf (OutputFile,"%02.0f%02.0f%02.0f",
         Hour,
         Minutes,
         Seconds);
   }else{
      fprintf (OutputFile,"%02.0f%02.0f",
         Hour,
         Minute);
   }

/*  get mode info */
   WhatMode();

/*  if Omnidirectional antennas on ao-10/ao-13... then adjust squint angle */

   if (strncmpi(CurrentMode,"Bo",2) == 0) {
      SquintAngle = ABS(90.0 - SquintAngle);
   }

   if (PlotFlag){
      if (GroundTrace){
         Altitude = ZERO;
      }else{
         Altitude = AltitudeOfSatellite - RE;
      }

      fprintf(PlotFile1," %10.4f\t%02.0f%02.0f",
         DayNow+(Hour+Minute/60.0)/24.0-DayStart,
         Hour,
         Minute);

      fprintf(PlotFile1,"\t%8.1f\t%8.1f",
         Azimuth*D2R,
         Elevation*D2R);

      fprintf(PlotFile1,"\t%8.4f\t%8.4f\t%14.3f",
         asin(Sz/AltitudeOfSatellite)*D2R,
         ATAN2(Sy,Sx)*D2R,
         Altitude);
   }

   fprintf (OutputFile," %4.0f %4.0f %7.0f %4.0f",
      AINT(Azimuth*D2R+0.50),
      AINT(Elevation*D2R+0.50),
      AINT(RangeToSatellite+0.50),
      AINT(SquintAngle+0.50));

   Doppler(SATS[WhichSatellite].Beacon);

/* dump out mode */
   fprintf (OutputFile,"   %5.1f  %s",CurrentMAd,CurrentMode);

/* sunlit status */
   fprintf (OutputFile," %s",ECL);

/*  compute minimum signal level...only for ao-10 and ao-13 */
    PercivedSignalStrength(OutputFile);

   fprintf (OutputFile,"\n");
}
int DeltTime(float time)
/*  determine if enough time has elapsed to dump output */
{
   float dhour,dmin,dsec,tmphr,tmpmin,tmpsec;

   tmphr=Hour;
   tmpmin=Minute;
   tmpsec=Second;

   dsec  = tmpsec - SecondOld;
   if (dsec < ZERO){
      dsec += 60.0;
      tmpmin -= 1.0;
   }

   dmin  = tmpmin - MinuteOld;
   if (dmin < ZERO){
      dmin += 60.0;
      tmphr -= 1.0;
   }

   dhour = tmphr - HourOld;
   if (dhour < ZERO){
      dhour = ZERO;
   }

   if (dhour*3600.0+dmin*60.0+dsec >= time){
      return 1;
   }else{
      return 0;
   }
}
void DetermineAosLos(int Relative)
/*  control program to determine aos/los times */
{
   float Time,TimeStart,TimeAOS,TimeLOS,DtAOS,DtLOS;

/*  initailize Qth parameters */
   InitQth();

/*  set observer to local Qth */

   WhichObserver = 0;
/*  print header */

   if(Relative){
      printf ("        Sat       AOS            LOS\n");
   }else{
      printf ("        Sat          Day    AOS");
      printf ("               Day     LOS\n");
   }

/*  
  get current time....
  AOS relative to current time...
  will be zero if satellite currently visible
*/
 
   UTCLOCTime();

   Year   = (float)GMTyear + BasicYear;
   Month  = (float)GMTmon + ONE;
   Day    = (float)GMTdaym;

   Hour   = (float)GMThour;
   Minute = (float)GMTmin;
   Second = (float)GMTsec;

/*  get day number */

   DayNow = DayNumber(Year,Month,Day);

/*  current time in hours */

   TimeStart = (Hour+Minute/60.0+Second/3600.0)/24.0;

/*  loop over each satellite and determine AOS/LOS */

   for (WhichSatellite=0;WhichSatellite<=NumberOfSats;WhichSatellite++){

      TimeNow = TimeStart;

/*  compute GHAE for each satellite epoch */
      InitSun();

/*  get current satellite location */
      SatellitePosition();

/*  determine azimuth and elevation from observers location */
      ObserverSatelliteGeometry();

/*  
   if satellite is currently up (Elevation > user value)
   then AOS problem solved (e.g. 0.0)...find LOS.
*/
      if (Elevation > LOSAngle(Azimuth)){
         TimeAOS=24.0*TimeNow;
         DtAOS=ZERO;
      }else{
         TimeAOS=FindAOS();

         if (Relative){
            DtAOS=TimeAOS-24.0*TimeStart;
         }else{
            DtAOS=TimeAOS;
         }
      }

/*  find LOS */

      TimeLOS=FindLOS();

      if (Relative){
         DtLOS=TimeLOS-TimeAOS;
      }else{
         DtLOS=TimeLOS;
      }

/*  if relative time then dont print out day of the week */
      if (Relative)GMTdayw = -1;

      printf (" %12s",SATS[WhichSatellite].SatsName);

      if (TimeAOS < ZERO || TimeLOS < ZERO){
         printf (" Rise/Set Time > 2 Days from now!!\n");
      }else{
         printf (" ");
         HourMinuteSecond(DtAOS,stdout,GMTdayw);

         printf ("    ");
         HourMinuteSecond(DtLOS,stdout,GMTdayw);
         printf ("\n");
      }
   }
}
void DoCheckSum(char FileName[],int PrintFlag)
/*  driver to perform check sum computation */
{
   FILE *InFile,*OutFile;

   int i,nlen,NewValue,OldValue,line[80];

   nlen=69;

/*  open the input file */

   if ((InFile = fopen(FileName,"r")) == 0){
      printf (" Can't open file %s\n",FileName);
      exit(-1);
   }

/*  open the output file */

   if ((OutFile = fopen ("newnasa.dat","w")) == 0){
      printf (" Can't open file newnasa.dat\n");
      exit(-1);
   }

/*  read the file */

   while ((fgets(SatName,40,InFile)) != 0){
      fprintf (OutFile,"%s",SatName);

      NewValue = CheckSum(InFile,line);

      if (PrintFlag){
         OldValue = line[68] - 48;
         if (NewValue != OldValue){
            printf (" CheckSum Error for Satellite %s",SatName);
            printf ("   line 1 Old = %d  New = %d\n",
               OldValue,
               NewValue);
         }
      }

      line[68] = NewValue + 48;

      for (i=0;i<=nlen;i++){
         fprintf (OutFile,"%c",line[i]);
      }

      NewValue = CheckSum(InFile,line);

      if (PrintFlag){
         OldValue = line[68] - 48;
         if (NewValue != OldValue){
            printf (" CheckSum Error for Satellite %s",SatName);
            printf ("   line 2 Old = %d  New = %d\n",
               OldValue,
               NewValue);
         }
      }

      line[68] = NewValue + 48;

      for (i=0;i<=nlen;i++){
         fprintf (OutFile,"%c",line[i]);
      }
   }

   fclose (InFile);
   fclose (OutFile);
}
void Doppler(float Frequency)
/*  compute doppler shift */
{
   float doppler;

   doppler = -Frequency*RangeRate/299792.0;
   fprintf (OutputFile," %+8.0f",doppler);
}
void EclipseSatellitePredict(int argv,char *argc[])
/*  
  controlling routine to predict satellite eclipses

  really only makes sense for ao-13 and ao-10
*/
{
   float CurrentDay,DayEnd,NumberOfDays;
   float ElevationLimit;
   int i,nc,RecordsWritten;

/*  see if user wants local eclipse's only */

   printf (" Do you want to determine eclipses for entire");
   printf (" orbit (press ENTER)\n");
   printf (" or determine eclipses that occur while in");
   printf (" view locally (enter a 1) :");

   if (GetIntNumberFromUser(&nc) == 0){
      ElevationLimit = -200.0;
   }else{
      ElevationLimit = ZERO;
   }

   RecordsWritten = 0;

/*  set observer to home Qth */

   WhichObserver=0;

/*  
  if user did not include satellite name on command line
  then show what we got and ask...
*/

   if (argv <= 2){
      for (i=0;i<=NumberOfSats;i++){
         printf (" %s\n",SATS[i].SatsName);
      }

      printf (" Which Satellite from the list ?  ");

      gets(SatName);
   } else {
      strcpy(SatName,argc[2]);
   }

   nc=strlen(SatName);

/*  can we match it */

   WhichSatellite=-1;

   for (i=0;i<=NumberOfSats;i++){
      if (strnicmp(SatName,SATS[i].SatsName,nc) == 0){
         if (WhichSatellite == -1){
            WhichSatellite = i;
         }else{
            printf (" Multiple Names.  Please be more specific \n");
            exit(-1);
         }
      }
   }

   if (WhichSatellite < 0){
      printf (" Could Not Match Satellite %s in List\n",SatName);
      exit (-1);
   }else{
      printf (" Matched your input as %s\n",SATS[WhichSatellite].SatsName);
   }

/*  
  get local date to check input (currently only warns user of 
  back propagation) and possibly for use as input
*/
   UTCLOCTime();

   Month = (float)GMTmon + ONE;
   Day   = (float)GMTdaym;
   Year  = (float)GMTyear + BasicYear;

/*  get current day number */
   CurrentDay=DayNumber(Year,Month,Day);

   printf (" Enter Start Day (month day year) (<cr> for current date) : ");

/*  allow user option to hit return to get current date */
   strcpy(line1,GetStringFromUser(&nc));

/*  if nc > 0 then user input...otherwise use current information */

   if (nc > 0){
      sscanf(line1,"%f%f%f",&Month,&Day,&Year);
   }else{
      Month = (float)GMTmon + ONE;
      Day   = (float)GMTdaym;
      Year  = (float)GMTyear + BasicYear;
   }

/*  check for back propagation times */

   if (Month < (float)LOCmon){
      printf (" ********* Input Month < Current Month\n\n");
   }
   if (Day < (float)LOCdaym && Month <= (float)LOCmon){
      printf (" ********* Input Day < Current Day\n\n");
   }
   if (Year < BasicYear +(float)LOCyear){
      printf (" ********* Input Year < Current Year\n");
      printf ("           MA calculations will be wrong\n\n");
   }

   printf (" Number of Days for Prediction: ");
   NumberOfDays=GetFloatNumberFromUser(&nc);

   printf (" Step Size (minutes....input negative value for seconds): ");
   Step=GetFloatNumberFromUser(&nc);

/* 
   if Step is negative then user has input time step in seconds...
   convert to minutes 
*/

   if (Step < ZERO){
      Step = ABS(Step)/60.0;
   }

   printf (" FileName to write output to (cr for screen): ");

/* read filename here */

   strcpy(FileName,GetStringFromUser(&nc));

   if (nc > 0){
      if ((OutputFile = fopen(FileName,"w")) == 0){
         printf ("Can't open file %s\n",FileName);
         exit(-1);
      }
   }else{
      OutputFile = stdout;
   }

   DayStart = DayNumber(Year,Month,Day);
   DayEnd   = DayStart + NumberOfDays - ONE;

/*  initailize Qth parameters */

   InitQth();

/*  initailize sun parameters */

   InitSun();
 
/*  initailize antenna parameters */

   InitAntenna();
 
/*  loop over time */
 
   RunningCounter    = 0;
   RunningCounterOld = 0;

   for (DayNow=DayStart;DayNow<=DayEnd;DayNow++){
      for (Hour=ZERO;Hour<=23.0;Hour++){
         for (Minute=ZERO;Minute<=60.0-Step;Minute=Minute+Step){

            RunningCounter++;

            TimeNow = (Hour+Minute/60.0)/24.0;

/*  get satellite location */
            SatellitePosition();

/*  determine azimuth and elevation from observers location */
            ObserverSatelliteGeometry();

/*  
   since we want total eclipse information check constanly

   compute sun illumination 
   squint angle (antenna pointing)
*/
            SunData();

/*  if eclipsed then dump time */

            if ((strcmp(ECL," ECL ") == 0) && Elevation > ElevationLimit){
               SquintRangeRate();
               DataPrint(CurrentDay);
               RecordsWritten++;
            }
         }
      }
   }

   if (RecordsWritten == 0){
      printf (" No eclipses found during the time interval!!\n");
   }

   fclose (OutputFile);
}
void EditKepler(void)
/*  allow user to edit satellite parameters in keplerian format*/
{
   FILE *EditFile;

   char dummy[2];
   int i,ndigit,ThisSat;
   float EpochDay,tempvar;

   for (i=0;i<=NumberOfSats;i++){
      printf (" %4d %s\n",i,SATS[i].SatsName);
   }

   printf (" Which sat (0-%d) ",NumberOfSats);
   ThisSat=InputCheck(0,NumberOfSats);

   if (ThisSat <= NumberOfSats){
      EpochDay=SATS[ThisSat].DE-
              DayNumber(SATS[ThisSat].YE,ONE,ZERO)+
              SATS[ThisSat].TE;

      printf (" Satellite      : %s \n",SATS[ThisSat].SatsName);
      printf (" Catalog Number : %15d \n",SATS[ThisSat].SatNum);
      printf (" Element Set    : %15d \n",SATS[ThisSat].EleSet);
      printf (" Epoch (year)   : %15.10f \n",SATS[ThisSat].YE-BasicYear);
      printf (" Epoch (day)    : %15.10f \n",EpochDay);
      printf (" Inclination    : %15.10f \n",SATS[ThisSat].IN*D2R);
      printf (" R.A.A.N        : %15.10f \n",SATS[ThisSat].RA*D2R);
      printf (" Eccentricity   : %15.10f \n",SATS[ThisSat].EC);
      printf (" Arg of Perigee : %15.10f \n",SATS[ThisSat].WP*D2R);
      printf (" Mean Anomaly   : %15.10f \n",SATS[ThisSat].MA*D2R);
      printf (" Mean Motion    : %15.10f \n",SATS[ThisSat].MM/TWOPI);
      printf (" Decay          : %15.10f \n",SATS[ThisSat].M2/TWOPI);
      printf (" Orbit #        : %15.10f \n",SATS[ThisSat].RV);

      printf ("\n");
      printf (" press return to leave value alone..");
      printf ("  enter new value to change\n\n");

/*  get carriage return from input line */

      gets(dummy);

      printf (" Element Set    : %15d ",SATS[ThisSat].EleSet);
      tempvar=GetFloatNumberFromUser(&ndigit);
      if (ndigit > 0){
         SATS[ThisSat].EleSet=(int)tempvar;
         printf (" new value %15.10f\n",tempvar);
      }else{
         SATS[ThisSat].EleSet=SATS[ThisSat].EleSet;
      }

      printf (" Epoch (year)   : %15.10f ",SATS[ThisSat].YE-BasicYear);
      tempvar=GetFloatNumberFromUser(&ndigit);
      if (ndigit > 0){
         SATS[ThisSat].YE=tempvar+BasicYear;
         printf (" new value %15.10f\n",tempvar);
      }else{
         SATS[ThisSat].YE=SATS[ThisSat].YE;
      }

      printf (" Epoch (day)    : %15.10f ",EpochDay);
      tempvar=GetFloatNumberFromUser(&ndigit);
      if (ndigit > 0){
         SATS[ThisSat].TE=tempvar;
         printf (" new value %15.10f\n",tempvar);
      }else{
         SATS[ThisSat].TE=EpochDay;
      }

      printf (" Inclination    : %15.10f ",SATS[ThisSat].IN*D2R);
      tempvar=GetFloatNumberFromUser(&ndigit);
      if (ndigit > 0){
         SATS[ThisSat].IN=tempvar;
         printf (" new value %15.10f\n",tempvar);
      }else{
         SATS[ThisSat].IN=SATS[ThisSat].IN;
      }

      printf (" R.A.A.N        : %15.10f ",SATS[ThisSat].RA*D2R);
      tempvar=GetFloatNumberFromUser(&ndigit);
      if (ndigit > 0){
         SATS[ThisSat].RA=tempvar;
         printf (" new value %15.10f\n",tempvar);
      }else{
         SATS[ThisSat].RA=SATS[ThisSat].RA;
      }

      printf (" Eccentricity   : %15.10f ",SATS[ThisSat].EC);
      tempvar=GetFloatNumberFromUser(&ndigit);
      if (ndigit > 0){
         SATS[ThisSat].EC=tempvar;
         printf (" new value %15.10f\n",tempvar);
      }else{
         SATS[ThisSat].EC=SATS[ThisSat].EC;
      }

      printf (" Arg of Perigee : %15.10f ",SATS[ThisSat].WP*D2R);
      tempvar=GetFloatNumberFromUser(&ndigit);
      if (ndigit > 0){
         SATS[ThisSat].WP=tempvar;
         printf (" new value %15.10f\n",tempvar);
      }else{
         SATS[ThisSat].WP=SATS[ThisSat].WP;
      }

      printf (" Mean Anomaly   : %15.10f ",SATS[ThisSat].MA*D2R);
      tempvar=GetFloatNumberFromUser(&ndigit);
      if (ndigit > 0){
         SATS[ThisSat].MA=tempvar;
         printf (" new value %15.10f\n",tempvar);
      }else{
         SATS[ThisSat].MA=SATS[ThisSat].MA;
      }

      printf (" Mean Motion    : %15.10f ",SATS[ThisSat].MM/TWOPI);
      tempvar=GetFloatNumberFromUser(&ndigit);
      if (ndigit > 0){
         SATS[ThisSat].MM=tempvar;
         printf (" new value %15.10f\n",tempvar);
      }else{
         SATS[ThisSat].MM=SATS[ThisSat].MM;
      }

      printf (" Decay          : %15.10f ",SATS[ThisSat].M2/TWOPI);
      tempvar=GetFloatNumberFromUser(&ndigit);
      if (ndigit > 0){
         SATS[ThisSat].M2=tempvar;
         printf (" new value %15.10f\n",tempvar);
      }else{
         SATS[ThisSat].M2=SATS[ThisSat].M2;
      }

      printf (" Orbit #        : %15.10f ",SATS[ThisSat].RV);
      tempvar=GetFloatNumberFromUser(&ndigit);
      if (ndigit > 0){
         SATS[ThisSat].RV=tempvar;
         printf (" new value %15.10f\n",tempvar);
      }else{
         SATS[ThisSat].RV=SATS[ThisSat].RV;
      }
   }

/*  if here then open new file and dump modified data......kepler format */

   if((EditFile=fopen("kepler.tmp","w")) == 0){
      printf (" cant open kepler.tmp\n");
      exit(-1);
   }

   for (i=0;i<=NumberOfSats;i++){
      fprintf (EditFile,"Satellite: %s\n",SATS[i].SatsName);
      fprintf (EditFile,"Catalog number: %d\n",SATS[i].SatNum);

      tempvar=SATS[i].YE-BasicYear;

/*  
   if satellite we modified then dump new info...
   otherwise generate correct date
*/
      if (i == ThisSat){
         EpochDay=SATS[i].TE;
      }else{
         EpochDay=SATS[i].DE-DayNumber(SATS[i].YE,ONE,ZERO)+SATS[i].TE;
      }

      fprintf (EditFile,"Epoch time:     %02.0f%9.6f\n",tempvar,EpochDay);
      fprintf (EditFile,"Element set:    %d\n",SATS[i].EleSet);
      fprintf (EditFile,"Inclination:    %12.6f\n",SATS[i].IN*D2R);
      fprintf (EditFile,"RA of node:     %12.6f\n",SATS[i].RA*D2R);
      fprintf (EditFile,"Eccentricity:   %12.6f\n",SATS[i].EC);
      fprintf (EditFile,"Arg of perigee: %12.6f\n",SATS[i].WP*D2R);
      fprintf (EditFile,"Mean anomaly:   %12.6f\n",SATS[i].MA*D2R);
      fprintf (EditFile,"Mean motion:    %12.6f\n",SATS[i].MM/TWOPI);
      fprintf (EditFile,"Decay rate:     %12.10f\n",SATS[i].M2/TWOPI);
      fprintf (EditFile,"Epoch rev:      %7.0f\n",SATS[i].RV);
      fprintf (EditFile,"\n");
   }
}
void EditNasa(void)
/*  edit data in nasa 2 line format */
{
   FILE *EditFile;

   char dummy[2];
   int ndigit,ThisSat,i,nc,idum,YearLau,LauNum;
   unsigned long int EpochRev;
   float EpochYear,EpochDay,DecayRate,MeanMotion,MeanAnomaly;
   float Inclination,RAofAN,Eccentricity,ArgPerigee,tempvar;

   printf ("\a\a\a\a\n");
   printf (" WARNING..... STP does not keep all the data stored in\n");
   printf ("              a 2-line element file.  It only keeps what\n");
   printf ("              is necessary for this formulation......\n");
   printf ("\n");

   for (i=0;i<=NumberOfSats;i++){
      printf (" %4d %s\n",i,SATS[i].SatsName);
   }

   printf (" Which sat (0-%d) ",NumberOfSats);
   ThisSat=InputCheck(0,NumberOfSats);

   if ((EditFile = fopen("nasa.tmp","w")) == 0){
      printf (" cant open nasa.tmp\n");
      exit(-1);
   }

   EpochDay    = SATS[ThisSat].DE 
                -DayNumber(SATS[ThisSat].YE,ONE,ZERO)
                +SATS[ThisSat].TE;

   printf (" Satellite      : %s \n",SATS[ThisSat].SatsName);
   printf (" Catalog Number : %15d \n",SATS[ThisSat].SatNum);
   printf (" Element Set    : %15d \n",SATS[ThisSat].EleSet);
   printf (" Epoch (year)   : %15.10f \n",SATS[ThisSat].YE-BasicYear);
   printf (" Epoch (day)    : %15.10f \n",EpochDay);
   printf (" Inclination    : %15.10f \n",SATS[ThisSat].IN*D2R);
   printf (" R.A.A.N        : %15.10f \n",SATS[ThisSat].RA*D2R);
   printf (" Eccentricity   : %15.10f \n",SATS[ThisSat].EC);
   printf (" Arg of Perigee : %15.10f \n",SATS[ThisSat].WP*D2R);
   printf (" Mean Anomaly   : %15.10f \n",SATS[ThisSat].MA*D2R);
   printf (" Mean Motion    : %15.10f \n",SATS[ThisSat].MM/TWOPI);
   printf (" Decay          : %15.10f \n",SATS[ThisSat].M2/TWOPI);
   printf (" Orbit #        : %15.10f \n",SATS[ThisSat].RV);

   printf ("\n");
   printf (" press return to leave value alone..");  
   printf ("  enter new value to change\n\n");
 
/*  get carriage return from input line */
 
   gets(dummy);
 
   printf (" Element Set    : %15d ",SATS[ThisSat].EleSet);
   tempvar=GetFloatNumberFromUser(&ndigit);
   if (ndigit > 0){
      SATS[ThisSat].EleSet=(int)tempvar;
      printf (" new value %15.10f\n",tempvar);
   }else{
      SATS[ThisSat].EleSet=SATS[ThisSat].EleSet;
   }

   printf (" Epoch (year)   : %15.10f ",SATS[ThisSat].YE-BasicYear);
   tempvar=GetFloatNumberFromUser(&ndigit);
   if (ndigit > 0){
      SATS[ThisSat].YE=tempvar+BasicYear;
      printf (" new value %15.10f\n",tempvar);
   }else{
      SATS[ThisSat].YE=SATS[ThisSat].YE;
   }

   printf (" Epoch (day)    : %15.10f ",EpochDay);
   tempvar=GetFloatNumberFromUser(&ndigit);
   if (ndigit > 0){
      SATS[ThisSat].TE=tempvar;
      printf (" new value %15.10f\n",tempvar);
   }else{
      SATS[ThisSat].TE=EpochDay;
   }

   printf (" Inclination    : %15.10f ",SATS[ThisSat].IN*D2R);
   tempvar=GetFloatNumberFromUser(&ndigit);
   if (ndigit > 0){
      SATS[ThisSat].IN=tempvar;
      printf (" new value %15.10f\n",tempvar);
   }else{
      SATS[ThisSat].IN=SATS[ThisSat].IN;
   }

   printf (" R.A.A.N        : %15.10f ",SATS[ThisSat].RA*D2R);
   tempvar=GetFloatNumberFromUser(&ndigit);
   if (ndigit > 0){
      SATS[ThisSat].RA=tempvar;
      printf (" new value %15.10f\n",tempvar);
   }else{
      SATS[ThisSat].RA=SATS[ThisSat].RA;
   }

   printf (" Eccentricity   : %15.10f ",SATS[ThisSat].EC);
   tempvar=GetFloatNumberFromUser(&ndigit);
   if (ndigit > 0){
      SATS[ThisSat].EC=tempvar;
      printf (" new value %15.10f\n",tempvar);
   }else{
      SATS[ThisSat].EC=SATS[ThisSat].EC;
   }

   printf (" Arg of Perigee : %15.10f ",SATS[ThisSat].WP*D2R);
   tempvar=GetFloatNumberFromUser(&ndigit);
   if (ndigit > 0){
      SATS[ThisSat].WP=tempvar;
      printf (" new value %15.10f\n",tempvar);
   }else{
      SATS[ThisSat].WP=SATS[ThisSat].WP;
   }

   printf (" Mean Anomaly   : %15.10f ",SATS[ThisSat].MA*D2R);
   tempvar=GetFloatNumberFromUser(&ndigit);
   if (ndigit > 0){
      SATS[ThisSat].MA=tempvar;
      printf (" new value %15.10f\n",tempvar);
   }else{
      SATS[ThisSat].MA=SATS[ThisSat].MA;
   }

   printf (" Mean Motion    : %15.10f ",SATS[ThisSat].MM/TWOPI);
   tempvar=GetFloatNumberFromUser(&ndigit);
   if (ndigit > 0){
      SATS[ThisSat].MM=tempvar;
      printf (" new value %15.10f\n",tempvar);
   }else{
      SATS[ThisSat].MM=SATS[ThisSat].MM;
   }

   printf (" Decay          : %15.10f ",SATS[ThisSat].M2/TWOPI);
   tempvar=GetFloatNumberFromUser(&ndigit);
   if (ndigit > 0){
      SATS[ThisSat].M2=tempvar;
      printf (" new value %15.10f\n",tempvar);
   }else{
      SATS[ThisSat].M2=SATS[ThisSat].M2;
   }

   printf (" Orbit #        : %15.10f ",SATS[ThisSat].RV);
   tempvar=GetFloatNumberFromUser(&ndigit);
   if (ndigit > 0){
      SATS[ThisSat].RV=tempvar;
      printf (" new value %15.10f\n",tempvar);
   }else{
      SATS[ThisSat].RV=SATS[ThisSat].RV;
   }

/*  dump out new data */

   for (i=0;i<=NumberOfSats;i++){

/*
   if satellite we modified then dump new info...
   otherwise generate correct date
*/
      if (i == ThisSat){
         EpochDay = SATS[i].TE;
      }else{
         EpochDay = SATS[i].DE
                   -DayNumber(SATS[i].YE,ONE,ZERO)
                   +SATS[i].TE;
      }     

      EpochRev    = (int)SATS[i].RV;
      EpochYear   = SATS[i].YE - BasicYear;
      DecayRate   = SATS[i].M2/TWOPI;
      MeanMotion  = SATS[i].MM/TWOPI;
      MeanAnomaly = SATS[i].MA*D2R;
      Inclination = SATS[i].IN*D2R;
      RAofAN      = SATS[i].RA*D2R;
      Eccentricity= SATS[i].EC;
      ArgPerigee  = SATS[i].WP*D2R;


/*  satellite name */

      fprintf (EditFile,"%s\n",SATS[i].SatsName);

/*  1st line */

      fprintf (EditFile,"1 ");
      fprintf (EditFile,"%5dU",
         SATS[i].SatNum);
      fprintf (EditFile," 00000A");
      fprintf (EditFile,"   %2.0f%11.8f",
         EpochYear,
         EpochDay);
      if (DecayRate < ZERO){
         fprintf (EditFile," -.%08.0f",
            ABS(DecayRate)*1.0e8);
      }else{
         fprintf (EditFile,"  .%08.0f",
            DecayRate*1.0e8);
      }
      fprintf (EditFile,"  00000-0");
      fprintf (EditFile,"  10000-3 0");
      fprintf (EditFile,"% 5d",
         SATS[i].EleSet);
      fprintf (EditFile,"0");
      fprintf (EditFile,"\n");

/*  2nd line */

      fprintf (EditFile,"2 ");
      fprintf (EditFile,"%5d",
         SATS[i].SatNum);
      fprintf (EditFile," %8.4f",
         Inclination);
      fprintf (EditFile," %8.4f",
         RAofAN);
      fprintf (EditFile," %07.0f",
         Eccentricity*1.0e7);
      fprintf (EditFile," %8.4f",
         ArgPerigee);
      fprintf (EditFile," %8.4f",
         MeanAnomaly);
      fprintf (EditFile,"% 12.8f",
         MeanMotion);
      fprintf (EditFile,"%5d",
         EpochRev*10);
      fprintf (EditFile,"0");
      fprintf (EditFile,"\n");
   }

/*  close file and reread and write checksums */
   fclose (EditFile);

   DoCheckSum("new.tmp",0);
}
float FindAOS(void)
/* 
    find acquisition time...

    StepSize algorithm based on work by NZ3F and
    WN2A (author of SatSked)

    use current elevation angle to determine stepsize
    big angle....big stepsize
*/
{
   float Stepp,TotalElapsedTime;
   float xneg,yneg;

   Stepp = FindStep*ABS(Elevation*Elevation);

/* 
   step time forward and determine elevation angle.
   when we find a elevation angle > requirment then
   we linear interpolate between last negative answer
   and this one to get the answer
*/

   TotalElapsedTime = ZERO;

   while (Elevation < ZERO){
      yneg=Elevation;
      xneg=TimeNow;

      TimeNow += Stepp;

      TotalElapsedTime += Stepp;
      if (TotalElapsedTime > 2.0){
         return -ONE;
      }

      SatellitePosition();
      ObserverSatelliteGeometry();

      Stepp = FindStep*ABS(Elevation*Elevation);
      if (Stepp < FindStepMinimum) Stepp = FindStepMinimum;
   }

/*  if within error then return */
    
    if (ABS(Elevation) < FindAngleError){
       return 24.0*TimeNow;
    }

/*  get better answer */

   TimeNow = xneg;
   Elevation = yneg;

   Stepp /= 10.0;

   while (Elevation < ZERO){
      yneg=Elevation;

      TimeNow += Stepp;

      SatellitePosition();
      ObserverSatelliteGeometry();

      Stepp = FindStep*ABS(Elevation*Elevation);
      if (Stepp < FindStepMinimum) Stepp = FindStepMinimum;
   }

   return 24.0*TimeNow;
}
float FindLOS()
/*  find out when a satellite goes below horizion limit */
{
   float Stepp,TotalElapsedTime;
  
   float xpos,ypos;

   Stepp = FindStep*ABS(Elevation*Elevation);
/* 
   step time forward and determine elevation angle.
   when we find a elevation angle < requirment then
   we linear interpolate between last positive answer
   and this one to get the answer
*/

   TotalElapsedTime = ZERO;

   while (Elevation > ZERO){

      xpos=TimeNow;
      ypos=Elevation;

      TimeNow += Stepp;

      TotalElapsedTime += Stepp;
      if (TotalElapsedTime > 2.0){
         return -ONE;
      }

      SatellitePosition();
      ObserverSatelliteGeometry();

      Stepp = FindStep*ABS(Elevation*Elevation);
      if (Stepp < FindStepMinimum) Stepp = FindStepMinimum;
   }

/*  if within limit then return */
    
    if (ABS(Elevation) < FindAngleError){
       return 24.0*TimeNow;
    }

/*  get better answer */

   TimeNow = xpos;
   Elevation = ypos;

   Stepp /= 10.0;

   while (Elevation > ZERO){
      ypos=Elevation;

      TimeNow += Stepp;

      SatellitePosition();
      ObserverSatelliteGeometry();

      Stepp = FindStep*ABS(Elevation*Elevation);
      if (Stepp < FindStepMinimum) Stepp = FindStepMinimum;
   }

   return 24.0*TimeNow;
}
float FNatn(float Y,float X)
/*  G3RUH atan function....*/
{
   float A;

   if (X == ZERO){
      A = PIOVR2*SGN(Y);
   }else{
      A = atan(Y/X);
   }
   if (X < ZERO) A += PI;
   if (A < ZERO) A += TWOPI;

   return A;
}
void FootPrintInit(void)
{
   int i;
   float A;

   NumberOfFootprint = 40;

   if (NumberOfFootprint > MaxFootprint){
      printf (" NumberofFootprint > MaxFootprint %d %d\n",
         NumberOfFootprint,
         MaxFootprint);
      exit(-1);
   }

   for (i=0;i<=NumberOfFootprint;i++){
      A = TWOPI*(float)i/(float)NumberOfFootprint;
      SinOfA[i] = sin(A);
      CosOfA[i] = cos(A);
   }
}
void FootPrint(void)
/*  determine lat,lon on ground that defines footprint of satellite */
{
   int i;
   float RadiusOfFootprint,SubSatLatitude,SubSatLongitude;
   float CosLat,CosLon,SinLat,SinLon,CosRad,SinRad,X,Y,Z,x,y,z;

/*  maximum distance from subsatellite point that can view satellite */

   RadiusOfFootprint = acos(RE/AltitudeOfSatellite);

/*  compute subsatellite point at current time */

   SubSatLatitude  = asin(Sz/AltitudeOfSatellite);
   SubSatLongitude = ATAN2(Sy,Sx);

/*  pre compute some constants */

   CosLat = cos(SubSatLatitude);
   CosLon = cos(SubSatLongitude);
   SinLat = sin(SubSatLatitude);
   SinLon = sin(SubSatLongitude);

   CosRad = cos(RadiusOfFootprint);
   SinRad = sin(RadiusOfFootprint);

/*  determine the points */

   for (i=0;i<=NumberOfFootprint;i++){

/*  point centered at 0,0 */

      X = CosRad;
      Y = SinRad*SinOfA[i];
      Z = SinRad*CosOfA[i];

/*  rotate up by latitude */

      x = X*CosLat - Z*SinLat;
      y = Y;
      z = X*SinLat + Z*CosLat;

/*  rotate around by longitude */

      X = x*CosLon - y*SinLon;
      Y = x*SinLon + y*CosLon;
      Z = z;

/*  convert to altitude and longitude */

      FootprintLat[i] = asin(Z)*D2R;
      FootprintLon[i] = ATAN2(Y,X)*D2R;

      fprintf (PlotFile2," %10.3f %10.3f %4.0f %02.0f%02.0f\n",
         FootprintLat[i],
         FootprintLon[i],
         ZERO,
         Hour,Minute);
   }
}
float GetFloatNumberFromUser(int *m)
/*
  return value of number from line....if none entered return 0.
  also return number of values on line (m)

  method suggested my eric odell
*/
{
   char numbers[40];
   int ich,n;

   n=0;

/*  
  read numbers from line and store as characters
  when you get to carriage return then finished
*/

   while ((ich=getchar()) !='\n'){
      numbers[n]=ich;
      n++;
   }

/*  put end of line mark for atof function */

   numbers[n]='\0';

   *m=n;

/*  if n > 0 then return atof(numbers), otherwise return 0.0 */

   return (n) ? atof(numbers): 0.0;
}
int GetIntNumberFromUser(int *m)
/*
  return int value of number from line....if none entered return 0.
  also return number of values on line (m)
*/
{
   char numbers[40];
   int ich,n;
 
   n=0;
 
/*
  read numbers from line and store as characters
  when you get to carriage return then finished
*/
 
   while ((ich=getchar()) !='\n'){
      numbers[n]=ich;
      n++;
   }
 
/*  put end of line mark for atoi function */
 
   numbers[n]='\0';
 
   *m=n;
 
/*  if n > 0 then return atoi(numbers), otherwise return 0 */
 
   return (n) ? atoi(numbers): 0;
}
char *GetStringFromUser(int *m)
/*
  return string from line....if none entered return 0.
  also return number of letters on line (m)
*/
{
   char garbage[132],numbers[132];
   int ich,n,l;
 
   n=0;
 
/*
  read letters from line and store.
  when you get to carriage return then finished
*/
 
   while ((ich=getchar()) !='\n'){
      garbage[n]=ich;
      n++;
   }
 
   garbage[n]='\0';
   strcpy(numbers,garbage);
 
/*  put end of line mark for atof function */

   numbers[n]='\0';                           

   *m=n;            

/*  if n > 0 then return string, otherwise return NULL */

   return (n) ? numbers : BLANK;                          
}      
int GridLocator(char *loc,float *Latitude, float *Longitude)
/*  convert grid locator to latitude and longitude */
{
   char locator[6];
   float lat,lon;
   int i,nc;

   nc=strlen(loc);

   for ( i=0; i<=5; i++ ) {
      locator[i]=tolower(loc[i]);
   }

   for ( i=0; i<=1; i++ ) {
      if ((locator[i]<'a')||(locator[i]>'r')) return 1;
   }

   for ( i=2; i<=3; i++ ) {
      if ( (locator[i]<'0')||(locator[i]>'9') ) return 1;
   }

   if (nc > 4){
      for ( i=4; i<=5; i++ ) {
         if ( (locator[i]<'a')||(locator[i]>'x') ) return 1;
      }
   }

   lat = 10.0*(locator[1] - 'a') - 90.0;
   lat += locator[3] - '0';

   lon = 20.0*(locator[0] - 'a') - 180.0;
   lon += 2.0*(locator[2] - '0');

   if (nc > 4){
      lat += (locator[5] - 'a')/24.0 + 1.0/48.0;
      lon += (locator[4] - 'a')/12.0 + 1.0/24.0;
   }

   *Latitude  = lat;
   *Longitude = lon;

   return 0;
}
void Help(void)
{
   ClearTheScreen();

   printf (" You MUST specify an argument on command line for STP to do");
   printf (" anything\n");
   printf (" Enter STP and press ENTER to see list of commands available\n");
   printf ("\n");
   printf (" For DOS machines:\n");
   printf (" Make sure environment variable TZ is set in AUTOEXEC.BAT\n");
   printf (" Example for KD4QIO (Central Time Zone)\n");
   printf ("    SET TZ=CDT6CST\n");
   printf ("\n");
   printf (" On certain machines some of the calculations");
   printf (" requested take some time.\n");
   printf (" Please be patient....\n");
   printf ("\n");
   printf (" If all else fails try the following :\n");
   printf ("\n");
   printf (" Give me a call at (205) 837-5282 x1216 during the day\n");
   printf ("\n");
   printf (" mail:\n kd4qio@amsat.org\n harper@huntsville.sparta.com\n");
   printf ("\n or\n\n");
   printf (" C.Harper\n P.O. Box 18786\n Huntsville,Al 35804-8786\n");
   printf ("\n");
}
void HourMinuteSecond(float time,FILE *WhereToWrite,int DayOfTheWeek)
/*  convert time (in decimal hours) to day:hour:minute:second format */
{
   float Temptime;

   Temptime = time;

   TempDay  = ZERO;
   TempHour = AINT(Temptime);

/*  days */

   if (TempHour > 24.0){
      TempDay  = AINT(TempHour/24.0);
      TempHour = TempHour - 24.0*TempDay;
   }

/*  minutes */
   Temptime -= (24.0*TempDay+TempHour);
   Temptime *= 60.0;

   TempMin = AINT(Temptime);
  
/*  seconds */
   Temptime -= TempMin;
   Temptime *= 60.0;

   TempSec = AINT(Temptime);

/*  if DayOfTheWeek == -999 dont print anything */

   if (DayOfTheWeek == -999)return;

/* 
   set day of the week

   if DayOfTheWeek is < 0 then user does NOT want this info
*/
   if (DayOfTheWeek >= 0){
      if (TempDay > ZERO){
         DayOfTheWeek += (int)TempDay;
         if (DayOfTheWeek > 6){
            DayOfTheWeek -= 7;
         }
      }
      fprintf (WhereToWrite,"%12s ",
      DayNames[DayOfTheWeek]);
   }else{
      fprintf (WhereToWrite,"%02.0f:",
      TempDay);
   }

   fprintf (WhereToWrite,"%02.0f:%02.0f:%02.0f",
      TempHour,
      TempMin,
      TempSec);
}
void InitConstants(void)
/* initialize constants */
{

   TRUE   = 1;

   ONE    = 1.0;
   ZERO   = 0.0;
   PI     = 3.141592654;
   TWOPI  = 2.0*PI;
   PIOVR2 = PI/2.0;
   D2R    = 180.0/PI;
   THIRD  = 1.0/3.0;

   RE     = 6378.140;       /* radius of the earth */
   GM     = 3.986E5;        /* earths gravitational constant km^3/s^2 */
   J2     = 1.08263E-3;     /* 2nd zonal coeff, earths gravity field */

   YM     = 365.25;         /* mean year, days */
   YT     = 365.2421938;    /* tropical year, days */

   YG     = 1992.0;     /*  year sun data from */
   G0     = 98.9205;

   MAS0 = 356.1230621;  /* mean anomaly for sun, deg */
   MASD = 0.98560027;   /* mean anomaly rate for sun, deg/day */
   INS  = 23.4403;      /* suns inclination, deg */

   EQC1 = 0.0334263;    /* suns equation of center terms */
   EQC2 = 0.00034915;   /* suns equation of center terms */
   EQC3 = 5.0e-6;       /* suns equation of center terms */

   AstroUnit = 1.495985e8; /*  one astronomical unit km */

/* 
   moon constants......

   Epoch.....
*/

   Month=1.0;
   Day=0.0;
   Year=1993.0;

   MoonEpoch = DayNumber(Year,Month,Day);

   LO1 = 6.288750;
   LO2 =-1.274018;
   LO3 = 0.658309;
   LO4 = 0.213616;
   LO5 =-0.185596;
   LO6 =-0.114336;
   LO7 =-0.058793;
   LO8 =-0.057212;
   LO9 = 0.053320;
   LO10=-0.045874;
   LO11= 0.041024;
   LO12=-0.034718;

   LA1= 5.128189;
   LA2= 0.280606;
   LA3=-0.277693;
   LA4=-0.173238;
   LA5= 0.055413;
   LA6= 0.046272;
   LA7= 0.032573;

   HP0=0.950724;
   HP1=0.051818;
   HP2=0.009531;
   HP3=0.007843;
   HP4=0.002824;
   HP5=0.000857;

   LM0=359.6821;
   LMD=13.17640;

   MA0=201.2434;
   MAD=13.06499;

   F0 =99.2094;
   FD =13.22935;
   D0 =80.0093;
   DD =12.19075;

   RevolutionOld = 0.0;
   MonthOld      = 0;
   DayOld        = 0;
   HourOld       = 0.0;
   MinuteOld     = 0.0;
   SecondOld     = 0.0;

/*  length of line....how many dashes to print out in prediction mode */

   LengthOfLine = 80 - strlen(BLANKS) - 7;

/*  open file and read some control flags */

   if ((ControlFile = fopen (FlagFile,"r")) == 0){
      printf (" Cant open %s\n",FlagFile);
      exit(-1);
   }

   fgets (line1,80,ControlFile);
   sscanf (line1,"%f",&FindStep);
   FindStep /= 1440.0;

   fgets (line1,80,ControlFile);
   sscanf (line1,"%f",&FindAngleError);
   FindAngleError /= D2R;

   fgets (line1,80,ControlFile);
   sscanf (line1,"%f",&FindStepMinimum);
   FindStepMinimum /= 1440.0;

   fgets (line1,80,ControlFile);
   sscanf (line1,"%d",&PlotFlag);

   fgets (line1,80,ControlFile);
   sscanf (line1,"%d",&PlusMinus);

   fgets (line1,80,ControlFile);
   sscanf (line1,"%d",&GroundTrace);

   fgets (line1,80,ControlFile);
   sscanf (line1,"%f %f",&FrequencyModeB,&FrequencyModeS);

   fgets (line1,80,ControlFile);
   sscanf (line1,"%f %f",&BandWidthModeB,&BandWidthModeS);

   fgets (line1,80,ControlFile);
   sscanf (line1,"%f %f",&ReceiveGainModeB,&ReceiveGainModeS);

   fgets (line1,80,ControlFile);
   sscanf (line1,"%f %f",&ReceiveNoiseModeB,&ReceiveNoiseModeS);

   fgets (line1,80,ControlFile);
   sscanf (line1,"%f",&AverageNumberOfUsers);

/*  convert to dB */

   AverageNumberOfUsers = 10.0*log10(AverageNumberOfUsers);

   fclose (ControlFile);

   NumberOfPlots= - 1;

}
void InitAntenna(void)
/* initailize antenna unit vector in orbit plane coordinates */
{
   float COa,SOa,CLa,SLa;

   COa = cos(SATS[WhichSatellite].Alon);
   SOa = sin(SATS[WhichSatellite].Alon);
   CLa = cos(SATS[WhichSatellite].Alat);
   SLa = sin(SATS[WhichSatellite].Alat);

   SATS[WhichSatellite].ax = -CLa*COa;
   SATS[WhichSatellite].ay = -CLa*SOa;
   SATS[WhichSatellite].az = -SLa;
}
void InitQth(void)
/* process input Qth information */
{

   float D,SL,CL,Rx,Rz,SO,CO;
   int i;

   for (i=0;i<=NumberOfQth;i++){
      Qth[i].LA /=D2R;
      Qth[i].LO /=D2R;
      Qth[i].HT /=1000.;

      CL = cos(Qth[i].LA);
      SL = sin(Qth[i].LA);
      CO = cos(Qth[i].LO);
      SO = sin(Qth[i].LO);

      D = sqrt(RE2*CL*CL + ZZ*SL*SL);
      Rx = RE2/D + Qth[i].HT;
      Rz = ZZ/D + Qth[i].HT;

/* observers unit vectors UP EAST and NORTH in geocentric coords */

      Qth[i].Ux = CL*CO;
      Qth[i].Ex = -SO;
      Qth[i].Nx = -SL*CO;
      Qth[i].Uy = CL*SO;
      Qth[i].Ey = CO;
      Qth[i].Ny = -SL*SO;
      Qth[i].Uz = SL;
      Qth[i].Ez = ZERO;
      Qth[i].Nz = CL;

/* observers XYZ coords at earths surface */

      Qth[i].Ox = Rx*Qth[i].Ux;
      Qth[i].Oy = Rx*Qth[i].Uy;
      Qth[i].Oz = Rz*Qth[i].Uz;

/* observers velocity in geocentric coords (vz=0) */

      Qth[i].V0x = -Qth[i].Oy*W0;
      Qth[i].V0y =  Qth[i].Ox*W0;
      Qth[i].V0z = ZERO;
   }
}
void InitSatellite(void)
/* compute parameters associated with keplerian data */
{
   int i;

   for (i=0;i<=NumberOfSats;i++){

      RiseSet[i].RiseDay = ZERO;
      RiseSet[i].RiseHour = ZERO;
      RiseSet[i].RiseMinute = ZERO;
      RiseSet[i].RiseSecond = ZERO;
      RiseSet[i].SetDay = ZERO;
      RiseSet[i].SetHour = ZERO;
      RiseSet[i].SetMinute = ZERO;
      RiseSet[i].SetSecond = ZERO;

      SATS[i].RA /=D2R;
      SATS[i].IN /=D2R;
      SATS[i].WP /=D2R;
      SATS[i].MA /=D2R;
      SATS[i].MM *=TWOPI;
      SATS[i].M2 *=TWOPI;

/* convert satellite epoch to day number and fraction of a day */

      SATS[i].DE = DayNumber(SATS[i].YE,ONE,ZERO) + AINT(SATS[i].TE);
      SATS[i].TE = SATS[i].TE - AINT(SATS[i].TE);

/* mean motion, rad/s */
      SATS[i].N0 = SATS[i].MM/86400.0;            
/* semi major axis, km */
      SATS[i].A0 = (GM/SATS[i].N0/SATS[i].N0);
/*  
   if the sun has been input then force the semi-major axis to the 
   correct value.
   determine approximate period (minutes)
*/
      if (strncmpi(SATS[i].SatsName,"sun",3) == 0){
         SATS[i].A0 = 149597870.0;
         SATS[i].Period = 365.0*24.0*60.0;
      }else{
         SATS[i].A0 = pow(SATS[i].A0,THIRD);         
         SATS[i].Period = 1.7374e-4*pow(SATS[i].A0,1.5);
      }

/*  escobal stuff 
      printf (" mean motion...file/1440  %15.8f",SATS[i].MM/1440.0);
      printf (" mean motion...escobal    %15.8f\n",
         0.07436574/pow(SATS[i].A0/RE,3.0/2.0));
*/

/* semi minor axis, km */
      SATS[i].B0 = SATS[i].A0*sqrt(ONE-SATS[i].EC*SATS[i].EC);

/* sin & cos of inclination */
      SATS[i].SI = sin(SATS[i].IN);
      SATS[i].CI = cos(SATS[i].IN);

/* precession rate, rad/day */
      SATS[i].PC = RE*SATS[i].A0/(SATS[i].B0*SATS[i].B0);
      SATS[i].PC = 1.5*J2*SATS[i].PC*SATS[i].PC*SATS[i].MM;        

/* node precession rate, rad/day */
      SATS[i].QD = -SATS[i].PC*SATS[i].CI;                 

/* perigee precession rate, rad/day */
      SATS[i].WD = SATS[i].PC*(5.0*SATS[i].CI*SATS[i].CI-ONE)/2.0; 

/* drag coefficient */
      SATS[i].DC = -2.0*SATS[i].M2/SATS[i].MM/3.0;         
   }
}
void InitSun(void)
/*  bring sun data to current satellite epoch */
{
   float TEG;

   TEG = SATS[WhichSatellite].DE
        -DayNumber(YG,ONE,ZERO)
        +SATS[WhichSatellite].TE;

   GHAE = G0 + TEG*WE ;
   MRSE = G0 + TEG*WW + PI;
   MASE = (MAS0 + MASD*TEG)/D2R;
}
int InputCheck(int minans,int maxans)
/*  read input and check for answer between minans and maxans */
{
   int retvar;
loop:
   scanf("%d",&retvar);
   if (retvar < minans || retvar > maxans) {
      printf (" Improper Response!!!! %d\n",retvar);
      printf (" Should be between %d and %d\n",minans,maxans);
      printf (" Try Again : ");
      goto loop;
   }
   return retvar;
}
int KeyEvent(void)
{
#ifdef MSDOS
 
   int key = bioskey(1);
 
   if (key) key = bioskey(0);
 
   return key;
 
#else
 
   return bioskey();
 
#endif
}
float LinInt(int npt,float x[],float y[],float xin)
/*  linear interpolation routine */
{
   int i,ib;
   float scale,add;

/*  less than 1st value...return 1st */
   if (xin <= x[0]){
      return y[0];
   }
 
/*  greater than last value...return last */
   if (xin >= x[npt]){
      return y[npt];
   }

/*  interpolate.....linear */

   for (i=0;i<=npt;i++){
      if (xin <= x[i]){
         ib=i-1;
         scale=(xin-x[ib])/(x[i]-x[ib]);
         add=scale*(y[i]-y[ib]);
         return y[ib]+add;
      }
   }

/*  if failed for some reason....return 0.0 */
   return ZERO;
}
float LOSAngle(float AzimuthToCheck)
/*  determine minimum line of sight angle for home qth */
{
    float Angle;
   
/*  if only one angle then return that value */

   if (NumberOfObsAngles <= 0){
      return ObsAngle[0];
   }

/*  more than one value...interpolate */
   
   Angle=AzimuthToCheck;
   if (Angle < ZERO) Angle+=360.0;

   return LinInt(NumberOfObsAngles,ObsAzimuth,ObsAngle,Angle);
}
void MatchQth(char WhatToMatch[])
/*  read qth.dat and look for a match */
{
   FILE *OQthFile;

   char DummyLine[132],RestOfLine[80];
   char *p;
   float lat,lon,alt;
   int i,nc,Ndigit;

   if ((OQthFile = fopen(DxQthFile,"r")) == 0){
      printf (" could not open file %s\n",DxQthFile);
      exit(-1);
   } 

   for (i=0;i<strlen(WhatToMatch);i++){
      WhatToMatch[i]=tolower(WhatToMatch[i]);
   }

   while (fgets(DummyLine,132,OQthFile)){

      for (i=0;i<strlen(DummyLine);i++){
         DummyLine[i]=tolower(DummyLine[i]);
      }

      nc=strlen(DummyLine);

/*  handle cases where stophere and starthere are found */

      if(nc > 10){

/*  latitude */
         p=strtok(DummyLine," ");
         lat=atof(p);

/*  longitude */
         p=strtok(NULL," ");
         lon=atof(p);

/*  altitude */
         p=strtok(NULL," ");
         alt=atof(p);

/*  grid locator */
/*
         p=strtok(NULL," ");
*/

/*  rest of string....*/
         p=strtok(NULL,"\n");

         if (strstr(p,WhatToMatch)){

            printf (" Matched %s ",p);

            if (strncmpi(GetStringFromUser(&Ndigit),"y",1) == 0){
               strncpy(line1,p,35);
               line1[35]='\0';
               AddQth(lat,lon,alt,line1);
            }
         }
      }
   } 

   fclose (OQthFile);
}
void MonthDay(float DN)
/*   
  return day of the week (monday,tuesday...) and
  month (january,febuary,...) given Day Number
*/
{
   float D,Y,Dw,Mn;

   D   = DN;
   D  += 428.0;
   Dw  = fmod(D + 5.0, 7.0);
   Y   = AINT((D - 122.1)/YM);
   D  -= AINT(Y*YM);
   Mn  = AINT(D/30.61);
   D  -= AINT(Mn*30.61);
   Mn -= ONE;

   if (Mn > 12.0){
      Mn -= 12.0;
      Y  += ONE;
   }

   MonthPointer = (3*Mn-2)/3;
   DayPointer   = (int)Dw;

   LocalDay  = D;
   LocalYear = Y;
}
void MoonAzimuthElevation(int PrintOut)
/*  compute moon azimuth and elevation at current time */
{
   float T,LM,MA,MS,F,D,LS;
   float LOMoon,LAMoon,HPMoon;
   float XMoon,YMoon,ZMoon,Xm,Ym,Zm,DECMoon,RAMoon;

   float C,S;
   float xRx,xRy,xRz,xR,xU,xE,xN;
   float Hx,Hy,Hz;

   T  = DayNow - MoonEpoch + TimeNow;

   LM = (LM0 + LMD*T)/D2R;
   MA = (MA0 + MAD*T)/D2R;
   F  = (F0  + FD*T)/D2R;
   D  = (D0  + DD*T)/D2R;

   LOMoon  = LO1*sin(MA)+LO2*sin(MA-2.0*D)+LO3*sin(2.0*D)+LO4*sin(2.0*MA)
            +LO5*sin(MS)+LO6*sin(2.0*F)+LO7*sin(2.0*(MA-D))+LO8*sin(MA+MS-2.0*D)
            +LO9*sin(MA+2.0*D)+LO10*sin(MS-2.0*D)+LO11*sin(MA-MS)+LO12*sin(D);

   LOMoon = LM + LOMoon/D2R;

   LAMoon = LA1*sin(F)+LA2*sin(MA+F)+LA3*sin(F-MA)+LA4*sin(F-2.0*D)
           +LA5*sin(F+2.0*D-MA)+LA6*sin(2.0*D-MA-F)+LA7*sin(F+2.0*D); 

   LAMoon /= D2R;

   HPMoon = HP0+HP1*cos(MA)+HP2*cos(MA-2.0*D)+HP3*cos(2.0*D)
           +HP4*cos(2.0*MA)+HP5*cos(MA+2.0*D);

   HPMoon /= D2R;

   DistanceToMoon = (double)(RE/sin(HPMoon));

   XMoon = (float)DistanceToMoon*cos(LAMoon)*cos(LOMoon);
   YMoon = (float)DistanceToMoon*cos(LAMoon)*sin(LOMoon);
   ZMoon = (float)DistanceToMoon*sin(LAMoon);

   Xm = XMoon;
   Ym = YMoon*CNS - ZMoon*SNS;
   Zm = YMoon*SNS + ZMoon*CNS;

   C = cos(-GHAA);
   S = sin(-GHAA);

/*  moon vector in geocentric coordinates */
   Hx = Xm*C - Ym*S;
   Hy = Xm*S + Ym*C;
   Hz = Zm;

/*  azimuth/elevation */

   xRx = Hx-Qth[WhichObserver].Ox;
   xRy = Hy-Qth[WhichObserver].Oy;
   xRz = Hz-Qth[WhichObserver].Oz;
   xR = sqrt(xRx*xRx+xRy*xRy+xRz*xRz);

   xRx = xRx/xR;
   xRy = xRy/xR;
   xRz = xRz/xR;

   xU = xRx*Qth[WhichObserver].Ux
       +xRy*Qth[WhichObserver].Uy
       +xRz*Qth[WhichObserver].Uz;

/*  Qth[].Ez == ZERO */

   xE = xRx*Qth[WhichObserver].Ex
       +xRy*Qth[WhichObserver].Ey;

   xN = xRx*Qth[WhichObserver].Nx
       +xRy*Qth[WhichObserver].Ny
       +xRz*Qth[WhichObserver].Nz;

   CurrentMoonAz = ATAN2(xE,xN)*D2R;
   CurrentMoonEl = asin(xU)*D2R;

   if (PrintOut){
      printf (" Moon: %7.2f %7.2f %15.3f",
         CurrentMoonAz,
         CurrentMoonEl,
         DistanceToMoon);
   }
}
void MutualRealTime(int argv,char *argc[])
/*  determine mutual window real time */
{
   char UserInput[80];
   int i,nc;
   int KeyStroke;
   float CurrentDay,DayEnd,NumberOfDays,OutputRate;
   float lat,lon,alt;

/*  set outputfile to stdout for other routines */

   OutputFile = stdout;

/*  process qth parameters */
   InitQth();

/*  handle cases where user may not follow rules....

    2 arguments....need satellite name...force update rate to 1 second
    3 arguments....decide if name or update rate
    4 arguments....hope its all correct
*/

   switch (argv){
      case 0:
      case 1:
      case 2:
         for (i=0;i<=NumberOfSats;i++){
            printf (" %s\n",SATS[i].SatsName);
         }

         printf (" Which Satellite from the list ?  ");

         gets(SatName);

         break;

      case 3:
         OutputRate=atof(argc[2]);

/*  
  if 3 arguments and numeric equivalent of argument is zero then
  assume it is a satellite name ... if not then assume user
  input update rate...get satellite name
*/
         if (OutputRate == ZERO){
            strcpy(SatName,argc[2]);
            OutputRate = ONE;
         }else{
            for (i=0;i<=NumberOfSats;i++){
               printf (" %s\n",SATS[i].SatsName);
            }

            printf (" Which Satellite from the list ?  ");

            gets(SatName);
            nc=strlen(SatName);

         }

         break;

      case 4:
         strcpy(SatName,argc[2]);
         OutputRate = atof(argc[3]);

         break;
   } 

   nc=strlen(SatName);

/*  match satellite name with current data */
 
   WhichSatellite=-1;
   for (i=0;i<=NumberOfSats;i++){
      if (strnicmp(SatName,SATS[i].SatsName,nc) == 0){
         if (WhichSatellite == -1){
            WhichSatellite = i;
         }else{
            printf (" Multiple Names.  Please be more specific \n");
            exit(-1);
         }
      }
   }

   if (WhichSatellite < 0){
      printf (" Could Not Match Satellite %s in List\n",SatName);
      exit (-1);
   }else{
      printf (" Matched your input as %s\n",
         SATS[WhichSatellite].SatsName);
   }     

/*  initailize sun parameters */
 
      InitSun();

/*  initailize antenna parameters */
 
      InitAntenna();

   while (TRUE){
/*
  get local date for possible use as input
*/
      UTCLOCTime();
 
      Month = (float)GMTmon + ONE;
      Day   = (float)GMTdaym;
      Year  = (float)GMTyear + BasicYear;

      Hour   = (float)GMThour;
      Minute = (float)GMTmin;
      Second = (float)GMTsec;

/*  look for keystroke */
      KeyStroke = KeyEvent();

      if (KeyStroke){
#ifdef MSDOS
         KeyStroke &= 0x00ff;
#endif
/*  user wants to enter an observer in the form of a grid locator */

         if ((char)KeyStroke == 'g'){
            ClearTheScreen();

            printf (" Grid Locator: ");

            strcpy(UserInput,GetStringFromUser(&nc));

            if (nc){
               if (GridLocator(UserInput,&lat,&lon) == 0){
                  AddQth(lat,lon,ZERO,UserInput);
               }else{
                  printf (" \a\aGrid Locator input error\n");
               }
            }
         }

/*  user wants to enter an observer in the form of lat,lon,alt */
         if ((char)KeyStroke == 'l'){
            ClearTheScreen();

            printf (" Latitude,Longitude,Altitude: ");

            strcpy(UserInput,GetStringFromUser(&nc));
            sscanf(UserInput,"%f %f %f",&lat,&lon,&alt);
            AddQth(lat,lon,alt,UserInput);
         }

/*  user wants to enter an observer name to match in qth.dat */
         if ((char)KeyStroke == 'o'){
            ClearTheScreen();

            printf (" City,Callsign or Grid: ");

            strcpy(UserInput,GetStringFromUser(&nc));
            MatchQth(UserInput);
         }

/* quit */
         if ((char)KeyStroke == 'q'){
            exit(0);
         }
      }

/*  get current day number */
      DayNow=DayNumber(Year,Month,Day);

      if (DeltTime(OutputRate)){

        ClearTheScreen();

/*  show user current local and GMT time */

         printf (" LOC %10s %10s %02d 19%02d %02d:%02d:%02d\n",
            DayNames[LOCdayw],
            MonthNames[LOCmon],
            LOCdaym,
            LOCyear,
            LOChour,
            LOCmin,
            LOCsec);

         printf (" GMT %10s %10s %02d 19%02d %02d:%02d:%02d\n",
            DayNames[GMTdayw],
            MonthNames[GMTmon],
            GMTdaym,
            GMTyear,
            GMThour,
            GMTmin,
            GMTsec);

         printf ("\n");

/*  current time in hours */

         TimeNow = (Hour+Minute/60.0+Second/3600.0)/24.0;
 
/*  get satellite location */
         SatellitePosition();
 
/*  get local information */
         WhichObserver = 0;

/*  determine azimuth and elevation from observers location */
         ObserverSatelliteGeometry();

/*  compute sun lit status */

         SunData();

/*  get mode info */
 
         WhatMode();

         printf (" Real Time Mutual for: %s",
            SATS[WhichSatellite].SatsName);

         printf (" MA: %5.1f Mode: %s",CurrentMAd,CurrentMode);
 
/*  sunlit status */
 
         printf (" Satel/SUN (%s)",ECL);

         printf ("\n");
         printf ("\n");

         printf ("  Az    El  Range Squnt  Doppler");
         printf ("  Status");

         printf ("\n");

         printf(" %4.0f %4.0f %6.0f",
            Azimuth*D2R,
            Elevation*D2R,
            RangeToSatellite);

/*  compute squint/rangerate */

         SquintRangeRate();

/*  if Omnidirectional antennas on ao-10/ao-13... then adjust squint angle */

         if (strncmpi(CurrentMode,"Bo",2) == 0) {
            SquintAngle = ABS(90.0 - SquintAngle);
         }
                   
         printf (" %4.0f",SquintAngle);
 
/*  compute doppler */
 
         Doppler(SATS[WhichSatellite].Beacon);

         printf ("   ");
 
         if (Elevation > ZERO){
            printf ("  Up   Home QTH");
         }else{
            printf (" Down  Home QTH");
         }

         printf ("\n");

         if (NumberOfQth > 0){
            for (i=1;i<=NumberOfQth;i++){
               WhichObserver = i;

               ObserverSatelliteGeometry();

               printf(" %4.0f %4.0f %6.0f",
                  Azimuth*D2R,
                  Elevation*D2R,
                  RangeToSatellite);

/*  compute squint/rangerate */

               SquintRangeRate();

               if (strncmpi(CurrentMode,"Bo",2) == 0) {
                  SquintAngle = ABS(90.0 - SquintAngle);
               }

               printf (" %4.0f",SquintAngle);
 
/*  compute doppler */
 
               Doppler(SATS[WhichSatellite].Beacon);

               printf ("   ");

               if (Elevation > ZERO){
                  printf ("  Up  ");
               }else{
                  printf (" Down ");
               }

               printf (" %s",
                  Qth[WhichObserver].QthLoc);

               printf ("\n");
            }
         }

         SecondOld = Second;
         MinuteOld = Minute;
         HourOld   = Hour;
      }
   }
}
void MutualViewing(int argv,char *argc[])
/*  determine mutual windows */
{
   int i,nc;
   float CurrentDay,DayEnd,NumberOfDays;
   float RangeLoc,AzimuthLoc,ElevationLoc,SquintLoc;

/*  read other qth data */
   ReadOtherQth();

   if (NumberOfQth == 0){
      printf (" NO Qth specified in qth.dat\n");
      exit(-1);
   }

/*  process qth parameters */
   InitQth();

/*  if user did not input satellite on command line then ask for input */

   if (argv <= 2){
      for (i=0;i<=NumberOfSats;i++){
         printf (" %s\n",SATS[i].SatsName);
      }

      printf (" Which Satellite from the list ?  ");

      gets(SatName);
   } else {
      strcpy(SatName,argc[2]);
   }

   nc=strlen(SatName);

/*  match satellite name with current data */
 
   WhichSatellite=-1;
   for (i=0;i<=NumberOfSats;i++){
      if (strnicmp(SatName,SATS[i].SatsName,nc) == 0){
         if (WhichSatellite == -1){
            WhichSatellite = i;
         }else{
            printf (" Multiple Names.  Please be more specific \n");
            exit(-1);
         }
      }
   }

   if (WhichSatellite < 0){
      printf (" Could Not Match Satellite %s in List\n",SatName);
      exit (-1);
   }else{
      printf (" Matched your input as %s\n",SATS[WhichSatellite].SatsName);
   }     

/*
  get local date for possible use as input
*/
   UTCLOCTime();
 
   Month = (float)GMTmon + ONE;
   Day   = (float)GMTdaym;
   Year  = (float)GMTyear + BasicYear;
 
/*  get current day number */
   CurrentDay=DayNumber(Year,Month,Day);
 
   printf (" Enter Start Day (month day year) (<cr> for current date) : ");
 
/*  allow user option to hit return to get current date */
   strcpy(line1,GetStringFromUser(&nc));
 
/*  if nc > 0 then user input...otherwise use current information */
 
   if (nc > 0){
      sscanf(line1,"%f%f%f",&Month,&Day,&Year);
   }else{
      Month = (float)GMTmon + ONE;
      Day   = (float)GMTdaym;
      Year  = (float)GMTyear + BasicYear;
   }

   printf (" Number of Days for Prediction: ");
   NumberOfDays=GetFloatNumberFromUser(&nc);
 
   printf (" Step Size (minutes....input negative value for seconds): ");
   Step=GetFloatNumberFromUser(&nc);
 
/* 
   if Step is negative then user has input time step in seconds...
   convert to minutes 
*/

   if (Step < ZERO){
      Step = ABS(Step)/60.0;
   }

   printf (" FileName to write output to (cr for screen): ");
 
/* read filename here */
 
   strcpy(FileName,GetStringFromUser(&nc));
 
   if (nc > 0){
      if ((OutputFile = fopen(FileName,"w")) == 0){
         printf ("Can't open file %s\n",FileName);
         exit(-1);
      }
   }else{
      OutputFile = stdout;
   }     
 
   DayStart = DayNumber(Year,Month,Day);
   DayEnd   = DayStart + NumberOfDays - ONE;

/*  initailize sun parameters */
 
   InitSun();

/*  initailize antenna parameters */
 
   InitAntenna();

/*  loop over time */

   RunningCounter    = 0;
   RunningCounterOld = 0;

   for (DayNow=DayStart;DayNow<=DayEnd;DayNow++){
      for (Hour=ZERO;Hour<=23.0;Hour++){
         for (Minute=ZERO;Minute<=60.0-Step;Minute=Minute+Step){

            RunningCounter++;

            TimeNow = (Hour+Minute/60.0)/24.0;
 
/*  get satellite location */
            SatellitePosition();
 
/*  get local information */
            WhichObserver = 0;

/*  determine azimuth and elevation from observers location */
            ObserverSatelliteGeometry();

            WhatMode();

            if (strncmpi(CurrentMode,"Bo",2) == 0) {
               SquintAngle = ABS(90.0 - SquintAngle);
            }
 
/* can we see it locally then check the other qth's */

            if (Elevation >= LOSAngle(Azimuth)){

               SunData();
               SquintRangeRate();

               RangeLoc     = RangeToSatellite;
               AzimuthLoc   = Azimuth;
               ElevationLoc = Elevation;
               SquintLoc    = SquintAngle;

               for (i=1;i<=NumberOfQth;i++){

                  WhichObserver = i;
                  ObserverSatelliteGeometry();

                  if (Elevation >= ZERO){
                     SunData();
                     SquintRangeRate();

                     PrintHeader(1,CurrentDay);

                     if (strncmpi(CurrentMode,"Bo",2) == 0) {
                        SquintAngle = ABS(90.0 - SquintAngle);
                        SquintLoc   = ABS(90.0 - SquintLoc);
                     }

                     fprintf (OutputFile,"%s",BLANKS);

                     if (Step < ONE){
                        Seconds = 60.0*(Minute-AINT(Minute));
                        Minutes = AINT(Minute);
                        fprintf (OutputFile,"%02.0f%02.0f%02.0f",
                           Hour,
                           Minutes,
                           Seconds);
                     }else{
                        fprintf (OutputFile,"%02.0f%02.0f",
                           Hour,
                           Minute);
                     }

                     fprintf (OutputFile," %4.0f %4.0f %7.0f %4.0f",
                        AzimuthLoc*D2R,
                        ElevationLoc*D2R,
                        RangeLoc,
                        SquintLoc);

                     fprintf (OutputFile,"  |%4.0f %4.0f %7.0f %4.0f",
                        Azimuth*D2R,
                        Elevation*D2R,
                        RangeToSatellite,
                        SquintAngle);

                     fprintf (OutputFile,"  %5.1f  %3s",
                        CurrentMAd,
                        CurrentMode);

                     fprintf (OutputFile," %s",
                        Qth[WhichObserver].QthLoc);

                     fprintf (OutputFile,"\n");
                  }
               }
            }
         }

         SecondOld = Second;
         MinuteOld = Minute;
         HourOld   = Hour;
      }
   }
 
   fclose (OutputFile);
}
void ObserverSatelliteGeometry(void)
/* determine azimuth and elevation from observer to satellite */
{
   Rx = Sx-Qth[WhichObserver].Ox;
   Ry = Sy-Qth[WhichObserver].Oy;
   Rz = Sz-Qth[WhichObserver].Oz;

   RangeToSatellite = sqrt(Rx*Rx+Ry*Ry+Rz*Rz);

   Rx = Rx/RangeToSatellite;
   Ry = Ry/RangeToSatellite;
   Rz = Rz/RangeToSatellite;

   U = Rx*Qth[WhichObserver].Ux
      +Ry*Qth[WhichObserver].Uy
      +Rz*Qth[WhichObserver].Uz;
   E = Rx*Qth[WhichObserver].Ex
      +Ry*Qth[WhichObserver].Ey;
   N = Rx*Qth[WhichObserver].Nx
      +Ry*Qth[WhichObserver].Ny
      +Rz*Qth[WhichObserver].Nz;

   Azimuth   = ATAN2(E,N);
   Elevation = asin(U);
}
float P0to360(float ans)
/*  Insure ans between 0 and 360 */
{
   float n360,m360;
 
   if (ans < 0.0){
      n360=ABS(ans)/360.0;
      m360=AINT(n360);
      return ans+m360*360.0;
   }   
   if (ans > 360.0){
      n360=ans/360.0;
      m360=AINT(n360);
      return ans-m360*360.0;
   }
   return ans;
}
void PercivedSignalStrength(FILE *WhereToWrite)
/*  estimate signal strength */
{
   float Gain,SofT,Freq,PathLoss,Ps,Pn,Tsky,SquintTerm;

/*  only perform this calculation for ao-13 and ao-10 */

   if (strncmpi(SATS[WhichSatellite].SatsName,"ao-13",5) != 0 && 
       strncmpi(SATS[WhichSatellite].SatsName,"oscar10",7) != 0 &&
       strncmpi(SATS[WhichSatellite].SatsName,"oscar 10",8) != 0 &&
       strncmpi(SATS[WhichSatellite].SatsName,"ao13sm",6) != 0){

      return;
   }

/*
   Methodology taken from Satellite Experimenters Handbook

   Predicting signal levels pg 13-12

   assumptions:

      antenna pattern modeled using 2(n+1)cos(theta)^n

       n   Gain    Half-Power
           (dBi)   BeamWidth
      -------------------------
       0   3.0       180
      0.5  4.8       151
       1   6.0       120
       2   7.8        90
       3   9.0        74.9
       4  10.0        65.5
       6  11.5        54.0
       8  12.6        47.0
      12  14.1        38.6
      16  15.3        33.5
      20  16.2        30.0
      24  17.0        27.4


   ao-13 parameters

   Mode B
     Gain is 6.0 dbi
     Half-Power Beamwidth 100 deg
     Power is 12.5 W average
     n = 1.5 (match beamwidth)
  
   Mode S
     Gain is 14.2 dbi
     Half-Power Beamwidth 39 deg
     Power is 1.25 W continous
     n = 12 (match beamwidth)
*/

/*  set gain factor for mode B antennas */

   if (strncmpi(CurrentMode,"Bo",2) == 0) {
      Gain=0.1;
   } else {
      Gain=1.5;
   }

/*  limit squint angles to 90 degrees or less */

   if (SquintAngle < 89.99){
      SquintTerm = log10(cos(SquintAngle/D2R));
   }else{
      SquintTerm = -4.7;
   }

    
/*  if Mode B, Mode Bo or Mode BS then compute gain and write out */

   SofT = ZERO;

   if (strncmpi(CurrentMode,"B",1) == 0){

/*  path loss at mode b frequency */

      PathLoss = 32.4 
                +20.0*log10(FrequencyModeB) 
                +20.0*log10(RangeToSatellite);

/* 
   40.969  is 10.0*log10(12.5*1000.0) 

   33.010  is 10.0*log10(2000)  follow same logic as G3RUH for Mode S

   This assumes you are only person on satellite.....

   Correct for number of users by subtracting off...
      10.0*log10(Number Of Users)
*/

      Ps = 33.010 - AverageNumberOfUsers
          +10*log(2.0*Gain+ONE)
          +10.0*Gain*SquintTerm
          +ReceiveGainModeB
          -PathLoss;

/*  -198.79  is 10.0*log10(boltzmans constant in mw) */

      Pn = -198.79
           +10.0*log10(ReceiveNoiseModeB + TemperatureOfSky(FrequencyModeB))
           +10.0*log10(BandWidthModeB);

      SofT = Ps - Pn;
      
      fprintf (WhereToWrite," %8.2f ",SofT);
   }

   if (PlotFlag)fprintf (PlotFile1,"\t%8.2f",SofT);

/*  perform Mode S calculation in case BS or S only  */

   PathLoss = 32.4 
             +20.0*log10(FrequencyModeS) 
             +20.0*log10(RangeToSatellite);

/* 
   30.969  is 10.0*log10(1.25*1000.0)

   23.010  is 10.0*log10(200)   G3RUH (10/1/94)... avg power

   This assumes you are only person on satellite.....

   Correct for number of users by subtracting off...
      10.0*log10(Number Of Users)

   13.9794 is 10.0*log (2*12+1)
*/

   Ps = 23.010 - AverageNumberOfUsers
       +13.9794
       +120.0*SquintTerm
       +ReceiveGainModeS
       -PathLoss;

/*  -198.79  is 10.0*log10(boltzmans constant in mw) */

   Pn = -198.79
        +10.0*log10(ReceiveNoiseModeS + TemperatureOfSky(FrequencyModeS));
        +10.0*log10(BandWidthModeS);

   SofT = Ps - Pn;

/*  if Mode S only then format slightly different from Mode BS */

   if (strncmpi(CurrentMode,"S..",3) == 0){
      fprintf (WhereToWrite,"           %8.2f ",SofT);
   }
   if (strncmpi(CurrentMode,"BS.",3) == 0){
      fprintf (WhereToWrite," %8.2f ",SofT);
   }

   if (PlotFlag)fprintf (PlotFile1,"\t%8.2f\n",SofT);

}
void PrintHeader(int OptionFlag,float CurrentDay)
/*  print header */
{
   int TimeLag;

/*  
  convert daynumber to a month (January,February,...) and 
  day (Monday,Tuesday,...)
*/
   MonthDay(DayNow);

/*  
  look for cases where we have had a gap in prediction
  so we can write a new header block
*/

   TimeLag = 0;

   if (RunningCounter - RunningCounterOld > 1)TimeLag = 1;

   RunningCounterOld = RunningCounter;

   if (MonthPointer != MonthOld || DayPointer != DayOld || TimeLag){

      if (PlotFlag){

         NumberOfPlots++;

/*  close up old files */

         if (NumberOfPlots > 0){
            fclose (PlotFile1);
            fclose (PlotFile2);
         }

/*  azimuth,elevation.... data */

         sprintf(PlotTemp,"azel%d.dat",NumberOfPlots);
         PlotFile1 = fopen (PlotTemp,"w");

         fprintf (PlotFile1,"9\nTime\nHour/Minute\nAz\nEl\nLat\nLon\nAlt\n");
         fprintf (PlotFile1,"S/N B\nS/N S\n");

/*  sub satellite footprint */

         sprintf(PlotTemp,"sbst%d.dat",NumberOfPlots);
         PlotFile2 = fopen (PlotTemp,"w");

         fprintf (PlotFile2,"4\nLat\nLon\nAlt\nTime\n");

/*  rise/set data */

         sprintf(PlotTemp,"rsst%d.dat",NumberOfPlots);
         PlotFile3 = fopen (PlotTemp,"w");

         fprintf (PlotFile3,"6\nTime\nAz\nEl\nLat\nLon\nAlt\n");

         fprintf (PlotFile3,"%02.0f%02.0f %7.2f %7.2f %10.3f %10.3f",
            Hour,Minute,
            Azimuth*D2R,
            Elevation*D2R,
            ATAN2(Sy,Sx)*D2R,
            asin(Sz/AltitudeOfSatellite)*D2R);

         if (GroundTrace){
            fprintf (PlotFile3,"%10.3f\n",ZERO);
         }else{
            fprintf (PlotFile3,"%10.3f\n",AltitudeOfSatellite-RE);
         }

         fclose (PlotFile3);
      }

      RevolutionOld = RevolutionNumber;
      MonthOld      = MonthPointer;
      DayOld        = DayPointer;

      fprintf (OutputFile,"%s",BLANKS);
      DashLine(OutputFile);

      fprintf (OutputFile,"%s",BLANKS);
      fprintf (OutputFile,"Prediction Date : %s %s %02.0f %4.0f",
         DayNames[DayPointer],
         MonthNames[MonthPointer],
         LocalDay,
         LocalYear);
      fprintf (OutputFile,"    Amsat Day: %6.0f\n",
              DayNow - 722100.0);

      fprintf (OutputFile,"%s",BLANKS);
 
/*  Qth[].QthLoc from qth.dat file will not have a carriage return at end */

      switch (OptionFlag){
         case 0:
            fprintf (OutputFile,"Prediction For  : %s",
               Qth[WhichObserver].QthLoc);
            break;

         case 1:
            if (NumberOfQth == 1){
               fprintf (OutputFile,"Prediction For  : %s\n",
                  Qth[WhichObserver].QthLoc);
            }else{
               fprintf (OutputFile,"Prediction For  : %2d Sites\n",
                  NumberOfQth);
            }
            break;
      }

      fprintf (OutputFile,"%s",BLANKS);
      fprintf (OutputFile,"Performed on    : %s %02d 19%02d %02d:%02d:%02d\n",
         MonthNames[LOCmon],
         LOCdaym,
         LOCyear,
         LOChour,
         LOCmin,
         LOCsec);

      fprintf (OutputFile,"%s",BLANKS);
      fprintf (OutputFile,"Satellite       : %s",
         SATS[WhichSatellite].SatsName);

      fprintf (OutputFile," (Set:%6d, Rev #:%6.0f, Age:%6.1f Days)",
         SATS[WhichSatellite].EleSet,
         RevolutionNumber,
         CurrentDay-(SATS[WhichSatellite].DE+SATS[WhichSatellite].TE));
      fprintf (OutputFile,"\n");

      fprintf (OutputFile,"%s",BLANKS);
      fprintf (OutputFile,"Illumination    : %3.1f%%  SAZ/SEL %4.1f/%4.1f ",
         Illumination,
         SunAzimuth*D2R,
         SunElevation*D2R);
      fprintf (OutputFile,"AP/RAAN %4.1f/%4.1f \n",
         AP*D2R,
         RAAN*D2R);

      fprintf (OutputFile,"%s",BLANKS);
      fprintf (OutputFile,"Alon/Alat       : %4.1f/%4.1f  ",
         SATS[WhichSatellite].Alon*D2R,SATS[WhichSatellite].Alat*D2R);
      fprintf (OutputFile,"Frequency: %6.4f MHz \n",
         SATS[WhichSatellite].Beacon*1.0e-6);

      fprintf (OutputFile,"%s",BLANKS);
      DashLine(OutputFile);

      switch (OptionFlag){
         case 0:
            fprintf (OutputFile,"%s",BLANKS);
            fprintf (OutputFile," UTC");
            if (Step < ONE)fprintf (OutputFile,"  ");
            fprintf (OutputFile,"   Az   El   Range  Sqnt    ");
            fprintf (OutputFile,"Dopplr    MA  Mode");
            fprintf (OutputFile,"  Sun    PRSL");
            fprintf (OutputFile,"\n");

            fprintf (OutputFile,"%s",BLANKS);
            DashLine(OutputFile);

            break;

         case 1:
            fprintf (OutputFile,"%s",BLANKS);
            fprintf (OutputFile,"            Local");
            if (Step < ONE)fprintf (OutputFile,"  ");
            fprintf (OutputFile,"            |");
            fprintf (OutputFile,"             Dx");
            fprintf (OutputFile,"\n");

            fprintf (OutputFile,"%s",BLANKS);
            fprintf (OutputFile," UTC");
            if (Step < ONE)fprintf (OutputFile,"  ");
            fprintf (OutputFile,"   Az   El   Range  Sqnt");
            fprintf (OutputFile," |");
            fprintf (OutputFile,"  Az   El   Range  Sqnt");
            fprintf (OutputFile,"   MA   Mde");
            fprintf (OutputFile,"\n");

            fprintf (OutputFile,"%s",BLANKS);
            DashLine(OutputFile);
            break;

         default:
            printf (" Cant match option %d in PrintHeader\n",
               OptionFlag);
            exit(-1);
      }
   }
}
void ReadKepler(void)
/*  read kepler data in kepler text format */
{
   char line[80],string1[80],string2[80],string3[80];

   int i,imat,nc,EpochYear;
   long int SatNum,EleSet,EpochRev;

   float EpochDay,Inclination,RAofAN,Eccentricity,ArgPerigee;
   float MeanMotion,DecayRate;

/*  get lengths of key words */

   for (i=0;i<=MaxKeyWords;i++){
      LenKeyWords[i]=strlen(KeyWords[i]);
   }

   NumberOfSats=-1;

/*  
   read through file.
   try to match to one of keywords.
*/

   while (fgets(line,80,KeplerDataFile)){
      imat=0;

      for (i=0;i<=MaxKeyWords;i++){
         if (strncmpi(line,KeyWords[i],LenKeyWords[i]) == 0){
            imat=1;
            switch (i){
               case 0:
                  NumberOfSats++;

                  if (NumberOfSats > MaxSat) {
                     NumberOfSats--;
                     printf (" Read in Maximum Number of Satellites %d\n",
                                    MaxSat);
                     printf (" Last Satellite Read in is %s\n",
                                    SATS[NumberOfSats].SatsName);
                     return;
                  }

                  sscanf(line,"%s %s",&string1,&string2);

                  nc=RealEndOfLine(string2);
                  strncpy(SATS[NumberOfSats].SatsName,string2,nc);
/*
                  SATS[NumberOfSats].SatsName = strdup(string2);
*/
 
/*  set default values for mode information */
 
                  SATS[NumberOfSats].SquintOpt = 0;
                  SATS[NumberOfSats].NumModes = 0;

                  Modes[NumberOfSats][0].MinPhase = 0;
                  Modes[NumberOfSats][0].MaxPhase = 256;

                  strcpy(Modes[NumberOfSats][0].ModeStr,"---");
 
/*  set default values for beacon frequency, alat and alon */
 
                  SATS[NumberOfSats].Beacon = 145.00 * 1.0e6;
                  SATS[NumberOfSats].Alat   = 0.0;
                  SATS[NumberOfSats].Alon   = 0.0;

                  break;
     
               case 1:
                  sscanf(line,"%s %s %ld",&string1,&string2,&SatNum);
                  SATS[NumberOfSats].SatNum=SatNum;
                  break;

               case 2:
                  sscanf(line,"%s %s %2d %f",
                     &string1,&string2,&EpochYear,&EpochDay);
                  SATS[NumberOfSats].YE = BasicYear + EpochYear;
                  SATS[NumberOfSats].TE = EpochDay;
                  break;

               case 3:
                  sscanf(line,"%s %s %ld",&string1,&string2,&EleSet);
                  SATS[NumberOfSats].EleSet=EleSet;
                  break;

               case 4:
                  sscanf(line,"%s %f",&string1,&Inclination);
                  SATS[NumberOfSats].IN = Inclination;
                  break;

               case 5:
                  sscanf(line,"%s %s %s %f",
                     &string1,&string2,&string3,&RAofAN);
                  SATS[NumberOfSats].RA = RAofAN;
                  break;

               case 6:
                  sscanf(line,"%s %f",&string1,&Eccentricity);
                  SATS[NumberOfSats].EC = Eccentricity;
                  break;

               case 7:
                  sscanf(line,"%s %s %s %f",
                     &string1,&string2,&string3,&ArgPerigee);
                  SATS[NumberOfSats].WP = ArgPerigee;
                  break;

               case 8:
                  sscanf(line,"%s %s %f",&string1,&string2,&MeanAnomaly);
                  SATS[NumberOfSats].MA = MeanAnomaly;
                  break;

               case 9:
                  sscanf(line,"%s %s %f",&string1,&string2,&MeanMotion);
                  SATS[NumberOfSats].MM = MeanMotion;
                  break;

               case 10:
                  sscanf(line,"%s %s %f",&string1,&string2,&DecayRate);
                  SATS[NumberOfSats].M2 = DecayRate;
                  break;

               case 11:
                  sscanf(line,"%s %s %ld",&string1,&string2,&EpochRev);
                  SATS[NumberOfSats].RV = EpochRev;
                  break;

               case 12:
                  break;
            }
         }
      }
      if (imat == 0 && strlen(line) > 1){
         printf (" Could Not Match KeyWord %s",line);
         exit(-1);
      }
   }
}
void ReadKeps(void)
/*  
  control routine to read in keplerian data
*/
{
/*  open data file
    nasa.dat is searched for 1st
    kepler.dat is next
*/

   if ((NasaDataFile = fopen(NasaFile,"r")) == 0){
      if ((KeplerDataFile = fopen(KeplerFile,"r")) == 0){
         printf (" Cant open %s or %s !!!\n",
            NasaFile,KeplerFile);
         exit(-1);
      }else{
         ReadKepler();
      }
   }else{
      ReadNasa();
   }

/*  correct revolution counter */

   RevolutionCorrection();

/*  initialize satellite values */
 
   InitSatellite();
}
void ReadLocalQth(void)
{
/*  read in Qth information */

   FILE *QthFile;

   char line[80];

   int i,n;
   float adum,bdum;

   NumberOfQth = 0;

/*  if file does not exists then get info and create it */

   if ((QthFile = fopen(MyQthFile,"r")) == 0){
      printf (" STP initialization....Qth information\n\n");

      printf (" Your Latitude (Degrees): ");
      Qth[0].LA=GetFloatNumberFromUser(&n);

      printf (" Your Longitude (Degrees): ");
      Qth[0].LO=GetFloatNumberFromUser(&n);

      printf (" Your Altitude (Meters): ");
      Qth[0].HT=GetFloatNumberFromUser(&n);

      printf (" Identifier (Callsign, etc): ");
      strcpy(Qth[0].QthLoc,GetStringFromUser(&n));

      printf (" # of entries in minimum look angle table (at least 1): ");

      NumberOfObsAngles=InputCheck(1,MaxObsAng);

      printf (" input values seperated by a space \n");

      for (i=0;i<NumberOfObsAngles;i++){
         printf (" azimuth # %3d , minimum angle ",i+1);
         scanf("%f%f",&ObsAzimuth[i],&ObsAngle[i]);
         gets(line);
      }

      QthFile = fopen(MyQthFile,"a");

      fprintf (QthFile,"%s\n",Qth[0].QthLoc);
      fprintf (QthFile,"%12.6f\n",Qth[0].LA);
      fprintf (QthFile,"%12.6f\n",Qth[0].LO);
      fprintf (QthFile,"%12.6f\n",Qth[0].HT);

      for (i=0;i<NumberOfObsAngles;i++){
         fprintf (QthFile,"%10.2f %10.2f\n",ObsAzimuth[i],ObsAngle[i]);
      }

      fclose (QthFile);

      return;
   }

/*  file exists....read in data */

   fgets (Qth[0].QthLoc,40,QthFile);

   fgets (line,80,QthFile);
   sscanf(line,"%f",&Qth[0].LA);

   fgets (line,80,QthFile);
   sscanf(line,"%f",&Qth[0].LO);

   fgets (line,80,QthFile);
   sscanf(line,"%f",&Qth[0].HT);

/*  read in minimum line of sight angle */

   NumberOfObsAngles=-1;

   while(fgets(line,80,QthFile)){
      sscanf(line,"%f %f",&adum,&bdum);

      if(adum < ZERO)adum=360.0+adum;

      NumberOfObsAngles++;

/*  convert to radians */

      ObsAzimuth[NumberOfObsAngles] = adum/D2R;
      ObsAngle[NumberOfObsAngles]   = bdum/D2R;

      if (bdum < ZERO)return;

      if (NumberOfObsAngles > MaxObsAng){
         printf (" Too many angles in Qth file !!!!\n");
         exit (-1);
      }
   }
}
void ReadMode(void)
/*  read satellite mode file */
{
   char dum[11],mode[3];
   int i,min,max,imatch,whichsat,nc;
   float frqbec;

   if ((ModeDataFile = fopen(ModeFile,"r")) == 0){
      printf(" %s not found......use default data\n",ModeFile);
      return;
   }

/*
    read file and look for record... satellite:
    when found check for match to current sat
*/

   imatch = 0;
   whichsat = -1;

   while (fgets(line1,80,ModeDataFile)){

      if(strnicmp(line1,"satellite:",10) == 0){
         imatch = 0;
	 sscanf(line1,"%s%s",dum,SatName);

/*  try to match satellite name */

         whichsat=-1;
         for (i=0;i<=NumberOfSats;i++){
            nc=strlen(SATS[i].SatsName);
            if (strncmpi(SatName,SATS[i].SatsName,nc) == 0){
               whichsat=i;
            }
         }
      }

/*  
  if whichsat is >= 0 then check to see if we are on line
  with the word satellite (imatch=0) or on a mode data line
  (imatch=1)

  if satellite line (imatch=0) then get alon,alat and beacon frequency
  otherwise store mode data
*/
      if(whichsat >= 0){
         if (imatch == 1){
      	    sscanf(line1,"%3d%3d%3s",&min,&max,&mode);

	    Modes[whichsat][SATS[whichsat].NumModes].MinPhase = min;
            Modes[whichsat][SATS[whichsat].NumModes].MaxPhase = max;

	    strcpy(Modes[whichsat][SATS[whichsat].NumModes].ModeStr,mode);

	    SATS[whichsat].NumModes++;

/*  too many ? */
            if (SATS[whichsat].NumModes > MaxModes){
               printf (" Too many modes specified for %s\n",
                  SATS[whichsat].SatsName);
               exit(-1);
            }
	 }
         if (imatch == 0){
            imatch = 1;

	    fgets(line1,80,ModeDataFile);
	    sscanf(line1,"%f%f%f",&frqbec,&Alon,&Alat);

/*  set squint computation option */

            if (Alon == ZERO && Alat == ZERO) {
               SATS[whichsat].SquintOpt=0;
            } else {
               SATS[whichsat].SquintOpt=1;
            }

            SATS[whichsat].Beacon=frqbec*1.0e6;
            SATS[whichsat].Alon=Alon/D2R;
            SATS[whichsat].Alat=Alat/D2R;
	 }
      }
   }
   fclose (ModeDataFile);
}
void ReadNasa(void)
/*  read kepler data in nasa 2 line format */
{
   int i,nc,idum,SatNum,YearLau,LauNum;
   unsigned long int EpochRev,ElementSet;
   float EpochYear,EpochDay,DecayRate,MeanMotion;
   float Inclination,RAofAN,Eccentricity,ArgPerigee;

/* read until end of file */

   NumberOfSats = -1;

   while (fgets(SatName,40,NasaDataFile)){
      NumberOfSats++;

      if (NumberOfSats > MaxSat) {
         NumberOfSats--;
         printf (" Read in Maximum Number of Satellites %d\n",MaxSat);
         printf (" Last Satellite Read in is %s\n",
            SATS[NumberOfSats].SatsName);
         return;
      }

/*  find real end of this string */
      nc=RealEndOfLine(SatName);

      strncpy(SATS[NumberOfSats].SatsName,SatName,nc);

/*
      SATS[NumberOfSats].SatsName = strdup(SatName);
      SATS[NumberOfSats].SatsName[nc] = '\0';
*/

      fgets(line1,80,NasaDataFile);
      fgets(line2,80,NasaDataFile);

      sscanf(line1,"%*2c%5d%*10c%2f%12f%10f%*21c%5lu",
            &SatNum,&EpochYear,&EpochDay,&DecayRate,&ElementSet);

      SATS[NumberOfSats].SatNum=SatNum;

/* strip off checksum */
      ElementSet /= 10;
      SATS[NumberOfSats].EleSet=ElementSet;
 
      sscanf(line2,"%*8c%8f%8f%7f%8f%8f%11f%5lu",
         &Inclination,&RAofAN,&Eccentricity,&ArgPerigee,&MeanAnomaly,
         &MeanMotion,&EpochRev);

      SATS[NumberOfSats].SquintOpt= 0;
      SATS[NumberOfSats].NumModes = 0;

/*  set default values for mode information */

      Modes[NumberOfSats][0].MinPhase = 0;
      Modes[NumberOfSats][0].MaxPhase = 256;

      strcpy(Modes[NumberOfSats][0].ModeStr,"---");

/*  set default values for beacon frequency, alat and alon */

      SATS[NumberOfSats].Beacon = 145.00 * 1.0e6;
      SATS[NumberOfSats].Alat   = 0.0;
      SATS[NumberOfSats].Alon   = 0.0;

/*  store/convert values */

      SATS[NumberOfSats].YE = BasicYear + EpochYear;
      SATS[NumberOfSats].TE = EpochDay;
      SATS[NumberOfSats].IN = Inclination;
      SATS[NumberOfSats].RA = RAofAN;
      SATS[NumberOfSats].EC = Eccentricity*1.0e-7;
      SATS[NumberOfSats].WP = ArgPerigee;
      SATS[NumberOfSats].MA = MeanAnomaly;
      SATS[NumberOfSats].MM = MeanMotion;
      SATS[NumberOfSats].M2 = DecayRate;
      SATS[NumberOfSats].RV = EpochRev/10;
   }
}
int RealEndOfLine(char string[])
/*  find real end of line padded with blanks */
{
   int i,last,nc;
   
/*  eat off last character...most likely carriage return */
   nc=strlen(string)-1;

   last=nc;

/*  
  start at end and work backwards...if location is blank then
  set last = i.  

  if string then blanks last will contain the position of the last
  character

  if string then blank string blank... then want to return the
  value of the first character we hit. if we have found a word
  then last contains the location of the end of the word, now if
  we hit a blank and another word last will be greater than i
  and we stop 
*/
   for (i=nc;i>=0;i--){
      if(string[i] == ' '){
         last=i;
      }
      if(last > i)return last;
   }

   if (last == 0)last=nc;

   return last;
}
void ReadOtherQth(void)
/*  
    read in other qth file info.  used to do mutual visibilities 
    user can use commands (starthere) and (stophere) to pull out
    the qth(s) of interest 
*/
{
   FILE *OQthFile;

   int On;
   char DummyLine[132];
   char *p;

   if ((OQthFile = fopen(DxQthFile,"r")) == 0){
      printf (" could not open file %s\n",DxQthFile);
      exit(-1);
   } 

   On = -1;

   while (fgets(DummyLine,132,OQthFile)){

      if(strnicmp(DummyLine,"starthere",9) == 0){
         On=1;
         fgets (DummyLine,132,OQthFile);
      }

      if(strnicmp(DummyLine,"stophere",8) == 0){
         On=-1;
         fgets (DummyLine,132,OQthFile);
      }

      if(On > 0) {
         NumberOfQth++;

/*  latitude */
         p=strtok(DummyLine," ");
         Qth[NumberOfQth].LA=atof(p);

/*  longitude */
         p=strtok(NULL," ");
         Qth[NumberOfQth].LO=atof(p);

/*  altitude */
         p=strtok(NULL," ");
         Qth[NumberOfQth].HT=atof(p);

/*  grid locator */
         p=strtok(NULL," ");

/*  rest of line....identifier */
         p=strtok(NULL,"\n");

/*  only keep first 8 characters */
         strncpy(Qth[NumberOfQth].QthLoc,p,8);
         Qth[NumberOfQth].QthLoc[8] = '\0';
      }
   } 

   fclose (OQthFile);

   printf (" Read in %d locations\n",NumberOfQth);
}
void RealTimeDisplay(int argv,char *argc[],int WhatToShow)
/*  
   real time of satellites:
      WhatToShow = 0 currently visible satellites 
      WhatToShow = 1 all satellites 
      WhatToShow = 2 selected set of satellites
*/
{
   int i,nc;
   float OutputRate,ElevationLimit;
   float TimeAOS,TimeLOS,DtHour,DtMin,DtSec;

   OutputFile = stdout;

/*  
  set up array of satellites to use

  also catch non defined cases here.....
*/

   switch (WhatToShow){
      case 0:
      case 1:
         NumberOfSatsSelected = NumberOfSats; 

         for (i=0;i<=NumberOfSats;i++){
            WhichSatelliteSelected[i] = i;
         }

         break;

      case 2:
         
         if ((SelectFile = fopen ("select.dat","r")) == 0){
            printf (" cant open select.dat\n");
            printf (" run STP with the S option to generate. (STP S)\n");
            exit(-1);
         }

         NumberOfSatsSelected = -1;

         while(fgets(SatName,40,SelectFile)){
            nc=strlen(SatName)-1;

/*  handle cases where null line entered....strncmpi will match all lines...*/ 

            if (nc > 0) {
               for (i=0;i<=NumberOfSats;i++){
                  if (strncmpi(SatName,SATS[i].SatsName,nc) == 0){
                     NumberOfSatsSelected++;
                     WhichSatelliteSelected[NumberOfSatsSelected] = i;
                  }
               }
            }
         }

         if (NumberOfSatsSelected <= -1){
            printf (" No sats matched from select.dat\n");
            exit(-1);
         }
 
         fclose (SelectFile);

         break;

      default:
         printf (" can't match option %d in RealTimeDisplay\n",
            WhatToShow);
         exit(-1);
    }

/*  initialize all values in array */
    for (i=0;i<MaxSat;i++){
       UpDown[i] = 0;
    }

/*  see if user specified an update rate...otherwise set default */
    OutputRate = 1.0;

    if (argv == 3){
       OutputRate = atof(argc[2]);
    }

/* set observer as local site */
   WhichObserver = 0;

/*  initailize Qth parameters */

   InitQth();

/*  
  initialize rise/set times

  performed here to minmize switching in real time loop
  also makes variable housekeeping easier
*/

   switch (WhatToShow){
      case 0:
         break;

      case 1:
      case 2:
         UTCLOCTime();

         Year   = (float)GMTyear + BasicYear;
         Month  = (float)GMTmon + ONE;
         Day    = (float)GMTdaym;

         Hour   = (float)GMThour;
         Minute = (float)GMTmin;
         Second = (float)GMTsec;

/*  get day number */

         DayNow = DayNumber(Year,Month,Day);

/*  loop over satellites */

         for (i=0;i<=NumberOfSatsSelected;i++){
            WhichSatellite = WhichSatelliteSelected[i];

/*  current time in hours.... set here since FindAOS/FindLOS reset */

            TimeNow = (Hour+Minute/60.0+Second/3600.0)/24.0;

/*  initailize sun parameters...a must do for each satellite */

            InitSun();
 
/*  initailize antenna parameters...a must do for each satellite */

            InitAntenna();

/*  compute satellite position */
            SatellitePosition();

/* determine azimiuth/elevation between observer and satellite */
            ObserverSatelliteGeometry();
/*  
  find AOS/LOS times.... current clock time 
*/

             if (Elevation >= ZERO){
                RiseSet[WhichSatellite].RiseHour   = Hour;
                RiseSet[WhichSatellite].RiseMinute = Minute;
                RiseSet[WhichSatellite].RiseSecond = Second;
             }else{
                TimeAOS = FindAOS();

                HourMinuteSecond(TimeAOS,stdout,-999);

                RiseSet[WhichSatellite].RiseHour   = TempHour;
                RiseSet[WhichSatellite].RiseHour  += TempDay*24.0;
                RiseSet[WhichSatellite].RiseMinute = TempMin;
                RiseSet[WhichSatellite].RiseSecond = TempSec;

/* if TimeAOS is negative then rise time is greater than 2 days away */

                if (TimeAOS < ZERO){
                   RiseSet[WhichSatellite].RiseHour   = Hour + 48.0;
                   RiseSet[WhichSatellite].RiseMinute = Minute;
                   RiseSet[WhichSatellite].RiseSecond = Second;
                }
             }

             TimeLOS = FindLOS();
             HourMinuteSecond(TimeLOS,stdout,-999);

             RiseSet[WhichSatellite].SetHour   = TempHour;
             RiseSet[WhichSatellite].SetHour  += TempDay*24.0;
             RiseSet[WhichSatellite].SetMinute = TempMin;
             RiseSet[WhichSatellite].SetSecond = TempSec;

/* if TimeLOS is negative then set time is greater than 2 days away */

             if (TimeLOS < ZERO){
                RiseSet[WhichSatellite].SetHour   = Hour + 48.0;
                RiseSet[WhichSatellite].SetMinute = Minute;
                RiseSet[WhichSatellite].SetSecond = Second;
             }
         }
         break;
   }

/*  real time loop over each satellite */

   while (bioskey(1) == 0){

/*  get current time */
      UTCLOCTime();

      Year   = (float)GMTyear + BasicYear;
      Month  = (float)GMTmon + ONE;
      Day    = (float)GMTdaym;

      Hour   = (float)GMThour;
      Minute = (float)GMTmin;
      Second = (float)GMTsec;

/*  get day number */

      DayNow = DayNumber(Year,Month,Day);

/*  if times up then dump data */

      if (DeltTime(OutputRate)){
         ClearTheScreen();

/*  show user current local and GMT time */

         printf (" LOC %10s %10s %02d 19%02d %02d:%02d:%02d\n",
            DayNames[LOCdayw],
            MonthNames[LOCmon],
            LOCdaym,
            LOCyear,
            LOChour,
            LOCmin,
            LOCsec);
 
         printf (" GMT %10s %10s %02d 19%02d %02d:%02d:%02d\n",
            DayNames[GMTdayw],
            MonthNames[GMTmon],
            GMTdaym,
            GMTyear,
            GMThour,
            GMTmin,
            GMTsec);

         switch (WhatToShow){
            case 0:
               printf ("  Az    El    Range Squnt  Doppler");
               printf ("  MA    Mode Sun\n");
               break;

            case 1:
            case 2:
               printf ("  Az    El    Range Squnt  Doppler");
               printf ("  MA    Mode Sun Status AOS--LOS\n");
               break;
         }

         for (i=0;i<=NumberOfSatsSelected;i++){
            WhichSatellite = WhichSatelliteSelected[i];

/*  current time in hours.... set here since FindAOS/FindLOS reset */

            TimeNow = (Hour+Minute/60.0+Second/3600.0)/24.0;

/*  initailize sun parameters...a must do for each satellite */

            InitSun();
 
/*  initailize antenna parameters...a must do for each satellite */

            InitAntenna();

/*  compute satellite position */

            SatellitePosition();

/* determine azimiuth/elevation between observer and satellite */

            ObserverSatelliteGeometry();

/*  set elevation limit based on option */

            switch (WhatToShow){
               case 0:
                  ElevationLimit = ZERO;
                  break;

               case 1:
               case 2:
                  ElevationLimit = -1000.0;
                  break;
            }

            if (Elevation > ElevationLimit){

/*  compute sun lit status */

               SunData();

               printf(" %4.0f %4.0f",
                  Azimuth*D2R,
                  Elevation*D2R);

/*  compute squint/rangerate */

               SquintRangeRate();


/* 
   which way is elevation changing....
      if RangeRate is positive then going away
      if RangeRate is negative then coming towards
*/

               if (RangeRate >= ZERO)printf (" v");
               if (RangeRate  < ZERO)printf (" ^");

               printf (" %6.0f",
                  RangeToSatellite);

/*  get mode info */

               WhatMode();

/*  if Omnidirectional antennas on ao-10/ao-13... then adjust squint angle */

               if (strncmpi(CurrentMode,"Bo",2) == 0) {
                  SquintAngle = ABS(90.0 - SquintAngle);
               }

               printf (" %4.0f",SquintAngle);

/*  compute doppler */

               Doppler(SATS[WhichSatellite].Beacon);

               printf ("  %5.1f   %s",CurrentMAd,CurrentMode);

/*  sunlit status */

               printf ("%s",ECL);

/*  
  different options show some extra stuff

  also recompute AOS/LOS times if sat has just risen or set
*/

               switch (WhatToShow){
                  case 0:
                     break;

                  case 1:
                  case 2:
                     if (Elevation > ZERO){
/*
                        printf ("  Up  ");
*/
                        printf (" -LOS-");

/* if satellite was down and now up compute new LOS time */

                        if (UpDown[WhichSatellite] == -1){

                           TimeLOS = FindLOS();
                           HourMinuteSecond(TimeLOS,stdout,-999);

                           RiseSet[WhichSatellite].SetHour   = TempHour;
                           RiseSet[WhichSatellite].SetHour  += TempDay*24.0;
                           RiseSet[WhichSatellite].SetMinute = TempMin;
                           RiseSet[WhichSatellite].SetSecond = TempSec;
                        }

                        UpDown[WhichSatellite] = 1;

                        DtHour = RiseSet[WhichSatellite].SetHour - Hour;
                        DtMin  = RiseSet[WhichSatellite].SetMinute - Minute;
                        DtSec  = RiseSet[WhichSatellite].SetSecond - Second;

                        if (DtSec < ZERO){
                           DtMin -= ONE;
                           DtSec += 60.0;
                        }
                        if (DtMin < ZERO){
                            DtHour -= ONE;
                            DtMin  += 60.0;
                        }
                     }else{
/*
                        printf (" Down ");
*/
                        printf ("  aos ");

/* if satellite was up and now down compute new AOS time */

                        if (UpDown[WhichSatellite] == 1){
                           UpDown[WhichSatellite] = -1;

                           TimeAOS = FindAOS();
                           HourMinuteSecond(TimeAOS,stdout,-999);

                           RiseSet[WhichSatellite].RiseHour   = TempHour;
                           RiseSet[WhichSatellite].RiseHour  += TempDay*24.0;
                           RiseSet[WhichSatellite].RiseMinute = TempMin;
                           RiseSet[WhichSatellite].RiseSecond = TempSec;
                        }

                        DtHour = RiseSet[WhichSatellite].RiseHour - Hour;
                        DtMin  = RiseSet[WhichSatellite].RiseMinute - Minute;
                        DtSec  = RiseSet[WhichSatellite].RiseSecond - Second;

                        if (DtSec < ZERO){
                           DtMin -= ONE;
                           DtSec += 60.0;
                        }
                        if (DtMin < ZERO){
                            DtHour -= ONE;
                            DtMin  += 60.0;
                        }
                     }

/*  AOS/LOS times.....Relative */

                     printf (" %02.0f:%02.0f:%02.0f",
                        DtHour,
                        DtMin,
                        DtSec);

                     break;

               }

               printf (" %12s\n",
                  SATS[WhichSatellite].SatsName);
            }
         }   /*  end of WhichSatellite loop */

/*  save current time for output determination */

         HourOld   = Hour;
         MinuteOld = Minute;
         SecondOld = Second;
      }
   }
}
void RunCheckSum(int argv, char *argc[])
/*  perform checksum calculations as a standalone feature */
{
/*  if user did not specify filename on command line then ask for it */

    if (argv <= 2){
       printf (" Filename to check: ");
       gets(line1);
    }else{
       strcpy(line1,argc[2]);
    }

/*  perform checksum calculations */

    DoCheckSum(line1,1);
}
void RevolutionCorrection(void)
/*  
   read revolution correction database...if it exists
   if it does then reset rev counter
*/
{
   int i,nc;
   float RevCorrection;

/*  if file does not exist then return */
   if ((RevolutionDataFile = fopen(RevCorFile,"r")) == 0){
      return;
   }

/* 
    read file
    loop over input satelites and try to match satellite
    correct data
*/

   while (fgets(line1,80,RevolutionDataFile)){

      fgets(line2,80,RevolutionDataFile);
      sscanf(line2,"%f",&RevCorrection);

      for (i=0;i<=NumberOfSats;i++){
         nc=strlen(line1)-1;
         if (strnicmp(line1,SATS[i].SatsName,nc) == 0){
            SATS[i].RV=SATS[i].RV + RevCorrection;
         }
      }
   }

   fclose (RevolutionDataFile);
}
void SatelliteDump(void)
/*  dump out info on satellites */
{
   int i;
   float CurrentDay;

   UTCLOCTime();

   Month = (float)GMTmon + ONE;
   Day   = (float)GMTdaym;
   Year  = (float)GMTyear + BasicYear;

/*  get current day number */
   CurrentDay=DayNumber(Year,Month,Day);

   printf (" #    Beacon  Alon  Alat");
   printf ("   Apogee    Perigee   Period");
   printf ("   Ecc     Age");
   printf ("\n");

   printf ("        Mhz   deg   deg ");
   printf ("     Km         Km       min ");
   printf ("           days");
   printf ("\n");

   for (i=0;i<=NumberOfSats;i++){

      printf (" %2d %9.4f % 4.0f % 4.0f",
      SATS[i].NumModes,
      SATS[i].Beacon*1.0e-6,
      SATS[i].Alon*D2R,
      SATS[i].Alat*D2R);

/*  apogee,perigee calculations */
      printf (" % 9.1f % 9.1f   % 7.1f",
         SATS[i].A0*(ONE+SATS[i].EC)-RE,
         SATS[i].A0*(ONE-SATS[i].EC)-RE,
         SATS[i].Period);

      printf (" % 8.5f",
         SATS[i].EC);

      printf (" % 6.2f",
         CurrentDay-(SATS[i].DE+SATS[i].TE));

      printf (" %s \n",
         SATS[i].SatsName);
   }
}
void SatellitePosition(void)
/* calculate satellite position at DayNow,TimeNow */
{
   float DT,KD,KDP,DR,EA,EccSin,EccCos,DEA,FKepler,FKeplerP;
   float CW,SW,CQ,SQ;
   float C,S,A,B;
   float Denominator;
   float tempvar;

/* elapsed time since epoch, days */
   TimeSinceEpoch = (DayNow  - SATS[WhichSatellite].DE) + 
                    (TimeNow - SATS[WhichSatellite].TE);       

/* linear drag terms */
   DT  = 0.50*SATS[WhichSatellite].DC*TimeSinceEpoch;	        
   KD  = ONE + 4.0*DT;
   KDP = ONE - 7.0*DT;

/* mean anomaly at Year,TimeNow */
   MeanAnomaly = SATS[WhichSatellite].MA 
                +SATS[WhichSatellite].MM*TimeSinceEpoch*(ONE-3.0*DT);  

/* strip out whole # of revs */
   DR = AINT(MeanAnomaly/TWOPI);          

/* M now in range 0 - 2 pi */
   MeanAnomaly = MeanAnomaly - DR*TWOPI;            

/* current orbit number */
   RevolutionNumber = SATS[WhichSatellite].RV + DR;                

/* 
   solve M = EA - EC*sin(EA) for EA given M by 
   Newtons Method with acceleration

   Inital guess for EA taken from:

      Fundamentals of Celestial Mechanics
      J.M.A. Danby
      2nd Edition

*/
   EA=MeanAnomaly+0.85*SGN(sin(MeanAnomaly))*SATS[WhichSatellite].EC;

   EccSin=SATS[WhichSatellite].EC*sin(EA);

   FKepler=EA-EccSin-MeanAnomaly;

   while (ABS(FKepler) > 0.00001){
      EccCos   =  SATS[WhichSatellite].EC*cos(EA);
      FKeplerP =  ONE-EccCos;
      DEA      = -FKepler/FKeplerP;
      DEA      = -FKepler/(FKeplerP+0.50*DEA*EccSin);
      DEA      = -FKepler/(FKeplerP+0.50*DEA*EccSin+DEA*DEA*EccCos/6.0);
      EA      +=  DEA;
      EccSin   =  SATS[WhichSatellite].EC*sin(EA);
      FKepler  =  EA-EccSin-MeanAnomaly;
   }

   C = cos(EA);
   S = sin(EA);

   Denominator=ONE-SATS[WhichSatellite].EC*C;
   
   A = SATS[WhichSatellite].A0*KD;
   B = SATS[WhichSatellite].B0*KD;
   AltitudeOfSatellite = A*Denominator;

/* calculate satellite positions and velocity in plane of ellipse */

   tempvar=SATS[WhichSatellite].N0/Denominator;

   Sx = A*(C-SATS[WhichSatellite].EC);
   Vx = -A*S*tempvar;
   Sy = B*S;
   Vy = B*C*tempvar;

   tempvar = TimeSinceEpoch*KDP;

   AP = SATS[WhichSatellite].WP
       +SATS[WhichSatellite].WD*tempvar;
   CW = cos(AP);
   SW = sin(AP);

   RAAN = SATS[WhichSatellite].RA
         +SATS[WhichSatellite].QD*tempvar;
   CQ = cos(RAAN);
   SQ = sin(RAAN);

/* plane -> celestial coordinate transformation [C] = [RAAN][IN][AP] */

   CXx =  CW*CQ-SW*SATS[WhichSatellite].CI*SQ;
   CXy = -SW*CQ-CW*SATS[WhichSatellite].CI*SQ;
   CXz =           SATS[WhichSatellite].SI*SQ;

   CYx =  CW*SQ+SW*SATS[WhichSatellite].CI*CQ;
   CYy = -SW*SQ+CW*SATS[WhichSatellite].CI*CQ;
   CYz =       -   SATS[WhichSatellite].SI*CQ;

   CZx =        SW*SATS[WhichSatellite].SI;
   CZy =        CW*SATS[WhichSatellite].SI;
   CZz =           SATS[WhichSatellite].CI;

/* compute satellites position vector, antenna axis unit vector and
   velocity in celestial coordinates
*/

   SATx = Sx*CXx+Sy*CXy;
   SATy = Sx*CYx+Sy*CYy;
   SATz = Sx*CZx+Sy*CZy;

   ANTx = SATS[WhichSatellite].ax*CXx
         +SATS[WhichSatellite].ay*CXy
         +SATS[WhichSatellite].az*CXz;
   ANTy = SATS[WhichSatellite].ax*CYx
         +SATS[WhichSatellite].ay*CYy
         +SATS[WhichSatellite].az*CYz;
   ANTz = SATS[WhichSatellite].ax*CZx
         +SATS[WhichSatellite].ay*CZy
         +SATS[WhichSatellite].az*CZz;

   VELx = Vx*CXx+Vy*CXy;
   VELy = Vx*CYx+Vy*CYy;
   VELz = Vx*CZx+Vy*CZy;

/* express SAT, ANT and VEL in geocentric coordinates */

   GHAA = GHAE + WE*TimeSinceEpoch;

   C = cos(-GHAA);
   S = sin(-GHAA);

   Sx = SATx*C - SATy*S;
   Sy = SATx*S + SATy*C;
   Sz = SATz;

   Ax = ANTx*C - ANTy*S;
   Ay = ANTx*S + ANTy*C;
   Az = ANTz;

   Vx = VELx*C - VELy*S;
   Vy = VELx*S + VELy*C;
   Vz = VELz;
}
void SatellitePredict(int argv,char *argc[],int Option)
/*  controlling routine to predict satellite passes */
{
   float CurrentDay,DayEnd,NumberOfDays;
   float HourStart;
   int i,nc;

/*  set observer to home Qth */

   WhichObserver=0;

/*  
  if user did not include satellite name on command line
  then show what we got and ask...
*/

   if (argv <= 2){
      for (i=0;i<=NumberOfSats;i++){
         printf (" %s\n",SATS[i].SatsName);
      }

      printf (" Which Satellite from the list ?  ");

      gets(SatName);
   } else {
      strcpy(SatName,argc[2]);
   }

   nc=strlen(SatName);

/*  can we match it */

   WhichSatellite=-1;

   for (i=0;i<=NumberOfSats;i++){
      if (strnicmp(SatName,SATS[i].SatsName,nc) == 0){
         if (WhichSatellite == -1){
            WhichSatellite = i;
         }else{
            printf (" Multiple Names.  Please be more specific \n");
            exit(-1);
         }
      }
   }

   if (WhichSatellite < 0){
      printf (" Could Not Match Satellite %s in List\n",SatName);
      exit (-1);
   }else{
      printf (" Matched your input as %s\n",SATS[WhichSatellite].SatsName);
   }

/*  
  get local date to check input (currently only warns user of 
  back propagation) and possibly for use as input
*/
   UTCLOCTime();

   Month = (float)GMTmon + ONE;
   Day   = (float)GMTdaym;
   Year  = (float)GMTyear + BasicYear;

/*  get current day number */
   CurrentDay=DayNumber(Year,Month,Day);

   printf (" Enter Start Date (month day year) (<cr> for current date) : ");

/*  allow user option to hit return to get current date */
   strcpy(line1,GetStringFromUser(&nc));

/*  if nc > 0 then user input...otherwise use current information */

   if (nc > 0){
      sscanf(line1,"%f%f%f",&Month,&Day,&Year);
   }else{
      Month = (float)GMTmon + ONE;
      Day   = (float)GMTdaym;
      Year  = (float)GMTyear + BasicYear;
   }

/*  check for back propagation times */

   if (Month < (float)LOCmon){
      printf (" ********* Input Month < Current Month\n\n");
   }
   if (Day < (float)LOCdaym && Month <= (float)LOCmon){
      printf (" ********* Input Day < Current Day\n\n");
   }
   if (Year < BasicYear +(float)LOCyear){
      printf (" ********* Input Year < Current Year\n");
      printf ("           MA calculations will be wrong\n\n");
   }

   printf (" Number of Days for Prediction: ");
   NumberOfDays=GetFloatNumberFromUser(&nc);

   printf (" Step Size (minutes....input negative value for seconds): ");
   Step=GetFloatNumberFromUser(&nc);

/* 
   if Step is negative then user has input time step in seconds...
   convert to minutes 
*/

   if (Step < ZERO){
      Step = ABS(Step)/60.0;
   }

/*  if option P...Option = 1 then ask for start hour */

   if (Option){
      printf (" Start Hour (<cr> for 0.0): ");
      HourStart = GetFloatNumberFromUser(&nc);
   }else{
      HourStart = 0.0;
   }

   printf (" FileName to write output to (cr for screen): ");

/* read filename here */

   strcpy(FileName,GetStringFromUser(&nc));

   if (nc > 0){
      if ((OutputFile = fopen(FileName,"w")) == 0){
         printf ("Can't open file %s\n",FileName);
         exit(-1);
      }
   }else{
      OutputFile = stdout;
   }

   DayStart = DayNumber(Year,Month,Day);
   DayEnd   = DayStart + NumberOfDays - ONE;

/*  initailize Qth parameters */

   InitQth();

/*  initailize sun parameters */

   InitSun();
 
/*  initailize antenna parameters */

   InitAntenna();

/*  footprint calculations.... */

   if (PlotFlag){
      FootPrintInit();
   }
 
/*  loop over time */
 
   RunningCounter    = 0;
   RunningCounterOld = 0;

   for (DayNow=DayStart;DayNow<=DayEnd;DayNow++){

      if (DayNow > DayStart)HourStart = ZERO;

      for (Hour=HourStart;Hour<=23.0;Hour++){
         for (Minute=ZERO;Minute<=60.0-Step;Minute=Minute+Step){

            RunningCounter++;

            TimeNow = (Hour+Minute/60.0)/24.0;

/*  get satellite location */
            SatellitePosition();

/*  determine azimuth and elevation from observers location */
            ObserverSatelliteGeometry();

/*  
   can we see it....
   if we can compute sun illumination 
   squint angle (antenna pointing)
   ..... print data
*/
            if (Elevation >= LOSAngle(Azimuth)){
               SunData();
               SquintRangeRate();
               DataPrint(CurrentDay);

               if (PlotFlag){
                  FootPrint();
               }
            }
         }
      }
   }

   fclose (OutputFile);
}
void SelectSatellites(void)
/*  select satellites for option s */
{
   int i,ndigit;

   NumberOfSatsSelected=-1;

   if((SelectFile=fopen("select.dat","w")) == 0){
      printf (" cant open select.dat\n");
      exit(-1);
   }

   for (i=0;i<=NumberOfSats;i++){
      printf (" %2d %s\n",i,SATS[i].SatsName);
   }

   ndigit=1;
   while (ndigit > 0){
      printf (" Which Satellite from the list (<cr> to stop) ?  ");

      i=GetIntNumberFromUser(&ndigit);
 
      if (ndigit > 0){
         NumberOfSatsSelected++;
         WhichSatelliteSelected[NumberOfSatsSelected]=i;
         fprintf (SelectFile,"%s\n",SATS[i].SatsName);
      }
   }
 
   fclose (SelectFile);
}
void SquintRangeRate(void)
/*  compute squint angle and range rate..used for doppler */
{
/*
   resolve antenna vector along unit range vector -r.a = cos(sq)
   (SATS[].SquintOpt = 1)...e.g. AO-13, AO-10, Arsene,...

   assume antenna axis is aligned with satellite position vector
   (SATS[].SquintOpt = 0)...e.g. RS-10/11, AO-21, FO-20,...
*/

   if (SATS[WhichSatellite].SquintOpt == 1) {
      SquintAngle = acos(-(Ax*Rx + Ay*Ry + Az*Rz)   )*D2R;
   } else {
      SquintAngle = acos( (Sx*Rx + Sy*Ry + Sz*Rz)/AltitudeOfSatellite)*D2R;
   }
 
/* 
  resolve sat-obs velocity vector along unit range vector
  
  note:   Qth[WhichObserver].V0z == ZERO
*/
 
   RangeRate = (Vx-Qth[WhichObserver].V0x)*Rx
              +(Vy-Qth[WhichObserver].V0y)*Ry
              +(Vz                       )*Rz;
}
void SunAzimuthElevation(int PrintOut)
/*  compute sun azimuth and elevation from observers location */
{
   float MAS,TAS,C,S,Hx,Hy,Hz,SNx,SNy,SNz;
   float xRx,xRy,xRz,xR,xU,xE,xN;

/*  ma of sun round its orbit */
   MAS = MASE + (MASD*TimeSinceEpoch)/D2R; 

/*  suns true anomaly */
   TAS = MRSE + WW*TimeSinceEpoch
        +EQC1*sin(MAS)
        +EQC2*sin(2.0*MAS)
        +EQC3*sin(3.0*MAS);

   C = cos(TAS);
   S = sin(TAS);

   DistanceToSun = AstroUnit*(1.00014-0.01671*C-0.00140*cos(2.0*TAS));

/*  sun unit vector celestial coords */
   SNx = (float)DistanceToSun*C;
   SNy = (float)DistanceToSun*S*CNS;
   SNz = (float)DistanceToSun*S*SNS;

   C = cos(-GHAA);
   S = sin(-GHAA);

/*  sun unit vector in geocentric coordinates */
   Hx = SNx*C - SNy*S;
   Hy = SNx*S + SNy*C;
   Hz = SNz;

/*  azimuth/elevation */

   xRx = Hx-Qth[WhichObserver].Ox;
   xRy = Hy-Qth[WhichObserver].Oy;
   xRz = Hz-Qth[WhichObserver].Oz;
   xR = sqrt(xRx*xRx+xRy*xRy+xRz*xRz);

   xRx = xRx/xR;
   xRy = xRy/xR;
   xRz = xRz/xR;

   xU = xRx*Qth[WhichObserver].Ux
       +xRy*Qth[WhichObserver].Uy
       +xRz*Qth[WhichObserver].Uz;
   xE = xRx*Qth[WhichObserver].Ex
       +xRy*Qth[WhichObserver].Ey;
   xN = xRx*Qth[WhichObserver].Nx
       +xRy*Qth[WhichObserver].Ny
       +xRz*Qth[WhichObserver].Nz;

   CurrentSunAz = ATAN2(xE,xN)*D2R;
   CurrentSunEl = asin(xU)*D2R;

   if (PrintOut){
      printf (" Sun : %7.2f %7.2f %15.3f",
         CurrentSunAz,
         CurrentSunEl,
         DistanceToSun);
   }
}
void SunData(void)
/*  compute sun data */
{
   float CUA,UMD,MAS,TAS,C,S,sunt1;
   float Hx,Hy,Hz,Distance;
   float booger;

/*  ma of sun round its orbit */
   MAS = MASE + (MASD*TimeSinceEpoch)/D2R; 

/*  suns true anomaly */
   TAS = MRSE + WW*TimeSinceEpoch
        +EQC1*sin(MAS)
        +EQC2*sin(2.0*MAS)
        +EQC3*sin(3.0*MAS);

   C = cos(TAS);
   S = sin(TAS);

   Distance = 1.00014-0.01671*cos(MAS)-0.00140*cos(2.0*MAS);

/*  sun unit vector celestial coords */
   SUNx = Distance*C;
   SUNy = Distance*S*CNS;
   SUNz = Distance*S*SNS;

/*  sin of sun angle */
   SinOfSunAngle = -(ANTx*SUNx + ANTy*SUNy + ANTz*SUNz);

/*  illumination */

   booger=ONE-SinOfSunAngle*SinOfSunAngle;
   if(booger < ZERO)booger = ZERO;

   Illumination = 100.0*sqrt(booger);

/*  cos of umbral angle */
   CUA = -(SATx*SUNx + SATy*SUNy + SATz*SUNz)/AltitudeOfSatellite;

/*  umbral distance in units of earth radii */
   booger=ONE-CUA*CUA;

   if (booger < ZERO)booger = ZERO;

   UMD = AltitudeOfSatellite*sqrt(booger)/RE;

/* 
    + means for shadow side
    - means for sunny side
*/
   if (CUA >= ZERO){
      strcpy(ECL,"  +  ");
   }else{
      strcpy(ECL,"  -  ");
   }

   if (UMD <= ONE && CUA >= ZERO) strcpy(ECL," ECL ");

   C = cos(-GHAA);
   S = sin(-GHAA);

/*  sun unit vector in geocentric coordinates */
   Hx = SUNx*C - SUNy*S;
   Hy = SUNx*S + SUNy*C;
   Hz = SUNz;

   sunt1 = Hx*Qth[WhichObserver].Ux
          +Hy*Qth[WhichObserver].Uy
          +Hz*Qth[WhichObserver].Uz;

/*  
  if sun more than 10 degrees below horizon then satellite possibly
  is visible.  depends upon satellite signature and brightness of
  background.  (10 degrees ~ 0.17 radians)
*/

   if (sunt1 <= -0.17 && (strcmpi(ECL," ECL ") != 0)){
      strcpy(ECL," vis ");
   }

/*  obtain sun unit vector in orbit coordinates */
   Hx = SUNx*CXx + SUNy*CYx + SUNz*CZx;
   Hy = SUNx*CXy + SUNy*CYy + SUNz*CZy;
   Hz = SUNx*CXz + SUNy*CYz + SUNz*CZz;

/*  cluge here for now to limit values */
   if (Hz > ONE)Hz = ONE;
   if (Hz < -ONE)Hz = -ONE;

   SunElevation = asin(Hz);
   SunAzimuth   = ATAN2(Hy,Hx);
}
float TemperatureOfSky(float Frequency)
/* 
   approximation to sky temperature as a function of frequency 

   Frequency (Mhz)
*/
{
      return exp(10.97 - 1.53*log(Frequency));
}
void UTCLOCTime(void)
/*  
   get local time (24 hour clock) and UTC 

   assumes variable TZ has been set
*/
{
 
   time_t t;
   struct tm *gmt,*loc;
 
   t = time(NULL);
 
   loc = localtime(&t);
/*
   printf("Local time is: %s", asctime(loc));
*/
 
   LOCmon =loc->tm_mon;
   LOCdaym=loc->tm_mday;
   LOCdayw=loc->tm_wday;
   LOCdayy=loc->tm_yday;
   LOCyear=loc->tm_year;
   LOChour=loc->tm_hour;
   LOCmin =loc->tm_min;
   LOCsec =loc->tm_sec;
 
   gmt = gmtime(&t);
/*
   printf("GMT is:        %s", asctime(gmt));
*/

   GMTmon =gmt->tm_mon;
   GMTdaym=gmt->tm_mday;
   GMTdayw=gmt->tm_wday;
   GMTdayy=loc->tm_yday;
   GMTyear=gmt->tm_year;
   GMThour=gmt->tm_hour;
   GMTmin =gmt->tm_min;
   GMTsec =gmt->tm_sec;
}
void WhatMode(void)
/* determine current mode and convert Mean Anomaly to 0-256 */
{
   int i;

   CurrentMAd = MeanAnomaly*128.0/PI;
   CurrentMA = AINT(CurrentMAd);

/*  handle back propagation cases.... */

   if (CurrentMAd < ZERO){
      strcpy(CurrentMode,Modes[WhichSatellite][0].ModeStr);
      return;
   }

   for (i=0;i<=SATS[WhichSatellite].NumModes;i++){
      if (CurrentMA >= Modes[WhichSatellite][i].MinPhase && 
          CurrentMA < Modes[WhichSatellite][i].MaxPhase)
         strcpy(CurrentMode,Modes[WhichSatellite][i].ModeStr);
   }
}
