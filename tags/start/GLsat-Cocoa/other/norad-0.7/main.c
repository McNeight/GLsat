/*
 *  main.c          April 10  2001
 *
 *  A skeleton main() function to demonstrate the use of
 *  the various NORAD ephimerides in the norad.c file.
 *  The TLE sets used are those provided in NORAD's spacetrack
 *  report #3 so that a comparison with their own results
 *  can be made. The results produced by these software agree
 *  with NORAD's to the 5th or 6th figure, whatever this means!
 *  (But please note that NORAD uses mostly 'float' types with
 *  only a few 'doubles' for the critical variables).
 */

#include "norad.h"

/* Main program */ 
int
main(void)
{
  vector_t vel, pos; /* Satellite position and velocity vectors */

  tle_t tle; /* Pointer to two-line elements set for satellite */

  /* Data for the prediction type and time period */
  double   ts = 0.;    /* Time since TLE epoch to start predictions */
  double   tf = 1440.; /* Time over which predictions are required  */
  double delt = 360.;  /* Time interval between predictions         */  

  double tsince; /* Time since epoch (in minutes) */

  char ephem[5]; /* Ephemeris type string */

  int i; /* Index for loops etc */

  /* Two-line Orbital Elements for the test case satellites */
  tle_t sdp_tle =  /* TLE's for SDP4 and SDP8 */
  { 80230.29629788, 1.431103E-2, 0.0, 1.4311E-2,
    46.7916, 230.4354, .7318036, 47.4722, 10.4117, 2.28537848 };

  tle_t sgp_tle =  /* TLE's for SGP, SGP4 and SGP8 */
  { 80275.98708465, 7.3094e-4, 1.3844e-4, 6.6816e-5,
    72.8435, 115.9689, .0086731, 52.6988, 110.5714, 16.05824518 };

  for (i=1; i<=5; i++)  /* Loop for each type of ephemeris */ 
    {
      /* !Clear all flags! */
      /* Before calling a different ephemeris  */
      /* or changing the TLE set, flow control */
      /* flags must be cleared in main().      */
      ClearFlag(ALL_FLAGS);

      /* Select the sgp or sdp TLE set for use below */
      if (i<=3)
	tle = sgp_tle;
      else
	tle = sdp_tle;

      /* Copy the apropriate ephemeris name into ephem[] */
      switch(i)
	{
	case 1: strcpy(ephem,"SGP ");
	  break;
	case 2: strcpy(ephem,"SGP4");
	  break;
	case 3: strcpy(ephem,"SGP8");
	  break;
	case 4: strcpy(ephem,"SDP4");
	  break;
	case 5: strcpy(ephem,"SDP8");
	}

      /* Select ephemeris type */
      /* Will set or clear the DEEP_SPACE_EPHEM_FLAG  */
      /* depending on the TLE parameters of the satellite   */
      select_ephemeris(&tle);

      if( isFlagSet(DEEP_SPACE_EPHEM_FLAG) )
	printf("\nDeep-Space type Ephemeris (SDP*) selected:");
      else
	printf("\nNear-Earth type Ephemeris (SGP*) selected:");

      /* Print some titles for the results */
      printf("\nEphem:%s   Tsince         "
             "X/Xdot           Y/Ydot           Z/Zdot\n",ephem);

      /* Calling of NORAD routines */
      /* Each NORAD routine (SGP, SGP4, SGP8, SDP4, SDP8)   */
      /* will be called in turn with the apropriate TLE set */
      for(tsince = ts; tsince <= tf; tsince += delt)
	{
	  switch(i)
	    {
	    case 1:  SGP(tsince, &tle, &pos, &vel);
	      break;
	    case 2: SGP4(tsince, &tle, &pos, &vel);
	      break;
	    case 3: SGP8(tsince, &tle, &pos, &vel);
	      break;
	    case 4: SDP4(tsince, &tle, &pos, &vel);
	      break;
	    case 5: SDP8(tsince, &tle, &pos, &vel);
	      break;
	    }

	  /* Calculate and print results */
	  pos.x = pos.x*xkmper/ae; /* Cartesian Position x */
	  pos.y = pos.y*xkmper/ae; /* Cartesian Position y */
	  pos.z = pos.z*xkmper/ae; /* Cartesian Position z */
	  vel.x = vel.x*xkmper/ /* Velocity x */
                  ae*xmnpda/86400.0;
	  vel.y = vel.y*xkmper/ /* Velocity y */
                  ae*xmnpda/86400.0;
	  vel.z = vel.z*xkmper/ /* Velocity z */
                  ae*xmnpda/86400.0;

	  printf("       %12.4f %16.6f %16.6f %16.6f \n",
		 tsince,pos.x,pos.y,pos.z);
	  printf("                    %16.6f %16.6f %16.6f \n",
		 vel.x,vel.y,vel.z);

	} /* End of for(tsince = ts; tsince <= tf; tsince += delt) */
    } /* End of for (i=1; i<=5; i++) */

  return(0);
} /* End of main() */

/*------------------------------------------------------------------*/
