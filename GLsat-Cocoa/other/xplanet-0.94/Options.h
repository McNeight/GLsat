/****************************************************************************
    Xplanet 0.94 - render an image of a planet into an X window
    Copyright (C) 2002 Hari Nair <hari@alumni.caltech.edu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#ifndef OPTIONS_H
#define OPTIONS_H

#include <list>
#include <string>
using namespace std;

#include <sys/time.h>

#include "checkfuncs.h"

#ifdef HAVE_LIBX11

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#else

#include "ParseGeom.h"
typedef unsigned long Window;

#endif /* HAVE_LIBX11 */

#include "keywords.h"

class Options 
{
 public:
    string          background;
    int             blend;
    body_name       body;
    bool            center;
    int             centx;
    int             centy;
    string          cloud;
    double          cloud_gamma;
    bool            cloud_shade;
    int             cloud_threshold;
    string          color;
    string          day;
    int             debug;
    double          dellat;
    double          dellon;
    string          dem;
    double          demscale;
    int             display;
    string          font;
    list<string>    fontdir;
    int             fontsize;
    int             fullscreen;
    int             fuzz;
    bool            gmtlabel;
    list<string>    greatarcfile;
    int             grid1;
    int             grid2;
    bool            label;
    string          label_format;
    int             label_mask;
    bool            label_name;
    bool            label_rot;
    int             label_x;
    int             label_y;
    double          localtime;
    bool            mapbounds;
    double          map_ulx, map_uly, map_lrx, map_lry;
    list<string>    mapdir;
    string          markerbounds;
    list<string>    markerfile;
    bool            markers;
    bool            print_coords;
    int             projection;
    string          night;
    int             north;
    string          output;
    int             position;
    int             quality;
    double          radius;
    double          range;
    double          rotate;
    list<string>    satellitefile;
    int             sattrackid;
    double          shade;
    int             size;
    double          spacing;
    string          specular_file;
    int             ssec;
    double          starfreq;
    int             swap;
    int             terminator;
    bool            transpng;
    bool            truetype;
    struct timeval  time;
    string          title;
    int             trans;
    bool            vroot;
    unsigned int    window_width;
    unsigned int    window_height;
    int             window_x;
    int             window_y;
    int             window_mask;
    Window          XID;

    Options();
    void processArgs(int argc, char **argv, int &glutargc, char **glutargv);

    double getObsLat() { return(obslat); }
    void setObsLat(const double lat) { obslat = lat; }
    double getObsLon() { return(obslon); }
    void setObsLon(const double lon) { obslon = lon; }

    void setBody();
    void setPosition();

 private:
    void show_help();

    double          obslat, obslon;
}; 

const char* const body_string[RANDOMBODY] = {"mercury", "venus", "earth",
                                             "moon", "mars", "jupiter", 
                                             "io", "europa", "ganymede",
                                             "callisto", "saturn", "titan",
                                             "uranus", "neptune", "pluto"};
extern Options opts;

#endif
