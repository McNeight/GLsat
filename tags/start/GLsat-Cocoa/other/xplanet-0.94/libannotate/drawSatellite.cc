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

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

#include <unistd.h>

#include "auxfiles.h"
#include "Location.h"
#include "Options.h"
#include "parse.h"
#include "Satellite.h"
#include "util.h"

static list<Satellite> full_satellite_list;  // need this for -sattrackid

static bool
satelliteFileExists(string &filename)
{
    if (opts.debug) 
    {
        cout << "\nLooking for satellite file ...\n"
             << filename << endl;
    }

    if (fileExists(filename)) return(true);

    string satellite_file = "satellites";
    satellite_file += separator;
    satellite_file += filename;
    
    if (fileExists(satellite_file))
    {
        filename = satellite_file;
        return(true);
    }

    satellite_file = prefix;
    satellite_file += separator;
    satellite_file += "satellites";
    satellite_file += separator;
    satellite_file += filename;

    if (fileExists(satellite_file))
    {
        filename = satellite_file;
        return(true);
    }

    return(false);
}

//  Given an value X and radius d, this routine draws a half-circle in
//  the plane where X is constant.
static void
drawAltitudeHalfCirc(const double rot[3][3], const double X, const double d,
                     bool Z_positive, const string arcopts, ofstream &sat_a)
{
    double Y = d;
    double Z = (1 - X*X - Y*Y);
    if (fabs(Z) < 1e-5) Z = 0;
    Z = (Z_positive ? sqrt(Z) : -sqrt(Z));

    Location point(0);
    point.setXYZ(X, Y, Z);
    point.rotate(rot);
    point.rectangularToSpherical();

    for (double siny = 1; siny >= -1; siny -= 1./10.)
    {
        double prev_lat = point.lat;
        double prev_lon = point.lon;

        Y = sin(M_PI_2 * siny) * d;
        Z = (1 - X*X - Y*Y);
        if (fabs(Z) < 1e-5) Z = 0;
        Z = (Z_positive ? sqrt(Z) : -sqrt(Z));

        point.setXYZ(X, Y, Z);
        point.rotate(rot);
        point.rectangularToSpherical();
        sat_a << prev_lat/deg_to_rad << '\t' << prev_lon/deg_to_rad 
              << '\t' << point.lat/deg_to_rad 
              << '\t' << point.lon/deg_to_rad;
        if (!arcopts.empty()) sat_a << " " << arcopts;
        sat_a << endl;
    }
}

// write the entry in the marker/arc file for this satellite
static void
writeSatFiles(Satellite *this_sat, char *options, 
              ofstream &sat_a, ofstream &sat_m)
{
    string arcopts;

    int align = AUTO;
    list<double> altcirc;
    string color;
    
    string font;
    int fontsize = opts.fontsize;
    
    string image;
    string name = this_sat->getName();
    
    char *returnstring = NULL;
    
    int trail_type = ORBIT;
    int trail_start = 0;
    int trail_end = 0;
    int trail_interval = 1;
    string transparency;
    
    bool end_of_line = false;
    bool syntax_error = false;

    int i = 0;
    while (!end_of_line)
    {
        switch (parse(i, options, returnstring))
        {
        case ALIGN:
            if (returnstring == NULL) break;
            switch (returnstring[0])
            {
            case 'r':
            case 'R':
                align = RIGHT;
                break;
            case 'l':
            case 'L':
                align = LEFT;
                break;
            case 'a':
            case 'A':
                align = ABOVE;
                break;
            case 'b':
            case 'B':
                align = BELOW;
                break;
            case 'c':
            case 'C':
                align = CENTER;
                break;
            default:
                cerr << "Unrecognized option for align in "
                     << "satellite file" << endl;
                break;
            }
            break;
        case ALTCIRC:
        {
            double angle;
            if (returnstring == NULL) break;
            sscanf(returnstring, "%lf", &angle);
            if (angle < 0) angle *= -1;
            if (angle > 90) angle = 90;
            angle += 90;
            altcirc.push_back(angle * deg_to_rad);
        }
        break;
        case COLOR:
            if (returnstring == NULL) break;
            color.assign(returnstring);
            arcopts += (" color=" + color);
            break; 
        case DELIMITER:
            break;
        case ENDOFLINE:
            end_of_line = true;
            break;
        case FONT:
            if (returnstring == NULL) break;
            font.assign(returnstring);
            break;
        case FONTSIZE:
            if (returnstring == NULL) break;
            sscanf(returnstring, "%d", &fontsize);
            if (fontsize < 0) fontsize = opts.fontsize;
            break;
        case IMAGE:
            if (returnstring == NULL) break;
            image.assign(returnstring);
            break;
        case NAME:
            if (returnstring == NULL) break;
            name.assign(returnstring);
            break;
        case SPACING:
	{
            if (returnstring == NULL) break;
	    float spacing;
            sscanf(returnstring, "%f", &spacing);
            if (spacing < 0.001) spacing = 0.001;
            arcopts += " spacing=";
            arcopts += returnstring;
	}
	break;
        case TRAIL:
        {
            char *ptr = returnstring;
            while (ptr[0] != ',') 
            {
                if (ptr[0] == '\0') 
                {
                    syntax_error = true;
                    break;
                }
                ptr++;
            }

            if (syntax_error) break;

            if (!sscanf(++ptr, "%d,%d,%d", &trail_start, &trail_end,
                        &trail_interval) == 3)
            {
                cerr << "Need four values for trail{}!\n";
                syntax_error = true;
            }
            else
            {
                switch (returnstring[0])
                {
                case 'g':
                case 'G':
                    trail_type = GROUND;
                    break;
                case 'o':
                case 'O':
                    trail_type = ORBIT;
                    break;
                default:
                    cerr << "Unknown type of orbit trail!\n";
                    syntax_error = true;
                    break;
                }
                if (trail_interval < 1) trail_interval = 1;
            }
        }
        break;
        case TRANSPARENT:
            if (returnstring == NULL) break;

            transparency.assign(returnstring);
            
            int r, g, b;
            if (!sscanf(returnstring, "%d,%d,%d", &r, &g, &b) == 3)
            {
                cerr << "Need three values for transparency pixel!\n";
                syntax_error = true;
            }

            break;
        default:
        case UNKNOWN:
            syntax_error = true;
            break;
        }

        if (returnstring != NULL)
        {
            delete [] returnstring;
            returnstring = NULL;
        }

        if (syntax_error)
        {
            cerr << "Syntax error in satellite file\n"
                 << "line is \"" << this_sat->getID() << " " << options 
                 << "\"" << endl;
            return;
        }

        i++;
    }

    const double EARTH_RADIUS = 6378.140;
    
    // Reload TLE data here since select_ephemeris() in libsgp4sdp4
    // changes it.  This is in case the user wants to have two entries
    // with the same satellite.
    this_sat->loadTLE();
    
    time_t start_time = (time_t) (opts.time.tv_sec + trail_start * 60);
    time_t end_time = (time_t) (opts.time.tv_sec + trail_end * 60);
    time_t interval = (time_t) (trail_interval * 60);

    if (start_time > end_time)
    {
        time_t tmp = start_time;
        start_time = end_time;
        end_time = tmp;
    }
    
    double lat = 0, lon = 0, alt = 0;
    this_sat->getSpherical(start_time, lat, lon, alt);
    
    for (time_t t = start_time + interval; t <= end_time; t += interval)
    {
        double prev_lat = lat;
        double prev_lon = lon;
        double prev_alt = alt;
        this_sat->getSpherical(t, lat, lon, alt);
        sat_a << prev_lat/deg_to_rad << '\t' << prev_lon/deg_to_rad 
              << '\t' << lat/deg_to_rad << '\t' << lon/deg_to_rad;
        
        if (!arcopts.empty()) sat_a << " " << arcopts;
        if (trail_type == ORBIT)
            sat_a << " radius=" << 1 + prev_alt/EARTH_RADIUS
                  << " radius=" << 1 + alt/EARTH_RADIUS;

        sat_a << endl;
    }
    
    this_sat->getSpherical((time_t) opts.time.tv_sec, lat, lon, alt);
    
    sat_m << lat/deg_to_rad << ' ' << lon/deg_to_rad;
    if (trail_type == ORBIT)
        sat_m << " radius=" << 1 + alt/EARTH_RADIUS;

    if (align != AUTO) 
    {
        sat_m << " align=";
        switch (align)
        {
        case RIGHT:
            sat_m << "right";
            break;
        case ABOVE:
            sat_m << "above";
            break;
        case BELOW:
            sat_m << "below";
            break;
        case LEFT:
            sat_m << "left";
            break;
        case CENTER:
            sat_m << "center";
            break;
        default:
            cerr << "Unknown alignment????\n";
        }
    }
    if (!color.empty()) sat_m << " color=" << color;
    if (!font.empty()) sat_m << " font=" << font;
    if (fontsize != opts.fontsize) sat_m << " fontsize=" << fontsize;
    if (!image.empty()) sat_m << " image=" << image;
    if (!name.empty()) sat_m << " {" << name << "}";
    if (!transparency.empty()) 
        sat_m << " transparent={" << transparency << "}";

    sat_m << endl;
    
    // Distance of satellite from Earth's center, in units of Earth radii
    double Rs = 1 + alt/EARTH_RADIUS;
    
    double rot[3][3];
    rotateXYZ(rot, 0, lat, -lon);

    list<double>::iterator a = altcirc.begin();
    while (a != altcirc.end())
    {
        double phi = asin(sin(*a) / Rs);
        double X = -cos(*a + phi);
        double d = sin(*a + phi);

        drawAltitudeHalfCirc(rot, X, d, true, arcopts, sat_a);
        drawAltitudeHalfCirc(rot, X, d, false, arcopts, sat_a);
        a++;
    }
}

static int
readSatFileLine(const char *line, list<Satellite> &satellite_list,
                Satellite *&this_sat, char *&options)
{
    int i = 0;
    while (isDelimiter(line[i])) i++;
    if (isEndOfLine(line[i])) return(-1);

    int id;
    sscanf(line + i, "%d", &id);
    while (!(isDelimiter(line[i]) || isEndOfLine(line[i]))) i++; 

    if (id != 0)
    {
        list<Satellite>::iterator p = satellite_list.begin();
        while (p != satellite_list.end())
        {
            if (id == p->getID())
            {
                this_sat = &(*p);
                if (opts.debug) 
                {
                    cout << "Found ID #" << id << " (" 
                         << this_sat->getName() << ")\n";
                }
                break;
            }
            p++;
        }

        if (this_sat == NULL)
        {
            cerr << "Satellite ID #" << id << " not found in TLE file\n";
            return(-1);
        }
    }

    strcpy(options, line + i);

    return(id);
}

// Run through the list of satellite files and check to see that
// they exist, along with the corresponding TLE files.
static void
cullList()
{
    list<string> remove_list;

    list<string>::iterator p = opts.satellitefile.begin();
    while(p != opts.satellitefile.end())
    {
        if (satelliteFileExists(*p))
        {
            string tlefile = *p + ".tle";
            if (!satelliteFileExists(tlefile))
            {
                cerr << "Can't open TLE file " << tlefile << endl;
                remove_list.push_back(*p);
            }
        }
        else
        {
            cerr << "Can't open satellite file " << *p << endl;
            remove_list.push_back(*p);
        }
        p++;
    }

    p = remove_list.begin();
    while (p != remove_list.end())
    {
        opts.satellitefile.remove(*p);
        p++;
    }
}

void
createSatelliteList()
{
    cullList();

    list<string>::iterator p = opts.satellitefile.begin();
    while(p != opts.satellitefile.end())
    {
        string tlefile = *p + ".tle";
        ifstream infile(tlefile.c_str());

        char lines[3][80];
        while (infile.getline(lines[0], 80) != NULL)
        {
            if ((infile.getline(lines[1], 80) == NULL) 
                || (infile.getline(lines[2], 80) == NULL))
            {
                cerr << "Malformed TLE file?\n";
                break;
            }
            
            Satellite sat(lines);
            
            if (!sat.isGoodData()) 
            {
                cerr << "Bad TLE data\n";
                continue;
            }
            if (opts.debug) cout << "Read satellite with ID "
                                 << sat.getID() << endl;
            
            full_satellite_list.push_back(sat);
        }

        infile.close();
        p++;
    }
}

void
createSatelliteFiles(string tmpdir)
{
    int count = 0;
    list<string>::iterator p = opts.satellitefile.begin();
    while(p != opts.satellitefile.end())
    {
        list<Satellite> satellite_list;
        list<Satellite> zero_list;
        
        string tlefile = *p + ".tle";
        if (opts.debug) cout << "Opening TLE file " << tlefile << endl;
        
        ifstream infile(tlefile.c_str());
        char lines[3][80];
        while (infile.getline(lines[0], 80) != NULL)
        {
            if ((infile.getline(lines[1], 80) == NULL) 
                || (infile.getline(lines[2], 80) == NULL))
            {
                cerr << "Malformed TLE file?\n";
                break;
            }
            
            Satellite sat(lines);
            if (!sat.isGoodData()) 
            {
                cerr << "Bad TLE data\n";
                continue;
            }
            if (opts.debug) cout << "Read ID " << sat.getID() << endl;
            
            satellite_list.push_back(sat);
            zero_list.push_back(sat);
        }
        
        infile.close();

        // Now write one marker and one great arc file for each
        // satellite file.

        char pid[9];
        int digits = (int) (log10((double) opts.satellitefile.size()) 
                            + 1);
        sprintf(pid, "%5.5d-%.*d", ((int) (getpid() % 10000)), 
                digits, count++);
        
        string sat_markers = (tmpdir + separator 
                              + "xplanet-markers." + pid);
        string sat_arcs = tmpdir + separator + "xplanet-arcs." + pid;
        
        ofstream sat_a(sat_arcs.c_str());
        ofstream sat_m(sat_markers.c_str());
        
        if (opts.debug) cout << "Opening satellite file " << *p << endl;
        infile.open(p->c_str());
        
        // Now read each line and write the appropriate entry to
        // the marker/arc files
        char *line = new char[256];
        char *zero_options = NULL;
        while (infile.getline(line, 256, '\n') != NULL)
        {
            Satellite *this_sat = NULL;
            char *options = new char[256];
            int id = readSatFileLine(line, satellite_list, 
                                     this_sat, options);
	    if (id == 0)
            {
                if (zero_options == NULL)
                {
                    zero_options = new char[256];
                    strcpy(zero_options, options);
                }
                else
                {
                    cerr << "ID 0 already specified in satellite file\n";
                }
            }
            else if (id > 0)
            {
                writeSatFiles(this_sat, options, sat_a, sat_m);
                zero_list.remove(*this_sat);
            }
            delete [] options;
        }

        if (zero_options != NULL)
        {
            list<Satellite>::iterator p2 = zero_list.begin();
            while (p2 != zero_list.end())
            {
                writeSatFiles(&(*p2), zero_options, sat_a, sat_m);
                p2++;
            }
            delete [] zero_options;
        }

        infile.close();
        
        sat_m.close();
        sat_a.close();
        
        opts.markerfile.push_back(sat_markers);
        opts.greatarcfile.push_back(sat_arcs);

	delete [] line;
        p++;
    }
}

void
cleanupSatelliteFiles(string tmpdir)
{
    int count = 0;
    list<string>::iterator p = opts.satellitefile.begin();
    while(p != opts.satellitefile.end())
    {
        char pid[9];
        int digits = (int) (log10((double) opts.satellitefile.size()) 
                            + 1);
        sprintf(pid, "%5.5d-%.*d", ((int) (getpid() % 10000)), 
                digits, count++);
        
        string sat_markers = tmpdir + separator + "xplanet-markers." + pid;
        string sat_arcs = tmpdir + separator + "xplanet-arcs." + pid;

        unlink(sat_markers.c_str());
        unlink(sat_arcs.c_str());
        p++;
    }
}

bool
getSatLocation(const int id, const time_t time, double &lat, double &lon,
               double &alt)
{
    Satellite *this_sat = NULL;
    list<Satellite>::iterator p = full_satellite_list.begin();
    while (p != full_satellite_list.end())
    {
        if (id == p->getID())
        {
            this_sat = &(*p);
            if (opts.debug) 
            {
                cout << "Found ID #" << id << " (" << this_sat->getName() 
                     << ")\n";
            }
            break;
        }
        p++;
    }

    if (this_sat == NULL) 
    {
        if (opts.satellitefile.size() == 0) 
        {
            cerr << "A TLE file must be specified using the "
                 << "-satfile option.\n";
        }
        else
        {
            cerr << "Satellite ID #" << id << " not found in TLE file\n";
        }
        return(false);
    }

    // Reload TLE data here since select_ephemeris() in libsgp4sdp4
    // changes it.  This is in case the user wants to have two entries
    // with the same satellite.
    this_sat->loadTLE();
    
    this_sat->getSpherical(time, lat, lon, alt);
    return(true);
}

