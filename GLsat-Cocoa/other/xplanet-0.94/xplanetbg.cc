/****************************************************************************
    Xplanetbg 0.94 - run Xplanet in the background
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

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
using namespace std;

#include <unistd.h>

#include "orbit.h"
#include "xp_getopt.h"
#include "xplanetbg.h"

#include "libimage/libimage.h"

#include <sys/time.h>
struct timeval tval;

#ifdef HAVE_STRPTIME
float timewarp = 1;
bool use_date = false;
#endif

bool debug = false;

string title;                 // window title

string start_cloud_name;      // filename of first cloud image
string stop_cloud_name;       // filename of last cloud image

string prev_command;          // run this command before xplanet
string post_command;          // run this command after xplanet

int nice_value = 0;           // xplanet execution priority

int num_times = 0;            // number of times to run xplanet
string base, extension = defaultmapext;       // output base filename and extension
string output_option;         // -output or -transpng

int start_index = 0;          // starting output file number

double rotation = 0;

struct s_orbit_info {
    double pos_lat, pos_long, duration, inclination;
};

bool fullscreen = false;

#ifdef HAVE_LIBX11

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#ifdef HAVE_XSS
#include <X11/extensions/scrnsaver.h>
#endif // HAVE_XSS

Display *display = NULL;
Window window = 0;
int window_mask;                // geometry flags
int x=0, y=0;                   // Window position
unsigned int width = 512;
unsigned int height = 512;

Window
open_window()
{
    Window root;
  
    display = XOpenDisplay(NULL);
    if (display == NULL)
    { 
        cerr << "Can't open display\n";
        exit(EXIT_FAILURE);
    }

    int screen_num = DefaultScreen(display);
    root = RootWindow(display, screen_num);

    if (fullscreen)
    {
        width = DisplayWidth(display, screen_num);
        height = DisplayHeight(display, screen_num);
    }

    if (window_mask & XNegative) 
        x += (DisplayWidth(display, screen_num) - width);
    if (window_mask & YNegative) 
        y += (DisplayHeight(display, screen_num) - height);
  
    window = XCreateSimpleWindow(display, root, x, y, width, height, 4, 
                                 WhitePixel (display, screen_num),
                                 BlackPixel (display, screen_num));

    if ((window_mask & XValue) || (window_mask & YValue)
        || (window_mask & XNegative) || (window_mask & YNegative))
    {
        XSizeHints *hints = XAllocSizeHints();
        hints->flags = USPosition;
        XSetWMNormalHints(display, window, hints);
    }

    XTextProperty windowName;
    
    const char *temp;
    if (title.empty())
        temp = versionstring.c_str();
    else
        temp = title.c_str();
    
    XStringListToTextProperty((char **) &temp, 1, &windowName);
    XSetWMName(display, window, &windowName);
    XSync(display, True);
    return(window);
}
#else
extern "C" {
    extern int 
    XParseGeometry(const char *string, int *x, int *y, unsigned int *width, 
                   unsigned int *height);
}
#endif // HAVE_LIBX11

void show_help()
{
    cout << "valid options:\n";
    cout << "\t-ba[ckground]         image_file (string)\n";
    cout << "\t-bl[end]                                 \n";
    cout << "\t-bo[dy]               body       (string)\n";
    cout << "\t-ce[nter]             x,y        (int, int)\n";
    cout << "\t-cloud_i[mage]        image_file (string)\n";
    cout << "\t-cloud_s[sec]         image_file (string)\n";
    cout << "\t-cloud_t[hreshold]    threshold  (int)\n";
    cout << "\t-col[or]              colorname  (string)\n";
    cout << "\t-dat[e]               string     (string)\n";
    cout << "\t-day[side]\n";
    cout << "\t-demf[ile]            demfile    (string)\n";
    cout << "\t-dems[cale]           scale      (float)\n";
    cout << "\t-e[arthside]\n";
    cout << "\t-fo[nt]               fontname   (string)\n";
    cout << "\t-fontd[ir]            directory  (string)\n";
    cout << "\t-fonts[ize]           size       (int)\n";
    cout << "\t-ful[lscreen]\n";
    cout << "\t-fu[zz]               pixels     (int)   \n";
    cout << "\t-ge[ometry]           string     (string)\n";
    cout << "\t-gmtlabel\n";
    cout << "\t-greatarcfile         filename   (string)\n";
    cout << "\t-grid\n";
    cout << "\t-grid1                spacing    (int)\n";
    cout << "\t-grid2                spacing    (int)\n";
    cout << "\t-he[lp]\n";
    cout << "\t-hi[bernate]          seconds    (int)\n";
    cout << "\t-id[lewait]             seconds    (int)\n";
    cout << "\t-im[age]              image_file (string)\n";
    cout << "\t-label                                   \n";
    cout << "\t-labeln[ame]\n";
    cout << "\t-labelp[os]           string     (string)\n";
    cout << "\t-labelr[ot]\n";
    cout << "\t-lat[itude]           degrees    (float) \n";
    cout << "\t-loc[altime]          time       (float) \n";
    cout << "\t-lon[gitude]          degrees    (float) \n";
    cout << "\t-mapb[ounds]          ulx, uly, lrx, rly (float, float, float, float)\n";
    cout << "\t-mapd[ir]             directory  (string)\n";
    cout << "\t-markerb[ounds]       filename   (string)\n";
    cout << "\t-markerf[ile]         filename   (string)\n";
    cout << "\t-markers                                 \n";
    cout << "\t-mo[onside]\n";
    cout << "\t-nice                 priority   (int)\n";
    cout << "\t-night_[image]        image_file (string)\n";
    cout << "\t-nights[ide]\n";
    cout << "\t-no[transparency]\n";
    cout << "\t-nu[m_times]          num_times  (int)   \n";
    cout << "\t-orb[it] duration:inclination    (float):(float)\n";
    cout << "\t-ou[tput]             filename   (string)\n";
    cout << "\t-po[st_command]            command    (string)\n";
    cout << "\t-pre[v_command]            command    (string)\n";
    cout << "\t-pri[nt_coords]\n";
    cout << "\t-pro[jection]             type       (string)\n";
    cout << "\t-q[uality]            quality    (int)\n";
    cout << "\t-rad[ius]             percent    (int)   \n";
    cout << "\t-rand[om]                                \n";
    cout << "\t-rang[e]              range      (float) \n";
    cout << "\t-roo[t]\n";
    cout << "\t-rot[ate]             degrees    (float) \n";
    cout << "\t-satf[ile]            filename   (string)\n";
    cout << "\t-satt[rackid]               id   (string)\n";
    cout << "\t-sh[ade]              percent    (int)   \n";
    cout << "\t-spa[cing]            spacing    (float)\n";
    cout << "\t-spe[cular_file]      filename   (string)\n";
    cout << "\t-starf[req]           density    (float) \n";
    cout << "\t-start_c[loud]        filename   (string)\n";
    cout << "\t-start_i[ndex]        index      (int)\n";
    cout << "\t-stop_c[loud]         filename   (string)\n";
    cout << "\t-su[nrel]     del_lon,del_lat    (float, float)\n";
    cout << "\t-sw[ap]\n";
    cout << "\t-te[rminator]        terminator  (string)\n";
    cout << "\t-ti[mewarp]           factor     (float) \n";
    cout << "\t-tra[nspng]           filename   (string)\n";
    cout << "\t-tru[etype]\n";
    cout << "\t-ve[rsion]\n";
    cout << "\t-vr[oot]\n";
    cout << "\t-wa[it]               seconds    (int)   \n";
    cout << "\t-wi[ndow]\n";
    cout << "\t-x[screensaver]\n";
    cout << "\nCharacters inside the square braces are optional\n";
    exit (EXIT_SUCCESS);
}

void 
process_args (string &command, int argc, char **argv, int &wait,
              unsigned long &idlewait, unsigned long &hibernate,
              struct s_orbit_info &orbit_info)
{
    enum optionType
        {
            UNKNOWN = '?', 
            DATE, DAYSIDE, DEBUG, EARTHSIDE, FULLSCREEN, 
            GEOMETRY, 
            HELP, HIBERNATE, IDLEWAIT, LATITUDE, LONGITUDE, MOONSIDE,
            NICE, NIGHTSIDE, NUM_TIMES, ORBIT, OUTPUT, PASS0, PASS1, PASSQ,
            POSTCOMMAND, PREVCOMMAND, ROTATE, 
            START_CLOUD, START_INDEX, STOP_CLOUD,
            TIMEWARP, TRANSPNG, VERSION, WAIT, WINDOW  
        };
  
    static struct option long_options[] =
        {
            {"background",      required_argument, NULL, PASS1},
            {"blend",           no_argument,       NULL, PASS0},
            {"body",            required_argument, NULL, PASS1},
            {"center",          required_argument, NULL, PASS1},
            {"cloud_gamma",     required_argument, NULL, PASS1},
            {"cloud_image",     required_argument, NULL, PASS1},
            {"cloud_ssec",      required_argument, NULL, PASS1},
            {"cloud_threshold", required_argument, NULL, PASS1},
            {"color",           required_argument, NULL, PASSQ},
            {"date",            required_argument, NULL, DATE},
            {"dayside",         no_argument,       NULL, DAYSIDE},
            {"debug",           no_argument,       NULL, DEBUG},
            {"demfile",         required_argument, NULL, PASS1},
            {"demscale",        required_argument, NULL, PASS1},
            {"earthside",       no_argument,       NULL, EARTHSIDE},
            {"font",            required_argument, NULL, PASSQ},
            {"fontdir",         required_argument, NULL, PASS1},
            {"fontsize",        required_argument, NULL, PASS1},
            {"fullscreen",      no_argument,       NULL, FULLSCREEN},
            {"fuzz",            required_argument, NULL, PASS1},
            {"geometry",        required_argument, NULL, GEOMETRY},
            {"gmtlabel",        no_argument,       NULL, PASS0},
            {"greatarcfile",    required_argument, NULL, PASS1},
            {"grid",            no_argument,       NULL, PASS0},
            {"grid1",           required_argument, NULL, PASS1},
            {"grid2",           required_argument, NULL, PASS1},
            {"help",            no_argument,       NULL, HELP},
            {"hibernate",       required_argument, NULL, HIBERNATE},
            {"idlewait",        required_argument, NULL, IDLEWAIT},
            {"image",           required_argument, NULL, PASS1},
            {"label",           no_argument,       NULL, PASS0},
            {"labelformat",     required_argument, NULL, PASSQ},
            {"labelname",       no_argument,       NULL, PASS0},
            {"labelpos",        required_argument, NULL, PASS1},
            {"labelrot",        no_argument,       NULL, PASS0},
            {"latitude",        required_argument, NULL, LATITUDE},
            {"localtime",       required_argument, NULL, PASS1},
            {"longitude",       required_argument, NULL, LONGITUDE},
            {"mapbounds",       required_argument, NULL, PASSQ},
            {"mapdir",          required_argument, NULL, PASS1},
            {"markerbounds",    required_argument, NULL, PASS1},
            {"markerfile",      required_argument, NULL, PASS1},
            {"markers",         no_argument,       NULL, PASS0},
            {"moonside",        no_argument,       NULL, MOONSIDE},
            {"nice",            required_argument, NULL, NICE},
            {"night_image",     required_argument, NULL, PASS1},
            {"nightside",       no_argument,       NULL, NIGHTSIDE},
            {"north",           required_argument, NULL, PASS1},
            {"notransparency",  no_argument,       NULL, PASS0},
            {"num_times",       required_argument, NULL, NUM_TIMES},
            {"orbit",           required_argument, NULL, ORBIT},
            {"output",          required_argument, NULL, OUTPUT},
            {"post_command",    required_argument, NULL, POSTCOMMAND},
            {"prev_command",    required_argument, NULL, PREVCOMMAND},
            {"print_coords",    no_argument,       NULL, PASS0},
            {"projection",      required_argument, NULL, PASS1},
            {"quality",         required_argument, NULL, PASS1},
            {"radius",          required_argument, NULL, PASS1},
            {"random",          no_argument,       NULL, PASS0},
            {"range",           required_argument, NULL, PASS1},
            {"root",            no_argument,       NULL, PASS0},
            {"rotate",          required_argument, NULL, ROTATE},
            {"satfile",         required_argument, NULL, PASS1},
            {"sattrackid",      required_argument, NULL, PASS1},
            {"shade",           required_argument, NULL, PASS1},
            {"spacing",         required_argument, NULL, PASS1},
            {"specular_file",   required_argument, NULL, PASSQ},
            {"starfreq",        required_argument, NULL, PASS1},
            {"start_cloud",     required_argument, NULL, START_CLOUD},
            {"start_index",     required_argument, NULL, START_INDEX},
            {"stop_cloud",      required_argument, NULL, STOP_CLOUD},
            {"sunrel",          required_argument, NULL, PASS1},
            {"swap",            no_argument,       NULL, PASS0},
            {"terminator",      required_argument, NULL, PASS1},
            {"timewarp",        required_argument, NULL, TIMEWARP},
            {"transpng",        required_argument, NULL, TRANSPNG},
            {"truetype",        no_argument,       NULL, PASS0},
            {"version",         no_argument,       NULL, VERSION},
            {"vroot",           no_argument,       NULL, PASS0},
            {"wait",            required_argument, NULL, WAIT},
            {"window",          no_argument,       NULL, WINDOW},
            {"xscreensaver",    no_argument,       NULL, PASS0},
            {NULL,              0,                 NULL, 0}
        };

    int this_option;
    int option_index = 0;
    while((this_option = xp_getopt_long_only(argc, argv, "+", long_options, 
                                             &option_index)) >= 0)
    {
        switch (this_option)
        {
        case DATE:
        {
#ifdef HAVE_STRPTIME
            use_date = true;
            struct tm tm;
            tm.tm_sec = 0;
            tm.tm_min = 0;
            tm.tm_hour = 0;
            tm.tm_mday = 0;
            tm.tm_mon = 0;
            tm.tm_year = 0;
            tm.tm_isdst = 0;
            strptime(optarg, "%d %b %Y %T", &tm);
            tval.tv_sec = mktime(&tm);
            if (tm.tm_isdst > 0) tval.tv_sec -= 3600;
#else
            cerr << "This system does not have the strptime function.\n";
            cerr << "The -date option will be ignored.\n";
#endif
        }
        break;
        case DAYSIDE:
        {
            command += "-";
            command += long_options[option_index].name;
            command += " ";
            title = "Dayside view";
        }
        break;
        case DEBUG:
            debug = true;
            command += "-";
            command += long_options[option_index].name;
            command += " ";
            break;
        case EARTHSIDE:
        {
            command += "-";
            command += long_options[option_index].name;
            command += " ";
            title = "View from earth";
        }
        break;
        case FULLSCREEN:
            fullscreen = true;
            command += "-";
            command += long_options[option_index].name;
            command += " ";
#ifdef HAVE_LIBX11
            window = 1;
#endif
            break;
        case GEOMETRY:
            command += "-";
            command += long_options[option_index].name;
            command += " ";
            command += optarg;
            command += " ";
#ifdef HAVE_LIBX11
            window_mask = XParseGeometry(optarg, &x, &y, &width, &height);
            window = 1;
#endif
            break;
        case HELP:
            show_help();
            break;
        case HIBERNATE:
#ifdef HAVE_XSS
            sscanf(optarg, "%lu", &hibernate);
            hibernate *= 1000;
#else
            cerr << "This binary was compiled without the X Screensaver\n"
                 << "extension.  The -hibernate option will be ignored.\n";
#endif
            break;
        case IDLEWAIT:
#ifdef HAVE_XSS
            sscanf(optarg, "%lu", &idlewait);
            idlewait *= 1000;
#else
            cerr << "This binary was compiled without the X Screensaver\n"
                 << "extension.  The -idlewait option will be ignored.\n";
#endif
            break;
        case LATITUDE:
        {
            double obslat;
            sscanf(optarg, "%lf", &obslat);
            if (obslat < -90) obslat = -90;
            if (obslat > 90) obslat = 90;
            orbit_info.pos_lat = obslat;
        }
        break;
        case LONGITUDE:
        {
            double obslon;
            sscanf(optarg, "%lf", &obslon);
            if (obslon < 0 || obslon > 360) 
            {
                obslon = fmod(obslon, 360.);
                if (obslon < 0) obslon += 360;
            }
            orbit_info.pos_long = obslon;
        } 
        break;
        case MOONSIDE:
        {
            command += "-";
            command += long_options[option_index].name;
            command += " ";
            title = "View from the moon";
        }
        break;
        case NICE:
            sscanf(optarg, "%d", &nice_value);
            if (nice_value < 0 || nice_value > 19) 
            {
                cerr << "-nice must take an argument between 0 and 19\n";
                exit(EXIT_FAILURE);
            }
            break;
        case NIGHTSIDE:
        {
            command += "-";
            command += long_options[option_index].name;
            command += " ";
            title = "Nightside view";
        }
        break;
        case NUM_TIMES:
            sscanf(optarg, "%d", &num_times);
            if (num_times < 1) 
            {
                cerr << "-num_times must be positive\n";
                exit (EXIT_FAILURE);
            }
            break;
        case ORBIT:
        {
            float duration, inclination;
            sscanf(optarg, "%f:%f", &duration, &inclination);
            if (duration < 0)
            {
                cerr << "orbit duration must be positive\n";
                exit(EXIT_FAILURE);
            }
            orbit_info.duration = duration;
            orbit_info.inclination = inclination;
        }
        break;
        case OUTPUT:
        {
            base = optarg;
            if (base.find('.') == string::npos)
            {
                extension = defaultmapext;
            }
            else
            {
                extension.assign(base, base.find('.'), base.size());
                base.assign(base, 0, base.find('.'));
            }

            output_option = "-output";
#ifdef HAVE_LIBX11          
            window = 0;
#endif
        }
        break;
        case PASS0:
            command += "-";
            command += long_options[option_index].name;
            command += " ";
            break;
        case PASS1:
            command += "-";
            command += long_options[option_index].name;
            command += " ";
            command += optarg;
            command += " ";
            break;
        case PASSQ:
            command += "-";
            command += long_options[option_index].name;
            command += " \"";
            command += optarg;
            command += "\" ";
            break;
        case PREVCOMMAND:
            prev_command = optarg;
            break;
        case POSTCOMMAND:
            post_command = optarg;
            break;
        case ROTATE:
        {
            double obsrot;
            sscanf(optarg, "%lf", &obsrot);
            if (obsrot < 0 || obsrot > 360) 
            {
                obsrot = fmod(obsrot, 360.);
                if (obsrot < 0) obsrot += 360;
            }
            rotation = obsrot;
        }
        break;
        case START_CLOUD:
            start_cloud_name = optarg;
            break;
        case START_INDEX:
            sscanf(optarg, "%d", &start_index);
            if (start_index < 1) 
            {
                cerr << "-start_index must be positive\n";
                exit (EXIT_FAILURE);
            }
            break;
        case STOP_CLOUD:
            stop_cloud_name = optarg;
            break;
        case TIMEWARP:
#ifdef HAVE_STRPTIME
            use_date = true;
            sscanf(optarg, "%f", &timewarp);
            if (timewarp == 0) 
            {
                cerr << "timewarp must be non-zero\n";
                exit(EXIT_FAILURE);
            }
#else
            cerr << "This system does not have the strptime function.\n";
            cerr << "The -timewarp option will be ignored.\n";
#endif
            break;
        case TRANSPNG:
        {
            base = optarg;
            if (base.find('.') == string::npos) extension = ".png";

            output_option = "-transpng";

#ifdef HAVE_LIBX11          
            window = 0;
#endif
        }
        break;
        case VERSION:
            cout << versionstring << endl;
            cout << "The latest version can be found at " 
                 << "http://xplanet.sourceforge.net\n";
            exit(EXIT_SUCCESS);
        case WAIT:
            sscanf(optarg, "%d", &wait);
            break;
        case WINDOW:
#ifdef HAVE_LIBX11
            window = 1;
#else
            cerr << "The -window option is not supported without X11.\n";
#endif
            break;
        default:
        case UNKNOWN:
            cerr << "Use \"xplanetbg -help\" for a list of valid options\n";
            exit(EXIT_FAILURE);
        }
    }
  
    if (optind < argc)
    {
        cerr << "unrecognized options: ";
        while (optind < argc) cerr << argv[optind++] << " ";
        cerr << "\nPerhaps you didn't supply an argument to -"
             << long_options[option_index].name << "?\n";
        exit(EXIT_FAILURE);
    }

    char cbuffer[64];

    /* If we haven't specified an orbit but we do have a 
       -longitude or -latitude argument */
    if (!orbit_info.duration)
    {
        if (orbit_info.pos_lat)
        {
            sprintf(cbuffer, "-latitude %5.1f ", orbit_info.pos_lat);
            command += cbuffer;
        }
        if (orbit_info.pos_long)
        {
            sprintf(cbuffer, "-longitude %5.1f ", orbit_info.pos_long);
            command += cbuffer;
        }
    }

#ifdef HAVE_LIBX11
    if (window) 
    {
        sprintf (cbuffer, "-XID %lu ", open_window());
        command += cbuffer;
    }
#endif
}

int 
main(int argc, char **argv)
{
    // default time, in seconds, between updates
    int wait = 300; 
    // Don't run xplanet unless the user has been idle for this much
    // time, in milliseconds
    unsigned long idlewait = 0;  

    // Don't run xplanet after the user has been idle for this much
    // time, in milliseconds
    unsigned long hibernate = 0; 

    // Initialize to current time
    gettimeofday(&tval, NULL);

    string stat_command = "xplanet ";
    string dyn_command;
    char cbuffer[256];

    struct s_orbit_info obi = {0,0,0,0};
    process_args(stat_command, argc, argv, wait, idlewait, 
                 hibernate, obi);

#ifdef HAVE_XSS
    if (display == NULL) {
        display = XOpenDisplay(NULL);
    }
    int screen_num = 0;
    int root = 0;
    XScreenSaverInfo* screenSaverInfo = NULL;
    if (display) {
        screen_num = DefaultScreen(display);
        root = RootWindow(display, screen_num);
        int event_base, error_base;

        if (XScreenSaverQueryExtension(display, &event_base, &error_base)) {
            screenSaverInfo = XScreenSaverAllocInfo();
        }
    }
#endif // HAVE_XSS

    Orbit orb(obi.duration, obi.pos_lat, obi.pos_long, obi.inclination);
 
    bool cloud_interp = false;
    short *cloud_base  = NULL;
    short *cloud_diff = NULL;
    int cloud_width = -1;
    int cloud_height = -1;
    int cloud_area = 0;

    if (num_times != 0 
        && !start_cloud_name.empty() 
        && !stop_cloud_name.empty())
    {
        unsigned char *first = NULL;
        unsigned char *last = NULL;
        if (read_image(start_cloud_name.c_str(), &cloud_width, &cloud_height, 
                       &first) == 0)
        {
            cerr << "Can't read first cloud image!\n";
            exit(EXIT_FAILURE);
        }
        if (read_image(stop_cloud_name.c_str(), &cloud_width, &cloud_height, 
                       &last) == 0)
        {
            cerr << "Can't read last cloud image!\n";
            exit(EXIT_FAILURE);
        }

        cloud_area = cloud_width * cloud_height;

        cloud_interp = true;
        cloud_base = new short [cloud_area];
        cloud_diff = new short [cloud_area];
        int ipos = 0;
        int ipos3 = 0;
        for (int i = 0; i < cloud_area; i++)
        {
            cloud_base[ipos] = first[ipos3];
            cloud_diff[ipos] = last[ipos3] - first[ipos3];
            ipos++;
            ipos3 += 3;
        }
        delete_image(&first);
        delete_image(&last);
    }

    int current_index = start_index;
    int stop_index = start_index + num_times;

    bool first_time = true;

    while (1)
    {
        if (nice_value) 
        {
            dyn_command = nicecmd;
            sprintf(cbuffer, "-%d ", nice_value);
            dyn_command += cbuffer;
            dyn_command += stat_command;
        }
        else
            dyn_command = stat_command;

        if (num_times != 0) 
        {
            if (current_index >= stop_index) break;

            if (cloud_interp)
            {
                double frac = ((double) (current_index - start_index)) 
                    / (num_times - 1);

                unsigned char *cloud_image = new unsigned char [3 * cloud_area];
                for (int i = 0; i < cloud_area; i++)
                {
                    int value = (int) (cloud_base[i] + frac * cloud_diff[i]);
                    memset (cloud_image + 3 * i, value, 3);
                }

                char cloud_name[128];
                int digits = (int) (log10((double) stop_index) + 1);
                sprintf(cloud_name, "clouds%.*d%s", 
                        digits, current_index, extension.c_str());
                
                set_quality(80);
                if (!write_image(cloud_name, cloud_width, cloud_height, cloud_image))
                {
                    cerr << "Can't create image file " << cloud_name << "\n";
                    exit(EXIT_FAILURE);
                }
                
                dyn_command += "-cloud_image ";
                dyn_command += cloud_name;
                dyn_command += " ";

                delete [] cloud_image;
            }

            if (!base.empty())
            {
                int digits = (int) (log10((double) stop_index) + 1);
                sprintf(cbuffer, "%s %s%.*d%s ", output_option.c_str(),
                        base.c_str(), digits, current_index, 
                        extension.c_str());
                dyn_command += cbuffer;
            }
            current_index++;
        }
        else
        {
            if (!base.empty())
            {
                sprintf(cbuffer, "%s %s%s ", output_option.c_str(),
                        base.c_str(), extension.c_str());
                dyn_command += cbuffer;
            }
        }             
      
        if (obi.duration > 0)
        {
            double orblat = orb.Lat();
            double orblon = orb.Long();
            sprintf(cbuffer, "-latitude %5.1f ", orblat);
            dyn_command += cbuffer;
            sprintf(cbuffer, "-longitude %5.1f ", orblon);
            dyn_command += cbuffer;
            sprintf(cbuffer, "-rotate %5.1f", (rotation + obi.inclination 
                                               - orb.Theta()));
            dyn_command += cbuffer;
        }
        else if (rotation != 0)
        {
            sprintf(cbuffer, "-rotate %5.1f", rotation);
            dyn_command += cbuffer;
        }
      
#ifdef HAVE_STRPTIME
        if (use_date)
        {
            struct tm *tm_ptr = localtime((time_t *) &tval.tv_sec);
            strftime(cbuffer, sizeof(cbuffer), "%d %b %Y %T", tm_ptr);
            dyn_command += " -date \"";
            dyn_command += cbuffer;
            dyn_command += "\" ";
            tval.tv_sec += (time_t) timewarp * wait;
        }
#endif

#ifdef HAVE_XSS
        if (screenSaverInfo != NULL)
        {
            if (!first_time && idlewait > 0) 
            {
                XScreenSaverQueryInfo(display, root, screenSaverInfo);
                while (screenSaverInfo->idle < idlewait) 
                {
                    usleep((idlewait - screenSaverInfo->idle) * 1000);
                    XScreenSaverQueryInfo(display, root, screenSaverInfo);
                }
            }

            if (hibernate > 0)
            {
                XScreenSaverQueryInfo(display, root, screenSaverInfo);
                while (screenSaverInfo->idle > hibernate) 
                {
                    sleep(1);
                    XScreenSaverQueryInfo(display, root, screenSaverInfo);
                }
            }
        }

#endif

        if (debug || !base.empty()) cout << dyn_command << endl;
        
        struct timeval current_time;
        gettimeofday(&current_time, NULL);
        time_t next_update = current_time.tv_sec + wait;

        if (!prev_command.empty())
        {
            if (system(prev_command.c_str()) != 0) 
            {
                cerr << "Can't execute " << prev_command << endl;
            }
        }
            
        if (debug)
            cout << "Executing xplanet at " 
                 << ctime((time_t *) &current_time.tv_sec);

        if (system(dyn_command.c_str()) != 0) 
        {
            cerr << "Xplanetbg exiting\n";
            break;
        }
        
        first_time = false;

        if (!post_command.empty())
        {
            if (system(post_command.c_str()) != 0) 
            {
                cerr << "Can't execute " << post_command << endl;
            }
        }
            
#ifdef HAVE_STRPTIME
        if (obi.duration > 0) orb.WaitTime(timewarp * wait/3600.0);
#endif
      
        gettimeofday(&current_time, NULL);
        
        time_t sleep_time = next_update - current_time.tv_sec;
        if (sleep_time < 1) sleep_time = 1;
        
        if (debug)
        {
            cout << "Finished at " << ctime(&current_time.tv_sec);
            cout << "Sleeping for " << sleep_time << " seconds until "
                 << ctime((time_t *) &next_update) << endl;
        }

#ifdef HAVE_LIBX11
        if (window)
        {
            Atom wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", 
                                              False);
            XSetWMProtocols(display, window, &wmDeleteWindow, 1);
            XSelectInput(display, window, KeyPressMask);
            XEvent event;
            for (int i = 0; i < sleep_time * 10; i++)
            {
                if (XCheckTypedWindowEvent(display, window, ClientMessage, 
                                           &event) == True) 
                {
                    if ((unsigned int) event.xclient.data.l[0] 
                        == wmDeleteWindow)
                        exit(EXIT_SUCCESS);
                }
                else if (XCheckTypedWindowEvent(display, window, KeyPress, 
                                                &event) == True)
                {
                    KeySym keysym;
                    char keybuf;
                    XLookupString(&(event.xkey), &keybuf, 1, &keysym, NULL);
                    if (keybuf == 'q' || keybuf == 'Q') 
                        exit(EXIT_SUCCESS);
                }
                else
                {
                    usleep(100000);   // sleep for 1/10 second
                }
            }
        }  // end if (window)
        else
#endif // HAVE_LIBX11
        {
            sleep(sleep_time);
        }
    } // end while(1)

    if (cloud_interp) 
    {
        delete [] cloud_base;
        delete [] cloud_diff;
    }

    exit(EXIT_SUCCESS);
}
