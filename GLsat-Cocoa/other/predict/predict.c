/****************************************************************************
*          PREDICT: A satellite tracking/orbital prediction program         *
*               Copyright John A. Magliacane, KD2BD 1991-2000               *
*                      Project started: 26-May-1991                         *
*                        Last update: 01-Jan-2002                           *
*****************************************************************************
*         Network sockets added by Ivan Galysh, KD4HBO  10-Jan-2000         *
*               The network port is 1210.  Protocol is UDP.                 *
*                    The pthreads library is required.                      *
*         The socket server is spawned to operate in the background.        *
*****************************************************************************
*    Code to send live AZ/EL tracking data to the serial port for antenna   *
*    tracking was contributed by Vittorio Benvenuti, I3VFJ : 13-Jul-2000    *
*****************************************************************************
*                                                                           *
* This program is free software; you can redistribute it and/or modify it   *
* under the terms of the GNU General Public License as published by the     *
* Free Software Foundation; either version 2 of the License or any later    *
* version.                                                                  *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
* General Public License for more details.                                  *
*                                                                           *
*****************************************************************************/

#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "predict.h"

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef PI
#define PI 3.141592653589793
#endif

/* Global Variables */

struct	{  char name[25];
 	   long catnum;
 	   int year;
	   double refepoch;
	   double incl;
	   double raan;
	   double eccn;
	   double argper;
	   double meanan;
	   double meanmo;
	   double drag;
	   long orbitnum;
	}  sat[24];

struct	{  char callsign[10];
	   double stnlat;
	   double stnlong;
	   int stnalt;
	}  qth;

char	qthfile[50], tlefile[50], temp[80], output[25], serial_port[15],
	resave=0, reload_tle=0, netport[6];

unsigned char val[256];

int	indx, ma256, iaz, iel, isplat, isplong, socket_flag=0, antfd;

long	i1, i2, i3, de, rv, irk;

double	age, daynum, epoch, sma, range, t1, se, e1, e2, n0, c[4][3],
	k2, s1, c1, l8, s9, c9, s8, c8, r9, z9, x9, y9, o, w, q, s0, s2,
	c2, q0, m, e,  s3, c3, c0, r3, m1, m5, x0, yzero, x1, yone, r,
	z1, g7, s7, c7, x,  y,  z, x5, y5, z5, z8, x8, y8, df, aostime,
	lostime, apogee, perigee, azimuth, ssplat, ssplong, elevation,
	vk, vm, rm, rk, ak, am, fk, fm, yr, TP=6.283185307179586,
	PT=1.570796326794897, S1=0.397818675, C1=0.91746406,
	deg2rad=1.74532925199e-02, R0=6378.135, FF=3.35289186924e-03,
	KM=1.609344;


/* The following variables are used by the socket server.  They
   are updated in the MultiTrack() and SingleTrack() functions. */

char	visibility_array[24];

float	az_array[24], el_array[24], long_array[24], lat_array[24],
	footprint_array[24], range_array[24], altitude_array[24],
	velocity_array[24], moon_az, moon_el, sun_az, sun_el;

double	doppler100[24], nextevent[24];

long	aos_array[24], orbitnum_array[24];

unsigned short portbase=0;

/******************** Program functions begin below ************************/

void bailout(string)
char *string;
{
	/* This function quits ncurses, resets and "beeps"
	   the terminal, and displays an error message (string)
	   when we need to bail out of the program in a hurry. */

	beep();	
	curs_set(1);
	bkgdset(COLOR_PAIR(1));
	clear();
	refresh();
	endwin();
	fprintf(stderr,"*** predict: %s!\n",string);
}

void TrackDataOut(antfd, elevation, azimuth)
int antfd;
double elevation, azimuth;
{
	/* This function sends Azimuth and Elevation data
	   to an antenna tracker connected to the serial port */

	int n, port;
	char message[30]="\n";

	port=antfd;

	sprintf(message, "AZ%3.1f EL%3.1f \x0D\x0A", azimuth,elevation);
	n=write(port,message,30);

	if (n<0)
	{
		bailout("Error Writing To Antenna Port");
		exit(-1);
	}
}

int passivesock(char *service, char *protocol, int qlen)
{
	/* This function opens the socket port */

	struct servent *pse;
	struct protoent *ppe;
	struct sockaddr_in sin;
	int s, type;
	
	bzero((char *)&sin,sizeof(struct sockaddr_in));
	sin.sin_family=AF_INET;
	sin.sin_addr.s_addr=INADDR_ANY;
	
	if ((pse=getservbyname(service,protocol)))
		sin.sin_port=htons(ntohs((unsigned short)pse->s_port)+portbase);

	else if ((sin.sin_port=htons((unsigned short)atoi(service)))==0)
	{
		bailout("Can't get service");
		exit(-1);
	}
	
	if ((ppe=getprotobyname(protocol))==0)
	{
		bailout("Can't get protocol");
		exit(-1);
	}
	
	if (strcmp(protocol,"udp")==0)
		type=SOCK_DGRAM;
	else
		type=SOCK_STREAM;
	
	s=socket(PF_INET,type, ppe->p_proto);

	if (s<0)
	{
		bailout("Can't open socket");
		exit(-1);
	}
	
	if (bind(s,(struct sockaddr *)&sin,sizeof(sin))<0)
	{
		bailout("Can't bind");
		exit(-1);
	}
	
	if ((type=SOCK_STREAM && listen(s,qlen))<0)
	{
		bailout("Listen fail");
		exit(-1);
	}

	return s;
}

void socket_server()
{
	/* This is the socket server code */

	int i, j, sock, alen;
	struct sockaddr_in fsin;
	char buf[80], buff[1000], satname[50], tempname[30];
	time_t t;
	FILE *fd=NULL;

	/* Open a socket port at "predict" or netport if defined */

	if (netport[0]==0)
		strncpy(netport,"predict",7);

	sock=passivesock(netport,"udp",10);
 	alen=sizeof(fsin);
	
	/* This is the main loop for monitoring the socket
	   port and sending back replies to clients */

	while (1)
	{
		/* Get datagram from socket port */
		if (recvfrom(sock,buf,sizeof(buf),0,(struct sockaddr *)&fsin,&alen) < 0)
			exit(-1);

		/* Parse the command in the datagram */
		if (strncmp("GET_SAT",buf,7)==0)
		{
			/* Parse "buf" for satellite name */
			for (i=0; buf[i]!=32 && buf[i]!=0 && i<39; i++);

			for (j=++i; buf[j]!='\n' && buf[j]!=0 && (j-i)<25; j++)
				satname[j-i]=buf[j];

			satname[j-i]=0;

			/* Do a simple search for the matching satellite name */

			for (i=0; i<24; i++)
			{
				if ((strcmp(satname,sat[i].name)==0) || (atol(satname)==sat[i].catnum))
				{
					/* Build text buffer with satellite data */
					sprintf(buff,"%s\n%-7.2f\n%+-6.2f\n%-7.2f\n%+-6.2f\n%ld\n%-7.2f\n%-7.2f\n%-7.2f\n%-7.2f\n%ld\n%c\n",sat[i].name,long_array[i],lat_array[i],az_array[i],el_array[i],(long)rint(86400.0*(nextevent[i]+3651.0)),footprint_array[i],range_array[i],altitude_array[i],velocity_array[i],orbitnum_array[i],visibility_array[i]);

					/* Send buffer back to the client that sent the request */
					sendto(sock,buff,strlen(buff),0,(struct sockaddr*)&fsin,sizeof(fsin));
					break;
				}
			}
		}

		if (strncmp("GET_TLE",buf,7)==0)
		{
			/* Parse "buf" for satellite name */
			for (i=0; buf[i]!=32 && buf[i]!=0 && i<39; i++);

			for (j=++i; buf[j]!='\n' && buf[j]!=0 && (j-i)<25; j++)
				satname[j-i]=buf[j];

			satname[j-i]=0;

			/* Do a simple search for the matching satellite name */

			for (i=0; i<24; i++)
			{
				if ((strcmp(satname,sat[i].name)==0) || (atol(satname)==sat[i].catnum))
				{
					/* Build text buffer with satellite data */
					sprintf(buff,"%s\n%ld\n%02d %.8f\n%.4f\n%.4f\n%g\n%.4f\n%.4f\n%.8f\n%g\n%ld\n",sat[i].name,sat[i].catnum,sat[i].year,sat[i].refepoch,sat[i].incl,sat[i].raan,sat[i].eccn,sat[i].argper,sat[i].meanan,sat[i].meanmo,sat[i].drag,sat[i].orbitnum);

					/* Send buffer back to the client that sent the request */
					sendto(sock,buff,strlen(buff),0,(struct sockaddr*)&fsin,sizeof(fsin));
					break;
				}
			}
		}

		if (strncmp("GET_DOPPLER",buf,11)==0)
		{
			/* Parse "buf" for satellite name */
			for (i=0; buf[i]!=32 && buf[i]!=0 && i<39; i++);

			for (j=++i; buf[j]!='\n' && buf[j]!=0 && (j-i)<25; j++)
				satname[j-i]=buf[j];

			satname[j-i]=0;

			/* Do a simple search for the matching satellite name */

			for (i=0; i<24; i++)
			{
				if ((strcmp(satname,sat[i].name)==0) || (atol(satname)==sat[i].catnum))
				{
					/* Get Normalized (100 MHz)
					   Doppler shift for sat[i] */

					sprintf(buff,"%f\n",doppler100[i]);

					/* Send buffer back to client who sent request */
					sendto(sock,buff,strlen(buff),0,(struct sockaddr*)&fsin,sizeof(fsin));
					break;
				}
			}
		}

		if (strncmp("GET_LIST",buf,8)==0)
		{
			buff[0]=0;

			for (i=0; i<24; i++)
			{
				if (sat[i].name[0]!=0)
					strcat(buff,sat[i].name);

				strcat(buff,"\n");
			}

			sendto(sock,buff,strlen(buff),0,(struct sockaddr *)&fsin,sizeof(fsin));
		}

		if (strncmp("RELOAD_TLE",buf,10)==0)
		{
			buff[0]=0;
			sendto(sock,buff,strlen(buff),0,(struct sockaddr *)&fsin,sizeof(fsin));
			reload_tle=1;
		}

		if (strncmp("GET_SUN",buf,7)==0)
		{
			buff[0]=0;
			sprintf(buff,"%-7.2f\n%+-6.2f\n",sun_az, sun_el);
			sendto(sock,buff,strlen(buff),0,(struct sockaddr *)&fsin,sizeof(fsin));
		}

		if (strncmp("GET_MOON",buf,8)==0)
		{
			buff[0]=0;
			sprintf(buff,"%-7.2f\n%+-6.2f\n",moon_az, moon_el);
			sendto(sock,buff,strlen(buff),0,(struct sockaddr *)&fsin,sizeof(fsin));
		}

		if (strncmp("GET_VERSION",buf,11)==0)
		{
			buff[0]=0;
			sprintf(buff,"%s\n",version);
			sendto(sock,buff,strlen(buff),0,(struct sockaddr *)&fsin,sizeof(fsin));
		}

		if (strncmp("GET_QTH",buf,7)==0)
		{
			buff[0]=0;
			sprintf(buff,"%s\n%g\n%g\n%d\n",qth.callsign, qth.stnlat, qth.stnlong, qth.stnalt);
			sendto(sock,buff,strlen(buff),0,(struct sockaddr *)&fsin,sizeof(fsin));
		}

		if (strncmp("GET_TIME$",buf,9)==0)
		{
			buff[0]=0;
			t=time(NULL);
			sprintf(buff,"%s",asctime(gmtime(&t)));

			if (buff[8]==32)
				buff[8]='0';

			sendto(sock,buff,strlen(buff),0,(struct sockaddr *)&fsin,sizeof(fsin));
			buf[0]=0;
		}

		if (strncmp("GET_TIME",buf,8)==0)
		{
			buff[0]=0;
			t=time(NULL);
			sprintf(buff,"%lu\n",(unsigned long)t);
			sendto(sock,buff,strlen(buff),0,(struct sockaddr *)&fsin,sizeof(fsin));
		}

		if (strncmp("GET_SAT_POS",buf,11)==0)
		{
			/* Parse "buf" for satellite name and arguments */
			for (i=0; buf[i]!=32 && buf[i]!=0 && i<39; i++);

			for (j=++i; buf[j]!='\n' && buf[j]!=0 && (j-i)<49; j++)
				satname[j-i]=buf[j];

			satname[j-i]=0;

			/* Send request to predict with output
			   directed to a temporary file under /tmp */

			strcpy(tempname,"/tmp/XXXXXX\0");
			i=mkstemp(tempname);

			sprintf(buff,"predict -f %s -t %s -q %s -o %s\n",satname,tlefile,qthfile,tempname);
			system(buff);

			/* Append an EOF marker (CNTRL-Z) to the end of file */

			fd=fopen(tempname,"a");
			fprintf(fd,"%c\n",26);  /* Control-Z */
			fclose(fd);

			buff[0]=0;

			/* Send the file to the client */

			fd=fopen(tempname,"rb");

			fgets(buff,80,fd);

			do
			{
				sendto(sock,buff,strlen(buff),0,(struct sockaddr *)&fsin,sizeof(fsin));
				fgets(buff,80,fd);
				/* usleep(2);  if needed (for flow-control) */

			} while (feof(fd)==0);

			fclose(fd);
			unlink(tempname);
			close(i);
		}

		if (strncmp("PREDICT",buf,7)==0)
		{
			/* Parse "buf" for satellite name and arguments */
			for (i=0; buf[i]!=32 && buf[i]!=0 && i<39; i++);

			for (j=++i; buf[j]!='\n' && buf[j]!=0 && (j-i)<49; j++)
				satname[j-i]=buf[j];

			satname[j-i]=0;

			/* Send request to predict with output
			   directed to a temporary file under /tmp */

			strcpy(tempname,"/tmp/XXXXXX\0");
			i=mkstemp(tempname);

			sprintf(buff,"predict -p %s -t %s -q %s -o %s\n",satname,tlefile,qthfile,tempname);
			system(buff);

			/* Append an EOF marker (CNTRL-Z) to the end of file */

			fd=fopen(tempname,"a");
			fprintf(fd,"%c\n",26);  /* Control-Z */
			fclose(fd);

			buff[0]=0;

			/* Send the file to the client */

			fd=fopen(tempname,"rb");

			fgets(buff,80,fd);

			do
			{
				sendto(sock,buff,strlen(buff),0,(struct sockaddr *)&fsin,sizeof(fsin));
				fgets(buff,80,fd);
				/* usleep(2);  if needed (for flow-control) */

			} while (feof(fd)==0);

			fclose(fd);
			unlink(tempname);
			close(i);
		}
	} 	
}

void Banner()
{
	curs_set(0);
	bkgdset(COLOR_PAIR(3));
	clear();
	refresh();

	attrset(COLOR_PAIR(6)|A_REVERSE|A_BOLD);
	mvprintw(3,18,"                                           ");
	mvprintw(4,18,"         --== PREDICT  v%s ==--         ",version);
	mvprintw(5,18,"           By John A. Magliacane           ");
	mvprintw(6,18,"   KD2BD Software -- Copyright 1991-2002   ");
	mvprintw(7,18,"                                           ");
}

void AnyKey()
{
	mvprintw(23,24,"<< Press Any Key To Continue >>");
	refresh();
	getch();
}

double FixAngle(x)
double x;
{
	/* This function reduces angles greater than
	   two pi by subtracting two pi from the angle */

	while (x>TP)
		x-=TP;

	return x;
}

char *SubString(string,start,end)
unsigned char *string, start, end;
{
	/* This function returns a substring based on the starting
	   and ending positions provided.  It is used heavily in
	   the AutoUpdate function when parsing 2-line element data. */

	unsigned x, y;

	if (end>=start)
	{
		for (x=start, y=0; x<=end && string[x]!=0; x++)
			if (string[x]!=' ')
			{
				temp[y]=string[x];
				y++;
			}

		temp[y]=0;
		return temp;
	}
	else
		return NULL;
}

void CopyString(source, destination, start, end)
unsigned char *source, *destination, start, end;
{
	/* This function copies elements of the string "source"
	   bounded by "start" and "end" into the string "destination". */

	unsigned j, k=0;

	for (j=start; j<=end; j++)
		if (source[k]!=0)
		{
			destination[j]=source[k];
			k++;
		}
}

char *Abbreviate(string,n)
unsigned char *string, n;
{
	/* This function returns an abbreviated substring of the original,
	   including a '~' character if a non-blank character is chopped
	   out of the generated substring.  n is the length of the desired
	   substring.  It is used for abbreviating satellite names. */

	strncpy(temp,string,79);

	if (temp[n]!=0 && temp[n]!=32)
	{
		temp[n-2]='~';
		temp[n-1]=temp[strlen(temp)-1];
	}

	temp[n]=0;

	return temp;
}

char KepCheck(line1,line2)
char *line1, *line2;
{
	/* This function scans line 1 and line 2 of a NASA 2-Line element
	   set and returns a 1 if the element set appears to be valid or
	   a 0 if it does not.  If the data survives this torture test,
	   it's a pretty safe bet we're looking at a valid 2-line
	   element set and not just some random garbage that might pass
	   as orbital data based on a simple checksum calculation alone. */

	int x;
	unsigned sum1, sum2;

	/* Compute checksum for each line */

	for (x=0, sum1=0, sum2=0; x<=67; sum1+=val[(int)line1[x]], sum2+=val[(int)line2[x]], x++);

	/* Perform a "torture test" on the data */

	x=(val[(int)line1[68]]^(sum1%10)) | (val[(int)line2[68]]^(sum2%10)) |
	  (line1[0]^'1')  | (line1[1]^' ')  | (line1[7]^'U')  |
	  (line1[8]^' ')  | (line1[17]^' ') | (line1[23]^'.') |
	  (line1[32]^' ') | (line1[34]^'.') | (line1[43]^' ') |
	  (line1[52]^' ') | (line1[61]^' ') | (line1[63]^' ') |
	  (line2[0]^'2')  | (line2[1]^' ')  | (line2[7]^' ')  |
	  (line2[11]^'.') | (line2[16]^' ') | (line2[20]^'.') |
	  (line2[25]^' ') | (line2[33]^' ') | (line2[37]^'.') |
	  (line2[42]^' ') | (line2[46]^'.') | (line2[51]^' ') |
	  (line2[54]^'.') | (line1[2]^line2[2]) | (line1[3]^line2[3]) |
	  (line1[4]^line2[4]) | (line1[5]^line2[5]) | (line1[6]^line2[6]) |
	  (isdigit(line1[68]) ? 0 : 1) | (isdigit(line2[68]) ? 0 : 1) |
	  (isdigit(line1[18]) ? 0 : 1) | (isdigit(line1[19]) ? 0 : 1) |
	  (isdigit(line2[31]) ? 0 : 1) | (isdigit(line2[32]) ? 0 : 1);

	return (x ? 0 : 1);
}

char ReadDataFiles()
{
	/* This function reads "predict.qth" and "predict.tle" files
	   into memory.  Return values are as follows:

	   0 : No files were loaded
	   1 : Only the qth file was loaded
	   2 : Only the tle file was loaded
	   3 : Both files were loaded successfully */

	int x=0, y;
	FILE *fd;
	char flag=0, name[80], line1[80], line2[80];

	fd=fopen(qthfile,"r");
	
	if (fd!=NULL)
	{
		fgets(qth.callsign,10,fd);
		qth.callsign[strlen(qth.callsign)-1]=0;
		fscanf(fd,"%lf", &qth.stnlat);
		fscanf(fd,"%lf", &qth.stnlong);
		fscanf(fd,"%d", &qth.stnalt);
		fclose(fd);
		flag=1;
	}

	fd=fopen(tlefile,"r");

	if (fd!=NULL)
	{
		while (x<24 && feof(fd)==0)
		{
			/* Read element set */

			fgets(name,75,fd);
			fgets(line1,75,fd);
			fgets(line2,75,fd);

			if (KepCheck(line1,line2))
			{
				/* We found a valid TLE! */

				/* Add decimal to eccentricity value */

				line2[25]='.';

				/* Some TLE sources left justify the sat
				   name in a 24-byte field that is padded
				   with blanks.  The following lines cut
				   out the blanks as well as the line feed
				   character read by the fgets() function. */
 
				y=strlen(name);

				while (name[y]==32 || name[y]==0 || name[y]==10 || name[y]==13 || y==0)
				{
					name[y]=0;
					y--;
				}
				
				/* Copy TLE data into the sat data structure */

				strncpy(sat[x].name,name,24);
				sat[x].catnum=atol(SubString(line1,2,6));
				sat[x].year=atoi(SubString(line1,18,19));
				sat[x].refepoch=atof(SubString(line1,20,31));
				sat[x].incl=atof(SubString(line2,8,15));
				sat[x].raan=atof(SubString(line2,17,24));
				sat[x].eccn=atof(SubString(line2,25,32));
				sat[x].argper=atof(SubString(line2,34,41));
				sat[x].meanan=atof(SubString(line2,43,50));
				sat[x].meanmo=atof(SubString(line2,52,62));
				sat[x].drag=atof(SubString(line1,33,42));
				sat[x].orbitnum=atof(SubString(line2,63,67));
				x++;
			}
		}

		fclose(fd);
		flag+=2;
		resave=0;
	}

	return flag;
}

void SaveQTH()
{
	/* This function saves QTH data file normally
	   found under ~/.predict/predict.qth */

	FILE *fd;	

	fd=fopen(qthfile,"w");

	fprintf(fd,"%s\n",qth.callsign);
	fprintf(fd," %g\n",qth.stnlat);
	fprintf(fd," %g\n",qth.stnlong);
	fprintf(fd," %d\n",qth.stnalt);

	fclose(fd);
}

void SaveTLE()
{
	FILE *fd;	
	int x, y;
	unsigned sum;
	unsigned char line1[80], line2[80], string[20];

 	/* Save orbital data in the form of NASA TLEs */

	fd=fopen(tlefile,"w");

	for (y=0; y<24; y++)
	{
		/* Fill lines with blanks */

		for (x=0; x<70; line1[x]=32, line2[x]=32, x++);

		line1[69]=0;
		line2[69]=0;

		/* Insert static characters */

		line1[0]='1';
		line1[7]='U';
		line2[0]='2';

		line1[51]='0';
		line1[62]='0';

		strcpy(string,"00000-0");
		CopyString(string,line1,54,60);
		CopyString(string,line1,45,51);

		/* Insert orbital data */

		sprintf(string,"%05ld",sat[y].catnum);
		CopyString(string,line1,2,6);
		CopyString(string,line2,2,6);

		sprintf(string,"%12.8f",sat[y].refepoch);
		CopyString(string,line1,20,32);

		sprintf(string,"%02d",sat[y].year);
		CopyString(string,line1,18,19);

		sprintf(string,"%9.4f",sat[y].incl);
		CopyString(string,line2,7,15);
				
		sprintf(string,"%9.4f",sat[y].raan);
		CopyString(string,line2,16,24);

		sprintf(string,"%13.12f",sat[y].eccn);
	
		/* Erase eccentricity's decimal point */

		for (x=2; x<=9; string[x-2]=string[x], x++);

		CopyString(string,line2,26,32);

		sprintf(string,"%9.4f",sat[y].argper);
		CopyString(string,line2,33,41);

		sprintf(string,"%9.5f",sat[y].meanan);
		CopyString(string,line2,43,50);

		sprintf(string,"%12.9f",sat[y].meanmo);
		CopyString(string,line2,52,62);

		sprintf(string,"%.9f",fabs(sat[y].drag));

		CopyString(string,line1,33,42);

		if (sat[y].drag < 0.0)	
			line1[33]='-';
		else
			line1[33]=32;

		sprintf(string,"%5lu",sat[y].orbitnum);
		CopyString(string,line2,63,67);

		/* Compute and insert checksum for line 1 and line 2 */

		for (x=0, sum=0; x<=67; sum+=val[(int)line1[x]], x++);
		line1[68]=(sum%10)+'0';

		for (x=0, sum=0; x<=67; sum+=val[(int)line2[x]], x++);
		line2[68]=(sum%10)+'0';

		/* Write name, line 1, line 2 to predict.tle */

		fprintf(fd,"%s\n", sat[y].name);  
		fprintf(fd,"%s\n", line1);
		fprintf(fd,"%s\n", line2);
	}

	fclose(fd);
}

int AutoUpdate(string)
char *string;
{
	/* This function updates PREDICT's orbital datafile from a NASA
	   2-line element file either through a menu (interactive mode)
	   or via the command line.  string==filename of 2-line element
	   set if this function is invoked via the command line. */

	char line1[80], line2[80], str0[80], str1[80], str2[80],
	     filename[50], saveflag=0, interactive=0, savecount=0;

	float database_epoch=0.0, tle_epoch=0.0, database_year, tle_year;
	int i, success=0, kepcount=0;
	FILE *fd;

	do
	{
		if (string[0]==0)
		{
			interactive=1;
			curs_set(1);
			bkgdset(COLOR_PAIR(3));
			refresh();
			clear();
			echo();

			for (i=5; i<8; i+=2)
				mvprintw(i,19,"------------------------------------------");

			mvprintw(6,19,"* Keplerian Database Auto Update Utility *");
			bkgdset(COLOR_PAIR(2));
			mvprintw(19,18,"Enter NASA Two-Line Element Source File Name");
			mvprintw(13,18,"-=> ");
			refresh();
			wgetnstr(stdscr,filename,49);
			clear();
			curs_set(0);
		}
		else
			strcpy(filename,string);

		/* Prevent "." and ".." from being used as a
		   filename otherwise strange things happen. */

		if (strlen(filename)==0 || strncmp(filename,".",1)==0 || strncmp(filename,"..",2)==0)
			return 0;

		fd=fopen(filename,"r");

		if (interactive && fd==NULL)
		{
			bkgdset(COLOR_PAIR(5));
			clear();
			move(12,0);

			for (i=47; i>strlen(filename); i-=2)
				printw(" ");

			printw("*** ERROR: File \"%s\" not found! ***\n",filename);
			beep();
			attrset(COLOR_PAIR(7)|A_BOLD);
			AnyKey();
		}

		if (fd!=NULL)
		{
			success=1;

			fgets(str0,75,fd);
			fgets(str1,75,fd);
			fgets(str2,75,fd);
		
			do
			{
				if (KepCheck(str1,str2))
				{
					/* We found a valid TLE!
					   Copy strings str1 and
					   str2 into line1 and line2 */

					strncpy(line1,str1,75);
					strncpy(line2,str2,75);
					kepcount++;

					/* Scan for object number in datafile to see
					   if this is something we're interested in */

					for (i=0; (i<24 && sat[i].catnum!=atol(SubString(line1,2,6))); i++);

					if (i!=24)
					{
						/* We found it!  Check to see if it's more
						   recent than the data we already have. */

						if (sat[i].year<=50)
							database_year=365.25*(100.0+(float)sat[i].year);
						else
							database_year=365.25*(float)sat[i].year;

						database_epoch=(float)sat[i].refepoch+database_year;

						tle_year=(float)atof(SubString(line1,18,19));

						if (tle_year<=50.0)
							tle_year+=100.0;

						tle_epoch=(float)atof(SubString(line1,20,31))+(tle_year*365.25);

						/* Update only if TLE epoch >= epoch in data file
						   so we don't overwrite current data with older
						   data. */

						if (tle_epoch>=database_epoch)
						{
							if (saveflag==0)
							{
								if (interactive)
								{
									clear();
									bkgdset(COLOR_PAIR(2));
									mvprintw(3,35,"Updating.....");
									refresh();
									move(7,0);
								}
								saveflag=1;
							}

							if (interactive)
							{
								bkgdset(COLOR_PAIR(3));
								printw("     %-15s",sat[i].name);
							}

							savecount++;

							/* Add decimal point to eccentricity value */

							line2[25]='.';

							/* Copy TLE data into the sat data structure */

							sat[i].year=atoi(SubString(line1,18,19));
							sat[i].refepoch=atof(SubString(line1,20,31));
							sat[i].incl=atof(SubString(line2,8,15));
							sat[i].raan=atof(SubString(line2,17,24));
							sat[i].eccn=atof(SubString(line2,25,32));
							sat[i].argper=atof(SubString(line2,34,41));
							sat[i].meanan=atof(SubString(line2,43,50));
							sat[i].meanmo=atof(SubString(line2,52,62));
							sat[i].drag=atof(SubString(line1,33,42));
							sat[i].orbitnum=atof(SubString(line2,63,67));
						}
					}

					 fgets(str0,75,fd);     	
					 fgets(str1,75,fd);
					 fgets(str2,75,fd);
				}

				else
				{
					strcpy(str0,str1);   
					strcpy(str1,str2);   
					fgets(str2,75,fd);
				}
			
			} while (feof(fd)==0);

			fclose(fd);

			if (interactive)
			{
				bkgdset(COLOR_PAIR(2));

				if (kepcount==1)
					mvprintw(18,21,"  Only 1 NASA Two Line Element was found.");
				else
					mvprintw(18,21,"%3u NASA Two Line Elements were read.",kepcount);

				if (saveflag)
				{
					if (savecount==1)
						mvprintw(19,21,"  Only 1 satellite was updated.");
					else
					{
						if (savecount==24)
							mvprintw(19,21,"  All satellites were updated!");
						else
							mvprintw(19,21,"%3u out of 24 satellites were updated.",savecount);
					}
				}

				refresh();
			}
		}

		if (interactive)
		{
			noecho();

			if (strlen(filename) && fd!=NULL) 
			{
				attrset(COLOR_PAIR(4)|A_BOLD);
				AnyKey();
			}
		}

		if (saveflag)
			SaveTLE();
	}
	while (success==0 && interactive);

	return (saveflag ? 0 : -1);
}

int Select()
{
	/* This function displays the names of satellites contained
	   within the program's database and returns an index that
	   corresponds to the satellite selected by the user.  An
	   ESC or CR returns a -1. */

	int x, y, z, key=0;

	clear();

	bkgdset(COLOR_PAIR(2)|A_BOLD);
	printw("\n\n\t\t\t      Select a Satellite:\n\n");

	attrset(COLOR_PAIR(3)|A_BOLD);

	for (x=0, y=8, z=16; y<16; ++x, ++y, ++z)
	{
		printw("\n\t[%c]: %-15s", x+'A', Abbreviate(sat[x].name,15));
		printw("\t[%c]: %-15s", y+'A', Abbreviate(sat[y].name,15));
		printw("\t[%c]: %-15s\n", z+'A', Abbreviate(sat[z].name,15));
	}

	attrset(COLOR_PAIR(4)|A_BOLD);

	printw("\n\n\t\t<< Enter Selection  -  Press [ESC] To Exit >>");
	refresh();

	do
	{
		key=toupper(getch());

		if (key==27 || key=='\n')
			return -1;
	}
	while (key<'A' || key>'X');

	return(key-'A');
}

long DayNum(m,d,y)
int  m, d, y;
{
	/* This function calculates the day number from m/d/y. */

	long dn;
	double mm, yy;

	if (m<3)
	{ 
		y--; 
		m+=12; 
	}

	/* Correct for Y2K... */

	if (y<=50)
		y+=100;

	yy=(double)y;
	mm=(double)m;
	dn=(long)(floor(365.25*(yy-80.0))-floor(19.0+yy/100.0)+floor(4.75+yy/400.0)-16.0);
	dn+=d+30*m+(long)floor(0.6*mm-0.3);
	return dn;
}

double CurrentDaynum()
{
	/* Read the system clock and return the number
	   of days since 31Dec79 00:00:00 UTC (daynum 0) */

	struct timeb tptr;
	int x;

	x=ftime(&tptr);

	return ((((double)tptr.time+0.001*(double)tptr.millitm)/86400.0)-3651.0);
}

char *Daynum2String(daynum)
double daynum;
{
	/* This function takes the given epoch as a fractional number of
	   days since 31Dec79 00:00:00 UTC and returns the corresponding
	   date as a string of the form "Tue 12Oct99 17:22:37". */

	char timestr[25];
	time_t t;
	int x;

	/* Convert daynum to Unix time (seconds since 01-Jan-70) */
	t=(time_t)rint(86400.0*(daynum+3651.0));

	sprintf(timestr,"%s",asctime(gmtime(&t)));

	if (timestr[8]==' ')
		timestr[8]='0';

	for (x=0; x<=3; output[x]=timestr[x], x++);

	output[4]=timestr[8];
	output[5]=timestr[9];
	output[6]=timestr[4];
	output[7]=timestr[5];
	output[8]=timestr[6];
	output[9]=timestr[22];
	output[10]=timestr[23];
	output[11]=' ';

	for (x=12; x<=19; output[x]=timestr[x-1], x++);

	output[20]=0;
	return output;
}

double GetStartTime()
{
	/* This function prompts the user for the time and date
	   the user wishes to begin prediction calculations,
	   and returns the corresponding fractional day number.
	   31Dec79 00:00:00 returns 0.  Default is NOW. */

	int	x, h, m, s, mm=0, dd=0, yy; 
	char	good, mon[5], line[30], string[30], bozo_count=0,
		*month[12]= {"Jan", "Feb", "Mar", "Apr", "May",
		"Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	do
	{
		bkgdset(COLOR_PAIR(2)|A_BOLD);
		clear();
		printw("\n\n\n\t     Starting UTC Date and Time for Predictions of %-15s\n\n",sat[indx].name);
		bozo_count++;

		strcpy(string,Daynum2String(CurrentDaynum()));

		for (x=4; x<24; x++)
			string[x-4]=string[x];

		attrset(COLOR_PAIR(4)|A_BOLD);
		printw("\t\t    Format: %s -or- ",string);
		string[7]=0;
		printw("%s",string);
		attrset(COLOR_PAIR(2)|A_BOLD);
		mvprintw(21,30,"Default is `NOW'");
		attrset(COLOR_PAIR(3)|A_BOLD);
		mvprintw(13,1,"Enter Start Date & Time >> ");
		curs_set(1);
		refresh();
		echo();
		string[0]=0;
		wgetnstr(stdscr,string,29);
		curs_set(0);
		noecho();
		       
		if (strlen(string)!=0)
			strcpy(line,string);
		else
			/* Select `NOW' */
			return(CurrentDaynum());

		if (strlen(line)==7)
		{
			line[7]=' ';
			line[8]='0';
			line[9]='0';
			line[10]=':';
			line[11]='0';
			line[12]='0';
			line[13]=':';
			line[14]='0';
			line[15]='0';
			line[16]=0;
		}
			
		/* Check Day */
		good = (isdigit(line[0]) && isdigit(line[1])) ? 1 : 0;

		/* Month */
		good = (good && isalpha(line[2]) && isalpha(line[3]) && isalpha(line[4])) ? 1 : 0;

		/* Year */
		good = (good && isdigit(line[5]) && isdigit(line[6]) && (line[7]==' ')) ? 1 : 0;

		/* Hour */
		good = (good && isdigit(line[8]) && isdigit(line[9]) && (line[10]==':')) ? 1 : 0;

		/* Minute */
		good = (good && isdigit(line[11]) && isdigit(line[12]) && (line[13]==':')) ? 1 : 0;

		/* Seconds */
		good = (good && isdigit(line[14]) && isdigit(line[15])) ? 1 : 0;

	     	if (good)
		{ 
			/* Decode Day */
			dd=10*(line[0]-'0')+line[1]-'0';

			/* Decode Month Number */
			line[2]=toupper(line[2]);
			line[3]=tolower(line[3]);
			line[4]=tolower(line[4]);

			mon[0]=line[2];
			mon[1]=line[3];
			mon[2]=line[4];
			mon[3]=0;

			for (mm=0; (mm<12 && strcmp(mon,month[mm])!=0); mm++);

			mm++;
	
			good=(mm>12) ? 0 : 1;
		}

		if (good==0)
			beep();

	} while (good==0 && bozo_count<6);

	if (good==0)
	{
		/* If the user can't enter the starting date/time
		   correctly after several attempts, then the user
		   is a "bozo" and obviously can't follow directions. */

		bailout("Too Many Errors");
		exit(-1);
	}

	/* Decode Year */
	yy=10*(line[5]-'0')+line[6]-'0';

	/* Decode Time */
	for (x=8; x<16; x++)
		string[x-8]=line[x];

	string[8]=0;

	h=10*(line[8]-'0')+line[9]-'0';
	m=10*(line[11]-'0')+line[12]-'0';
	s=10*(line[14]-'0')+line[15]-'0';
	
	return ((double)DayNum(mm,dd,yy)+((h/24.0)+(m/1440.0)+(s/86400.0)));
}

char FindSun(daynum,display)
double daynum;
char display;
{
	/* This function calculates the position of the Sun.
	   It returns a blank when the satellite is in eclipse,
	   a '*' when both the satellite and the ground station
	   are in sunlight, and a '+' when the satellite is in
	   sun and the ground station is in darkness.  If a '+'
	   is returned when the satellite is above the horizon,
	   the satellite is visible at the ground station.

	   If the display variable is set high, the function prints
 	   azimuth and elevation headings in the lower right-hand
	   corner of the screen (used in real-time tracking modes). */

	double	c, ms, t1, t2, t3, ew, e, n, si, co, ra, gh,
		lh, gm, de, se, gs, dc, el[2], az[2], lat[2],
		lon[2], horizon_angle, yr;

	int	x;

	/* Calculate the year from "daynum" */

	t1=(double)(long)(daynum-39410.0);
	t2=floor((t1+32044.75)/36524.25);
	t2+=t1-t2/4.0+1486.0;
	yr=2084.0+floor((t2-122.1)/365.25);

	gm=24.0*(daynum-floor(daynum));
	c=((daynum+gm/24.0)/365.2422-0.010452395);
	ms=TP*(c-floor(c));
	t1=ms;
	t2=0.016718;

	do
	{
		t3=t1-t2*sin(t1)-ms;
		t1=t1-t3/(1.0-t2*cos(t1));
	}
	while (fabs(t3)>1.0e-6);

	t1=1.01686*tan(t1/2.0);
	ew=2.0*atan(t1)+4.932237686;
	si=S1*sin(ew);
	co=sqrt(1.0-(si*si));
	dc=atan2(si,co);
	si=sin(ew)*C1;
	co=cos(ew);
	ra=atan2(si,co);

	if (ra<0.0)
		ra+=TP;

	t1=yr-1.0;
	de=floor(365.25*(t1-1980.0))-floor(t1/100.0)+floor(t1/400.0)+381.0;
	t1=(de+29218.5)/36525.0;
	t1=6.6460656+t1*(2400.051262+t1*2.581e-5);
	se=t1-24.0*(yr-1900.0);
	t1=(se+0.0657098*(daynum-de)+gm*1.002738)/24.0;
	gs=24.0*(t1-floor(t1));
	t1=gs/24.0-ra/TP;
	gh=TP*(t1-floor(t1));

	lat[0]=qth.stnlat;
	lon[0]=qth.stnlong;

	lat[1]=ssplat;
	lon[1]=ssplong;

	for (x=0; x<2; x++)
	{
		n=lat[x]*deg2rad;  /* North latitude of tracking position */
		e=-lon[x]*deg2rad; /* East longitude of tracking position */

		lh=gh+e;
		si=cos(lh)*cos(dc)*cos(n)+sin(dc)*sin(n);
		co=sqrt(1.0-(si*si));
		el[x]=atan2(si,co);

		si=-sin(lh)*cos(dc)*cos(n);
		co=sin(dc)-sin(n)*sin(el[x]);
		az[x]=atan2(si,co);

		if (az[x]<0.0)
			az[x]+=TP;
	}

	sun_az=az[0]/deg2rad;
	sun_el=el[0]/deg2rad;

	if (display)
	{
		attrset(COLOR_PAIR(4)|A_BOLD);
		mvprintw(20,5,"   Sun   ");
		mvprintw(21,5,"---------");
		attrset(COLOR_PAIR(3)|A_BOLD);
		mvprintw(22,5,"%-7.2fAz",sun_az);
		mvprintw(23,4,"%+-6.2f  El",sun_el);
	}

	/* ak = altitude of the satellite in km */

	/* The sky is dark enough to see large satellites
	   when the sun is at least 12 degrees (0.209 radians)
	   below the horizon (Nautical Twilight). */

	horizon_angle=-acos(R0/(R0+ak));

	if (el[1]<horizon_angle)
		x=' ';

	if ((el[1]>=horizon_angle) && (el[0]<=-0.20944))
		x='+';

	if ((el[1]>=horizon_angle) && (el[0]>-0.20944))
		x='*';

	return (char)x;
}

void FindMoon(daynum)
double daynum;
{
	double t1, t2, t3, ew, en, e, n, si, co, ra, gh,
	       lh, gm, de, se, gs, dc, el, az, mm, yr, ro;

	gm=24.0*(daynum-floor(daynum));
	ew=FixAngle(1.134193+daynum*0.229971506);
	mm=FixAngle(1.319238+daynum*0.228027135);
	t1=FixAngle(6.217512+daynum*0.01720196977);
	t2=2.0*FixAngle(2.550677+daynum*0.212768711);
	t3=FixAngle(4.7652214+daynum*0.230895723);
	ew=ew+0.01148*sin(t2)+0.10976*sin(mm);
	ew=ew-0.022235*sin(mm-t2)-0.003246*sin(t1);
	ew=ew+0.003735*sin(2.0*mm)-0.0019897*sin(2.0*t3);
	ew=ew-0.0010297*sin(2.0*mm-t2)-0.0009948*sin(mm+t1-t2);
	en=t3+0.0115070*sin(t2)+0.10873924*sin(mm);
	en=en-0.0222006*sin(mm-t2);
	en=0.0897797*sin(en)-0.002548*sin(t3-t2);

	si=C1*sin(en)+S1*cos(en)*sin(ew);
	co=sqrt(1.0-(si*si));
	dc=atan2(si,co);
	si=sin(ew)*C1-tan(en)*S1;
	co=cos(ew);
	ra=atan2(si,co);

	if (ra<0.0)
		ra+=TP;

	t1=(double)(long)(daynum-39410.0);
	t2=floor((t1+32044.75)/36524.25);
	t2+=t1-t2/4.0+1486.0;
	yr=2084.0+floor((t2-122.1)/365.25);

	t1=yr-1.0;
	de=floor(365.25*(t1-1980.0))-floor(t1/100.0)+floor(t1/400.0)+381.0;
	t1=(de+29218.5)/36525.0;
	t1=6.6460656+t1*(2400.051262+t1*2.581e-5);
	se=t1-24.0*(yr-1900.0);
	t1=(se+0.0657098*(daynum-de)+gm*1.002738)/24.0;
	gs=24.0*(t1-floor(t1));

	t1=gs/24.0-ra/TP;
	gh=TP*(t1-floor(t1));

	n=qth.stnlat*deg2rad;    /* North latitude of tracking station */
	e=-qth.stnlong*deg2rad;  /* East longitude of tracking station */

	lh=gh+e;

	si=cos(lh)*cos(dc)*cos(n)+sin(dc)*sin(n);
	co=sqrt(1.0-(si*si));
	el=atan2(si,co);

	si=-sin(lh)*cos(dc)*cos(n);
	co=sin(dc)-sin(n)*sin(el);
	az=atan2(si,co);

	if (az<0.0)
		az+=TP;

	ro=0.996986/(1.0+0.0549*cos(mm+0.10976*sin(mm)));
	el=el-0.0166*cos(el)/ro;

	moon_az=az/deg2rad;
	moon_el=el/deg2rad;

	attrset(COLOR_PAIR(4)|A_BOLD);
	mvprintw(20,65,"  Moon  ");
	mvprintw(21,65,"---------");
	attrset(COLOR_PAIR(3)|A_BOLD);
	mvprintw(22,65,"%-7.2fAz",moon_az);
	mvprintw(23,64,"%+-6.2f  El",moon_el);
}

double PreCalc(daynum)
double daynum;
{
	/* This function performs preliminary
	   calculations prior to tracking or prediction. */
 
	epoch=DayNum(1,0,sat[indx].year)+sat[indx].refepoch;
	age=daynum-epoch;
	yr=(float)sat[indx].year;

	/* Do the Y2K thing... */

	if (yr<=50.0)
		yr+=100.0;

	t1=yr-1.0;
	df=366.0+floor(365.25*(t1-80.0))-floor(t1/100.0)+floor(t1/400.0+0.75);
	t1=(df+29218.5)/36525.0;
	t1=6.6460656+t1*(2400.051262+t1*2.581e-5);
	se=t1/24.0-yr;
	n0=sat[indx].meanmo+(age*sat[indx].drag);
	sma=331.25*pow((1440.0/n0),(2.0/3.0));
	e2=1.0-(sat[indx].eccn*sat[indx].eccn); 
	e1=sqrt(e2);
	k2=9.95*(exp(log(R0/sma)*3.5))/e2*e2;
	s1=sin(sat[indx].incl*deg2rad); 
	c1=cos(sat[indx].incl*deg2rad);
	l8=qth.stnlat*deg2rad; 
	s9=sin(l8); 
	c9=cos(l8);
	s8=sin(-qth.stnlong*deg2rad); 
	c8=cos(qth.stnlong*deg2rad);
	r9=R0*(1.0+(FF/2.0)*(cos(2.0*l8)-1.0))+(qth.stnalt/1000.0);
	l8=atan((1.0-FF)*(1.0-FF)*s9/c9);
	z9=r9*sin(l8); 
	x9=r9*cos(l8)*c8;
	y9=r9*cos(l8)*s8;
	apogee=sma*(1.0+sat[indx].eccn)-R0;
	perigee=sma*(1.0-sat[indx].eccn)-R0;

	return daynum;
}

void Calc()
{
	/* This is the stuff we need to do repetitively. */

	age=daynum-epoch;
	o=deg2rad*(sat[indx].raan-(age)*k2*c1);
	s0=sin(o); 
	c0=cos(o);
	w=deg2rad*(sat[indx].argper+(age)*k2*(2.5*c1*c1-0.5));
	s2=sin(w); 
	c2=cos(w);
	c[1][1]=c2*c0-s2*s0*c1;
	c[1][2]=-s2*c0-c2*s0*c1;
	c[2][1]=c2*s0+s2*c0*c1;
	c[2][2]=-s2*s0+c2*c0*c1;
	c[3][1]=s2*s1;
	c[3][2]=c2*s1;
	q0=(sat[indx].meanan/360.0)+sat[indx].orbitnum;
	q=n0*age+q0; 
	rv=(long)floor(q);
	q=q-floor(q);
	m=q*TP;
	e=m+sat[indx].eccn*(sin(m)+0.5*sat[indx].eccn*sin(m*2.0));

	do   /* Kepler's Equation */
	{
		s3=sin(e); 
		c3=cos(e); 
		r3=1.0-sat[indx].eccn*c3;
		m1=e-sat[indx].eccn*s3; 
		m5=m1-m;
		e=e-m5/r3;

	} while (fabs(m5)>=1.0e-6);

	x0=sma*(c3-sat[indx].eccn); 
	yzero=sma*e1*s3;
	r=sma*r3; 
	x1=x0*c[1][1]+yzero*c[1][2];
	yone=x0*c[2][1]+yzero*c[2][2];
	z1=x0*c[3][1]+yzero*c[3][2];
	g7=(daynum-df)*1.0027379093+se;
	g7=TP*(g7-floor(g7));
	s7=-sin(g7); 
	c7=cos(g7);
	x=x1*c7-yone*s7; 
	y=x1*s7+yone*c7;
	z=z1; 
	x5=x-x9; 
	y5=y-y9; 
	z5=z-z9;
	range=x5*x5+y5*y5+z5*z5;
	z8=x5*c8*c9+y5*s8*c9+z5*s9;
	x8=-x5*c8*s9-y5*s8*s9+z5*c9;
	y8=y5*c8-x5*s8; 
	ak=r-R0;
	elevation=atan(z8/sqrt(range-z8*z8))/deg2rad;
	azimuth=atan(y8/x8)/deg2rad;

	if (x8<0.0)
		azimuth+=180.0;

	if (azimuth<0.0)
		azimuth+=360.0;

	ma256=(int)256.0*q;
	am=ak/KM;
	rk=sqrt(range); 
	rm=rk/KM;
	vk=3.6*sqrt(3.98652e+14*((2.0/(r*1000.0))-1.0/(sma*1000.0)));
	vm=vk/KM;
	fk=12756.33*acos(R0/r);
	fm=fk/KM;
	ssplat=atan(z/sqrt(r*r-z*z))/deg2rad;
	ssplong=-atan(y/x)/deg2rad;

	if (x<0.0)
		ssplong+=180.0;

	if (ssplong<0.0)
		ssplong+=360.0;

	irk=(long)rint(rk);
	isplat=(int)rint(ssplat);
	isplong=(int)rint(ssplong);
	iaz=(int)rint(azimuth);
	iel=(int)rint(elevation);
}

char AosHappens(x)
int x;
{
	/* This function returns a 1 if the satellite pointed to by
	   "x" can ever rise above the horizon of the ground station. */

	double lin, sma, apogee;

	if (sat[x].meanmo==0.0)
		return 0;
	else
	{
		lin=sat[x].incl;

		if (lin>=90.0)
			lin=180.0-lin;

		sma=331.25*exp(log(1440.0/sat[x].meanmo)*(2.0/3.0));
		apogee=sma*(1.0+sat[x].eccn)-R0;

		if ((acos(R0/(apogee+R0))+(lin*deg2rad)) > fabs(qth.stnlat*deg2rad))
			return 1;
		else
			return 0;
	}
}

char Decayed(x,time)
int x;
double time;
{
	/* This function returns a 1 if it appears that the
	   satellite pointed to by 'x' has decayed at the
	   time of 'time'.  If 'time' is 0.0, then the
	   current date/time is used. */

	double satepoch;

	if (time==0.0)
		time=CurrentDaynum();

	satepoch=DayNum(1,0,sat[x].year)+sat[x].refepoch;

	if (satepoch+((16.666666-sat[x].meanmo)/(10.0*fabs(sat[x].drag))) < time)
		return 1;
	else
		return 0;
}

char Geostationary(x)
int x;
{
	/* This function returns a 1 if the satellite pointed
	   to by "x" appears to be in a geostationary orbit */

	if (fabs(sat[x].meanmo-1.0027)<0.0002) 

		return 1;
	else
		return 0;
}

double FindAOS()
{
	/* This function finds and returns the time of AOS (aostime). */

	aostime=0.0;

	if (AosHappens(indx) && Geostationary(indx)==0 && Decayed(indx,daynum)==0)
	{
		Calc();

		/* Get the satellite in range */

		while (elevation < -1.0)
		{
			daynum-=0.00035*(elevation*(((ak/8400.0)+0.46))-2.0);

			/* Technically, this should be:

			   daynum-=0.0007*(elevation*(((ak/8400.0)+0.46))-2.0);

			   but it sometimes skips passes for
			   satellites in highly elliptical orbits. */

			Calc();
		}

		/* Find AOS */

		/** Users using Keplerian data to track the Sun MAY find
		    this section goes into an infinite loop when tracking
		    the Sun if their QTH is below 30 deg N! **/

		while (aostime==0.0)
		{
			if (fabs(elevation) < 0.03)
				aostime=daynum;
			else
			{
				daynum-=elevation*sqrt(ak)/530000.0;
				Calc();
			}
		}
	}

	return aostime;
}

double FindLOS()
{
	lostime=0.0;

	if (Geostationary(indx)==0 && AosHappens(indx)==1 && Decayed(indx,daynum)==0)
	{
		Calc();

		do
		{
			daynum+=elevation*sqrt(ak)/502500.0;
			Calc();

			if (fabs(elevation) < 0.03)
				lostime=daynum;

		} while (lostime==0.0);
	}

	return lostime;
}

double FindLOS2()
{
	/* This function steps through the pass to find LOS.
	   FindLOS() is called to "fine tune" and return the result. */

	do
	{
		daynum+=cos((elevation-1.0)*deg2rad)*sqrt(ak)/25000.0;
		Calc();

	} while (elevation>=0.0);

	return(FindLOS());
}

double NextAOS()
{
	/* This function finds and returns the time of the next
	   AOS for a satellite that is currently in range. */

	aostime=0.0;

	if (AosHappens(indx) && Geostationary(indx)==0 && Decayed(indx,daynum)==0)
		daynum=FindLOS2()+0.014;  /* Move to LOS + 20 minutes */

	return (FindAOS());
}

int Print(string,mode)
char *string, mode;
{
	/* This function buffers and displays orbital predictions
	   and allows screens to be saved to a disk file. */

	char type[20], spaces[80], head1[70], head2[70], head3[70];
	int key, ans=0, l, x, t;
	static char buffer[1450], lines, quit;
	static FILE *fd;

	/* Pass a NULL string to initialize the buffer, counter, and flags */

	if (string[0]==0)
	{
		lines=0;
		quit=0;
		buffer[0]=0;
		fd=NULL;
	}

	else
	{
		if (mode=='p')
			strcpy(type,"Orbit");

		if (mode=='v')
			strcpy(type,"Visual");

		if (mode=='s')
			strcpy(type,"Solar Illumination");

		l=strlen(qth.callsign)+strlen(sat[indx].name)+strlen(type);

		spaces[0]=0;

		for (x=l; x<60; x+=2)
			strcat(spaces," ");

		sprintf(head1,"\n%s%s's %s Calendar For %s", spaces, qth.callsign, type, sat[indx].name);

		if (mode=='s')
			sprintf(head2,"\n\t  Date     Mins/Day    Sun%c          Date      Mins/Day    Sun%c\n",37,37);
		else
			sprintf(head2,"\n\t   Date     Time    El   Az  Phase  Lat   Long    Range   Orbit\n");

		sprintf(head3,"      -----------------------------------------------------------------");

		strcat(buffer,string);
		lines++;

		if (lines==18)
		{
			bkgdset(COLOR_PAIR(2)|A_BOLD);
			clear();
			addstr(head1);
			attrset(COLOR_PAIR(4)|A_BOLD);
			addstr(head2);
			addstr(head3);
			attrset(COLOR_PAIR(3)|A_BOLD);

			if (buffer[0]!='\n')
				printw("\n");

			addstr(buffer);
			attrset(COLOR_PAIR(4)|A_BOLD);

			if (buffer[0]=='\n')
				printw("\n");

			if (fd==NULL)
				mvprintw(23,63,"        ");
			else
				mvprintw(23,63,"Log = ON");

			mvprintw(23,6,"More? [y/n] >> ");
			curs_set(1);
			refresh();

			while (ans==0)
			{
				key=toupper(getch());

				if (key=='Y' || key=='\n' || key==' ')
				{
					key='Y';
					ans=1;
					quit=0;
				}
			
				if (key=='N' || key=='Q' || key==27)
				{
					key='N';
					ans=1;
					quit=1;
				}

				/* 'L' logs output to "satname.txt" */

				if (key=='L' && fd==NULL && buffer[0])
				{
					sprintf(temp,"%s.txt",sat[indx].name);

					l=strlen(temp)-4;

					for (x=0; x<l; x++)
					{
						t=temp[x];

						if (t==32 || t==17 || t==92 || t==46 || t=='/')
							t='_';

						temp[x]=t;
					}

					fd=fopen(temp,"a");
					fprintf(fd,"%s%s%s\n",head1,head2,head3);
					fprintf(fd,"%s",buffer);
					mvprintw(23,63,"Log = ON");
					move(23,21);
					refresh();
				}

				else if (fd!=NULL)
				{
					if (key=='L' || key=='N')
					{
						fprintf(fd,"%s\n\n",buffer);
						fclose(fd);
						fd=NULL;
						mvprintw(23,63,"        ");
						move(23,21);
						refresh();
					}

					else
						fprintf(fd,"%s",buffer);
				}
				buffer[0]=0;
			}

			lines=0;
			curs_set(0);
		}
	}
	return (quit);
}

int PrintVisible(string)
char *string;
{
	/* This function acts as a filter to display passes that could
	   possibly be visible to the ground station.  It works by first
	   buffering prediction data generated by the Predict() function
	   and then checking it to see if at least a part of the pass
	   is visible.  If it is, then the buffered prediction data
	   is sent to the Print() function so it can be displayed
	   to the user and optionally logged to a file. */

	static char buffer[10000];
	char line[80], plus, asterisk, visible;
	int x, y, quit=0;

	if (string[0]==0)
		buffer[0]=0;
	else
	{
		strcat(buffer,string);

		if (string[0]=='\n')
		{
			plus=0;
			visible=0;
			asterisk=0;

			for (x=0; buffer[x]!=0 && visible==0; x++)
			{
				if (buffer[x]=='+')
					plus++;

				if (buffer[x]=='*')
					asterisk++;

				/* At least 3 +'s or at least 2 +'s
				   combined with at least 2 *'s is
				   worth displaying as a visible pass. */

				if ((plus>3) || (plus>2 && asterisk>2))
					visible=1;
			}

			if (visible)
			{
				/* Dump buffer to Print() line by line */

				for (x=0, y=0; buffer[x]!=0 && quit==0; x++)
				{	
					line[y]=buffer[x];

					if (line[y]=='\n')
					{
						line[y+1]=0;
						quit=Print(line,'v');
						line[0]=0;
						y=0;
					}

					else
						y++;
				}
			}

			buffer[0]=0;
		}
	}

	return quit;
}

void Predict(mode)
char mode;
{
	/* This function predicts satellite passes.  It displays
	   output through the Print() function if mode=='p' (show
	   all passes), or through the PrintVisible() function if
	   mode=='v' (show only visible passes). */

	char string[80];
	int quit=0, lastel=0, breakout=0;

	daynum=PreCalc(GetStartTime());
	clear();

	/* Trap geostationary orbits and passes that cannot occur. */

	if (AosHappens(indx) && Geostationary(indx)==0 && Decayed(indx,daynum)==0)
	{
		do
		{
			daynum=FindAOS();

			/* Display the pass */

			while (iel>=0 && quit==0)
			{
				sprintf(string,"      %s%4d %4d  %4d  %4d   %4d   %6ld  %6ld %c\n",Daynum2String(daynum),iel,iaz,ma256,isplat,isplong,irk,rv,FindSun(daynum,0));
				lastel=iel;

				if (mode=='p')
					quit=Print(string,'p');

				if (mode=='v')
				{
					nodelay(stdscr,TRUE);
					attrset(COLOR_PAIR(4));
					mvprintw(23,6,"                 Calculating... Press [ESC] To Quit");

					/* Allow a way out if this
					   should continue forever... */

					if (getch()==27)
						breakout=1;

					nodelay(stdscr,FALSE);

					quit=PrintVisible(string);
				}

				daynum+=cos((elevation-1.0)*deg2rad)*sqrt(ak)/25000.0;
				Calc();
			}

			if (lastel!=0)
			{
				daynum=FindLOS();
				Calc();
				sprintf(string,"      %s%4d %4d  %4d  %4d   %4d   %6ld  %6ld %c\n",Daynum2String(daynum),iel,iaz,ma256,isplat,isplong,irk,rv,FindSun(daynum,0));

				if (mode=='p')
					quit=Print(string,'p');

				if (mode=='v')
					quit=PrintVisible(string);
			}

			if (mode=='p')
				quit=Print("\n",'p');

			if (mode=='v')
				quit=PrintVisible("\n");

			/* Move to next orbit */
			daynum=NextAOS();

		}  while (quit==0 && breakout==0 && AosHappens(indx) && Decayed(indx,daynum)==0);
	}

	else
	{
		bkgdset(COLOR_PAIR(5)|A_BOLD);
		clear();

		if (AosHappens(indx)==0 || Decayed(indx,daynum)==1)
			mvprintw(12,5,"*** Passes for %s cannot occur for your ground station! ***\n",sat[indx].name);

		if (Geostationary(indx)==1)
			mvprintw(12,3,"*** Orbital predictions cannot be made for a geostationary satellite! ***\n");

		beep();
		bkgdset(COLOR_PAIR(7)|A_BOLD);
		AnyKey();
		refresh();
	}
}

char KbEdit(x,y)
int x,y;
{
	/* This function is used when editing QTH
	   and orbital data via the keyboard. */

	char need2save=0, input[25];

	echo();
	move(y-1,x-1);
	wgetnstr(stdscr,input,24);

	if (strlen(input)!=0)
	{
		need2save=1;  /* Save new data to variables */
		resave=1;     /* Save new data to disk files */
		strcpy(temp,input);
	}

	mvprintw(y-1,x-1,"%-25s",temp);

	refresh();
	noecho();

	return need2save;
}

void ShowOrbitData()
{
	/* This function permits displays a satellite's orbital
	   data.  The age of the satellite data is also provided. */

	int c, x, namelength, age;	
	double an_period, no_period, satepoch;
	char days[5];

	x=Select();

	while (x!=-1)
	{
		if (sat[x].meanmo!=0.0)
		{
			bkgdset(COLOR_PAIR(2)|A_BOLD);
			clear();
			sma=331.25*exp(log(1440.0/sat[x].meanmo)*(2.0/3.0));
			an_period=1440.0/sat[x].meanmo;
			c1=cos(sat[x].incl*deg2rad);
			e2=1.0-(sat[x].eccn*sat[x].eccn); 
			no_period=(an_period*360.0)/(360.0+(4.97*pow((R0/sma),3.5)*((5.0*c1*c1)-1.0)/(e2*e2))/sat[x].meanmo);
			satepoch=DayNum(1,0,sat[x].year)+sat[x].refepoch;
			age=(int)rint(CurrentDaynum()-satepoch);

			if (age==1)
				strcpy(days,"day");
			else
				strcpy(days,"days");

			namelength=strlen(sat[x].name);

			printw("\n\n");

			for (c=41; c>namelength; c-=2)
				printw(" ");
	
			printw("Orbital Data For %s / Catalog Number %ld\n",sat[x].name,sat[x].catnum);
			attrset(COLOR_PAIR(3)|A_BOLD);
			printw("\n\t\t  Issued %d %s ago on %s UTC\n\n",age,days,Daynum2String(satepoch));

			attrset(COLOR_PAIR(4)|A_BOLD);
			mvprintw(7,21,"Reference Epoch");
			mvprintw(8,21,"Inclination");
			mvprintw(9,21,"RAAN");
			mvprintw(10,21,"Eccentricity");
			mvprintw(11,21,"Arg of Perigee");
			mvprintw(12,21,"Mean Anomaly");
			mvprintw(13,21,"Mean Motion");
			mvprintw(14,21,"Decay Rate");
			mvprintw(15,21,"Orbit Number");
			mvprintw(16,21,"Semi-Major Axis");
			mvprintw(17,21,"Apogee Altitude");
			mvprintw(18,21,"Perigee Altitude");
			mvprintw(19,21,"Anomalistic Period");
			mvprintw(20,21,"Nodal Period");

			attrset(COLOR_PAIR(2)|A_BOLD);
			mvprintw(7,40,": %02d %.8f",sat[x].year,sat[x].refepoch);
			mvprintw(8,40,": %.4f deg",sat[x].incl);
			mvprintw(9,40,": %.4f deg",sat[x].raan);
			mvprintw(10,40,": %g",sat[x].eccn);
			mvprintw(11,40,": %.4f deg",sat[x].argper);
			mvprintw(12,40,": %.4f deg",sat[x].meanan);
			mvprintw(13,40,": %.8f rev/day",sat[x].meanmo);
			mvprintw(14,40,": %g rev/day/day",sat[x].drag);
			mvprintw(15,40,": %ld",sat[x].orbitnum);
			mvprintw(16,40,": %.4f km",sma);
			mvprintw(17,40,": %.4f km",sma*(1.0+sat[x].eccn)-R0);
			mvprintw(18,40,": %.4f km",sma*(1.0-sat[x].eccn)-R0);
			mvprintw(19,40,": %.4f mins",an_period);
			mvprintw(20,40,": %.4f mins\n",no_period);

			attrset(COLOR_PAIR(3)|A_BOLD);
			refresh();
			AnyKey();
		}
		x=Select();
	 };
}	

void KepEdit()
{
	/* This function permits keyboard editing of the orbital database. */

	int x;

	do
	{
		x=Select();

		if (x!=-1)
		{
			bkgdset(COLOR_PAIR(3)|A_BOLD);
			clear();
			mvprintw(6,1,"\t\t   *  Orbital Database Editing Utility  *\n");
			attrset(COLOR_PAIR(4)|A_BOLD);

			printw("\n\t\t\tSpacecraft Name :");
			printw("\n\t\t\tCatalog Number  :");
			printw("\n\t\t\tReference Epoch :");
			printw("\n\t\t\tInclination     :");
			printw("\n\t\t\tRAAN            :");
			printw("\n\t\t\tEccentricity    :");
			printw("\n\t\t\tArg of Perigee  :");
			printw("\n\t\t\tMean Anomaly    :");
			printw("\n\t\t\tMean Motion     :");
			printw("\n\t\t\tDecay Rate      :");
			printw("\n\t\t\tOrbit Number    :");

			attrset(COLOR_PAIR(2)|A_BOLD);

			mvprintw(8,42,"%s",sat[x].name);
			mvprintw(9,42,"%ld",sat[x].catnum);
			mvprintw(10,42,"%02d %.8f",sat[x].year,sat[x].refepoch);
			mvprintw(11,42,"%.4f",sat[x].incl);
			mvprintw(12,42,"%.4f",sat[x].raan);
			mvprintw(13,42,"%g",sat[x].eccn);
			mvprintw(14,42,"%.4f",sat[x].argper);
			mvprintw(15,42,"%.4f",sat[x].meanan);
			mvprintw(16,42,"%.8f",sat[x].meanmo);
			mvprintw(17,42,"%g",sat[x].drag);
			mvprintw(18,42,"%ld",sat[x].orbitnum);

			curs_set(1);
			refresh();

			sprintf(temp,"%s",sat[x].name);

			if (KbEdit(43,9))
				strncpy(sat[x].name,temp,24);

			sprintf(temp,"%ld",sat[x].catnum);

			if (KbEdit(43,10))
				sscanf(temp,"%ld",&sat[x].catnum);

			sprintf(temp,"%02d %4.8f",sat[x].year,sat[x].refepoch);

			if (KbEdit(43,11))
				sscanf(temp,"%d %lf",&sat[x].year,&sat[x].refepoch);

			sprintf(temp,"%4.4f",sat[x].incl);

			if (KbEdit(43,12))
				sscanf(temp,"%lf",&sat[x].incl);
			    
			sprintf(temp,"%4.4f",sat[x].raan);

			if (KbEdit(43,13))
				sscanf(temp,"%lf",&sat[x].raan);

			sprintf(temp,"%g",sat[x].eccn);

			if (KbEdit(43,14))
				sscanf(temp,"%lf",&sat[x].eccn);
			    
			sprintf(temp,"%4.4f",sat[x].argper);

			if (KbEdit(43,15))
				sscanf(temp,"%lf",&sat[x].argper);
			    
			sprintf(temp,"%4.4f",sat[x].meanan);

			if (KbEdit(43,16))
				sscanf(temp,"%lf",&sat[x].meanan);

			sprintf(temp,"%4.8f",sat[x].meanmo);

			if (KbEdit(43,17))
				sscanf(temp,"%lf",&sat[x].meanmo);
			    
			sprintf(temp,"%g",sat[x].drag);

			if (KbEdit(43,18))
				sscanf(temp,"%lf",&sat[x].drag);
			    
			sprintf(temp,"%ld",sat[x].orbitnum);

			if (KbEdit(43,19))
				sscanf(temp,"%ld",&sat[x].orbitnum);
		  	
			curs_set(0);
		}

	} while (x!=-1);

	if (resave)
	{
		SaveTLE();
		resave=0;
	}
}	

void QthEdit()
{
	/* This function permits keyboard editing of
	   the ground station's location information. */

	bkgdset(COLOR_PAIR(3)|A_BOLD);
	clear();
	curs_set(1);
	mvprintw(7,0,"\t\t *  Ground Station Location Editing Utility  *\n\n\n");

	attrset(COLOR_PAIR(4)|A_BOLD);
	printw("\n\t\t\tStation Callsign :");
	printw("\n\t\t\tStation Latitude :");
	printw("\n\t\t\tStation Longitude:");
	printw("\n\t\t\tStation Altitude :");

	attrset(COLOR_PAIR(2)|A_BOLD);
	mvprintw(11,43,"%s",qth.callsign);
	mvprintw(12,43,"%g",qth.stnlat);
	mvprintw(13,43,"%g",qth.stnlong);
	mvprintw(14,43,"%d",qth.stnalt);
	refresh();

	sprintf(temp,"%s",qth.callsign);

	mvprintw(18,20,"Enter the callsign of your ground station");

	if (KbEdit(44,12))
		strncpy(qth.callsign,temp,7);

	sprintf(temp,"%g",qth.stnlat);

	mvprintw(18,10,"Enter your latitude in decimal degrees NORTH (south=negative)");

	if (KbEdit(44,13))
		sscanf(temp,"%lf",&qth.stnlat);

	sprintf(temp,"%g",qth.stnlong);

	mvprintw(18,10,"Enter your longitude in decimal degrees WEST (east=negative) ");

	if (KbEdit(44,14))
		sscanf(temp,"%lf",&qth.stnlong);

	mvprintw(18,10,"       Enter your altitude above sea level (in meters)       ");

	sprintf(temp,"%d",qth.stnalt);

	if (KbEdit(44,15))
		sscanf(temp,"%d",&qth.stnalt);

	if (resave)
	{
		SaveQTH();
		resave=0;
	}
}

void SingleTrack(x,speak)
int x;
char speak;
{
	/* This function tracks a single satellite in real-time
	   until 'Q' or ESC is pressed.  x represents the index
	   of the satellite being tracked.  If speak=='T', then
	   the speech routines are enabled. */

	int	ans, oldaz=0, oldel=0;

	char	going_or_coming=0, command[80], oncethru=0,
		alarm=0, geostationary=0, aoshappens=0, decayed=0;

	double	pl146=0.0, pl435=0.0, old_clock=0.0, old_range=0.0,
	      	dp=0.0, dt=0.0, doppler146=0.0, doppler435=0.0,
	      	fraction=0.0, oldtime=0.0, nextaos=0.0, lostime=0.0,
		aoslos=0.0;

	curs_set(0);
	bkgdset(COLOR_PAIR(3));
	clear();
	refresh();

	attrset(COLOR_PAIR(6)|A_REVERSE|A_BOLD);
	mvprintw(1,15,"                                                ");
	mvprintw(2,15,"         PREDICT Real-Time Tracking Mode        ");
	mvprintw(3,15,"  Tracking: %-10sOn:                       ",Abbreviate(sat[x].name,9));
	mvprintw(4,15,"                                                ");

	attrset(COLOR_PAIR(4)|A_BOLD);

	mvprintw(6,20,"SATELLITE     ALTITUDE     SLANT RANGE");
	mvprintw(7,20,"---------     --------     -----------");
	mvprintw(8,20,"        .           mi              mi");
	mvprintw(9,20,"       W.           km              km");
	mvprintw(11,20,"DIRECTION     VELOCITY   DOPPLER SHIFT");
	mvprintw(12,20,"---------     --------   -------------");
	mvprintw(13,20,"       Az           mi   146:       Hz");
	mvprintw(14,20,"       El           km   435:       Hz");
	mvprintw(16,20,"PATH - LOSS   FOOTPRINT  ORBITAL PHASE");
	mvprintw(17,20,"-----------   ---------  -------------");
	mvprintw(18,20,"146:     dB          mi          (360)");
	mvprintw(19,20,"435:     dB          km          (256)");

	attrset(COLOR_PAIR(3)|A_BOLD);
	mvprintw(21,22,"Orbit Number:");

	if (strcmp("/dev/null\0",serial_port))
	{
		attrset(COLOR_PAIR(3)|A_BOLD);
		mvprintw(24,22,"Tracking data sent to: %s ",serial_port);
	}

	halfdelay(2);
	PreCalc(CurrentDaynum());

	aoshappens=AosHappens(indx);
	geostationary=Geostationary(indx);
	decayed=Decayed(indx,0.0);

	do
	{
		attrset(COLOR_PAIR(6)|A_REVERSE|A_BOLD);
		daynum=CurrentDaynum();
		mvprintw(3,41,"%s",Daynum2String(daynum));
		Calc();
		attrset(COLOR_PAIR(2)|A_BOLD);


		if (ssplat>=0.0)
		{
			mvprintw(8,20,"%-6.2f",ssplat);
			attrset(COLOR_PAIR(4)|A_BOLD);
			mvprintw(8,27,"N");
		}

		else
		{
			mvprintw(8,20,"%-6.2f",-ssplat);
			attrset(COLOR_PAIR(4)|A_BOLD);
			mvprintw(8,27,"S");
		}

		attrset(COLOR_PAIR(2)|A_BOLD);
		mvprintw(8,34,"%0.f ",am);
		mvprintw(9,34,"%0.f ",ak);
		mvprintw(8,47,"%-5.0f",rm);
		mvprintw(9,47,"%-5.0f",rk);
		mvprintw(9,20,"%-7.2f",ssplong);
		mvprintw(13,20,"%-7.2f",azimuth);
		mvprintw(14,19,"%+-6.2f",elevation);
		mvprintw(13,34,"%0.f ",vm);
		mvprintw(14,34,"%0.f ",vk);

		/* Send data to serial port antenna tracker */

		if (elevation>=0.0 && (strcmp("/dev/null\0",serial_port)))
		{
			if (oldel!=iel || oldaz!=iaz)
			{
				TrackDataOut(antfd,(float)iel,(float)iaz);
				oldel=iel;
				oldaz=iaz;
			}
		}

		dt=daynum*86400.0-old_clock;
		dp=(rk*1000.0)-old_range;

		if (dt>0.0)
		{
			fraction=-((dp/dt)/299792458.0);
			doppler146=fraction*146.0e6;
			doppler435=fraction*435.0e6;
			old_clock=86400.0*daynum;
			old_range=rk*1000.0;
		}

		if (elevation>=0.0)
		{
			pl146=75.6870571232+(20.0*log10(rk));
			pl435=pl146+9.482728;
			mvprintw(18,25,"%0.f",pl146);
			mvprintw(19,25,"%0.f",pl435);

			if (alarm==0)
			{
				beep();
				alarm=1;
			}

			if (oncethru && dt>0.0)
			{
				mvprintw(13,50,"%+5.0f",doppler146);
				mvprintw(14,49,"%+6.0f",doppler435);

				if (doppler435>0.0)
					going_or_coming='+';
				else
					going_or_coming='-';

				if (speak=='T' && soundcard && (CurrentDaynum()-oldtime)>(0.00003*sqrt(ak)))
				{
					sprintf(command,"%svocalizer/vocalizer %.0f %.0f %c &",predictpath,azimuth,elevation,going_or_coming);
					system(command);
  					oldtime=CurrentDaynum();
					going_or_coming=0;
				}

			}
		}

		else
		{
			mvprintw(18,25,"---");
			mvprintw(19,25,"---");
			mvprintw(13,50,"-----");
			mvprintw(14,49," ----- ");
			lostime=0.0;
			alarm=0;
		}

		mvprintw(18,34,"%0.f ",fm);
		mvprintw(19,34,"%0.f ",fk);
		mvprintw(18,45,"%-5.0f",q*360.0);
		mvprintw(19,45,"%-5.0f",q*256.0);
		attrset(COLOR_PAIR(3)|A_BOLD);
		mvprintw(21,36,"%ld",rv);
		mvprintw(23,22,"Spacecraft is currently ");

		switch (FindSun(CurrentDaynum(),1))
		{
			case '*':
				mvprintw(23,46,"in sunlight");
				visibility_array[indx]='D';
				break;

			case '+':
				if (elevation>0.0)
				{
					mvprintw(23,46,"visible    ");
					visibility_array[indx]='V';
				}

				else
				{
					mvprintw(23,46,"in sunlight");
					visibility_array[indx]='D';
				}
				break;

			case ' ':
				mvprintw(23,46,"in eclipse ");
				visibility_array[indx]='N';
				break;
		}

		if (geostationary==1 && elevation>=0.0)
		{
			mvprintw(22,22,"Satellite orbit is geostationary");
			aoslos=-3651.0;
		}

		if (geostationary==1 && elevation<0.0)
		{
			mvprintw(22,22,"This satellite never reaches AOS");
			aoslos=-3651.0;
		}

		if (aoshappens==0 || decayed==1)
		{
			mvprintw(22,22,"This satellite never reaches AOS");
			aoslos=-3651.0;
		}

		if (elevation>=0.0 && geostationary==0 && decayed==0 && daynum>lostime)
		{
			lostime=FindLOS2();
			mvprintw(22,22,"LOS at: %s UTC  ",Daynum2String(lostime));
			aoslos=lostime;
		}

		else if (elevation<0.0 && geostationary==0 && decayed==0 && aoshappens==1 && daynum>nextaos)
		{
			daynum+=0.003;  /* Move ahead slightly... */
			nextaos=FindAOS();
			mvprintw(22,22,"Next AOS: %s UTC",Daynum2String(nextaos));
			aoslos=nextaos;
		}

		FindMoon(CurrentDaynum());

		/* This is where the variables for the socket server are updated. */

		if (socket_flag)
		{
			az_array[indx]=azimuth;
			el_array[indx]=elevation;
			lat_array[indx]=ssplat;
			long_array[indx]=ssplong;
			footprint_array[indx]=fk;
			range_array[indx]=rk;
			altitude_array[indx]=ak;
			velocity_array[indx]=vk;
			orbitnum_array[indx]=rv;
			doppler100[indx]=doppler146/1.46;
			nextevent[indx]=aoslos;
		}

		refresh();
		ans=tolower(getch());

		/* We can force PREDICT to speak by pressing 'T' */

		if (ans=='t')
			oldtime=0.0;

		oncethru=1;

		/* If we receive a RELOAD_TLE command through the
		   socket connection, reload the TLE file, and run
		   through the preliminary orbital calculations
		   again to get everything up to date */

		if (reload_tle || ans=='r')
		{
			if (ReadDataFiles()&2)
				PreCalc(CurrentDaynum());

			reload_tle=0;
		}

	} while (ans!='q' && ans!=27);

	cbreak();
}

void MultiTrack()
{
	/* This function tracks all satellites in the program's
	   database simultaneously until 'Q' or ESC is pressed.
	   Satellites in range are HIGHLIGHTED.  Coordinates
	   for the Sun and Moon are also displayed. */

	int		x, y, z, ans;

	unsigned char	satindex[24], inrange[24], sunstat=0, ok2predict[24];

	double		aos[24], aos2[24], temptime, nextcalctime=0.0, dt[24],
			dp[24], old_clock[24], old_range[24], fraction[24],
			doppler[24], los[24], aoslos[24];

	curs_set(0);
	attrset(COLOR_PAIR(6)|A_REVERSE|A_BOLD);
	clear();
	refresh();

	printw("                                                                                ");
	printw("                     PREDICT Real-Time Multi-Tracking Mode                      ");
	printw("                    Current Date/Time:                                          ");
	printw("                                                                                ");

	attrset(COLOR_PAIR(2)|A_REVERSE);
	printw(" Satellite  Az   El  Lat  Long  Range  | Satellite  Az   El  Lat  Long  Range   ");

	for (x=0; x<24; x++)
	{
		if (Geostationary(x)==0 && AosHappens(x)==1 && Decayed(x,0.0)!=1)
			ok2predict[x]=1;
		else
			ok2predict[x]=0;

		aoslos[x]=0.0;
		los[x]=0.0;
		aos[x]=0.0;
		aos2[x]=0.0;
	}

	do
	{
		for (z=0; z<24; z++)
		{
			y=z/2;

			if (z%2)
			{
				x=41;
				indx=y+12;
			}

			else
			{
				x=1;
				indx=y;
			}

			if (sat[indx].meanmo!=0.0 && Decayed(indx,0.0)!=1)
			{
				daynum=CurrentDaynum();
				PreCalc(daynum);
				Calc();

				if (socket_flag)
				{
					dt[indx]=daynum*86400.0-old_clock[indx];
					dp[indx]=(rk*1000.0)-old_range[indx];
				}

				if (elevation>=0.0)
				{
					attrset(COLOR_PAIR(2)|A_BOLD);
					inrange[indx]=1;

					if (socket_flag)
					{
						fraction[indx]=-((dp[indx]/dt[indx])/299792458.0);
						doppler[indx]=fraction[indx]*100.0e6;
						old_clock[indx]=86400.0*daynum;
						old_range[indx]=rk*1000.0;
					}
				}

				else
				{
					attrset(COLOR_PAIR(2));
					inrange[indx]=0;
				}

				switch (FindSun(daynum,0))
				{
					case '*':
						sunstat='D';
						break;

					case '+':
						if (inrange[indx])
							sunstat='V';
						else
							sunstat='D';
						break;

					case ' ':
						sunstat='N';
						break;
				}

				mvprintw(y+6,x,"%-10s%3.0f  %+3.0f  %3.0f   %3.0f %6.0f %c", Abbreviate(sat[indx].name,9),azimuth,elevation,ssplat,ssplong,rk,sunstat);

				if (socket_flag)
				{
					az_array[indx]=azimuth;
					el_array[indx]=elevation;
					lat_array[indx]=ssplat;
					long_array[indx]=ssplong;
					footprint_array[indx]=fk;
					range_array[indx]=rk;
					altitude_array[indx]=ak;
					velocity_array[indx]=vk;
					orbitnum_array[indx]=rv;
					visibility_array[indx]=sunstat;

					if (inrange[indx] && (fabs(doppler[indx])>0.5))
						doppler100[indx]=doppler[indx];
					else
						doppler100[indx]=0.0;
				}

				/* Calculate Next Event (AOS/LOS) Times */

				if (ok2predict[indx] && daynum>los[indx] && inrange[indx])
					los[indx]=FindLOS2();

				if (ok2predict[indx] && daynum>aos[indx])
				{
					if (inrange[indx])
						aos[indx]=NextAOS();
					else
						aos[indx]=FindAOS();
				}

				if (inrange[indx])
					aoslos[indx]=los[indx];
				else
					aoslos[indx]=aos[indx];

				if (socket_flag)
				{
					if (ok2predict[indx])
						nextevent[indx]=aoslos[indx];
					else
						nextevent[indx]=-3651.0;
				}

				aos2[indx]=aos[indx];
				satindex[indx]=indx;
			}

			if (Decayed(indx,0.0))
			{
				attrset(COLOR_PAIR(2));
				mvprintw(y+6,x,"%-10s---------- Decayed ---------", Abbreviate(sat[indx].name,9));
			}
 		}

		attrset(COLOR_PAIR(6)|A_REVERSE|A_BOLD);

		daynum=CurrentDaynum();
		mvprintw(2,39,"%s",Daynum2String(daynum));

		if (daynum>nextcalctime)
		{
			/* Bubble sort the AOS times */

			for (z=22; z>=0; z--)
				for (y=0; y<=z; y++)
					if (aos2[y]>=aos2[y+1])
					{
						temptime=aos2[y];
						aos2[y]=aos2[y+1];
						aos2[y+1]=temptime;

						x=satindex[y];
						satindex[y]=satindex[y+1];
						satindex[y+1]=x;
					}

			/* Display list of upcoming passes */

			attrset(COLOR_PAIR(4)|A_BOLD);
			mvprintw(19,31,"Upcoming Passes");
			mvprintw(20,31,"---------------");
			attrset(COLOR_PAIR(3)|A_BOLD);

			for (x=0, y=0, z=-1; x<21 && y!=3; x++)
			{
				if (ok2predict[satindex[x]] && aos2[x]!=0.0)
				{
					mvprintw(y+21,19,"%10s on %s UTC",Abbreviate(sat[(int)satindex[x]].name,9),Daynum2String(aos2[x]));

					if (z==-1)
						z=x;
					y++;
				}
			}

			if (z!=-1)
				nextcalctime=aos2[z];
		}

		FindSun(CurrentDaynum(),1);
		FindMoon(CurrentDaynum());

		refresh();
		halfdelay(2);  /* Increase if CPU load is too high */
		ans=tolower(getch());

		/* If we receive a RELOAD_TLE command through the
		   socket connection, reload the TLE file, and run
		   through the preliminary orbital calculations
		   again to get everything up-to-date */

		if (reload_tle || ans=='r')
		{
			if (ReadDataFiles()&2)
				PreCalc(CurrentDaynum());

			reload_tle=0;
			nextcalctime=0.0;
		}

	} while (ans!='q' && ans!=27);

	cbreak();
}

void Illumination()
{
	double startday, oneminute, sunpercent;
	int eclipses, minutes, quit, breakout=0;
	char string1[40], string[80], datestring[25], count;

	oneminute=1.0/(24.0*60.0);

	daynum=floor(PreCalc(GetStartTime()));
	startday=daynum;
	count=0;

	curs_set(0);
	clear();

	do
	{
		attrset(COLOR_PAIR(4));
		mvprintw(23,6,"                 Calculating... Press [ESC] To Quit");
		refresh();

		count++;
		daynum=startday;

		for (minutes=0, eclipses=0; minutes<1440; minutes++)
		{
			Calc();

			if (FindSun(daynum,0)==' ')
				eclipses++;

			daynum=startday+(oneminute*(double)minutes);
		}

		sunpercent=((double)eclipses)/((double)minutes);
		sunpercent=100.0-(sunpercent*100.0);

		strcpy(datestring,Daynum2String(startday));
		datestring[11]=0;
		sprintf(string1,"      %s    %4d    %6.2f%c",datestring,1440-eclipses,sunpercent,37);

		/* Allow a quick way out */

		nodelay(stdscr,TRUE);

		if (getch()==27)
			breakout=1;

		nodelay(stdscr,FALSE);

		startday+=18.0;

		daynum=startday;

		for (minutes=0, eclipses=0; minutes<1440; minutes++)
		{
			Calc();

			if (FindSun(daynum,0)==' ')
				eclipses++;

			daynum=startday+(oneminute*(double)minutes);
		}

		sunpercent=((double)eclipses)/((double)minutes);
		sunpercent=100.0-(sunpercent*100.0);

		strcpy(datestring,Daynum2String(startday));
		datestring[11]=0;
		sprintf(string,"%s\t %s    %4d    %6.2f%c\n",string1,datestring,1440-eclipses,sunpercent,37);
		quit=Print(string,'s');

		/* Allow a quick way out */

		nodelay(stdscr,TRUE);

		if (getch()==27)
			breakout=1;

		nodelay(stdscr,FALSE);

		if (count<18)
			startday-=17.0;
		else
		{
			count=0;
			startday+=1.0;
		}
	}
	while (quit!=1 && breakout!=1 && Decayed(indx,daynum)==0);
}

void MainMenu()
{
	/* Start-up menu.  Your wish is my command. :-) */

	Banner();
	attrset(COLOR_PAIR(4)|A_BOLD);
	mvprintw(10,21,"Available Functions.....");

	attrset(COLOR_PAIR(3)|A_BOLD);
	mvprintw(12,21,"[P]: Predict Satellite Passes");
	mvprintw(13,21,"[V]: Predict Visible Passes");
	mvprintw(14,21,"[S]: Solar Illumination Predictions");
	mvprintw(15,21,"[T]: Single Satellite Tracking Mode");
	mvprintw(16,21,"[M]: Multi-Satellite Tracking Mode");
	mvprintw(17,21,"[U]: Update Database From NASA TLEs");
	mvprintw(18,21,"[E]: Keyboard Edit of Orbital Database");
	mvprintw(19,21,"[G]: Edit Ground Station Information");
	mvprintw(20,21,"[D]: Display Satellite Orbital Data");
	mvprintw(21,21,"[I]: Program Information");
	mvprintw(22,21,"[Q]: Exit Program");
	refresh();
}

void ProgramInfo()
{
	Banner();
	attrset(COLOR_PAIR(3)|A_BOLD);

	mvprintw(10,2,"PREDICT is a satellite tracking and orbital prediction program written for\n");
	printw("  the Linux operating system by John A. Magliacane, KD2BD (kd2bd@amsat.org).\n");
	printw("  PREDICT is free software.  You can redistribute it and/or modify it under\n");
	printw("  the terms of the GNU General Public License as published by the Free\n");
	printw("  Software Foundation, either version 2 of the License or any later version.\n\n");
	printw("  PREDICT is distributed in the hope that it will be useful, but WITHOUT ANY\n");
	printw("  WARRANTY, without even the implied warranty of MERCHANTABILITY or FITNESS\n");
	printw("  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more\n");
	printw("  details.");
	refresh();
	attrset(COLOR_PAIR(4)|A_BOLD);
	AnyKey();
}

void NewUser()
{
	int byte, *mkdir();
	FILE *infile, *outfile;

	Banner();
	attrset(COLOR_PAIR(3)|A_BOLD);

	mvprintw(12,2,"WELCOME to PREDICT!  Since you are a new user to the program, default\n");
	printw("  orbital data and ground station location information was copied into\n");
	printw("  your home directory to get you going.  Please select option [G] from\n");
	printw("  PREDICT's main menu to edit your ground station information, and update\n");
	printw("  your orbital database using option [U] or [E].  Enjoy the program!  :-)");
	refresh();

	/* Make "~/.predict" subdirectory */

	sprintf(temp,"%s/.predict",getenv("HOME"));
	mkdir(temp,511);  /* 511 is a guess, but it seems to work! :-) */

	/* Copy default files into ~/.predict directory */

	sprintf(temp,"%sdefault/predict.tle",predictpath);
	infile=fopen(temp,"rb");
	outfile=fopen(tlefile,"w");

	if (infile!=NULL)
	{
		byte=fgetc(infile);

		while (feof(infile)==0)
		{
			fputc(byte,outfile);
			byte=fgetc(infile);
		}

		fclose(outfile);
		fclose(infile);
	}

	sprintf(temp,"%sdefault/predict.qth",predictpath);
	infile=fopen(temp,"rb");
	outfile=fopen(qthfile,"w");

	if (infile!=NULL)
	{
		byte=fgetc(infile);

		while (feof(infile)==0)
		{
			fputc(byte,outfile);
			byte=fgetc(infile);
		}

		fclose(outfile);
		fclose(infile);
	}

	attrset(COLOR_PAIR(4)|A_BOLD);
	AnyKey();
}

int QuickFind(string, outputfile)
char *string, *outputfile;
{
	int x, y, z, step=1;
	long start, now, end, count;
	char satname[50], startstr[20], endstr[20];
	time_t t;
	FILE *fd;

	if (outputfile[0])
		fd=fopen(outputfile,"w");
	else
		fd=stdout;

	startstr[0]=0;
	endstr[0]=0;

	ReadDataFiles();

	for (x=0; x<48 && string[x]!=0 && string[x]!='\n'; x++)
		satname[x]=string[x];

	satname[x]=0;
	x++;

	for (y=0; string[x+y]!=0 && string[x+y]!='\n'; y++)
		startstr[y]=string[x+y];

	startstr[y]=0;
	y++;

	for (z=0; string[x+y+z]!=0 && string[x+y+z]!='\n'; z++)
		endstr[z]=string[x+y+z];

	endstr[z]=0;
 
	/* Do a simple search for the matching satellite name */

	for (z=0; z<24; z++)
	{
		if ((strcmp(sat[z].name,satname)==0) || (atol(satname)==sat[z].catnum))
		{
			start=atol(startstr);

			if (endstr[strlen(endstr)-1]=='m')
			{
				step=60;
				endstr[strlen(endstr)-1]=0;
			}
			
			if (endstr[0]=='+')
				end=start+((long)step)*atol(endstr);
			else
				end=atol(endstr);

			indx=z;

			t=time(NULL);
			now=(long)t;

			if (start==0)
				start=now;

			if (startstr[0]=='+')
			{
				start=now;

				if (startstr[strlen(startstr)-1]=='m')
				{
					step=60;
					startstr[strlen(startstr)-1]=0;
				}

				end=start+((long)step)*atol(startstr);

				/* Prevent a list greater than
				   24 hours from being produced */

				if ((end-start)>86400)
				{
					start=now;
					end=now-1;
				}
			}

			if ((start>=now-31557600) && (start<=now+31557600) && end==0)
			{
				/* Start must be one year from now */
				/* Display a single position */
				daynum=((start/86400.0)-3651.0);
				PreCalc(daynum);
				Calc();

				if (Decayed(indx,daynum)==0)
					fprintf(fd,"%ld %s %4d %4d %4d %4d %4d %6ld %6ld %c\n",start,Daynum2String(daynum),iel,iaz,ma256,isplat,isplong,irk,rv,FindSun(daynum,0));
				break;
			}

			else
			{
				/* Display a whole list */
				for (count=start; count<=end; count+=step)
				{
					daynum=((count/86400.0)-3651.0);
					PreCalc(daynum);
					Calc();

					if (Decayed(indx,daynum)==0)
						fprintf(fd,"%ld %s %4d %4d %4d %4d %4d %6ld %6ld %c\n",count,Daynum2String(daynum),iel,iaz,ma256,isplat,isplong,irk,rv,FindSun(daynum,0));
				}
				break;
			}
		}
	}

	if (outputfile[0])
		fclose(fd);

	return 0;
}

int QuickPredict(string, outputfile)
char *string, *outputfile;
{
	int x, y, z, lastel=0;
	long start, now;
	char satname[50], startstr[20];
	time_t t;
	FILE *fd;

	if (outputfile[0])
		fd=fopen(outputfile,"w");
	else
		fd=stdout;

	startstr[0]=0;

	ReadDataFiles();

	for (x=0; x<48 && string[x]!=0 && string[x]!='\n'; x++)
		satname[x]=string[x];

	satname[x]=0;
	x++;

	for (y=0; string[x+y]!=0 && string[x+y]!='\n'; y++)
		startstr[y]=string[x+y];

	startstr[y]=0;
	y++;

	/* Do a simple search for the matching satellite name */

	for (z=0; z<24; z++)
	{
		if ((strcmp(sat[z].name,satname)==0) || (atol(satname)==sat[z].catnum))
		{
			start=atol(startstr);
			indx=z;

			t=time(NULL);
			now=(long)t;

			if (start==0)
				start=now;

			if ((start>=now-31557600) && (start<=now+31557600))
			{
				/* Start must within one year of now */
				daynum=((start/86400.0)-3651.0);
				PreCalc(daynum);
				Calc();

				if (AosHappens(indx) && Geostationary(indx)==0 && Decayed(indx,daynum)==0)
				{
					/* Make Predictions */
					daynum=FindAOS();

					/* Display the pass */

					while (iel>=0)
					{
						fprintf(fd,"%.0f %s %4d %4d %4d %4d %4d %6ld %6ld %c\n",floor(86400.0*(3651.0+daynum)),Daynum2String(daynum),iel,iaz,ma256,isplat,isplong,irk,rv,FindSun(daynum,0));
						lastel=iel;
						daynum+=cos((elevation-1.0)*deg2rad)*sqrt(ak)/25000.0;
						Calc();
					}

					if (lastel!=0)
					{
						daynum=FindLOS();
						Calc();
						fprintf(fd,"%.0f %s %4d %4d %4d %4d %4d %6ld %6ld %c\n",floor(86400.0*(3651.0+daynum)),Daynum2String(daynum),iel,iaz,ma256,isplat,isplong,irk,rv,FindSun(daynum,0));
					}
				}
				break;
			}

		}
	}

	if (outputfile[0])
		fclose(fd);

	return 0;
}

int main(argc,argv)
char argc, *argv[];
{
	pthread_t thread;
	int x, y, z, key=0;
	char updatefile[80], quickfind=0, quickpredict=0,
	     quickstring[40], outputfile[42], newuser=0;
	FILE *qth, *tle;

	/* Set up translation table for computing TLE checksums */

	for (x=0; x<=255; val[x]=0, x++);
	for (x='0'; x<='9'; val[x]=x-'0', x++);

	val['-']=1;

	updatefile[0]=0;
	quickstring[0]=0;
	outputfile[0]=0;
	temp[0]=0;
	tlefile[0]=0;
	qthfile[0]=0;
	netport[0]=0;
		
	y=argc-1;

	/* Set up default serial port (/dev/null) for
	   sending out tracking data with -a option */

	strcpy(serial_port,"/dev/null\0");

	/* Set up default qth and tle filenames and paths */

	sprintf(qthfile,"%s/.predict/predict.qth",getenv("HOME"));
	sprintf(tlefile,"%s/.predict/predict.tle",getenv("HOME"));
	
	/* If these files do not exist, then we have a new user */

	qth=fopen(qthfile,"r");
	tle=fopen(tlefile,"r");

	if (qth==NULL)
		newuser=1;
	else
		fclose(qth);
		
	if (tle==NULL)
		newuser++;
	else
		fclose(tle);

	/* Scan for optional command line arguments if not a new user */

	if (newuser==0)
	{
		for (x=1; x<=y; x++)
		{
			if (strcmp(argv[x],"-f")==0)
			{
				quickfind=1;
				z=x+1;
				while (z<=y && argv[z][0] && argv[z][0]!='-')
				{
					if ((strlen(quickstring)+strlen(argv[z]))<37)
					{
						strncat(quickstring,argv[z],15);
						strcat(quickstring,"\n");
						z++;
					}
				}
				z--;
			}

			if (strcmp(argv[x],"-p")==0)
			{
				quickpredict=1;
				z=x+1;
				while (z<=y && argv[z][0] && argv[z][0]!='-')
				{
					if ((strlen(quickstring)+strlen(argv[z]))<37)
					{
						strncat(quickstring,argv[z],15);
						strcat(quickstring,"\n");
						z++;
					}
				}
				z--;
			}
			if (strcmp(argv[x],"-u")==0)
			{
				z=x+1;
				while (z<=y && argv[z][0] && argv[z][0]!='-')
				{
					if ((strlen(updatefile)+strlen(argv[z]))<75)
					{
						strncat(updatefile,argv[z],75);
						strcat(updatefile,"\n");
						z++;
					}
				}
				z--;	
			}

			if (strcmp(argv[x],"-t")==0)
			{
				z=x+1;
				if (z<=y && argv[z][0] && argv[z][0]!='-')
					strncpy(tlefile,argv[z],48);
			}

			if (strcmp(argv[x],"-q")==0)
			{
				z=x+1;
				if (z<=y && argv[z][0] && argv[z][0]!='-')
					strncpy(qthfile,argv[z],48);
			}

			if (strcmp(argv[x],"-a")==0)
			{
				z=x+1;
				if (z<=y && argv[z][0] && argv[z][0]!='-')
				strncpy(serial_port,argv[z],13);
			}

			if (strcmp(argv[x],"-o")==0)
			{
				z=x+1;
				if (z<=y && argv[z][0] && argv[z][0]!='-')
				strncpy(outputfile,argv[z],40);
			}

                        if (strcmp(argv[x],"-n")==0)
                        {
                                z=x+1;
				if (z<=y && argv[z][0] && argv[z][0]!='-')
                                strncpy(netport,argv[z],5);
                        }

			if (strcmp(argv[x],"-s")==0)
				socket_flag=1;
		}

		x=ReadDataFiles();
	}
	else
		x=3;

	if (x==3)
	{
		if (updatefile[0])
		{
			y=0;
			z=0;
			temp[0]=0;

			while (updatefile[y]!=0)
			{
				while (updatefile[y]!='\n' && updatefile[y]!=0 && y<79)
				{
					temp[z]=updatefile[y];
					z++;
					y++;
				}

				temp[z]=0;

				if (temp[0])
				{
					AutoUpdate(temp);
					temp[0]=0;
					z=0;
					y++;
				}
			}
			exit(0);
		}

		if (quickfind)
			exit(QuickFind(quickstring,outputfile));

		if (quickpredict)
			exit(QuickPredict(quickstring,outputfile));

		initscr();
		start_color();
		cbreak();
		noecho();
		scrollok(stdscr,TRUE);
		curs_set(0);

		init_pair(1,COLOR_WHITE,COLOR_BLACK);
		init_pair(2,COLOR_WHITE,COLOR_BLUE);
		init_pair(3,COLOR_YELLOW,COLOR_BLUE);
		init_pair(4,COLOR_CYAN,COLOR_BLUE);
		init_pair(5,COLOR_WHITE,COLOR_RED);
		init_pair(6,COLOR_RED,COLOR_WHITE);
		init_pair(7,COLOR_CYAN,COLOR_RED);

		if (newuser)
		{
			NewUser();
			ReadDataFiles();
			QthEdit();
		}

		/* Socket activated here.  Remember that
		   the socket data is updated only when
		   running in the real-time tracking modes. */

		if (socket_flag)
		{
			pthread_create(&thread,NULL,(void *)socket_server,NULL);
			bkgdset(COLOR_PAIR(3));
			MultiTrack();
		}

		/* Open serial port to send data to
		   the antenna tracker if present */

		if (strcmp("/dev/null\0",serial_port))
		{
			antfd=open(serial_port, O_RDWR | O_NOCTTY | O_NDELAY);

			if (antfd!=-1)
				fcntl(antfd, F_SETFL, 0);
			else
			{
				bailout("Unable To Open Antenna Port");
				exit(-1);
			}
		}
	
		MainMenu();

		do
		{	
			key=getch();

			switch (tolower(key))
			{
				case 'p':
				case 'v':
					Print("",0);
					PrintVisible("");
					indx=Select();

					if (indx!=-1 && sat[indx].meanmo!=0.0 && Decayed(indx,0.0)==0)
						Predict(tolower(key));

					MainMenu();
					break;

				case 'u':
					AutoUpdate("");
					MainMenu();
					break;

				case 'e':
					KepEdit();
					MainMenu();
					break;

				case 'd':
					ShowOrbitData();
					MainMenu();
					break;

				case 'g':
					QthEdit();
					MainMenu();
					break;

				case 't':
					indx=Select();

					if (indx!=-1 && sat[indx].meanmo!=0.0 && Decayed(indx,0.0)==0)
						SingleTrack(indx,key);

					MainMenu();
					break;

				case 'm':
					MultiTrack();
					MainMenu();
					break;

				case 'i':
					ProgramInfo();
					MainMenu();
					break;

				case 's':
					indx=Select();
					if (indx!=-1 && sat[indx].meanmo!=0.0 && Decayed(indx,0.0)==0)
					{
						Print("",0);
						Illumination();
					}
					MainMenu();
					break;
			}

		} while (key!='q' && key!=27 && key!='Q');

		curs_set(1);	
		bkgdset(COLOR_PAIR(1));
		clear();
		refresh();
		endwin();
	}

	else
	{
		/* "Houston, we have a problem..." */

		printf("\n%c",7);

		if (x^1)
			printf("*** ERROR!  Your QTH file \"%s\" could not be loaded!\n",qthfile);

		if (x^2)
			printf("*** ERROR!  Your TLE file \"%s\" could not be loaded!\n",tlefile);

		printf("\n");
	}

	exit(0);
}
