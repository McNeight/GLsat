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
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

#include "auxfiles.h"
#include "checkfuncs.h"
#include "DisplayBase.h"
#include "Location.h"
#include "Marker.h"
#include "Options.h"
#include "Planet.h"
#include "util.h"

#include "libannotate/libannotate.h"
#include "libprojection/libprojection.h"

#ifdef HAVE_LIBX11
#include <X11/Xlib.h>
#endif

int
DisplayBase::getType()
{
#ifdef HAVE_LIBX11
    Display *d = XOpenDisplay(NULL);
    if (d != NULL) 
    {
        XCloseDisplay(d);
        return(X11);
    }
#endif 

#ifdef HAVE_AQUA
    return(AQUA);
#endif

#ifdef HAVE_CYGWIN
    return(MSWIN);
#endif

    if (opts.display == OUTPUT) return(OUTPUT);

    return (UNKNOWN);
}

DisplayBase::~DisplayBase()
{
}

void
DisplayBase::initialize()
{
    if (opts.label || opts.markers || !opts.satellitefile.empty())
    {
        memset(fg, 255, 3);
        memset(bg, 0, 3);

        loadRGB();
        
        setColor(opts.color);
#ifdef HAVE_LIBFREETYPE 
        int error = FT_Init_FreeType(&library);
        
        if (error)
        {
            cerr << "Can't initialize freetype library\n";
            exit(EXIT_FAILURE);
        }
        fontsize = opts.fontsize;
#endif
        setFont(opts.font);
    }
}

int
DisplayBase::getHeight()
{
    return(window_height);
}

int
DisplayBase::getWidth()
{
    return(window_width);
}

void
DisplayBase::drawImage(Marker &m)
{
    if (m.image_width == 0) return;

    if (m.image_data == NULL)
    {
	int radius = m.image_width/2;
	setForeground(fg);
	drawCircle(m.x, m.y, radius-1);
	setForeground(bg);
	drawCircle(m.x, m.y, radius);
	drawCircle(m.x, m.y, radius-2);
        return;
    }

    unsigned char savecolor[3];
    memcpy(savecolor, fg, 3);
    int offset_x = m.x - m.image_width/2;
    int offset_y = m.y - m.image_height/2;
    for (int j = 0; j < m.image_height; j++)
    {
        int pixel_y = j + offset_y;
        if (pixel_y < 0 || pixel_y >= window_height) continue;
        for (int i = 0; i < m.image_width; i++)
        {
            int pixel_x = i + offset_x;
            if (pixel_y == window_height - 1 && pixel_x >= window_width)
                pixel_x -= window_width;

            setForeground(m.image_data + 3*(j*m.image_width + i));

            if (!m.isTransparent(i, j)) setPixel(pixel_x, pixel_y);
        }
    }

    setForeground(savecolor);
}

void 
DisplayBase::drawMarker(Marker &m)
{
    if (!m.color.empty()) setColor(m.color);
    if (m.fontsize != opts.fontsize) setFontSize(m.fontsize);
    if (m.font != opts.font) setFont(m.font);

    drawImage(m);
    drawOutlinedString(m.text_x, m.text_y, m.text);
    
    const bool wraparound_projection = ((opts.projection == RECTANGULAR 
					 || opts.projection == MERCATOR
					 || opts.projection == PETERS)
					&& !opts.mapbounds);
    if (wraparound_projection)
    {
        if (m.text_x < 0) 
            drawOutlinedString(m.text_x + window_width, m.text_y, m.text);
        else if ((m.text_x + getTextWidth(m.text)) > window_width) 
            drawOutlinedString(m.text_x - window_width, m.text_y, m.text);
    }

    if (!m.color.empty()) setColor(opts.color);
    if (m.fontsize != opts.fontsize) setFontSize(opts.fontsize);
    if (m.font != opts.font) setFont(opts.font);
}

int
DisplayBase::fontExists(string &fontname)
{
    string newfont;

    string defaultfont = "helr____.ttf";
    if (fontname.empty()) fontname = defaultfont;

    if (opts.fontdir.size() > 0)
    {
        list<string>::iterator p = opts.fontdir.begin();
        while (p != opts.fontdir.end())
        {
            newfont = *p;
            newfont += separator;
            newfont += fontname;
            
            if (opts.debug) cout << "Checking for " << newfont << endl;
            if (fopen(newfont.c_str(), "r") != NULL)
            {
                fontname = newfont;
                return(1);
            }
            p++;
        }
    }


    if (opts.debug) cout << "Checking for " << fontname << endl;
    if (fopen(fontname.c_str(), "r") != NULL) return(1);

    newfont = "fonts";
    newfont += separator;
    newfont += fontname;

    if (opts.debug) cout << "Checking for " << newfont << endl;
    if (fopen(newfont.c_str(), "r") != NULL)
    {
        fontname = newfont;
        return(1);
    }

    newfont = prefix;
    newfont += separator;
    newfont += "fonts";
    newfont += separator;
    newfont += fontname;

    if (opts.debug) cout << "Checking for " << newfont << endl;
    if (fopen(newfont.c_str(), "r") != NULL)
    {
        fontname = newfont;
        return(1);
    }

    cerr << "Can't find font " << fontname;

    if (fontname.compare(defaultfont) != 0)
    {
        cerr << ", looking for default font " << defaultfont << endl;

        if (fontExists(defaultfont) == 0)
        {
            cerr << "Can't find default font either!\n";
        }
        else
        {
            fontname = defaultfont;
            return(1);
        }
    }
    else
    {
        cerr << endl;
    }

    return(0);
}
 
int
DisplayBase::getFontAscent()
{
    // I'll do this right later
#ifdef HAVE_LIBFREETYPE
    return((int) (0.67 * getTextHeight() + 0.5));
#else
    cerr << "This binary was compiled without freetype support.\n";
    printError("Text annotation");
    return(0);
#endif    /* HAVE_LIBFREETYPE */
}

int
DisplayBase::getFontDescent()
{
    // I'll do this right later
#ifdef HAVE_LIBFREETYPE
    return((int) (-0.33 * getTextHeight() + 0.5));
#else
    cerr << "This binary was compiled without freetype support.\n";
    printError("Text annotation");
    return(0);
#endif    /* HAVE_LIBFREETYPE */
}

void 
DisplayBase::getRGB(string color, unsigned char *RGB)
{
    string defaultcolor = "red";
    if (color.empty()) color = defaultcolor;

    unsigned int value = 0xff0000;

    if (color[0] == '0' && color[1] == 'x')
    {
        value = strtoul(color.c_str(), NULL, 16);
    }
    else
    {
        // Convert to lower case
        for (unsigned int i = 0; i < color.size(); i++)
            color[i] = tolower(color[i]);
        
        // There's a DarkSlateGray but no DarkSlateGrey
        unsigned int grey = color.find("grey");
        if (grey != string::npos && grey < color.size()) color[grey+2] = 'a';
        
        memset(RGB, 0, 3);
        
        map<string, unsigned int>::iterator p;
        p = RGBColors.find(color);
        if (p != RGBColors.end())
        {
            value = p->second;
        }
        else
        {
            if (color.compare(defaultcolor) != 0)
                cerr << "Can't find color " << color << ", using " 
                     << defaultcolor << endl;
        }
    }

    RGB[0] = (unsigned char) ((value & 0xff0000) >> 16);
    RGB[1] = (unsigned char) ((value & 0x00ff00) >> 8);
    RGB[2] = (unsigned char)  (value & 0x0000ff);
}

int
DisplayBase::getTextHeight()
{
#ifdef HAVE_LIBFREETYPE
    return(face->size->metrics.y_ppem);
#else
    cerr << "This binary was compiled without freetype support.\n";
    cerr << "Font metrics are not supported.\n";
    return(0);
#endif    /* HAVE_LIBFREETYPE */
}

int
DisplayBase::getTextHeight(const string text)
{
#ifdef HAVE_LIBFREETYPE
    return(getTextHeight());
#else
    cerr << "This binary was compiled without freetype support.\n";
    cerr << "Font metrics are not supported.\n";
    return(0);
#endif    /* HAVE_LIBFREETYPE */
}

int
DisplayBase::getTextWidth(const string text)
{
#ifdef HAVE_LIBFREETYPE
    int w = 0;

    FT_UInt glyph_index, previous = 0;
    int use_kerning = FT_HAS_KERNING(face);

    for (unsigned int i = 0; i < text.size(); i++)
    {
        glyph_index = FT_Get_Char_Index(face, text[i]);

        if (use_kerning && previous && glyph_index)
        {
            FT_Vector delta;
            FT_Get_Kerning(face, previous, glyph_index,
                           ft_kerning_default, &delta);
            w += delta.x / 64;
        }

        FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER);

        w += face->glyph->advance.x / 64;
    }
    return(w);
#else
    cerr << "This binary was compiled without freetype support.\n";
    cerr << "Font metrics are not supported.\n";
    return(0);
#endif    /* HAVE_LIBFREETYPE */
}

void 
DisplayBase::setColor(string color)
{
    getRGB(color, fg);
}

void
DisplayBase::setFontSize(const int size)
{
#ifdef HAVE_LIBFREETYPE
    fontsize = size;

    int error = FT_Set_Pixel_Sizes(face, 0, fontsize);
    if (error) error = FT_Set_Pixel_Sizes(face, 0, 12);
    if (error)
    {
        cerr << "Error setting pixel size\n";
        exit(EXIT_FAILURE);
    }
#endif
}

void
DisplayBase::setFont(string fontname)
{
#ifdef HAVE_LIBFREETYPE
    if (!fontExists(fontname))
    {
        cerr << "Couldn't find any fonts.  Exiting.\n";
        exit(EXIT_FAILURE);
    }
    else
    {
        if (opts.debug) cout << "Found " << fontname << endl;
    }

    int error = FT_New_Face(library, fontname.c_str(), 0, &face);

    if (error == FT_Err_Unknown_File_Format)
    {
        cerr << "Unsupported font format.\n";
    }

    setFontSize(fontsize);

    if (opts.debug)
        cout << "Font set to " << fontname << endl;

#else
    cerr << "This binary was compiled without freetype support.\n";
    cerr << "Text annotation is not supported.\n";
    return;
#endif    /* HAVE_LIBFREETYPE */
}

void 
DisplayBase::createLabel()
{
    double sunlat = getSunLat();
    double sunlon = getSunLon();

    float sunlatdeg = sunlat / deg_to_rad;
    float sunlondeg = flipped * sunlon / deg_to_rad;
    if (sunlondeg > 180) 
        sunlondeg -= 360;
    else if (sunlondeg < -180) 
        sunlondeg += 360;

    float obslatdeg = opts.getObsLat() / deg_to_rad;
    float obslondeg = flipped * opts.getObsLon() / deg_to_rad;
    if (obslondeg > 180) 
        obslondeg -= 360;
    else if (obslondeg < -180) 
        obslondeg += 360;

    char body_label[64];
    char date_label[64];
    char illum_label[64];
    char obs_label[64];
    char rotate_label[64];
    char sun_label[64];

    strcpy(body_label, body_string[opts.body]);
    body_label[0] = toupper(body_label[0]);

    // Write the current time
    string tz_save = "";
    if (opts.gmtlabel) 
    {
        char *get_tz = getenv("TZ");
        if (get_tz != NULL)
        {
            tz_save = "TZ=";
            tz_save += get_tz;
        }
        putenv("TZ=GMT");
    }

    tzset();
    strftime(date_label, sizeof(date_label), opts.label_format.c_str(),
             localtime((time_t *) &opts.time.tv_sec));

    if (opts.gmtlabel)
    {
        if (tz_save.empty()) 
            removeFromEnvironment("TZ"); 
        else
            putenv((char *) tz_save.c_str()); 
    }

    // Write the sun's and observer's positions
    if (opts.body == EARTH || opts.body == MOON)
    {
        sprintf(sun_label, "sun %4.1f %c %5.1f %c",
                fabs(sunlatdeg), ((sunlatdeg < 0) ? 'S' : 'N'),
                fabs(sunlondeg), ((sunlondeg < 0) ? 'W' : 'E'));
        sprintf(obs_label, "obs %4.1f %c %5.1f %c",
                fabs(obslatdeg), ((obslatdeg < 0) ? 'S' : 'N'),
                fabs(obslondeg), ((obslondeg < 0) ? 'W' : 'E'));
    }
    else
    {
        if (sunlondeg < 0) sunlondeg += 360;
        sprintf(sun_label, "sun %4.1f %c %5.1f",
                fabs(sunlatdeg), ((sunlatdeg < 0) ? 'S' : 'N'),
                sunlondeg);
        if (obslondeg < 0) obslondeg += 360;
        sprintf(obs_label, "obs %4.1f %c %5.1f",
                fabs(obslatdeg), ((obslatdeg < 0) ? 'S' : 'N'),
                obslondeg);
    }

    if (opts.label_rot && opts.projection != RECTANGULAR)
    {
        double rotatedeg = drem(opts.rotate, TWO_PI) / deg_to_rad;
        sprintf(rotate_label, "rotate %5.1f", rotatedeg);
    }

    if (opts.projection == ORTHOGRAPHIC)
    {
        // Write percent illumination
        Location l1(sunlat, sunlon, 1);
        Location l2(opts.getObsLat(), opts.getObsLon(), 1);
        l1.sphericalToRectangular();
        l2.sphericalToRectangular();
        // Find the angle between the two vectors
        float phase = l1.dot(l2);
        phase = 50 * (1 + phase);

        sprintf(illum_label, "illumination %.1f %%", phase);
    }

    drawLabel(body_label, date_label, illum_label, obs_label, 
              rotate_label, sun_label);
}

void
DisplayBase::loadRGB()
{
    string rgbfile = "rgb.txt";

    ifstream infile(rgbfile.c_str());
    if (infile.bad()) 
    {
        infile.close();
        rgbfile = prefix;
        rgbfile += separator;
        rgbfile += "rgb.txt";
        infile.open(rgbfile.c_str());

        if (infile.bad())
        {
            cerr << "Can't open RGB file " << rgbfile << endl;
            return;
        }
    }

    char line[128], name[128];
    int r, g, b;
    while (infile.getline(line, 128))
    {
        if (sscanf(line, "%d %d %d %[^\n]\n", &r, &g, &b, name) != 4) 
            continue;

        if ((r < 0 || r > 255) || (g < 0 || g > 255) || (b < 0 || g > 255))
            continue;

        // strip off DOS end-of-line
        char *ptr = strchr(name, 13);
        if (ptr != NULL) *ptr = '\0';

        // convert to lower case
        for (ptr = name; *ptr != '\0'; ptr++) *ptr = tolower(*ptr);

        unsigned int c = (r << 16) + (g << 8) + b;
        
        RGBColors.insert(make_pair(string(name), c));
    }
    infile.close();
}

void
DisplayBase::setAlpha(unsigned char *c)
{
    png_alpha = c;
}

void 
DisplayBase::setImage(unsigned char *c)
{
    rgb_data = c;
}

void 
DisplayBase::drawLabel(const string body_label, const string date_label, 
                       const string illum_label, const string obs_label, 
                       const string rotate_label, const string sun_label)
{
    if (opts.label_mask & XNegative) opts.label_x += window_width;
    if (opts.label_mask & YNegative) opts.label_y += window_height;

    int x = opts.label_x;                    // label geometry
    int y = opts.label_y;
    int dy = getTextHeight();

    if (opts.label_mask & YNegative)
    {
        y = opts.label_y - 3 * dy;
        if (opts.label_name) y -= dy;
        if (opts.label_rot) y -= dy;
        if (opts.projection == ORTHOGRAPHIC) y -= dy;
    }

    if (opts.label_name)
    {
        if (opts.label_mask & XNegative) 
            x = opts.label_x - getTextWidth(body_label);
        y += dy;
        drawOutlinedString(x, y, body_label);
    }

    if (opts.label_mask & XNegative) 
        x = opts.label_x - getTextWidth(date_label);
    y += dy;
    drawOutlinedString(x, y, date_label);

    if (opts.label_mask & XNegative) 
        x = opts.label_x - getTextWidth(sun_label);
    y += dy;
    drawOutlinedString(x, y, sun_label);

    if (opts.label_mask & XNegative) 
        x = opts.label_x - getTextWidth(obs_label);
    y += dy;
    drawOutlinedString(x, y, obs_label);
  
    if (opts.label_rot)
    {
        if (opts.label_mask & XNegative) 
            x = opts.label_x - getTextWidth(rotate_label);
        y += dy;
        drawOutlinedString(x, y, rotate_label);
    }

    if (opts.projection == ORTHOGRAPHIC)
    {
        if (opts.label_mask & XNegative)
            x = opts.label_x - getTextWidth(illum_label);
        y += dy;
        drawOutlinedString(x, y, illum_label);
    }
}

void
DisplayBase::drawOutlinedString(const int x, const int y, const string text)
{
    setForeground(bg);
    drawText(x+1, y, text);
    drawText(x-1, y, text);
    drawText(x, y+1, text);
    drawText(x, y-1, text);

    setForeground(fg);
    drawText(x, y, text);
}

void 
DisplayBase::printError(const string message)
{
    cerr << message << " not supported for " << identifyMe() << " mode.\n";
}

void 
DisplayBase::drawCircle(const int x, const int y, const int r)
{
    int xx, yy;
    double r2 = r * r;
    double dd = 1 / (M_PI_2 * r);
    for (double d = 0; d < M_PI_2; d += dd)
    {
        xx = (int) (cos(d) * r + 0.5);
        yy = (int) (sin(d) * r + 0.5);
        double opacity = (xx * xx + yy * yy) / r2;
        if (opacity > 1) opacity = 1/opacity;

        setPixel(x + xx, y + yy, opacity);
        setPixel(x - xx, y + yy, opacity);
        setPixel(x + xx, y - yy, opacity);
        setPixel(x - xx, y - yy, opacity);
    }
    setPixel(x, y + r);
    setPixel(x, y - r);
}

void 
DisplayBase::drawText(const int x, const int y, const string text)
{
#ifdef HAVE_LIBFREETYPE
    int pen_x = x;
    int pen_y = y;

    FT_GlyphSlot slot = face->glyph;

    FT_UInt glyph_index, previous = 0;
    int use_kerning = FT_HAS_KERNING(face);

    for (unsigned int i = 0; i < text.size(); i++)
    {
        glyph_index = FT_Get_Char_Index(face, text[i]);

        if (use_kerning && previous && glyph_index)
        {
            FT_Vector delta;
            FT_Get_Kerning(face, previous, glyph_index,
                           ft_kerning_default, &delta);
            pen_x += delta.x / 64;
        }

        FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER);

        FT_Bitmap bitmap = slot->bitmap;

        for (int j = 0; j < bitmap.rows; j++)
        {
            int istart = j * bitmap.width;
            for (int k = 0; k < bitmap.width; k++)
            {
                if (bitmap.buffer[istart + k])
                {
                    double opacity = bitmap.buffer[istart + k]/255.0;
                    setPixel(pen_x + slot->bitmap_left + k, 
                             pen_y - slot->bitmap_top + j, 
                             opacity);
                }
            }
        }

        pen_x += slot->advance.x / 64;
    }
#else
    cerr << "drawText()\n";
    cerr << "This binary was compiled without freetype support.\n";
    cerr << "Text annotation is not supported.\n";
    return;
#endif    /* HAVE_LIBFREETYPE */
}

void
DisplayBase::setPixel(const int x, const int y)
{
    setPixel(x, y, 1.0);
}

void
DisplayBase::setPixel(const int x, const int y, double opacity)
{
    const bool wraparound_projection = ((opts.projection == RECTANGULAR 
					 || opts.projection == MERCATOR
					 || opts.projection == PETERS)
					&& !opts.mapbounds);
    
    if (!wraparound_projection && (x < 0 || x >= window_width)) return;

    if (y < 0 || y >= window_height) return;

    int ipos = y * window_width + x;

    if (y == window_height - 1 && x >= window_width) 
        ipos -= window_width;

    if (opts.transpng)
    {
        unsigned char old_alpha = png_alpha[ipos];
        unsigned char new_alpha = (unsigned char) (opacity * 255);
        if (old_alpha < new_alpha) 
        {
            png_alpha[ipos] = new_alpha;
            opacity = 1;
        }
    }

    ipos *= 3;
    for (int i = 0; i < 3; i++)
        rgb_data[ipos] = (unsigned char) ((current_color[i] * opacity)
                                          + (1 - opacity) 
                                          * rgb_data[ipos++]);
}

void
DisplayBase::setForeground(unsigned char *color)
{
    memcpy(current_color, color, 3);
}

void
DisplayBase::doAnnotation(ProjectionBase *image)
{
    if (opts.grid1) image->drawGrid();

    // read the list of satellites to plot and write the corresponding
    // marker/arc files
    if (!opts.satellitefile.empty()) createSatelliteFiles(getTempdir());

    if (!opts.greatarcfile.empty()) drawGreatArcs(image, this);

    setAlpha(image->png_alpha);
    setImage(image->rgb_data);
    
    if (!opts.markerfile.empty()) drawMarkers(image, this);

    if (opts.body == SATURN) image->drawRings();

    if (opts.label) createLabel();
}

string
DisplayBase::getTempdir()
{
    char *tmpdir = getenv("TMPDIR");
    if (tmpdir == NULL) 
        return(string("/tmp"));
    else
        return(string(tmpdir));
}
