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

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

#include "auxfiles.h"
#include "Options.h"
#include "parse.h"
#include "Planet.h"
#include "util.h"

#include "libdisplay/libdisplay.h"
#include "libprojection/libprojection.h"

static void
readGreatArcLine(const char *line, ProjectionBase *image, 
                 DisplayBase *display)
{
    int i = 0, j = 0;
    while (isDelimiter(line[i])) i++;
    if (isEndOfLine(line[i])) return;

    double lon1, lat1, lon2, lat2;
    double coord[4];
    
    char *returnstring = NULL;
    string colorname;
    unsigned char color[3] = {255, 255, 255};

    float spacing = opts.spacing;

    int num_radius = 0;
    double radius[2];

    bool end_of_line = false;
    bool syntax_error = false;
    while (!end_of_line)
    {
        switch (parse(i, line, returnstring))
        {
        case COLOR:
            if (returnstring == NULL) break;
            colorname.assign(returnstring);
            break;
        case DELIMITER:
            break;
        case ENDOFLINE:
            end_of_line = true;
            break;
        case LATLON:
            if (j < 4)
            {
                sscanf(returnstring, "%lf", &coord[j]);
                if (j%2 == 0)
                {
                    if (coord[j] < -90 || coord[j] > 90)
                    {
                        cerr << "Latitude value must be between "
                             << "-90 and 90 degrees\n";
                        syntax_error = true;
                    }
                }
                else 
                {
                    if (coord[j] < -360 || coord[j] > 360)
                    {
                        cerr << "Longitude value must be between "
                             << "-360 and 360 degrees\n";
                        syntax_error = true;
                    }
                }
                j++;
            }
            else
            {
                syntax_error = true;
            }
            break;
        case RADIUS:
            if (num_radius < 2) 
            {
                sscanf(returnstring, "%lf", &radius[num_radius]);
                if (radius[num_radius] < 0) 
                {
                    cerr << "Radius value must be positive\n";
                    syntax_error = true;
                }
                else
                    num_radius++;
            }
            break;
        case SPACING:
            if (returnstring == NULL) break;
            sscanf(returnstring, "%f", &spacing);
            if (spacing < 0.001) spacing = 0.001;
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
            cerr << "Syntax error in arc file\n";
            cerr << "line is \"" << line << "\"" << endl;
            return;
        }

        i++;
    }

    if (j != 4)
    {
        cerr << "Incomplete entry in arc file\n";
        cerr << "line is \"" << line << "\"" << endl;
        return;
    }
    
    lat1 = coord[0] * deg_to_rad;
    lon1 = coord[1] * deg_to_rad * flipped;
    lat2 = coord[2] * deg_to_rad;
    lon2 = coord[3] * deg_to_rad * flipped;
    
    if (lon1 > M_PI) lon1 -= TWO_PI;
    else if (lon1 < -M_PI) lon1 += TWO_PI;
    
    if (lon2 > M_PI) lon2 -= TWO_PI;
    else if (lon2 < -M_PI) lon2 += TWO_PI;
    
    if (!colorname.empty())
        if (display != NULL) display->getRGB(colorname, color);
    
    if (opts.projection == ORTHOGRAPHIC && num_radius > 0)
    {
        if (num_radius == 1) radius[1] = radius[0];
        image->drawGreatArc(lon1, lat1, radius[0], lon2, lat2, radius[1], 
                            spacing, color);
    }
    else
    {
        image->drawGreatArc(lon1, lat1, lon2, lat2, spacing, color);
    }
}

static bool
arcFileExists(string &filename)
{
    if (opts.debug) 
    {
        cout << "\nLooking for arc file ...\n"
             << filename << endl;
    }

    if (fileExists(filename)) return(true);

    string arc_file = "arcs";
    arc_file += separator;
    arc_file += filename;
    
    if (fileExists(arc_file))
    {
        filename = arc_file;
        return(true);
    }

    arc_file = prefix;
    arc_file += separator;
    arc_file += "arcs";
    arc_file += separator;
    arc_file += filename;

    if (fileExists(arc_file))
    {
        filename = arc_file;
        return(true);
    }

    return(false);
}

void 
drawGreatArcs(ProjectionBase *image, DisplayBase *display)
{
    list<string>::iterator p = opts.greatarcfile.begin();
    while(p !=  opts.greatarcfile.end())
    {
        if (arcFileExists(*p))
        {
            ifstream infile(p->c_str());
            char *line = new char[256];
            while (infile.getline(line, 256, '\n') != NULL)
                readGreatArcLine(line, image, display);
            
            infile.close();
	    delete [] line;
        }
        else
        {
            cerr << "Can't open great arc file " << *p << endl;
        }
        p++;
    }
}
