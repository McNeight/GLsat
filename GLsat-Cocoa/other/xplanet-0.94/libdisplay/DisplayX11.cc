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

#include <climits>
#include <clocale>
#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;

#include <X11/keysym.h>
#include <X11/Xatom.h>

#include "auxfiles.h"
#include "DisplayX11.h"
#include "Marker.h"
#include "Options.h"
#include "util.h"

#include "libannotate/libannotate.h"
#include "libimage/libimage.h"
#include "libprojection/libprojection.h"

DisplayX11::DisplayX11() : DisplayX11Base() 
{
}

void
DisplayX11::doAnnotation(ProjectionBase *image)
{
    if (opts.grid1) image->drawGrid();

    // read the list of satellites to plot and write the corresponding
    // marker/arc files
    if (!opts.satellitefile.empty()) createSatelliteFiles(getTempdir());

    if (!opts.greatarcfile.empty()) drawGreatArcs(image, this);
 
    pixmap = createPixmap(image->rgb_data, window_width,
                          window_height);

    if (!opts.markerfile.empty()) drawMarkers(image, this);

    if (opts.body == SATURN) 
    {
        decomposePixmap(pixmap, image->rgb_data);
        image->drawRings();
        pixmap = createPixmap(image->rgb_data, window_width, window_height);
    }

    if (opts.label) createLabel();
}

void
DisplayX11::initialize()
{
    if (opts.label || !opts.markerfile.empty() || !opts.satellitefile.empty())
    {
        bg_pixel = BlackPixel(display, DefaultScreen(display));

        setColor(opts.color);
        setFont(opts.font);
    }
}

int
DisplayX11::getFontAscent()
{
    int returnval;
    if (fontmode == FontStruct) 
        returnval = font->ascent;
    else
        returnval = -XExtentsOfFontSet(fontset)->max_logical_extent.y;

    return(returnval);
}

int
DisplayX11::getFontDescent()
{
    int returnval;

    if (fontmode == FontStruct) 
        returnval = font->descent;
    else
        returnval = (XExtentsOfFontSet(fontset)->max_logical_extent.height
                     + XExtentsOfFontSet(fontset)->max_logical_extent.y);

    return(returnval);
}

Pixel
DisplayX11::ulongToPixel(const unsigned long value)
{
    XVisualInfo visual_info;
    int entries;
    XVisualInfo v_template;
    v_template.visualid = XVisualIDFromVisual(visual);
    visual_info = *XGetVisualInfo(display, VisualIDMask, &v_template, 
                                  &entries);
    unsigned char red_left_shift;
    unsigned char red_right_shift;
    unsigned char green_left_shift;
    unsigned char green_right_shift;
    unsigned char blue_left_shift;
    unsigned char blue_right_shift;
        
    computeShift(visual_info.red_mask, red_left_shift, 
                 red_right_shift);
    computeShift(visual_info.green_mask, green_left_shift, 
                 green_right_shift);
    computeShift(visual_info.blue_mask, blue_left_shift, 
                 blue_right_shift);
        
    unsigned char r = (unsigned char) ((value & 0xff0000) >> 16);
    unsigned char g = (unsigned char) ((value & 0x00ff00) >> 8);
    unsigned char b = (unsigned char)  (value & 0x0000ff);

    unsigned long red = (unsigned long) r >> red_right_shift;
    unsigned long green = (unsigned long) g >> green_right_shift;
    unsigned long blue = (unsigned long) b >> blue_right_shift;

    Pixel pixel = (((red << red_left_shift) & visual_info.red_mask)
                   | ((green << green_left_shift) 
                      & visual_info.green_mask)
                   | ((blue << blue_left_shift) 
                      & visual_info.blue_mask));
    return(pixel);
}

void
DisplayX11::getRGB(string color, unsigned char *RGB)
{
    string defaultcolor = "red";
    if (color.empty()) color = defaultcolor;

    if (color[0] == '0' && color[1] == 'x')
    {
        unsigned long value = strtoul(color.c_str(), NULL, 16);
        RGB[0] = (unsigned char) ((value & 0xff0000) >> 16);
        RGB[1] = (unsigned char) ((value & 0x00ff00) >> 8);
        RGB[2] = (unsigned char)  (value & 0x0000ff);
    }
    else
    {
        XColor xc;
        if (!XParseColor(display, colormap, color.c_str(), &xc))
        {
            cerr << "Can't find color " << color << ", using "
                 << defaultcolor << endl;
            XParseColor(display, colormap, defaultcolor.c_str(), &xc); 
        }
        RGB[0] = (unsigned char) (xc.red >> 8);
        RGB[1] = (unsigned char) (xc.green >> 8);
        RGB[2] = (unsigned char) (xc.blue >> 8);
    }
}

int
DisplayX11::getTextHeight()
{
    return(getFontAscent() + getFontDescent());
}

int
DisplayX11::getTextHeight(string text)
{
    return(getTextHeight());
}

int
DisplayX11::getTextWidth(string text)
{
    int returnval;
    if (fontmode == FontStruct)
        returnval = XTextWidth(font, text.c_str(), text.size());
    else
        returnval = XmbTextEscapement(fontset, text.c_str(), text.size());

    return(returnval);
}

void
DisplayX11::setColor(string color)
{
    string defaultcolor = "red";
    if (color.empty()) color = defaultcolor;
  
    if (color[0] == '0' && color[1] == 'x')
    {
        fg_pixel = ulongToPixel(strtoul(color.c_str(), NULL, 16));
    }
    else
    {
        XColor xc;      
        if (!XAllocNamedColor(display, colormap, color.c_str(), &xc, &xc)) 
        {
            cerr << "Can't find color " << color << ", using "
                 << defaultcolor << endl;
            XAllocNamedColor(display, colormap, defaultcolor.c_str(), 
                             &xc, &xc);
        }
        fg_pixel = xc.pixel;
    }
}

void
DisplayX11::setFontSize(const int size)
{
// overrides DisplayBase::setFontSize(const int size)
}

void
DisplayX11::setFont(string fontname)
{
    string defaultfont = "variable";
    string defaultfontset = "variable,*--14-*,*--16-*,*";

    char *locale = setlocale(LC_CTYPE, NULL);
    if (locale && strcmp(locale, "C") && strcmp(locale, "POSIX"))
    {
        char **miss, *def;
        int count;
        fontmode = FontSet;
        if (fontname.empty()) fontname = defaultfontset;
        fontset = XCreateFontSet(display, fontname.c_str(),
                                 &miss, &count, &def);
        if (fontset == NULL)
        {
            cerr << "Can't load font " << fontname << ", using "
                 << defaultfontset << endl;
            fontset = XCreateFontSet(display, defaultfontset.c_str(),
                                     &miss, &count, &def);
            if (fontset == NULL)
            {
                cerr << "Can't load default font " << defaultfontset 
                     << " either!  Exiting ..." << endl;
                exit(EXIT_FAILURE);
            }
        }
        if (count)
        {
            cerr << "Fonts for following encoding(s) are not available"
                 << endl;
            for (int i = 0; i < count; i++) cerr << miss[i] << endl;
        }
    } 
    else 
    {
        fontmode = FontStruct;
        if (fontname.empty()) fontname = defaultfont;
        font = XLoadQueryFont(display, fontname.c_str());
        if (font == NULL)
        {
            cerr << "Can't load font " << fontname << ", using " 
                 << defaultfont << endl;
            font = XLoadQueryFont(display, defaultfont.c_str());
        }
        XSetFont(display, gc, font->fid);
    }
}

void
DisplayX11::drawImage(Marker &m)
{
    if (m.image_width == 0) return;

    if (m.image_data == NULL)
    {
	int radius = m.image_width/2;
	XSetForeground(display, gc, fg_pixel);         
	drawCircle(m.x, m.y, radius-1);
	XSetForeground(display, gc, bg_pixel);
	drawCircle(m.x, m.y, radius);
	drawCircle(m.x, m.y, radius-2);
        return;
    }

    XImage *ximage = XGetImage(display, pixmap, 0, 0, window_width, 
                               window_height, AllPlanes, ZPixmap);

    Pixmap new_pixmap = createPixmap(m.image_data, m.image_width, 
                                     m.image_height);

    XImage *new_image = XGetImage(display, new_pixmap, 0, 0, m.image_width, 
                                  m.image_height, AllPlanes, ZPixmap);

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

            if (!m.isTransparent(i, j))
            {
                XPutPixel(ximage, pixel_x, pixel_y, 
                          XGetPixel(new_image, i, j));
            }
        }
    }
    
    XPutImage(display, pixmap, gc, ximage, 0, 0, 0, 0, window_width, 
              window_height);
}

void
DisplayX11::drawCircle(const int x, const int y, const int r)
{
    XDrawArc(display, pixmap, gc, x-r, y-r, 2*r, 2*r, 0, 23040);
}

void 
DisplayX11::drawOutlinedString(const int x, const int y, const string text)
{
    int len = text.size();
    const char *cstring = text.c_str();
    if (fontmode == FontStruct)
    {
        XSetForeground(display, gc, bg_pixel);
        XDrawString(display, pixmap, gc, x+1, y, cstring, len);
        XDrawString(display, pixmap, gc, x-1, y, cstring, len);
        XDrawString(display, pixmap, gc, x, y+1, cstring, len);
        XDrawString(display, pixmap, gc, x, y-1, cstring, len);
        XSetForeground(display, gc, fg_pixel);
        XDrawString(display, pixmap, gc, x, y, cstring, len);
    } 
    else 
    {
        XSetForeground(display, gc, bg_pixel);
        XmbDrawString(display, pixmap, fontset, gc, x+1, y, cstring, len);
        XmbDrawString(display, pixmap, fontset, gc, x-1, y, cstring, len);
        XmbDrawString(display, pixmap, fontset, gc, x, y+1, cstring, len);
        XmbDrawString(display, pixmap, fontset, gc, x, y-1, cstring, len);
        XSetForeground(display, gc, fg_pixel);
        XmbDrawString(display, pixmap, fontset, gc, x, y, cstring, len);
    }
}
