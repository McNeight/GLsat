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

// This has to come before the other includes on linux, otherwise
// strptime won't be found (?)
#include "checkfuncs.h" 

#include <clocale>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>
using namespace std;

#ifdef HAVE_LIBX11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#else
extern "C" {
    extern int 
    XParseGeometry(const char *string, int *x, int *y, unsigned int *width, 
                   unsigned int *height);
}
#endif

#include "auxfiles.h"
#include "Options.h"
#include "Planet.h"
#include "util.h"
#include "xp_getopt.h"

#include "libannotate/libannotate.h"
#include "libprojection/libprojection.h"

Options opts;

Options::Options() 
{
    background       = "";
    blend            = 0;
    body             = EARTH;
    center           = false;
    centx            = -1;
    centy            = -1;
    cloud            = "";
    cloud_gamma      = 1;
    cloud_shade      = false;
    cloud_threshold  = 90;
    color            = "";
    day              = "";
    debug            = 0;
    dellon           = 0;
    dellat           = 0;
    dem              = "";
    demscale         = 0.05;
    display          = ROOT;
    font             = "";
    fontsize         = 12;
    truetype         = false;
    fullscreen       = 0;
    fuzz             = 6;
    gmtlabel         = false;
    grid1            = 0;
    grid2            = 15;
    label            = 0;
    label_format     = "%a %d %b %Y %H:%M %Z";
    label_mask       = XNegative;
    label_name       = false;
    label_rot        = false;
    label_x          = -15;
    label_y          = 15;
    localtime        = 999;
    mapbounds        = false;
    markerbounds     = "";
    markers          = false;
    north            = 0;
    night            = "";
    obslat           = 0;
    obslon           = 0;
    output           = "";
    position         = LATLON;
    print_coords     = false;
    projection       = RECTANGULAR;
    quality          = 80;
    radius           = 50;
    range            = 1000;
    rotate           = 0;
    sattrackid       = -1;
    shade            = 0.3;
    spacing          = 0.1;
    specular_file    = "";
    ssec             = 0;
    starfreq         = 1E-3;
    swap             = 0;
    terminator       = 0;
    gettimeofday(&time, NULL);
    trans            = 1;
    vroot            = false;
    window_width     = 512;
    window_height    = 512;
    window_x         = 0;
    window_y         = 0;
    window_mask      = 0;
    XID              = 0;

    setlocale(LC_ALL, "");
    setlocale(LC_NUMERIC, "C");

    // These formats by Tomohiro Kubota <kubota@debian.org>
    char *locale = setlocale(LC_TIME, NULL);
    if (locale != NULL) {
        if (strncmp(locale, "ja", 2)==0)
            // Taken from GNU C library's strftime "%c" format
            // in EUC-JP encoding
            // %S is omitted, %a is added.
            label_format = "%Y\xc7\xaf%b%d\xc6\xfc (%a) %H\xbb\xfe%M\xca\xac %Z";
        else if (strncmp(locale, "ko", 2)==0)
            // Taken from GNU C library's ko_KR locale
            // %S is omitted.
            label_format = "%Y. %m. %d. (%a) %H:%M %Z";
        else if (strncmp(locale, "zh_CN", 5)==0)
            // Taken from strftime "%c" format, in EUC-CN (GB2312) encoding
            // %S is omitted.
            label_format = "%Y\xc4\xea%m\xd4\xc2%d\xc8\xd5 (%A) %H\xca\xb1%M\xb7\xd6 %Z";
        else if (strncmp(locale, "zh", 2)==0)
            // for zh_TW and zh_HK
            // Taken from strftime "%c" format, in Big5 encoding
            // %S is omitted.
            label_format = "%Y\xa6\x7e%m\xa4\xeb%d\xa4\xe9 (%A) %H\xae\xc9%M\xa4\xc0 %Z";
    }
}
  
void 
Options::show_help()
{
    cout << "valid options:\n";
    cout << "\t-ani[mate]\n";
    cout << "\t-ba[ckground]         image_file (string)\n";
    cout << "\t-bl[end]\n";
    cout << "\t-bo[dy]               body       (string)\n";
    cout << "\t-ce[nter]             x,y        (int, int)\n";
    cout << "\t-cloud_g[amma]        gamma      (float)\n";
    cout << "\t-cloud_i[mage]        image_file (string)\n";
    cout << "\t-cloud_sh[ade]        percent    (int)\n";
    cout << "\t-cloud_ss[ec]         image_file (string)\n";
    cout << "\t-cloud_t[hreshold]    threshold  (int)\n";
    cout << "\t-co[lor]              colorname  (string)\n";
    cout << "\t-dat[e]               string     (string)\n";
    cout << "\t-day[side]\n";
    cout << "\t-demf[ile]            demfile    (string)\n";
    cout << "\t-dems[cale]           scale      (float)\n";
    cout << "\t-e[arthside]\n";
    cout << "\t-font                 fontname   (string)\n";
    cout << "\t-fontd[ir]            directory  (string)\n";
    cout << "\t-fonts[ize]           size       (int)\n";
    cout << "\t-ful[lscreen]\n";
    cout << "\t-fuz[z]               pixels     (int)\n";
    cout << "\t-geometry             string     (string)\n";
    cout << "\t-gmtlabel\n";
    cout << "\t-greatarcfile         filename   (string)\n";
    cout << "\t-grid\n";
    cout << "\t-grid1                spacing    (int)\n";
    cout << "\t-grid2                spacing    (int)\n";
    cout << "\t-he[lp]\n";
    cout << "\t-i[mage]              image_file (string)\n";
    cout << "\t-label\n";
    cout << "\t-labelname\n";
    cout << "\t-labelp[os]           string     (string)\n";
    cout << "\t-labelrot\n";
    cout << "\t-lat[itude]           degrees    (float) \n";
    cout << "\t-loc[altime]          time       (float) \n";
    cout << "\t-lon[gitude]          degrees    (float) \n";
    cout << "\t-mapb[ounds]          ulx, uly, lrx, rly (float, float, float, float)\n";
    cout << "\t-mapd[ir]             directory  (string)\n";
    cout << "\t-markerb[ounds]       filename   (string)\n";
    cout << "\t-markerf[ile]         filename   (string)\n";
    cout << "\t-markers\n";
    cout << "\t-moo[nside]\n";
    cout << "\t-night_[image]        image_file (string)\n";
    cout << "\t-nights[ide]\n";
    cout << "\t-nor[th]                  type       (string)\n";
    cout << "\t-not[ransparency]\n";
    cout << "\t-ou[tput]             filename   (string)\n";
    cout << "\t-pri[nt_coords]\n";
    cout << "\t-pro[jection]             type       (string)\n";
    cout << "\t-q[uality]             quality    (int)\n";
    cout << "\t-rad[ius]              percent    (int)\n";
    cout << "\t-rand[om]\n";
    cout << "\t-rang[e]                 range      (float) \n";
    cout << "\t-roo[t]\n";
    cout << "\t-rot[ate]              degrees    (float)\n";
    cout << "\t-satf[ile]            filename   (string)\n";
    cout << "\t-satt[rackid]               id   (string)\n";
    cout << "\t-sh[ade]               percent    (int)\n";
    cout << "\t-spa[cing]            spacing    (float)\n";
    cout << "\t-spe[cular_file]      filename   (string)\n";
    cout << "\t-st[arfreq]          frequency  (float)\n";
    cout << "\t-su[nrel]      del_lon,del_lat  (float, float)\n";
    cout << "\t-sw[ap]\n";
    cout << "\t-te[rminator]       terminator   (string)\n";
    cout << "\t-tra[nspng]           filename   (string)\n";
    cout << "\t-tru[etype]\n";
    cout << "\t-ve[rsion]\n";
    cout << "\t-vr[oot]\n";
    cout << "\t-w[indow]\n";
    cout << "\t-x[screensaver]\n";
    cout << "\nCharacters inside the square braces are optional.\n";
    exit (EXIT_SUCCESS);
}

void 
Options::processArgs(int argc, char **argv, int &glutargc, char **glutargv)
{
    static struct option long_options[] =
        {
            {"animate",         no_argument,       NULL, ANIMATE},
            {"background",      required_argument, NULL, BACKGROUND},
            {"blend",           no_argument,       NULL, BLEND},
            {"body",            required_argument, NULL, BODY},
            {"center",          required_argument, NULL, CENTER},
            {"cloud_gamma",     required_argument, NULL, CLOUD_GAMMA},
            {"cloud_image",     required_argument, NULL, CLOUD_IMAGE},
            {"cloud_shade",     required_argument, NULL, CLOUD_SHADE},
            {"cloud_ssec",      required_argument, NULL, CLOUD_SSEC},
            {"cloud_threshold", required_argument, NULL, CLOUD_THRESHOLD},
            {"color",           required_argument, NULL, COLOR},
            {"date",            required_argument, NULL, DATE},
            {"dayside",         no_argument,       NULL, DAYSIDE},
            {"debug",           no_argument,       NULL, DEBUG},
            {"demfile",         required_argument, NULL, DEMFILE},
            {"demscale",        required_argument, NULL, DEMSCALE},
            {"earthside",       no_argument,       NULL, EARTHSIDE},
            {"font",            required_argument, NULL, FONT},
            {"fontdir",         required_argument, NULL, FONTDIR},
            {"fontsize",        required_argument, NULL, FONTSIZE},
            {"fullscreen",      no_argument,       NULL, FULLSCREEN},
            {"fuzz",            required_argument, NULL, FUZZ},
            {"geometry",        required_argument, NULL, GEOMETRY},
            {"gmtlabel",        no_argument,       NULL, GMTLABEL},
            {"greatarcfile",    required_argument, NULL, GREATARCFILE},
            {"grid",            no_argument,       NULL, GRID},
            {"grid1",           required_argument, NULL, GRID1},
            {"grid2",           required_argument, NULL, GRID2},
            {"help",            no_argument,       NULL, HELP},
            {"image",           required_argument, NULL, IMAGE},
            {"label",           no_argument,       NULL, LABEL},
            {"labelformat",     required_argument, NULL, LABELFMT},
            {"labelname",       no_argument,       NULL, LABELNAME},
            {"labelpos",        required_argument, NULL, LABELPOS},
            {"labelrot",        no_argument,       NULL, LABELROT},
            {"latitude",        required_argument, NULL, LATITUDE},
            {"localtime",       required_argument, NULL, LOCALTIME},
            {"longitude",       required_argument, NULL, LONGITUDE},
            {"mapbounds",       required_argument, NULL, MAPBOUNDS},
            {"mapdir",          required_argument, NULL, MAPDIR},
            {"markerbounds",    required_argument, NULL, MARKERBOUNDS},
            {"markerfile",      required_argument, NULL, MARKERFILE},
            {"markers",         no_argument,       NULL, MARKERS},
            {"moonside",        no_argument,       NULL, MOONSIDE},
            {"night_image",     required_argument, NULL, NIGHT_IMAGE},
            {"nightside",       no_argument,       NULL, NIGHTSIDE},
            {"north",           required_argument, NULL, NORTH},
            {"notransparency",  no_argument,       NULL, NOTRANSPARENCY},
            {"output",          required_argument, NULL, OUTPUT},
	    {"print_coords",    no_argument,       NULL, PRINT_COORDS},
            {"projection",      required_argument, NULL, PROJECTION},
            {"quality",         required_argument, NULL, QUALITY},
            {"radius",          required_argument, NULL, RADIUS},
            {"random",          no_argument,       NULL, RANDOM},
            {"range",           required_argument, NULL, RANGE},
            {"root",            no_argument,       NULL, ROOT},
            {"rotate",          required_argument, NULL, ROTATE},
            {"satfile",         required_argument, NULL, SATFILE},
            {"sattrackid",      required_argument, NULL, SATTRACKID},
            {"shade",           required_argument, NULL, SHADE},
	    {"spacing",         required_argument, NULL, SPACING},
	    {"specular_file",   required_argument, NULL, SPECULAR_FILE},
            {"starfreq",        required_argument, NULL, STARFREQ},
            {"sunrel",          required_argument, NULL, SUNREL},
            {"swap",            no_argument,       NULL, SWAP},
            {"terminator",      required_argument, NULL, TERMINATOR},
            {"transpng",        required_argument, NULL, TRANSPNG},
            {"truetype",        no_argument,       NULL, TRUETYPE},
            {"version",         no_argument,       NULL, VERSION},
            {"vroot",           no_argument,       NULL, VROOT},
            {"window",          no_argument,       NULL, WINDOW},
            {"XID",             required_argument, NULL, EXISTINGWINDOW},
            {"xscreensaver",    no_argument,       NULL, VROOT},
            {NULL,              0,                 NULL, 0}
        };

    int this_option;
    int option_index = 0;
    int default_north = BODY;

    srandom((unsigned int) time.tv_sec);
    while((this_option = xp_getopt_long_only(argc, argv, "+", long_options, 
                                             &option_index)) >= 0)
    {
        switch (this_option)
        {
        case ANIMATE:
#ifdef HAVE_ANIMATION_LIB
            display = ANIMATE;
            projection = RECTANGULAR;
#else
            cerr << "This binary was not built with the GL/GLUT "
                 << "libraries.\n";
            cerr << "The animate option is not valid.\n";
            exit (EXIT_FAILURE);
#endif
            break;
        case BACKGROUND:
            background = optarg;
            if (background.find('.') == string::npos)
                background += defaultmapext;
            if (projection == RECTANGULAR) projection = ORTHOGRAPHIC;
            break;
        case BLEND:
            blend = 1;
            break;
        case BODY:
        {
            char *lowercase = optarg;
            char *ptr = optarg;
            while (*ptr != '\0') *ptr++ = tolower(*optarg++);
            if (strncmp(lowercase, body_string[MERCURY], 2) == 0)
                body = MERCURY;
            else if (strncmp(lowercase, body_string[VENUS], 1) == 0)
                body = VENUS;
            else if (strncmp(lowercase, body_string[EARTH], 2) == 0)
                body = EARTH;
            else if (strncmp(lowercase, body_string[MOON], 2) == 0)
                body = MOON;
            else if (strncmp(lowercase, body_string[MARS], 2) == 0)
                body = MARS;
            else if (strncmp(lowercase, body_string[JUPITER], 1) == 0)
                body = JUPITER;
            else if (strncmp(lowercase, body_string[IO], 1) == 0)
                body = IO;
            else if (strncmp(lowercase, body_string[EUROPA], 2) == 0)
                body = EUROPA;
            else if (strncmp(lowercase, body_string[GANYMEDE], 1) == 0)
                body = GANYMEDE;
            else if (strncmp(lowercase, body_string[CALLISTO], 1) == 0)
                body = CALLISTO;
            else if (strncmp(lowercase, body_string[SATURN], 1) == 0)
                body = SATURN;
            else if (strncmp(lowercase, body_string[TITAN], 1) == 0)
                body = TITAN;
            else if (strncmp(lowercase, body_string[URANUS], 1) == 0)
                body = URANUS;
            else if (strncmp(lowercase, body_string[NEPTUNE], 1) == 0)
                body = NEPTUNE;
            else if (strncmp(lowercase, body_string[PLUTO], 1) == 0)
                body = PLUTO;
            else if (strncmp(lowercase, "random", 1) == 0)
                body = RANDOMBODY;
            else
            {
                cerr << "Unknown body specified, using earth.\n";
                body = EARTH;
            }
        }
        break;
        case CENTER:
	    center = true;
            sscanf(optarg, "%d,%d", &centx, &centy);
            if (projection == RECTANGULAR) projection = ORTHOGRAPHIC;
            break;
        case CLOUD_GAMMA:
            sscanf(optarg, "%lf", &cloud_gamma);
            break;
        case CLOUD_IMAGE:
        case CLOUD_SSEC:
            cloud = optarg;
            if (cloud.find('.') == string::npos)
                cloud += defaultmapext;
            if (this_option == CLOUD_SSEC) ssec = 1;
            break;
        case CLOUD_SHADE:
        case SHADE:
        {
            int s;
            sscanf(optarg, "%d", &s);
            if (s < 0) 
                s = 0;
            else if (s > 100) 
                s = 100;
            shade = s/100.;
            if (this_option == CLOUD_SHADE) cloud_shade = true;
        }
        break;
        case CLOUD_THRESHOLD:
        {
            int t;
            sscanf(optarg, "%d", &t);
            if (t < 0) 
                t = 0;
            else if (t > 255) 
                t = 255;
            cloud_threshold = t;
        }
        break;
        case COLOR:
            color = optarg;
            break;
        case DATE:
        {
#ifdef HAVE_STRPTIME
            struct tm tm;
            tm.tm_sec = 0;
            tm.tm_min = 0;
            tm.tm_hour = 0;
            tm.tm_mday = 0;
            tm.tm_mon = 0;
            tm.tm_year = 0;
            tm.tm_isdst = 0;
            strptime(optarg, "%d %b %Y %T", &tm);
            time.tv_sec = mktime(&tm);
            if (tm.tm_isdst > 0) time.tv_sec -= 3600;
#else
            cerr << "This binary was built on a system without the strptime "
                 << "function.\n";
            cerr << "The -date option will be ignored.\n";
#endif
        }
        break;
        case DAYSIDE:
            title = "Dayside view";
            if (position != LATLON) 
                cerr << "Setting observer position to dayside\n";
            position = DAYSIDE;
            if (projection == RECTANGULAR) projection = ORTHOGRAPHIC;
            break;
        case DEBUG:
            debug = 1;
            break;
        case DEMFILE:
            dem = optarg;
            projection = ORTHOGRAPHIC;
            break;
        case DEMSCALE:
            sscanf(optarg, "%lf", &demscale);
            if (demscale > 1) 
            {
                cerr << "Setting demscale to 1\n";
                demscale = 1;
            }
            break;
	case PRINT_COORDS:
	    print_coords = true;
	    break;
        case EARTHSIDE:
            title = "View from earth";
            if (position != LATLON) 
                cerr << "Setting observer position to earthside\n";
            position = EARTHSIDE;
            if (projection == RECTANGULAR) projection = ORTHOGRAPHIC;
            break;
        case FONT:
            font = optarg;
            if (font.find(".ttf") != string::npos) 
            {
#ifdef HAVE_LIBFREETYPE
                truetype = true;
#else
                cerr << "Sorry, this binary was built without FreeType "
                     << "support.  The -font option will be ignored.\n";
#endif
            }
            break;
        case FONTDIR:
        {
#ifdef HAVE_LIBFREETYPE
            fontdir.push_back(optarg);
            truetype = true;
#else
            cerr << "Sorry, this binary was built without FreeType "
                 << "support. The -fontdir option will be ignored.\n";
#endif
        }
        break;
        case FONTSIZE:
        {
#ifdef HAVE_LIBFREETYPE
            int val;
            sscanf(optarg, "%d", &val);
            if (val > 0) fontsize = val;
            truetype = true;
#else
            cerr << "Sorry, this binary was built without FreeType "
                 << "support. The -fontsize option will be ignored.\n";
#endif
        }
        break;
        case FULLSCREEN:
            fullscreen = 1;
            if (display == ROOT) display = WINDOW;
            break;
        case FUZZ:
        {
            int val;
            sscanf(optarg, "%d", &val);
            if (val >= 0) fuzz = val;
        }
        break;
        case GEOMETRY:
            window_mask = XParseGeometry(optarg, &window_x, 
                                         &window_y, 
                                         &window_width, 
                                         &window_height);

            // geometry info is valid for ANIMATE, OUTPUT, or WINDOW
            if (display == ROOT) display = WINDOW;

            // this is in case we want to pass geometry info to glut
            glutargc = 3;
            glutargv[1] = strdup("-geometry");
            glutargv[2] = strdup(optarg);
            break;
        case GMTLABEL:
            gmtlabel = true;
            label = true;
            break;
        case GREATARCFILE:
            greatarcfile.push_back(optarg);
            break;
        case GRID:
            grid1 = 6;
            break;
        case GRID1:
            sscanf(optarg, "%d", &grid1);
            if (grid1 < 0 || grid1 > 90) 
            {
                cerr << "argument to -grid1 must be between 0 and 90\n";
                exit(EXIT_FAILURE);
            }
            break;
        case GRID2:
            sscanf(optarg, "%d", &grid2);
            if (grid2 < 0)
            {
                cerr << "argument to -grid2 must be positive\n";
                exit(EXIT_FAILURE);
            }
            if (grid1 == 0) grid1 = 6;
            break;
        case HELP:
            show_help();
            break;
        case IMAGE:
            day = optarg;
            if (day.find('.') == string::npos) day += defaultmapext;
            break;
        case LABEL:
            label = true;
            break;
        case LABELFMT:
            label = true;
            label_format = optarg;
            break;
        case LABELNAME:
            label = true;
            label_name = true;
            break;
        case LABELPOS:
        {
            unsigned int temp;
            label_mask = XParseGeometry(optarg, &label_x, 
                                        &label_y, &temp, 
                                        &temp);
            
            if (label_mask & (WidthValue | HeightValue))
                cerr << "width and height supplied in -labelpos will "
                     << "be ignored\n";
            label = true;
        }
        break;
        case LABELROT:
            label = true;
            label_rot = true;
            break;
        case LATITUDE:
            sscanf(optarg, "%lf", &obslat);
            if (obslat < -90) obslat = -90;
            if (obslat > 90) obslat = 90;
            obslat *= deg_to_rad;
            if (projection == RECTANGULAR) projection = ORTHOGRAPHIC;
            if (position == SUNREL) 
                dellat = 999;
            break;
        case LOCALTIME:
            sscanf(optarg, "%lf", &localtime);
            if (localtime < 0 || localtime > 24) 
            {
                localtime = fmod(localtime, 24.);
                if (localtime < 0) localtime += 24;
                cerr << "localtime set to " << localtime << endl;
            }
            break;
        case LONGITUDE:
            sscanf(optarg, "%lf", &obslon);
            obslon *= deg_to_rad;
            if (position == SUNREL) 
                dellon = 999;
            break;
	case MAPBOUNDS:
	    mapbounds = true;
	    sscanf(optarg,"%lf,%lf,%lf,%lf", 
		   &map_uly, &map_ulx, &map_lry, &map_lrx);
	    break;
        case MAPDIR:
            mapdir.push_back(optarg);
            break;
        case MARKERBOUNDS:
            markerbounds = optarg;
            markers = true;
            break;
        case MARKERFILE:
            markerfile.push_back(optarg);
            markers = true;
            break;
        case MARKERS:
            markers = true;
            break;
        case MOONSIDE:
            title = "View from the moon";
            if (position != LATLON) 
                cerr << "Setting observer position to moonside\n";
            position = MOONSIDE;
            if (projection == RECTANGULAR) projection = ORTHOGRAPHIC;
            break;
        case NIGHT_IMAGE:
            night = optarg;
            if (night.find('.') == string::npos) night += defaultmapext;
            break;
        case NIGHTSIDE:
            title = "Nightside view";
            if (position != LATLON) 
                cerr << "Setting observer position to nightside\n";
            position = NIGHTSIDE;
            if (projection == RECTANGULAR) projection = ORTHOGRAPHIC;
            break;
        case NORTH:
        {
            char *lowercase = optarg;
            char *ptr = optarg;
            while (*ptr) *ptr++ = tolower(*optarg++);
            if (strncmp(lowercase, "orbit", 1) == 0)
            {
                north = ORBIT;
            }
            else if (strncmp(lowercase, "galactic", 1) == 0)
            {
                north = GALACTIC;
            }
            else 
            {
                if (strncmp(lowercase, "body", 1) != 0)
                    cerr << "Unknown value for -north, using body\n";
                north = BODY;
            }
            if (projection == RECTANGULAR) projection = ORTHOGRAPHIC;
            break;
        }
        case NOTRANSPARENCY:
            trans = 0;
            break;
        case OUTPUT:
            output = optarg;
            if (output.find('.') == string::npos)
                output += defaultmapext;
            display = OUTPUT;
            break;
        case PROJECTION:
            projection = getProjectionType(optarg);
            if (projection == RECTANGULAR) obslat = 0;
            break;
        case QUALITY:
            sscanf(optarg, "%d", &quality);
            if (quality < 0) quality = 0;
            if (quality > 100) quality = 100;
            break;
        case RADIUS:
            sscanf(optarg, "%lf", &radius);
            if (radius < 0)
            {
                cerr << "radius must be positive\n";
                exit(EXIT_FAILURE);
            }
            if (projection == RECTANGULAR) projection = ORTHOGRAPHIC;
            break;
        case RANDOM:
            obslon = random() % 360;
            obslon *= deg_to_rad;

            // Weight random latitudes towards the equator
            obslat = (random() % 2000)/1000.0 - 1;
            obslat = asin(obslat);

            rotate = random() % 360;
            rotate *= deg_to_rad;

            if (projection == RECTANGULAR) projection = ORTHOGRAPHIC;
            break;
        case RANGE:
            sscanf(optarg, "%lf", &range);
            if (range <= 1) cerr << "range must be greater than 1\n";
            projection = ORTHOGRAPHIC;
            break;
        case ROOT:
            display = ROOT;
            break;
        case ROTATE:
            sscanf(optarg, "%lf", &rotate);
            if (rotate < 0 || rotate > 360) 
            {
                rotate = fmod(rotate, 360.);
                if (rotate < 0) rotate += 360;
            }
            rotate *= deg_to_rad;
            if (projection == RECTANGULAR) projection = ORTHOGRAPHIC;
            break;
        case SATFILE:
            satellitefile.push_back(optarg);
            break;
        case SATTRACKID:
            sscanf(optarg, "%d", &sattrackid);  
            if (position != LATLON) 
                cerr << "Setting observer position to satellite\n";
            position = SATELLITE;
            if (projection == RECTANGULAR) projection = ORTHOGRAPHIC;
            break;
	case SPACING:
            sscanf(optarg, "%lf", &spacing);  
	    break;
	case SPECULAR_FILE:
	    specular_file = optarg;
	    break;
        case STARFREQ:
            sscanf(optarg, "%lf", &starfreq);
            if (starfreq < 0) 
                starfreq = 0;
            else if (starfreq > 1) 
                starfreq = 1;
            if (projection == RECTANGULAR) projection = ORTHOGRAPHIC;
            break;
        case SUNREL:
            sscanf (optarg, "%lf,%lf", &dellon, &dellat);
            dellon *= deg_to_rad;
            dellat *= deg_to_rad;
            if (position != LATLON) 
                cerr << "Setting observer position to sun-relative\n";
            position = SUNREL;
            if (projection == RECTANGULAR) projection = ORTHOGRAPHIC;
            break;
        case SWAP:
            swap = 1;
            break;
        case TERMINATOR:
        {
            char *lowercase = optarg;
            char *ptr = optarg;
            default_north = ORBIT;
            terminator = 0;
            while (*ptr != '\0') *ptr++ = tolower(*optarg++);
            if (strncmp(lowercase, "evening", 1) == 0)
                terminator = EVENING;
            else if (strncmp(lowercase, "morning", 1) == 0)
                terminator = MORNING;
            else
                cerr << "Use either 'morning' or 'evening' as an argument "
                     << "to -terminator.\n";

            if (terminator != 0)
            {
                if (position != LATLON) 
                    cerr << "Setting observer position to terminator\n";
                position = TERMINATOR;
            }
        }
        break;
        case TRANSPNG:
            output = optarg;
            if (output.find('.') == string::npos)
                output += defaultmapext;
            display = OUTPUT;
            transpng = true;
            break;
        case TRUETYPE:
#ifdef HAVE_LIBFREETYPE
            truetype = true;
#else
            cerr << "Sorry, this binary was built without FreeType "
                 << "support. The -truetype option will be ignored.\n";
#endif
            break;
        case VERSION:
            cout << versionstring << endl;
            cout << "The latest version can be found at " 
                 << "http://xplanet.sourceforge.net\n";
            exit(EXIT_SUCCESS);
        case VROOT:
            vroot = true;
            break;
        case WINDOW:
            display = WINDOW;
            break;
        case EXISTINGWINDOW:
            sscanf(optarg, "%lu", &XID);
            display = WINDOW;
            break;
        default:
        case UNKNOWN:
            cerr << "Use \"xplanet -help\" for a list of valid options.\n";
            exit(EXIT_FAILURE);
            break;
        }
    }

    if (optind < argc)
    {
        cerr << "unrecognized options: ";
        while (optind < argc) cerr << argv[optind++] << " ";
        cerr << endl;
        if (long_options[option_index].has_arg)
        {
            cerr << "Perhaps you didn't supply an argument to -"
                 << long_options[option_index].name << "?\n";
        }
        exit(EXIT_FAILURE);
    }

    if (north == 0) north = default_north;
}

void
Options::setBody()
{
    if (body == RANDOMBODY)
    {
        // if there are no maps, don't loop forever
        for (int i = 0; i < 1000; i++) 
        {
            srandom((unsigned int) random());
            body = (body_name) (random() % RANDOMBODY);
            string test = body_string[body];
            test += defaultmapext;

            if (imageFileExists(test)) break;
        }
    }

    if (markers && markerfile.empty())
        markerfile.push_back(body_string[body]);
}

void
Options::setPosition()
{
    if (mapbounds && projection == RECTANGULAR)
    {
	obslon = (map_ulx + (map_lrx - map_ulx) / 2) * deg_to_rad;
	obslat = (map_lry + (map_uly - map_lry) / 2) * deg_to_rad;
    }

    double sunlat = getSunLat();
    double sunlon = getSunLon();

    if (localtime <= 24) obslon = sunlon + localtime * M_PI / 12 - M_PI;

    switch (position)
    {
    case DAYSIDE:
        obslon = sunlon;
        obslat = sunlat;
        break;
    case NIGHTSIDE:
        obslon = sunlon + M_PI;
        obslat = -sunlat;
        break;
    case EARTHSIDE:
        // This case is handled in xplanet.cc
        break;
    case MOONSIDE:
        if (body == EARTH)
        {
            obslon = getMoonLon();
            obslat = getMoonLat();
        }
        else
        {
            cerr << "-moonside option is meaningless for any body "
                 << "other than earth.\n";
        }
        break;
    case SATELLITE:
        if (body == EARTH)
        {
            double lon, lat, alt;
            if (getSatLocation(sattrackid, (time_t) time.tv_sec,
                               lat, lon, alt))
            {
                obslat = lat;
                obslon = lon;
                if (opts.debug) cout << "Set position to satellite ID "
                                     << sattrackid << endl;
            }
        }
        else
        {
            cerr << "-sattrackid option is meaningless for any body "
                 << "other than earth.\n";
        }
        break;
    case SUNREL:
        /* 
           if either dellon or dellat is 999 user has explicitly set 
           lon or lat
        */
        if (dellon != 999) 
            obslon = sunlon + dellon;
        if (dellat != 999) 
            obslat = sunlat + dellat;

        break;
    case TERMINATOR:
    {
        double H0 = tan(sunlat) * tan(obslat);

        if (H0 > 1)                      // Polar day, return midnight
        {
            obslon = sunlon + M_PI;
        }
        else if (H0 < -1)                // Polar night, return noon
        {
            obslon = sunlon;
        }
        else if (opts.terminator == MORNING)
        {
            obslon = sunlon - (M_PI - acos(H0));
        }
        else
        {
            obslon = sunlon + M_PI - acos(H0);
        }
    }
    break;
    case LATLON:
        obslon *= flipped;
        break;
    default:
        cerr << "Unknown position?\n";
        break;
    }

    obslon = drem(obslon, TWO_PI);

    if (print_coords)
    {
	cout << obslat/deg_to_rad << '\t' << obslon/deg_to_rad << endl;
	exit(EXIT_SUCCESS);
    }
}
