This program is a very simple tool used to predict when satellites are
visible to a certain location.  It is based on PLAN13 by James Miller
(G3RUH) and is provided as is.  

It is written in C (my first attempt) and is not optimized for 
anything.  I am sure there are better ways to do some of the
things in here but this is pretty bullet proof and does what I
want.

Currently I am using Borland Turbo C++ 3.0 on my PC to compile it
and the GNU gcc-2.4.5 compiler on a SUN workstation.

There are a minimum set of data files required for STP to run.

1)  myqth.dat   ASCII file with the coordinates of the station

format:
         Station name (any string)
         Latitude (degrees)
         Longitude (degrees. Negative west of Greenwich)
         Altitude (meters)

table of minimum look angles (input as a function of azimuth)

EXAMPLE: (neighbors house in the way between 190-194 degrees azimuth)

azimuth  minimum angle
0        2
45       5
90       7
135      3
180      0
190      15
191      18
192      35
193      12
194      6
225      6
270      8
315      5
360      2

NOTE #1:   If you only input 1 (one) line of values (azimuth and minimum angle)
           the code will use this for all azimuths.

NOTE #2:   If you have input a table and want to turn it off temporarily,
           just change the first minimum angle to be a value less
           than zero (e.g. -0.10)

NOTE #3:   Currently put azimuth values in from 0 to 360.  Eventually
           it will work with plus/minus values.

Code linearly interpolates between values.

Maximum of 360 input values


STP will ask for all the data to write to this file the first time it is run
or you can use any text editor to create it.

2)  nasa.dat or kepler.dat   ASCII file with satellite elements.

format:

        nasa.dat  Standard NASA 2 Line orbital element file
        kepler.dat Orbital info in text format

        NOTE:  Currently only 50 (DOS) and 300 (UNIX) satellites and can be 
               loaded at any one time.

        NOTE:  STP currently looks for nasa.dat first.  If it is found then
               the orbital info is taken from it.  If you only want to use
               the kepler type of format only have kepler.dat in the current
               directory.

3)  stp.ini Control flags and information used to compute signal strengths.  

format:

15.0    FindStep (minutes) initial step size in finding aos/los
1.0     FindAngleError (degrees) if within this angle quit
0.1     FindStepMinimum (minutes)  minimum step size
0       plot flag  0-none  1-yes
0       0-360 (0)  1 -180 to 180
1       ground trace 0 - write altitude to plot file   1 write 0.0
145.9 2400.0    Frequency Mode B   Mode S (MHz)
2300.0 5000.0   Receiver Bandwidth Mode B   Mode S (Hz)
17.0   24.0     Receiver Antenna Gain Mode B Mode S (dBi)
60.0   100.0    Receiver Noise Temperature (Deg Kelvin)
30.0            Average number of users (adjusts downlink power)

FindStep is an initial step size used in finding AOS/LOS.  If you have a
fast computer (>30Mhz 486) or you have alot of patience you can decrease 
this value to get slightly better answers.

FindAngleError and FindStepMinimum determine the "accuracy" to which the
AOS/LOS times are determined.

plot flag is used to turn on/off the output of a set of data files that
can be plotted using a user supplied routine.

360 +-180 flag determines how azimuth values are written out.

ground trace flag is used in conjunction with the plot flag to determine 
if the true altitude of the satellite is included in the plot file or
if a zero is written.

The next set of lines are used to define the parameters for estimating
the downlink signal strength.

Optional Data files:

1)  mode.dat  ASCII file with operating modes of satellites.  If no mode.dat
              file is found or a satellite in nasa/kepler.dat is not defined
              the mode defaults to "???".

format:

        satellite:  satname1
        beacon frequency (Mhz)  blon blat
        schedule
        schedule
        schedule
        satellite:  satname2
        beacon frequency (Mhz)  blon blat
        schedule

        etc... as few or as many satellites as you want

        The schedule is entered in terms of Mean Anomaly.  Mean
        Anomaly describes the position of a satellite in its orbit.
        In tracking programs Mean Anomaly is an angle that goes between
        0 and 360 degrees.  For scheduling purposes it is useful to
        specify MA in units of 256 of a circle.

        Perigee (lowest point on orbit) is at MA 0 degrees and 0/256.
        Apogee (highest point) is at MA 180 degrees and 128/256.

        For example ao-13 has the following schedule (March 9,1994)

        From MA   0 to MA  30 Mode B omni antennas
        From MA  30 to MA  90 Mode B high gain antennas
        From MA  90 to MA 120 Mode BS high gain antennas
        From MA 120 to MA 145 Mode S high gain antennas
        From MA 145 to MA 150 S beacon only
        From MA 150 to MA 180 Mode BS high gain antennas
        from MA 180 to MA 230 Mode B high gain antennas
        From MA 230 to MA 256 Mode B omni antennas


        This schedule would be entered as;

        000030Bom
        030090B..
        090120BS.
        120145S..
        145150Sbc
        150180BS.
        180230B..
        230256Bom

        Simplier schedules can also be entered.  For example for AO-10

        000256B..

        STP uses the mode names of Bom or BS. or S.. to perform some simple
        received signal calculations and to modify the squint calculation
        between the omnidirectional and high gain antennas.

        The beacon frequency is used to compute the doppler shift.

        Blat and Blon refer to the pointing angles the antennas on the
        satellite make with the earth.  For example if Blon=180 and Blat=0
        then when the satellite is at apogee the antennas are pointing
        directly at the surface of the earth.  These values are only
        important for high altitude satellites like AO-13, AO-10 or ARSENE 
        (which is dead as of this writing (October 31,1994)).  These angles are
        used to compute the squint angle that the antennas make with
        your QTH.  At high squint angles (30-50 degrees) the signals get
        weak and due to the spin of the satellite the signals are modulated.

        See the included mode.dat file as an example.

2)  revcor.dat  ASCII file containing information relating to the correct
    revolution data.

        The elements that are normally used have a relative revolution number
        that changes at the beginning of each year.  Sometimes it is
        valuable to know what the correct revolution number is.  This file
        allows the user to include a factor that corrects the data in the
        element file.

format:
        satellite name
        correction factor

3)  qth.dat  ASCII file containing information relating to mutual visibility
    calculations.

        Sometimes you want to know when a mutual window will exist 
        between your location and some other location (e.g. chasing DX or
        working a friend on vacation).  The file qth.dat contains
        the latitude,longitude, altitude and name for these other locations.
        To select a location or locations the user edits the file and
        places the following command words around the cities of interest.

        starthere
          city1
          city2
          city3
        stophere

        Every city between starthere and stophere will be include in the
        calculation of mutual visibility.  Currently the limit is 90 cities
        at any one time.

Lack of Features:

This code will NOT drive any rotors as of yet.   It also does not have any 
fancy graphics as of yet. I work on the code mainly as a learning experience 
and as a practical means to operating the satellites without a computer 
controlled rotor system so there is no fixed set of releases or future plans.


NOTES on compiling:

If you have obtained the source code for STP then you will need to make some
minor modifications.

If you are operating on a UNIX based platform then insure that the following
statement exists in stp.c

#define UNIX

also make sure that the path statements in stp.unx agree with the location 
of the datafiles.

On a DOS platform:

#define MSDOS

and set the paths in stp.dos.  NOTE that 2 (two) backslashes are required
in the pathname.  C uses \ as indication of a special character and if you 
want to use a \ then you must use two \\ (first slash says use next item as
a character and not a special character).

With Borland Turbo C++ 3.0 I use to following command (on a 486 with 8Mb);

tcc -O1 -O2 -ml stp


Makefile for GNU C

# Makefile
CFLAGS =

PROGRAM=	stp
HEADERS=	stp.unx
SOURCES=	stp.c bioskey.c
OBJECTS=	stp.o bioskey.o
MAKEFILE=	Makefile

LIBS=		-lm

CC=		gcc

all:		$(PROGRAM)

$(PROGRAM):	$(OBJECTS)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(OBJECTS) $(LIBS)



Useage:

In the following discussion <cr> means press the return key.

STP is run from the command line.  if you just type  STP <cr> you will get a 
listing of the currently available commands and some acknowledgements.

NOTE: the options are case sensitive (e.g. r and R are different options).

  a....AOS/LOS times (relative time) 
  A....AOS/LOS times (absolute time) 
  c....Sun/Moon real time display
  C....Perform Check Sum calculations on 2-line nasa file
           (can put filename on command line... STP C nasa.dat)
  d....Dump information about satellites (beacon freq, alat,alon, # of modes)
  e....View/Edit orbital parameters
  h....Simple suggestions
  m....Mutual visibility calculation
  M....Real Time Mutual visibility.  Use the following to add observers
         g-add based on grid locator
         l-add based on lat,lon,alt
         o-add based on names in qth.dat
  p....Prediction for a single satellite
  r....Real-time table of currently visible satellites, additional parameter
           after r is update rate in seconds (default is 1 second)
  R....Real-time table of all satellites, additional parameter  after R is
           update rate in seconds (default is 1 second)
  s....Same as option R but uses a selected set of satellites
  S....Selects satellites for option s

The first time you run STP it will ask you for information about your qth,
unless you have created myqth.dat prior to execution.  After you finish 
inputting the info the above table will appear. 

After this first time any time you enter STP <cr> the above table will print
on the screen.

OPTION a:

This option loops over all satellites in the data file and determines the 
RELATIVE time to acquisition of the satellite (elevation > 0.0) and loss of
the satellite.  If the satellite rise time is greater than 2 days in the 
future that a message is printed to that affect.  

The format of the time is days:hours:minutes:seconds


You will note that some satellites show no AOS/LOS times.  These satellites are 
judged to be in a geostationary orbit by STP and are either constantly in view 
or out of view. 

If the satellite is currently in view then Dt AOS is shown as 00:00:00.

The following is an EXAMPLE;

        Sat       AOS            LOS
       AO13sm 00:07:01:44    00:08:16:07
      OSCAR10 00:03:18:07    00:10:40:18
       UOSAT2 00:02:21:28    00:00:09:39
          MIR 00:00:56:19    00:00:10:25
     RS-10/11 00:00:46:08    00:00:12:13
        AO-13 00:07:01:01    00:08:16:02
        UO-14 00:06:52:44    00:00:08:39
        UO-15 00:06:52:50    00:00:09:20
        DO-17 00:06:42:48    00:00:04:54
        WO-18 00:07:45:31    00:00:13:11
        LO-19 00:07:04:03    00:00:09:04
        FO-20 00:00:21:47    00:00:40:48
        AO-21 00:08:35:43    00:00:11:31
     RS-12/13 00:00:40:01    00:00:16:03
        UO-22 00:06:54:24    00:00:12:56
        KO-23 00:06:17:54    00:00:18:12
       ARSENE 01:03:02:55    00:18:43:03
        AO-27 00:07:08:16    00:00:13:29
      ITAMSAT 00:06:02:20    00:00:05:18
        KO-25 00:06:01:06    00:00:08:31
     1994067B Rise/Set Time > 2 Days from now!!
     1994067C Rise/Set Time > 2 Days from now!!

OPTION A:
Same as above except shows actual times instead of RELATIVE.

EXAMPLE:
        Sat          Day    AOS               Day     LOS
       AO13sm      Tuesday 02:41:51         Tuesday 10:57:53
      OSCAR10       Monday 22:58:07         Tuesday 09:38:31
       UOSAT2       Monday 22:01:32          Monday 22:11:12
          MIR       Monday 20:36:20          Monday 20:46:44
     RS-10/11       Monday 20:26:12          Monday 20:38:25
        AO-13      Tuesday 02:41:02         Tuesday 10:57:05
        UO-14      Tuesday 02:32:47         Tuesday 02:41:27
        UO-15      Tuesday 02:32:54         Tuesday 02:42:15
        DO-17      Tuesday 02:22:52         Tuesday 02:27:46
        WO-18      Tuesday 03:25:36         Tuesday 03:38:42
        LO-19      Tuesday 02:44:07         Tuesday 02:53:11
        FO-20       Monday 20:01:50          Monday 20:38:21
        AO-21      Tuesday 04:15:46         Tuesday 04:27:17
     RS-12/13       Monday 20:20:05          Monday 20:36:08
        UO-22      Tuesday 02:34:27         Tuesday 02:47:18
        KO-23      Tuesday 01:57:58         Tuesday 02:16:05
       ARSENE      Tuesday 22:42:53       Wednesday 17:26:02
        AO-27      Tuesday 02:48:20         Tuesday 03:01:50
      ITAMSAT      Tuesday 01:42:21         Tuesday 01:47:40
        KO-25      Tuesday 01:41:06         Tuesday 01:49:38

OPTION c:
Provides a realtime display of the sun/moon azimuth,elevation and range.
Additional parameter on command line is update rate in seconds.

Screen capture of option c;

 19:42:40 Sun :  218.69   31.83   151535606.370   
          Moon:  256.33   12.62      366631.759


format is Azimuth (degrees)   Elevation (degrees)   Range (Km)

OPTION C:
Perform checksum calculation on nasa 2 line file.

useage:

stp C <filename>

Outputs corrected file to newnasa.dat.

If the user fails to enter a name on the command line STP will
prompt for one.

OPTION d:

This option dumps out some basic information about the satellites as entered
through nasa.dat or kepler.dat and mode.dat.  

EXAMPLE:

 #    Beacon  Alon  Alat   Apogee    Perigee   Period   Ecc     Age
        Mhz   deg   deg      Km         Km       min            days
  8  145.8120  230    0   38016.1     790.6     719.2  0.72194  82.81 AO13sm 
  1  145.8120  209  -17   35451.1    3994.6     732.6  0.60259  14.50 OSCAR10 
  1  435.0250    0    0     671.1     655.7     102.7  0.00109   6.44 UOSAT2 
  1  145.5500    0    0     396.2     393.2      96.8  0.00022   5.19 MIR 
  1   29.3570    0    0    1000.5     981.9     109.9  0.00126   7.19 RS-10/11 
  8  145.8120  230    0   38072.2     733.2     719.2  0.72416   4.12 AO-13 
  1  435.0700    0    0     800.0     784.4     105.5  0.00109   6.75 UO-14 
  1  435.1200    0    0     801.6     787.3     105.5  0.00100   6.25 UO-15 
  1  145.8250    0    0     799.7     783.4     105.5  0.00114   6.25 DO-17 
  1  437.0751    0    0     800.1     783.2     105.5  0.00118   6.25 WO-18 
  1  437.1530    0    0     800.1     782.5     105.5  0.00123   7.31 LO-19 
  1  435.7970    0    0    1745.7     911.3     117.5  0.05413   6.69 FO-20 
  1  145.9870    0    0    1010.5     956.1     109.7  0.00370   4.62 AO-21 
  1   29.4080    0    0    1007.7     962.5     109.8  0.00307   7.06 RS-12/13 
  1  435.1200    0    0     773.5     763.7     105.0  0.00069   6.75 UO-22 
  1  437.1750    0    0    1328.1    1304.5     117.3  0.00153   5.75 KO-23 
  1 2447.0001  120    0   36748.1   17303.1    1060.7  0.29106   8.25 ARSENE 
  1  436.8000    0    0     805.6     793.7     105.7  0.00083   5.75 AO-27 
  0  145.0000    0    0     805.3     791.0     105.6  0.00100   5.81 ITAMSAT 
  1  436.5000    0    0     805.5     790.9     105.6  0.00102   7.25 KO-25 

If any mode data is set in modes.dat then # is how many modes it found.  
Beacon frequency in Mhz,  Alon/Alat are satellite pointing angles (sometimes 
called Blon and Blat), Apogee is apogee altitude in Km, Perigee is perigee 
altitude in Km, Period is orbital period in minutes, Ecc is orbit eccentricity 
(0 is perfect circular),  Age is the age of the element set and finally the 
satellite name.  This table is just a quick way to check the input data.

OPTION e:
Allows the user to edit/view the orbital parmeters for each satellite
loaded.  Edits which ever type of file STP is started with (nasa
2 line or kepler format).  

For the kepler format a new file named kepler.tmp is created.
For the nasa 2 line format nasa.tmp is created.

To utilize the modified database you must rename kepler.tmp to kepler.dat or
nasa.tmp to nasa.dat (see discussion of nasa/kepler.dat).

OPTION m:
This option allows you to determine the mutual visibility between your qth 
and some other location.  The included file qth.dat contains some of the more 
popular qth locations as well as some major cities.   To use this option you 
must edit the datafile and place the keywords starthere and stophere around the 
city or cites that you are interested in.  You can also just create a datafile
with then information you want in it.  Once you have done that then you issue 
the following;

1) STP m satellite name <cr> (if you know which satellite you want)

    or

2) STP then asks you for additional informtion

Enter Start Day (month day year):
Number of Days for Prediction:
Step Size (minutes....input negative value for seconds): 
FileName to write output to (cr for screen): 

Suggestions/Formats on inputs:

Start Day (format)
   Months 1-12  Days 1-31 Year 199?-???

Number of Days
   1 or more.  You can input as big a value as you want but remember that
   for LEO type satellites (RS-10/11, AO-21,...) the elements change due
   to drag and predictions longer than a month will start to show errors.

Step Size (minutes....input negative value for seconds): 
   5 is good for AO-13,AO-10 and other high altitude/Molynia type
   satellites.  Use 1 for RS-10/11, AO-21, or other LEO satellites.
   If the step size is input as negative then STP assumes the time
   is in seconds and adjusts the output accordingly.

FileName
   If you press <cr> at FileName prompt then the run is output to the screen,
   If you enter a filename the output is written to the filename specified.

The following example is looking for mutual visibility between KD4QIO and 
Stanley, Falkland Islands and Peter I using AO-13.  The left half of the table 
is the local information and the right half is for the other location(s).  
The information is provided as a function of UTC.

Az      azimuth of satellite (degrees), home QTH
EL      elevation of satellite (degrees), home QTH
Range   range from home QTH to satellite (Km)
Sqnt    "squint" angle (degrees) as viewed from home QTH

Az      azimuth of satellite (degrees), mutual site
EL      elevation of satellite (degrees), mutual site
Range   range from mutal site to satellite (Km)
Sqnt    "squint" angle (degrees)  as viewed from mutual site

Mde     mode the satellite is operating in

The last column is the site name (only usefull if more than one site selected)

    ----------------------------------------------------------------------
    Prediction Date : Monday October 31 1994    Amsat Day:   6147
    Prediction For  :  2 Sites
    Performed on    : October 31 1994 13:54:09
    Satellite       : AO-13 (Set:   988, Rev #:  5966, Age:   4.1 Days)
    Illumination    : 86.4%  SAZ/SEL 350.5/-1.3 AP/RAAN 353.6/222.8 
    Alon/Alat       : 230.0/ 0.0  Frequency: 145.8120 MHz 
    ----------------------------------------------------------------------
                Local            |             Dx
     UTC   Az   El   Range  Sqnt |  Az   El   Range  Sqnt   MA   Mde
    ----------------------------------------------------------------------
    0855  249   31   40803   51  | 316    0   43906   60  122.7  OFF Peter I 
    0900  250   30   40933   51  | 315    0   43908   59  124.6  OFF Peter I 
    .
    .
    1040  253    7   40969   39  | 294    5   41239   47  161.9  B.. Peter I 
    .
    .
    1105  254    2   40159   35  | 288    6   39706   44  171.2  B.. Peter I 
    1110  254    0   39953   35  | 287    6   39353   43  173.1  B.. Peter I 
    ----------------------------------------------------------------------
    Prediction Date : Tuesday November 01 1994    Amsat Day:   6148
    Prediction For  :  2 Sites
    Performed on    : October 31 1994 13:54:09
    Satellite       : AO-13 (Set:   988, Rev #:  5968, Age:   4.1 Days)
    Illumination    : 86.2%  SAZ/SEL 350.7/-2.3 AP/RAAN 353.7/222.7 
    Alon/Alat       : 230.0/ 0.0  Frequency: 145.8120 MHz 
    ----------------------------------------------------------------------
                Local            |             Dx
     UTC   Az   El   Range  Sqnt |  Az   El   Range  Sqnt   MA   Mde
    ----------------------------------------------------------------------
    0540  216   71   32866   68  | 324    0   38374   78   74.9  OFF Stanley 
    0545  217   70   33294   67  | 323    1   38729   77   76.8  OFF Stanley 
    .
    .
    0700  229   54   38123   56  | 308    4   42534   66  104.8  OFF Stanley 
    0705  230   53   38347   56  | 307    4   42690   65  106.6  OFF Stanley 
    0705  230   53   38347   56  | 340    0   43077   65  106.6  OFF Peter I 
    0710  230   51   38559   55  | 305    4   42834   65  108.5  OFF Stanley 
    0710  230   51   38559   55  | 339    1   43175   64  108.5  OFF Peter I 
    .
    .
    1035  244    5   36958   30  | 296   13   36131   39  184.9  B.. Peter I 
    1040  244    4   36634   29  | 295   13   35649   38  186.8  B.. Peter I 
    .
    .
    1050  243    2   35940   27  | 293   14   34631   36  190.5  BS. Peter I 
    1055  243    0   35566   27  | 292   14   34093   36  192.4  BS. Peter I 

OPTION M:
Realtime display of mutual visibility.

Useage:

stp M <update rate> <satellite>

Once this mode is operational additional sites can be added by pressing;

  g-add based on grid locator (e.g. fn13ed)
  l-add based on lat,lon,alt  (-68.783 -90.583 0)
  o-add based on names in qth.dat (beijing or om89ew or china) case independent

To stop this mode press q.

OPTION p:
This option generates a table that allows the user to predict when
the satellite will be in view of the local QTH.

You can start this option in two ways;

1) STP p satellite name <cr> (if you know which satellite you want)

   or

2) STP p <cr> (and STP will show you a list of the available satellites)

After selecting the satellite STP will want the following information:

Enter Start Day (month day year) (<cr> for current date):
Number of Days for Prediction:
Step Size (minutes....input negative value for seconds): 
FileName to write output to (cr for screen): 

Suggestions/Formats on inputs:

Start Day format
   Months 1-12  Days 1-31 Year 199?-???

Number of Days
   1 or more.  You can input as big a value as you want but remember that
   for LEO type satellites (RS-10/11, AO-21,...) the elements change due
   to drag and predictions longer than a month will start to show errors.

Step Size (minutes....input negative value for seconds): 
   5 is good for AO-13, AO-10 and other high altitude/Molynia type
   satellites.  Use 1 for RS-10/11, AO-21, or other LEO satellites.
   If the step size is input as negative then STP assumes the time
   is in seconds and adjusts the output accordingly.

FileName
   If you press <cr> at FileName prompt then the run is output to the screen,
   If you enter a filename the output is written to the filename specified.

EXAMPLE (prediction for ao-13 )


The header block provides the UTC date for the prediction, the current
revolution (REV) counter and the AMSAT day.  Also provided are the satellite
chosen, the sun illumination and the azimuth and elevation the sun makes with 
the satellites (SAZ/SEL) solar panels, the starting Argument of Perigee and 
the R.A.A.N (AP/RAAN).  The current pointing angle information (ALAT/ALON) is 
shown along with the frequency used to compute the doppler shift.

The format of the table is:

UTC       UTC time for prediciton
Az        Azimuth of satellite (degrees)
El        Elevation of satellite (degrees)
Range     Range from QTH to satellite (Km)
Sqnt      Squint angle as viewed by QTH (degrees)
          (for ao-13 corrections are made based on if the high gain on omni
           directional antennas are in use)
Dopplr    Doppler shift (Hz)
          (for ao-13 the beacon frequency will shift for Mode S)
MA        Mean Anomaly (0-256)
Mode      Operation Mode 

Sun       contains either a -,+,vis or ECL which indicate the following:

          +    shadow side of earth (high probability of being sunlit)
          -    sunny side (definitely sunlit)
          vis  possibily in view of observer (sun below 10 degrees elevation)
          ECL  eclipsed by earth (definitely not sunlit)

The data in the last one or two columns is only computed if AO-13 or AO-10 
is the chosen satellite.  STP performs a relative signal calculation 
based on the orientation of the satellite, the range from the satellite to the 
observer and the gain of the satellite transmitter.  These values are used only 
to provide an indication of the received signal strength (your milage may vary).

If AO-13 is in mode BS then calculations for both Mode B and Mode S are shown.
If only Mode B or Mode S then only calculations for that mode are shown.
When AO-13 is operating either the Mode S beacon (Sbc) or Mode S only (S..)
the doppler shift is computed assuming a beacon frequency of 2400.375 Mhz.
For all other modes for AO-13 the beacon frequency is 145.812 Mhz.  Changing
the values in mode.dat WILL NOT alter these calculations for AO-13 (must modify
the source code).

Anytime the revolution number or the date changes or a gap of more than 
StepSize appears in the table a new header block is written out.

    ----------------------------------------------------------------------
    Prediction Date : Monday October 31 1994    Amsat Day:   6147
    Prediction For  : kd4qio at home
    Performed on    : October 31 1994 14:02:57
    Satellite       : AO-13 (Set:   988, Rev #:  5966, Age:   4.1 Days)
    Illumination    : 86.4%  SAZ/SEL 350.5/-1.1 AP/RAAN 353.6/222.9 
    Alon/Alat       : 230.0/ 0.0  Frequency: 145.8120 MHz 
    ----------------------------------------------------------------------
     UTC   Az   El   Range  Sqnt    Dopplr    MA  Mode  Sun    PRSL
    ----------------------------------------------------------------------
    0355  319    9    9681   79     -470    10.9  Bom  vis     26.65 
    0400  320   20   10082   68     -811    12.8  Bom  vis     26.60 
    .
    .
    0445  309   67   17428   15    -1409    29.5  Bom  vis     22.25 
    0450  305   68   18290  102    -1385    31.4  OFF  vis 
    0455  301   70   19136   99    -1358    33.3  OFF  vis 
    .
    .
    0705  246   55   34917   66     -657    81.7  OFF  vis 
    0710  246   54   35316   65     -635    83.6  OFF  ECL 
    0715  246   53   35700   64     -614    85.5  OFF  ECL 
    .
    .
    0830  248   36   39977   54     -316   113.4  OFF  ECL 
    0835  248   35   40166   54     -297   115.3  OFF  vis 
    .
    .
    1005  252   15   41531   43      +56   148.8  OFF  vis 
    1010  252   14   41488   42      +76   150.7  B..  vis     24.81 
    .
    .
    1110  254    0   39953   35     +346   173.1  B..  vis     25.82 
    ----------------------------------------------------------------------
    Prediction Date : Tuesday November 01 1994    Amsat Day:   6148
    Prediction For  : kd4qio at home
    Performed on    : October 31 1994 14:02:57
    Satellite       : AO-13 (Set:   988, Rev #:  5968, Age:   4.1 Days)
    Illumination    : 86.2%  SAZ/SEL 350.7/-2.1 AP/RAAN 353.7/222.7 
    Alon/Alat       : 230.0/ 0.0  Frequency: 145.8120 MHz 
    ----------------------------------------------------------------------
     UTC   Az   El   Range  Sqnt    Dopplr    MA  Mode  Sun    PRSL
    ----------------------------------------------------------------------
    0245  317   11    8586   80     -305     9.7  Bom  vis     27.65 
    .  
    .
    1030  244    6   37264   30     +483   183.1  B..  vis     26.75 
    1035  244    5   36958   30     +509   184.9  B..  vis     26.87 
    1040  244    4   36634   29     +536   186.8  B..  vis     26.99 
    1045  244    3   36295   28     +564   188.7  B..  vis     27.12 
    1050  243    2   35940   27     +591   190.5  BS.  vis     27.25     27.70 
    1055  243    0   35566   27     +620   192.4  BS.  vis     27.38     28.14 

OPTION r:
This option performs a real-time (limited by you cpu speed) prediction of 
satellites currently visible from the home QTH.  Only satellites that are
in view (Elevation > 0.0) are shown.

The following is a screen capture of STP r:

 LOC     Sunday   November 06 1994 14:47:42
 GMT     Sunday   November 06 1994 20:47:42
  Az    El    Range Squnt  Doppler  MA    Mode Sun
   49    9 v  19371    2    -1867   25.6   Bom  +         AO13sm
  122   35 v  28890   60     -736   60.3   B..  -        OSCAR10
   49    8 v  19558    3    -1860   25.7   Bom  +          AO-13
  248   19 ^  25098  159   +10406  226.7   xxx  -         ARSENE

If no mode data has been set then ??? is displayed.

An additional parameter can be specified on the command line at execution
time.  This parameter is the update rate in seconds.  With no parameter
specified the default is 1 second.  To specify a rate of 5 seconds you 
would enter;

STP r 5

If you want to see relatively how fast your computer is enter;

STP r 0

and watch the seconds block of the LOC entry.  If your screen is flashing
several times prior to the seconds block updating then you are able to 
update the information several times a second.  If your screen does not
flash between updates then the time difference shown in the LOC block
is the slowest rate that you should try to run this option at.  For
example on an 8086 the seconds block updated at about once every 7 
seconds.  On this machine the following would be used;

STP r 10

OPTION R:
Identical to OPTION r except that all the satellites are shown.  The format
of the display is changed to indicate if the satellite is in view or not
and the RELATIVE time to AOS/LOS.

The following is a screen capture of STP R:

 LOC     Monday    October 31 1994 14:06:48
 GMT     Monday    October 31 1994 20:06:48
  Az    El    Range Squnt  Doppler  MA    Mode Sun Status AOS--LOS
   47  -40 v  48039   47     -106  117.0   OFF  +    aos  06:35:01       AO13sm
   40  -27 ^  30562   26     +967  210.9   B..  +    aos  02:51:20      OSCAR10
  134  -44 ^   9743   41    +5884   94.6   PKT  +    aos  01:54:45       UOSAT2
  229  -64 ^  11893   24    +1476   63.1   PKT  -    aos  00:29:31          MIR
  203  -39 ^   9437   42     +451  215.8   A..  -    aos  00:19:26     RS-10/11
   47  -40 v  48113   47     -103  117.2   OFF  +    aos  06:34:17        AO-13
    8  -17 ^   5706   58    +6827  216.3   FSK  +    aos  06:26:02        UO-14
    9  -18 ^   5804   58    +6759  212.9   TLE  +    aos  06:26:02        UO-15
  302   -3 v   3580   63     -414  247.6   PKT  -    aos  06:16:05        DO-17
  203  -54 v  11307   31    -4876   72.6   PKT  -    aos  07:18:46        WO-18
   34  -38 ^   9116   44    +6812  186.5   PSK ECL   aos  06:37:18        LO-19
   16   24 ^   2509   49    +7522   65.5   JA.  -   -LOS- 00:17:55        FO-20
  296   -2 ^   3892   60     +359   52.6   DIG  -    aos  08:08:54        AO-21
  172  -31 ^   8224   48     +516  167.0   K..  -    aos  00:13:17     RS-12/13
   28  -27 ^   7341   52    +6554  127.9   PKT ECL   aos  06:27:39        UO-22
  132  -47 ^  11024   34     +491   42.8   FSK  +    aos  05:51:09        KO-23
   14  -60 v  44545   20    -5573   80.1   S..  +    aos  26:36:02       ARSENE
   54  -47 ^  10368   37    +5581  155.2   FM. ECL   aos  06:41:33        AO-27
  175  -67 v  12621   20     -613   91.0   ---  -    aos  05:35:30      ITAMSAT
  178  -66 v  12532   21    -1832  101.1   FSK  -    aos  05:34:21        KO-25


The column after the elevation (El) has either a ^ to indicate the satellite
is approaching or a v to indicate going away.

The column after Mode contains either a +,-,vis or ECL showing the sun 
illumination on the satellite.

The next column will contain either aos or -LOS- indicating if the time in the
AOS--LOS column is time to acquisition of signal (aos) or loss of signal (los).


The next column is the RELATIVE time (hours:minutes:seconds) until
AOS/LOS.


OPTION s:
Similar to OPTION R except that only a selected set of satellites
are shown.

OPTION S:
Used to select the satellites for OPTION s.  Creates a file called
select.dat


Plug:

I would suggest that you join AMSAT and buy one of their codes if you are
interested in automating the tracking process (Instanttrack is sort of a
standard).  There are other codes available (RealTrack, Sattrack, etc.)
each of which do something a little different.

Other tools;

WISP.
SatSked.


Plea:

If you use the code and find problems, have suggestions for additions
or know better/faster ways to perform the same job please let me know.


Future efforts:

Break the code up to make it more managable.  
Rewrite the rise/set algorithm using the work of Escobal (same algorithm
used in most of the famous programs).
Rotor Control.
Graphics/Windows

Acknowlegdements:

I am very grateful to James Miller (G3RUH) for providing me a copy of his PLAN13
paper and answering the stupid questions that I have come up with over time. 

I would also like to thank Mark Phillips (N2RPZ) for helping me debug this code
and provide suggestions.



I would also like to thank some of my predicessors;

Robert W. Berger (N3EMO) the author of Orbit.  The methodology for reading 
the nasa format database came from N3EMO's code.

Manfred Bester (DL5KR) the author of SatTrack.  The initial qth.dat file came 
from the distribution tape of SatTrack.

Both of these authors have developed and provided very useful tools.


Internet:            harper@huntsville.sparta.com or kd4qio@amsat.org
Packet:              kd4qio@k4bft.#hsv.al.usa.na
Work:                (205) 837-5282 x1216

Snail Mail:          Christie Harper
                     P.O. Box 18786
                     Huntsville, Al 35804

73's and look forward to talking to you on the satellite.
