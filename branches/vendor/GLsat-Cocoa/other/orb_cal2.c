#if 0

Satellite Education News Tracker

Written by ??? (sorry, I've lost track of the original author)


Revision November 14, 2000 by Paul Williamson, KB5MU (flagged "PTW")
1. Corrected parsing of user-entered dates.
2. Expanded temporary string buffers as a partial patch for sloppy use of
   strcpy() in handling command line arguments. Note that for production
   use, buffer management is still buggy.

Revision July 7, 2000 by Paul Williamson, KB5MU (flagged "PTW" in comments)
1. Added Y2K patches
2. Corrected handling of daylight savings time. Note that this program relies
implicitly on the TZ environment variable to set the local timezone, at least
if compiled with any Microsoft DOS compiler.
3. Wrapped this header in pre-processor directives so the compiler will skip it



SB EDUC @ WW < EA2CLS   $EDP94060                              
EDN#58 Sat Tracking Program in 'C'

The Satellite Education News Tracker was written and successfully compiled
using MS Quick C.  Most C compilers should compile with very little
difficulty.

EDNTRACK.EXE requires the following arguments:
        s:[satellite name]--as it appears in orbital elements file
        e:[2-Line element file name]--must be in current directory
        d:--prompt user for date/time, default is system date/time <optional>
        l:[qth latitude]--N Lat positive/S Lat negative
        o:[qth longitude]--E Lon positive/W Lon negative

Example: C:>edntrack s:WO-18 e:orbs2l.056 l:45.6 o:11.5 <cr>

Two batch files, MULTISAT.BAT and SATLIST.BAT have been included to provide
the end user with an example of how the Ed News Tracker can be used.  The
use of DOS command line redirection i.e. '>' adds additional possibilities.

------------------------------Start of C Program ---------------------------
#endif

/*  Satellite Education News Tracker */
/*  Non-Commercial Use by Radio Amateurs and Educators is Permitted */
/*  Version Date -- 02/28/94  */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys\timeb.h>

#define PI      3.141592654
#define GHAA    100.29          /*Siderial starting point-1978 Jan 01 @ 0000 utc*/
#define EROTRAT 360.985647      /*Earth's rotation rate in deg/day*/
#define ERADIUS 6378.0          /*Earth's radius in km*/
#define MINDAY  1440            /*Minutes per day*/
#define DAYYR   365.25          /*Days per year*/
#define DAYMO   30.6            /*Days per month*/
#define HRSDAY  24              /*Hours per day*/
#define DAY1978 28125           /*Days since 1978 Jan 01 @ 0000 utc*/
#define GM4PI2  331.25          /*Numerical value of G * 4 * Earth Mass * PI^2*/
#define DT      0.000694444     /*Time change between AOS/LOS calculations 1/1440*/

/*-------------Function and Structure Declarations---------------*/
/*---------------------------------------------------------------*/
/*Structure for satellite element set*/
struct sat_ele
{
        char name[10];
        double epoch_yr;
        double epoch_day;
        double inc;
        double raan;
        double eccen;
        double arg_peri;
        double m_anomaly;
        double m_motion;
        double decay_rate;
};
/*Structure for saving calculated satellite position*/
struct sat_pos
{
        double elevation;
        double azimuth;
        double range;
        double ma;
        double lat;
        double lon;
};

int matherr(struct exception *error_info);
double cos(double x);
double sin(double x);
double floor(double x);
double sqrt(double x);
double pow(double x, double y);
int matherr(struct exception *error_info);
double atan2(double y, double x);

void ftime(struct timeb *timeptr);
char *_strdate(char *date);
char *_strtime(char *time);
int calculate_pos(struct sat_ele *se_ptr, struct sat_pos *sp_ptr, double la, double lo, double t);
void rad_to_polar(double x, double y, double *p, double *r);
void polar_to_rad(double p, double r, double *x, double *y);
double rad_to_deg(double x);
double deg_to_rad(double x);
double sgn(double x);
double dabs(double x);
void strmid(char str_out[10], char str_in[10], int x, int y);

/* ------------------Main---------------------*/

main(int argc, char *argv[])
{
        struct sat_ele orb_param =
        {
                "WO-18",
                94,
                045.763282,
                98.6054,
                133.5798,
                0.001151,
                188.3662,
                171.733,
                14.299932,
                0
        };                      /*Default satellite with orbital elements*/

        FILE *fp;
        struct timeb time_buffer;
        struct sat_pos sat;
        double qth_lat = 45.608;/*Default latitude and longitude for IK3WVJ*/
        double qth_lon = 11.484;/*Insert your QTH lat/lon here before recompiling*/
        double hr, min, sec, dt_temp, day_time, alos_temp;
        int i, ff, c, count, r, dp, day, mon, year, alos_hr, alos_min;
        char t_buffer[145], pos_time[8], pos_date[8], temp[150], ele_file[150], aoslos;

        if (argc < 2)          /*No argument help information*/
        {
                printf("EDNTRACK.EXE requires the following arguments:\n");
                printf("s:[satellite name] -- as it appears in orbital elements file\n");
                printf("e:[2-Line element file name] -- must be in current directory\n");
                printf("d: -- prompt user for date/time, default is system date/time\n");
                printf("l:[qth latitude] -- N Lat positive/S Lat negative\n");
                printf("o:[qth longitude] -- E Lon positive/W Lon negative\n");
                printf("Example: C:>edntrack s:WO-18 e:orbs2l.056 l:45.6 o:11.5 <cr>\n");

                exit(0);
        }

        dp = 0;
        for(i=0; i < argc; i++)         /*Save command line arguments*/
        {
                strcpy(temp, argv[i]);
                if(temp[0] == 'd')
                        dp = 1;

                if(temp[0] == 's')
                        strcpy(orb_param.name, &temp[2]);

                if(temp[0] == 'e')
                        strcpy(ele_file, &temp[2]);

                if(temp[0] == 'l')
                        qth_lat = atof(&temp[2]);

                if(temp[0] == 'o')
                        qth_lon = atof(&temp[2]);
        }

        count = 0;
        ff = 0;
        if(fp = fopen(ele_file, "rb"))  /*Open orbital element file*/
        {
                while(((c = fgetc(fp)) != EOF) && (ff == 0))
                {
                        if(orb_param.name[count] == c)  /*Search for satellite*/
                        {
                                count++;
                                if(count == strlen(orb_param.name))
                                        ff = 1;
                        }
                        else
                                count = 0;
                }
                if(ff == 0)
                        printf("Satellite `%s' not found in element file, using WO-18.\n",orb_param.name);
                else
                {
                        for(i = 0; i < 144; i++)        /*Load elements into buffer*/
                                t_buffer[i] = fgetc(fp);

                        strmid(temp, t_buffer, 19, 2);  /*Extract and save Epoch Year*/
                        orb_param.epoch_yr = atof(temp);
                        if (orb_param.epoch_yr < 50)    /* PTW: Y2K */
                                orb_param.epoch_yr += 100;    /* PTW: Y2K */

                        strmid(temp, t_buffer, 21, 12); /*Extract and save Epoch Day*/
                        orb_param.epoch_day = atof(temp);

                        strmid(temp, t_buffer, 81, 7);  /*Extract and save Inclination*/
                        orb_param.inc = atof(temp);

                        strmid(temp, t_buffer, 89, 8);  /*Extract and save R.A.A.N.*/
                        orb_param.raan = atof(temp);

                        strmid(temp, t_buffer, 98, 7);  /*Extract and save Eccentricity*/
                        orb_param.eccen = atof(temp) / 10000000;

                        strmid(temp, t_buffer, 106, 8); /*Extract and save Argument of Perigee*/
                        orb_param.arg_peri = atof(temp);

                        strmid(temp, t_buffer, 115, 8); /*Extract and save Mean Anomaly*/
                        orb_param.m_anomaly = atof(temp);

                        strmid(temp, t_buffer, 124, 11);/*Extract and save Mean Motion*/
                        orb_param.m_motion = atof(temp);

                        strmid(temp, t_buffer, 35, 9);  /*Extract and save Decay Rate -- Not used*/
                        orb_param.decay_rate = atof(temp);
                }
                fclose(fp);
        }
        else
        {
                printf("Error in opening file\n");

        }

        ftime(&time_buffer);    /*Get system date and time*/

        if (dp == 1)            /*Prompt user for date*/
        {
                printf("Enter Date mm/dd/yy : ");
                scanf("%s8",&pos_date);
                printf("Enter Time hh:mm:ss : ");
                scanf("%s8",&pos_time);
        }
        else                    /*Use system date and time*/
        {
                _strdate(pos_date);
                _strtime(pos_time);
        }
        temp[0] = '\0';                 /*Extract and save month from date string*/
        strmid(temp, pos_date, 0, 2);   /* PTW: Months can have two digits */
        mon = atoi(temp);

        strmid(temp, pos_date, 3, 2);   /*Extract and save day from date string*/
        day = atoi(temp);

        strmid(temp, pos_date, 6, 2);   /*Extract and save year from date string*/
        year = atoi(temp);
        if (year < 50)                  /* PTW: Y2K */
                year += 100;            /* PTW: Y2K */

        strncpy(temp, pos_time, 2);     /*Extract and save hour from time string*/
        hr = (double)(atoi(temp) + time_buffer.timezone / 60);
        if (time_buffer.dstflag != 0)   /* PTW: Daylight Savings time */
                hr -= 1;                /* PTW: Daylight savings time */

        strmid(temp, pos_time, 3, 2);   /*Extract and save minutes from time string*/
        min = (double)atoi(temp);

        strmid(temp, pos_time, 6, 2);   /*Extract and save seconds from time string*/
        sec = (double)atoi(temp);

        mon = mon + 1;  /*The following calculates # of days and fraction of a day*/
        if (mon < 4)    /*since GHAA reference point 1978 Jan 01 @0000 utc.*/
        {
                year = year - 1;
                mon = mon + 12;
        }

        day = day + floor(year * DAYYR) + floor(mon * DAYMO) - 28553;
        day_time = ( hr + min / 60 + sec / 3600) / 24 + (double)day;
        dt_temp = day_time;     /*Save time for use in calculating AOS/LOS*/

        r = calculate_pos(&orb_param, &sat, qth_lat, qth_lon, day_time);

        /*Prints out "Satellite name - Azimuth - Elevation - Range - MA - Latitude - Longitude*/
        printf("%s %s %7.2f %7.2f %8.1f %4.0f %5.1f %5.1f ", orb_param.name, pos_time, sat.azimuth, sat.elevation, sat.range, sat.ma, sat.lat, sat.lon);

        /*The following AOS/LOS routine is slow unless used with math cop*/
        /*This portion can be improved...So look for changes in future*/
        if (sat.elevation >= 0) /*Calculate time until LOS*/
        {
                do
                {
                        day_time = day_time + DT;
                        r = calculate_pos(&orb_param, &sat, qth_lat, qth_lon, day_time);
                }
                while (sat.elevation >= 0.00);

                aoslos = 1;     /*Set LOS flag*/
        }

        else
        {
                do
                {
                        day_time = day_time + DT;
                        r = calculate_pos(&orb_param, &sat, qth_lat, qth_lon, day_time);
                }
                while (sat.elevation <= 0.00);


                aoslos = 0;
        }

        alos_temp = (day_time - dt_temp) * 24;  /*Calculate AOS/LOS*/
        alos_hr = (int)floor(alos_temp);
        alos_min = (int)floor((alos_temp - floor(alos_temp)) * 60);

        if (aoslos == 1)
                printf("LOS %2.2d:%2.2d\n", alos_hr, alos_min); /*Print LOS*/
        else
                printf("AOS %2.2d:%2.2d\n", alos_hr, alos_min); /*Print AOS*/
}
/*Function call to do all the number crunching*/
int calculate_pos(struct sat_ele *se_ptr, struct sat_pos *sp_ptr, double la, double lo, double t)
{
        double period, x, y, l, d, m, n, p, a, b, u, g, h, q, v, k, r, z, s, j;
        char temp[2] = "";

        x = 0;
        y = 0;

        period = MINDAY / se_ptr->m_motion;
        g = floor(DAYYR * (se_ptr->epoch_yr - 1)) - DAY1978 + se_ptr->epoch_day - se_ptr->m_anomaly / se_ptr->m_motion / 360;
        /*Calculate orbital period*/
        n = se_ptr->m_motion * 2 * PI;
        /*Calculate semi-major axis*/
        a = GM4PI2 * pow(period, 0.6666666666667) / ERADIUS;/*Calculate semi-major axis*/
        /*Calculate rate of change of Argument of Perigee*/
        v = 4.97 * pow(a, -3.5) * (5 * pow(cos(deg_to_rad(se_ptr->inc)), 2)-1) / pow(1-pow(se_ptr->eccen,2),2);
        /*Calculate rate of change of R.A.A.N.*/
        q = -9.95 * pow(a, -3.5) * cos(deg_to_rad(se_ptr->inc)) / pow(1-pow(se_ptr->eccen,2),2);
        b = deg_to_rad(la);     /*Change Latitude to radians*/
        l = deg_to_rad(lo);     /*Change Longitude to radians*/

/*The following based on Karl Meinzer's (DJ4ZC) Algorithms for Sharp PC1246.*/
/*With minor changes added by James Miller (G3RUH)*/
/*The program makes extensive use of rectangular-to-polar and polar-to-*/
/*rectangular transformations, and unit geometry*/
        d = t - g;
        k = deg_to_rad(d * q + se_ptr->raan - GHAA - t * 360.985647) - l;

        m = d * n;
        r = se_ptr->eccen;
        p = m;

        /*Begin calculating True Anomaly and satellite geocenter distance*/
        do
        {
                polar_to_rad(p, r, &x, &y);
                h = (m - p + y) / (1 - x);
                p = p + h;

        }
        while (dabs(h) > 0.001);        /*Check for convergence*/

        r = 1;
        polar_to_rad(p, r, &x, &y);
        y = y * sqrt(1 - se_ptr->eccen * se_ptr->eccen);
        x = x - se_ptr->eccen;

        rad_to_polar(x, y, &p, &r);
        r = a * r;
        p = p + deg_to_rad(se_ptr->arg_peri + d * v);

        polar_to_rad(p, r, &x, &y);
        h = x;
        r = y;
        p = deg_to_rad(se_ptr->inc);

        polar_to_rad(p, r, &x, &y);
        s = y;
        y = x;
        x = h;

        rad_to_polar(x, y, &p, &r);
        p = p + k;
        k = p + l;
        u = r;

        polar_to_rad(p, r, &x, &y);
        z = s - sin(b);
        x = x - cos(b);

        h = y;
        y = z;
        rad_to_polar(x, y, &p, &r);
        p = p - b + PI / 2;

        polar_to_rad(p, r, &x, &y);
        j = y;
        y = h;
        rad_to_polar(x, y, &p, &r);

        sp_ptr->azimuth = rad_to_deg(PI - p);
        x = r;
        y = j;
        rad_to_polar(x, y, &p, &r);

        m = m / (2 * PI);
        h = t;
        sp_ptr->ma = 256 * (m - floor(m));
        sp_ptr->elevation = rad_to_deg(p);
        sp_ptr->range = r * ERADIUS;

        k = k / 2 / PI;
        k = (k - floor(k)) * 2 * PI;
        x = u;
        y = s;

        rad_to_polar(x, y, &p, &r);
        sp_ptr->lat = rad_to_deg(p);
        sp_ptr->lon = rad_to_deg(k);

        return(0);
}

/*Rectangular to polar transformation*/
void rad_to_polar(double x, double y, double *p, double *r)
{
        *r = sqrt(x * x + y * y);
        if (x == 0)
                *p = PI / 2 * sgn(y);
        else
                *p = atan2(y , x);
}

/*Polar to rectangular transformation*/
void polar_to_rad(double p, double r, double *x, double *y)
{
         *x = r * cos(p);
         *y = r * sin(p);
}

/*Return sign of a number*/
double sgn(double x)
{
        if (x >= 0)
                return(1);
        else
                return(-1);
}

/*Return absolute value of a double type*/
double dabs(double x)
{
        if (x >= 0)
                return(x);
        else
                return(x * -1.0);
}

/*Convert radians to degrees*/
double rad_to_deg(double x)
{
        return (x * 180 / PI);
}

/*Convert degrees to radians*/
double deg_to_rad(double x)
{
        return (PI * x / 180);

}

/*Extract y characters from a string starting at x*/
void strmid(char str_out[], char str_in[], int x, int y)
{
        int i;

        for(i=x; i < (x + y); str_out[i - x] = str_in[i], i++);
        str_out[i-x] = '\0';
}

#if 0
------------------------------End of C program-------------------------------

The following is the text for "multisat.bat":
echo off
rem The first parameter is the name of the orbital element file followed
rem by your latitude and longitude.
satlist orbs2l.056 45.6 11.5


The following is the text for "satlist.bat":
echo Name     Time  Azimuth   Elev  Range      MA  Lat  Lon   AOS/LOS
edntrack s:AO-13 e:%1 l:%2 o:%3
edntrack s:AO-16 e:%1 l:%2 o:%3
edntrack s:DO-17 e:%1 l:%2 o:%3
edntrack s:WO-18 e:%1 l:%2 o:%3
edntrack s:LO-19 e:%1 l:%2 o:%3
edntrack s:FO-20 e:%1 l:%2 o:%3
edntrack s:AO-21 e:%1 l:%2 o:%3
edntrack s:UO-22 e:%1 l:%2 o:%3
edntrack s:KO-23 e:%1 l:%2 o:%3
edntrack s:KO-25 e:%1 l:%2 o:%3
edntrack s:IO-26 e:%1 l:%2 o:%3
edntrack s:POSAT e:%1 l:%2 o:%3
edntrack s:MIR   e:%1 l:%2 o:%3
/EX

#endif
