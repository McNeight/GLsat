/****************************************************************************
*                  VOCALIZER: A speech articulation program                 *
*		Copyright John A. Magliacane, KD2BD 1999-2000               *
*	   		  Last update: 08-Jun-2000                          *
*****************************************************************************
* This program is used by PREDICT to annunciate satellite coordinates       *
* through the system's soundcard (/dev/dsp) using stored 8 kHz unsigned     *
* linear stored speech samples.  It is called as a background process.      *
*****************************************************************************
*                                                                           *
* This program is free software; you can redistribute it and/or modify it   *
* under the terms of the GNU General Public License as published by the     *
* Free Software Foundation; either version 2 of the License or any later    *
* version.                                                                  *
*                                                                           *
* This program is distributed in the hope that it will useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License     *
* for more details.                                                         *
*                                                                           *
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include "vocalizer.h"

int dsp;

void saynumber(numstr)
char *numstr;
{
	int x;
	FILE *fd;
	char filename[80];
	unsigned char byte;

	for (x=0; numstr[x]!=0; x++)
	{
		sprintf(filename,"%s%c",path,numstr[x]);
		fd=fopen(filename,"rb");

		byte=fgetc(fd);

		while (feof(fd)==0)
		{
			write(dsp,&byte,1);
			byte=fgetc(fd);
		}

		fclose(fd);
	}
}

void saydirection(dir)
char dir;
{
	unsigned char byte;
	char filenpath[80];
	FILE *fd;

	fd=NULL;

	if (dir=='+')
	{
		sprintf(filenpath,"%sapproaching",path);
		fd=fopen(filenpath,"rb");
	}

	if (dir=='-')
	{
		sprintf(filenpath,"%sreceding",path);
		fd=fopen(filenpath,"rb");
	}

	if (fd!=NULL)
	{
		byte=fgetc(fd);

		while (feof(fd)==0)
		{
			write(dsp,&byte,1);
			byte=fgetc(fd);
		}

		fclose(fd);
	}
}

void sayfile(name)
char *name;
{
	FILE *fd;
	unsigned byte;
	char filename[80];

	sprintf(filename,"%s%s",path,name);
	fd=fopen(filename,"rb");

	if (fd!=NULL)
	{
		byte=fgetc(fd);

		while (feof(fd)==0)
		{
			write(dsp,&byte,1);
			byte=fgetc(fd);
		}

		fclose(fd);
	}
}

int main(argc,argv)
int argc;
char *argv[];
{
	int c;

	dsp=open("/dev/dsp",O_WRONLY);

	if (argc>=3 && dsp!=-1)
	{
		/* Set up soundcard to read 8 bit,
		   single channel, 8 kHz sampled data */

		c=8;
		ioctl(dsp,SOUND_PCM_WRITE_BITS,&c);
		c=1;
		ioctl(dsp,SOUND_PCM_WRITE_CHANNELS,&c);
		c=8000;
		ioctl(dsp,SOUND_PCM_WRITE_RATE,&c);

		/* Start talking! */

		sayfile("intro");

		saynumber(argv[1]);
		sayfile("azimuth");

		saynumber(argv[2]);
		sayfile("elevation");

		if (argc==4)
			saydirection(argv[3][0]);

		close(dsp);
		exit(0);
	}

	else
		exit(-1);
}
