/******************************************************/
/**       PREDICT installation program by KD2BD      **/
/**  This program is compiled and executed through   **/
/**     the "configure" script in this directory.    **/
/**  Created: Oct 1999 --- Last update: 01-Jan-2002  **/
/******************************************************/

#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>

int main()
{
	char version[10], pwd[80], line[255], ans;
	int cc, dsp=-1;
	FILE *infile, *outfile;

	initscr();
	start_color();
	cbreak();
	scrollok(stdscr,TRUE);

	init_pair(1,COLOR_WHITE,COLOR_BLUE);
	init_pair(2,COLOR_RED,COLOR_BLUE);
	init_pair(3,COLOR_CYAN,COLOR_BLUE);
	init_pair(4,COLOR_GREEN,COLOR_BLUE);
	init_pair(5,COLOR_YELLOW,COLOR_BLUE);
	init_pair(6,COLOR_RED,COLOR_WHITE);

	cc=0;
	getcwd(pwd,79);
	dsp=open("/dev/dsp",O_WRONLY);

	if (dsp!=-1)
		close(dsp);

	infile=fopen(".version","r");
	fscanf(infile,"%s",version);
	fclose(infile);

	bkgdset(COLOR_PAIR(1)|A_BOLD);
	clear();
	refresh();
	attrset(COLOR_PAIR(6)|A_REVERSE|A_BOLD);
	mvprintw(3,18,"                                           ");
	mvprintw(4,18,"         --== PREDICT  v%s ==--         ",version);
	mvprintw(5,18,"           By John A. Magliacane           ");
	mvprintw(6,18,"   KD2BD Software -- Copyright 1991-2002   ");
	mvprintw(7,18,"                                           ");

	attrset(COLOR_PAIR(5)|A_BOLD);

	mvprintw(10,2,"PREDICT is a satellite tracking and orbital prediction program written for\n");
	printw("  the Linux operating system by John A. Magliacane, KD2BD (kd2bd@amsat.org).\n");
	printw("  PREDICT is free software.  You can redistribute it and/or modify it under\n");
	printw("  the terms of the GNU General Public License as published by the Free\n");
	printw("  Software Foundation, either version 2 of the License or any later version.\n\n");
	printw("  PREDICT is distributed in the hope that it will useful, but WITHOUT ANY\n");
	printw("  WARRANTY, without even the implied warranty of MERCHANTABILITY or FITNESS\n");
	printw("  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more\n");
	printw("  details.\n\n");

	attrset(COLOR_PAIR(1)|A_BOLD);
	printw("  Do you accept these conditions and wish to install this software? [Y/N] ");
	refresh();

	do
	{
		ans=getch();

	} while (ans!='n' && ans!='N' && ans!='y' && ans!='Y' && ans!=27);


	if (ans=='y' || ans=='Y')
	{
		clear();
		curs_set(0);
		attrset(COLOR_PAIR(6)|A_REVERSE|A_BOLD);
		mvprintw(3,18,"                                           ");
		mvprintw(4,18,"         --== PREDICT  v%s ==--         ",version);
		mvprintw(5,18,"           By John A. Magliacane           ");
		mvprintw(6,18,"   KD2BD Software -- Copyright 1991-2002   ");
		mvprintw(7,18,"                                           ");

		attrset(COLOR_PAIR(4)|A_BOLD);
		printw("\n\n\n\n  PREDICT appears to be installed under %s/\n",pwd);

		if (dsp!=-1)
			printw("  An audio device was found at /dev/dsp");
		else
			printw("  No soundcard was found in your system... Bummer!");

		printw("\n  Header files are now being created...\n");
		attrset(COLOR_PAIR(3)|A_BOLD);
		refresh();

		/* Write predict.h */

		outfile=fopen("predict.h","w");

		fprintf(outfile,"char *predictpath={\"%s/\"}, ",pwd);

		if (dsp==-1)
			fprintf(outfile, "soundcard=0,");
		else
			fprintf(outfile, "soundcard=1,");

		fprintf(outfile, " *version={\"%s\"};\n",version);

		fclose(outfile);

		printw("\n  Compiling PREDICT...\n");
		refresh();

		cc=system("cc -Wall -O3 -s -fomit-frame-pointer -I/usr/include/ncurses predict.c -lm -lncurses -lpthread -o predict");


		/* Create vocalizer.h */

		outfile=fopen("vocalizer/vocalizer.h","w");

		fprintf(outfile,"char *path={\"%s/vocalizer/\"};\n",pwd);

		fclose(outfile);

		if (dsp!=-1 && cc==0)
		{
			attrset(COLOR_PAIR(3)|A_BOLD);
			printw("  Compiling audio capabilities...\n");
			refresh();
			system("cc -Wall -O3 -s -fomit-frame-pointer vocalizer/vocalizer.c -o vocalizer/vocalizer");
		}
	
		/* Now install the programs and man page by creating symlinks
		   between the newly created executables and /usr/local/bin */

		if (cc==0)
		{
			attrset(COLOR_PAIR(3)|A_BOLD);
			printw("  Installing PREDICT...\n\n");
			unlink("/usr/local/bin/predict");
			sprintf(line,"%s/predict",pwd);
			symlink(line,"/usr/local/bin/predict");
			unlink("/usr/local/bin/xpredict");
			sprintf(line,"%s/xpredict",pwd);
			symlink(line,"/usr/local/bin/xpredict");
			unlink("/usr/local/man/man1/predict.1");
			sprintf(line,"%s/docs/man/predict.1",pwd);
			symlink(line,"/usr/local/man/man1/predict.1");
			attrset(COLOR_PAIR(5)|A_BOLD);
			printw("  Don't forget to check out the new graphical\n");
			printw("  client applications under the 'clients' directory!");
			attrset(COLOR_PAIR(1)|A_BOLD);
			printw("\n\n  Done!  Happy Tracking!  :-)");
		}

		else
		{
			attrset(COLOR_PAIR(2)|A_BOLD);
			printw("  *** Compilation failed.  Program not installed.  :-(");
			beep();
		}
	}
	refresh();
	unlink("installer");
	curs_set(1);	
	refresh();
	endwin();
	exit(0);
}
