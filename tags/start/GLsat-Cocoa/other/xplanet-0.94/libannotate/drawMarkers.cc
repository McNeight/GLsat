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

#include <clocale>
#include <cstdio>
#include <iostream>
#include <string>
using namespace std;

#include "auxfiles.h"
#include "Marker.h"
#include "Options.h"
#include "parse.h"
#include "Planet.h"
#include "util.h"
#include "xplanet.h"

#include "libdisplay/libdisplay.h"
#include "libprojection/libprojection.h"

static string tz_save;

static bool
markerfileExists(string &filename)
{
    string markerfile;

    if (opts.debug) cout << "\nLooking for markerfile ...\n";

    const string defaultmarkerfile = body_string[opts.body];
    if (filename.empty()) filename = defaultmarkerfile;

    // Thanks to Tomohiro Kubota <kubota@debian.org> for
    // transliteration code.  Xplanet will search for
    // locale/<languagename>/<filename> first, and if that isn't
    // found, search for <filename>
    char *locale = setlocale(LC_CTYPE, NULL);
    char localesave[6]={0,0,0,0,0,0};
    if (locale) strncpy(localesave, locale, 5);
    string locale_string = "locale" + separator + localesave + separator;

    // Check for the input filename
    if (locale)
    {
        markerfile = locale_string + filename;
        if (fileExists(markerfile))
        {
            filename = markerfile;
            return(true);
        }
    }
    if (fileExists(filename)) return(true);

    // Check for the input filename in the "markers" subdirectory
    if (locale)
    {
        markerfile = "markers";
        markerfile += separator;
        markerfile += locale_string;
        markerfile += filename;

        if (fileExists(markerfile))
        {
            filename = markerfile;
            return(true);
        }
    }

    markerfile = "markers";
    markerfile += separator;
    markerfile += filename;
    
    if (fileExists(markerfile))
    {
        filename = markerfile;
        return(true);
    }

    // Check for the input filename in $prefix/markers
    // Check for the input filename in the "markers" subdirectory
    if (locale)
    {
        markerfile = prefix;
        markerfile += separator;
        markerfile += "markers";
        markerfile += separator;
        markerfile += locale_string;
        markerfile += filename;

        if (fileExists(markerfile))
        {
            filename = markerfile;
            return(true);
        }
    }

    markerfile = prefix;
    markerfile += separator;
    markerfile += "markers";
    markerfile += separator;
    markerfile += filename;

    if (fileExists(markerfile))
    {
        filename = markerfile;
        return(true);
    }

    return(false);
}

static void
readMarkerLine(const char *line, list<Marker> &marker_list, 
               ProjectionBase *image)
{
    int i = 0;
    while (isDelimiter(line[i])) i++;
    if (isEndOfLine(line[i])) return;

    int align = AUTO;
    string color;
    
    string font;
    int fontsize = opts.fontsize;
    
    string image_string;
    
    bool need_lat = true;
    bool need_lon = true;
    double lat = 0, lon = 0;
    
    string name = "";
    
    bool pixel_coords = false;
    int pixel_x = 0, pixel_y = 0;
    
    double radius = -1;

    char *returnstring = NULL;

    int symbolsize = 6;

    string timezone = "";
    bool transparency = false;
    unsigned char transparent_pixel[3];
    
    bool end_of_line = false;
    bool syntax_error = false;
    while (!end_of_line)
    {
        switch (parse(i, line, returnstring))
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
                     << "marker file for " << name << endl;
                break;
            }
            break;
        case COLOR:
            if (returnstring == NULL) break;
            color.assign(returnstring);
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
            image_string.assign(returnstring);
            break;
        case LATLON:
            if (need_lat) 
            {
                sscanf(returnstring, "%lf", &lat);
                need_lat = false;
            }
            else if (need_lon)
            {
                sscanf(returnstring, "%lf", &lon);
                need_lon = false;
            }
            else
            {
                syntax_error = true;
            }
            break;
        case NAME:
            if (returnstring == NULL) break;
            name.assign(returnstring);
            break;
        case POSITION:
            if (returnstring == NULL) break;

            if (strncmp(returnstring, "pixel", 1) == 0)
            {
                pixel_coords = true;
                pixel_x = (int) lon;
                pixel_y = (int) lat;

                if (pixel_x < 0) pixel_x += image->getWidth();
                if (pixel_y < 0) pixel_y += image->getHeight();
            }
            else if (strncmp(returnstring, "sun", 1) == 0)
            {
                lat = getSunLat() / deg_to_rad;
                lon = flipped * getSunLon() / deg_to_rad;
            }
            else if (strncmp(returnstring, "moon", 1) == 0)
            {
                if (opts.body == EARTH)
                {
                    lat = getMoonLat() / deg_to_rad;
                    lon = getMoonLon() / deg_to_rad;
                }
                else
                {
                    cerr << "position=moon specifier is meaningless "
                         << "for any body other than earth.\n";
                    syntax_error = true;
                }
            }

            break;
        case RADIUS:
            sscanf(returnstring, "%lf", &radius);
            if (radius < 0) 
            {
                cerr << "Radius value must be positive\n";
                syntax_error = true;
            }
            break;
	case SYMBOLSIZE:
            sscanf(returnstring, "%d", &symbolsize);
            if (symbolsize < 0) symbolsize = 6;
	    break;
        case TIMEZONE:
            if (returnstring == NULL) break;
            timezone.assign(returnstring);
            break;
        case TRANSPARENT:
            if (returnstring == NULL) break;

            transparency = true;

            int r, g, b;
            if (sscanf(returnstring, "%d,%d,%d", &r, &g, &b) == 3)
            {
                transparent_pixel[0] = r & 0xff;
                transparent_pixel[1] = g & 0xff;
                transparent_pixel[2] = b & 0xff;
            }
            else
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
            cerr << "Syntax error in marker file\n";
            cerr << "line is \"" << line << "\"" << endl;
            return;
        }

        i++;
    }

    lon *= (deg_to_rad * flipped);
    lat *= deg_to_rad;

    if (opts.projection == RECTANGULAR && opts.getObsLon() != 0
	&& !opts.mapbounds)
        lon -= opts.getObsLon();

    int x, y;
    bool marker_visible = false;

    if (pixel_coords)
    {
        x = pixel_x;
        y = pixel_y;
    }
    else
    {
        if (lat < -90 || lat > 90)
        {
            cerr << "Latitude value must be between "
                 << "-90 and 90 degrees\n";
            cerr << "line is \"" << line << "\"" << endl;
            return;
        }

        if (lon < -360 || lon > 360)
        {
            cerr << "Longitude value must be between "
                 << "-360 and 360 degrees\n";
            cerr << "line is \"" << line << "\"" << endl;
            return;
        }

        if (opts.projection == ORTHOGRAPHIC && radius > 0)
            marker_visible = image->sphericalToPixel(lon, lat, radius, x, y);
        else
            marker_visible = image->sphericalToPixel(lon, lat, x, y);
    }
        
    if (pixel_coords || marker_visible) 
    {
        if (!timezone.empty())
        {
            string tz = "TZ=";
            tz += timezone;
            putenv((char *) tz.c_str());

            tzset();

            char name_str[256];
            strftime(name_str, sizeof(name_str), name.c_str(), 
                     localtime((time_t *) &opts.time.tv_sec));
            name.assign(name_str);

            if (tz_save.empty()) 
                removeFromEnvironment("TZ"); 
            else
                putenv((char *) tz_save.c_str()); 
        }

        Marker m(x, y, name, align);
        if (image_string.empty())
        {
            m.setImage(symbolsize, symbolsize);
        }
        else if (image_string.compare("none") == 0)
        {
            m.setImage(0, 0);
        }
        else
        {
            m.setImage(image_string);
            if (transparency) m.setTransparency(transparent_pixel);
        }

        if (!color.empty()) m.color = color;
        if (!font.empty()) m.font = font;
        m.fontsize = fontsize;

        marker_list.push_back(m);
    }
}

static void
createMarkerList(list<Marker> &marker_list, ProjectionBase *image)
{
    list<string>::iterator p = opts.markerfile.begin();
    while(p !=  opts.markerfile.end())
    {
        if (markerfileExists(*p))
        {
            if (opts.debug) cout << "Opening marker file " << *p << endl;
            ifstream infile(p->c_str());
            
            char *line = new char[256];
            while (infile.getline (line, 256, '\n') != NULL)
                readMarkerLine(line, marker_list, image);
            
            infile.close();
            delete [] line;
        }
        else
        {
            cerr << "Can't open marker file " << *p << endl;
        }
        p++;
    }
}

static int
findOverlap(list<Marker> &marker_list, Marker &this_marker)
{
    int total_overlap = 0;
    list<Marker>::iterator p = marker_list.begin();
    while (p != marker_list.end())
    {
        if (p->ID == this_marker.ID) 
        {
            p++;
            continue;
        }

        total_overlap += this_marker.overlap(*p);
        p++;
    }
    return(total_overlap);
}

static int
setBounds(Marker &m, int align, DisplayBase *display)
{
    if (m.fontsize != opts.fontsize) display->setFontSize(m.fontsize);
    if (m.font != opts.font) display->setFont(m.font);

    int string_width = display->getTextWidth(m.text);
    int string_height = display->getTextHeight(m.text);
    int ascent = display->getFontAscent();
    int descent = display->getFontDescent();

    int corner_x, corner_y;

    int returnval = 0;

    switch (align)
    {
    case RIGHT:
        m.text_x = m.x + m.image_width/2;
        m.text_y = m.y + ascent/2;
        break;
    case ABOVE:
        m.text_x = m.x - (string_width/2);
        m.text_y = m.y - (descent + m.image_height/2);
        break;
    case BELOW:
        m.text_x = m.x - (string_width/2);
        m.text_y = m.y + (ascent + m.image_height/2);
        break;
    case LEFT:
        m.text_x = m.x - (string_width + m.image_width/2);
        m.text_y = m.y + ascent / 2;
        break;
    case CENTER:
        m.text_x = m.x - string_width/2;
        m.text_y = m.y + ascent/2;
        break;
    default:
        cerr << "Unknown alignment????\n";
        return(0);
    }

    corner_x = m.text_x;
    corner_y = m.text_y + (ascent/2 + descent);

    m.setCorners(corner_x, corner_y, 
                 corner_x + string_width,
                 corner_y - string_height);
    
    int top_overhang = -min(0, corner_y - string_height);
    int bottom_overhang = (max(display->getHeight(), 
                               corner_y)
                           - display->getHeight());
    returnval += max(top_overhang, bottom_overhang) * string_width;

    if (!(opts.projection == RECTANGULAR || opts.projection == MERCATOR
          || opts.projection == PETERS))
    {
        int left_overhang = -min(0, corner_x);
        int right_overhang = (max(display->getWidth(), 
                                  corner_x + string_width) 
                              - display->getWidth());
        int overhang = max(left_overhang, right_overhang);
        returnval += overhang * string_height;
    }

    if (m.fontsize != opts.fontsize) display->setFontSize(opts.fontsize);
    if (m.font != opts.font) display->setFont(opts.font);
    return(returnval);
}

static void
arrangeMarkers(list<Marker> &marker_list, DisplayBase *display)
{
    int alignments[4] = {RIGHT, LEFT, ABOVE, BELOW};

    // initialize each individual Marker's bounding box
    list<Marker>::iterator p = marker_list.begin();
    while (p != marker_list.end())
    {
        setBounds(*p, p->align, display);
        p++;
    }

    marker_list.sort();
    p = marker_list.begin();
    while (p != marker_list.end())
    {
        if (p->fixed_align) 
        {
            p++;
            continue;
        }

        int total_overlap = 0;
        int max_overlap = 0;

        for (int i = 0; i < 4; i++)
        {
            if (i == 0 || total_overlap)
            {
                int overhang = setBounds(*p, alignments[i], display);
                total_overlap = findOverlap(marker_list, *p) + overhang;

                if (i == 0 || total_overlap < max_overlap)
                {
                    p->align = alignments[i];
                    max_overlap = total_overlap;
                }
            }
        }

        setBounds(*p, p->align, display);
        p++;
    }

}

void 
drawMarkers(ProjectionBase *image, DisplayBase *display)
{
    char *get_tz = getenv("TZ");
    if (get_tz != NULL)
    {
        tz_save = "TZ=";
        tz_save += get_tz;
    }

    list<Marker> marker_list;

    createMarkerList(marker_list, image);

    arrangeMarkers(marker_list, display);
    
    ofstream outfile;
    if (!opts.markerbounds.empty()) 
    {
        outfile.open(opts.markerbounds.c_str());
        if (outfile.bad())
        {
            cerr << "Can't open imagemap file " 
                 << opts.markerbounds << endl;
        }
    }
    
    list<Marker>::iterator p = marker_list.begin();
    while (p != marker_list.end())
    {
        display->drawMarker(*p);

        if (p->image_data != NULL) deleteImage(p->image_data);

        if (!opts.markerbounds.empty() && !outfile.bad()) 
            p->printCorners(outfile);
        p++;
    }
}

